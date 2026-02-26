#include "settings.h"

#include <qapplication.h>
#include <qlocale.h>

#include "config.h"

Settings::Settings()
{
    QMutexLocker<QMutex> locker(&mtx_);
    executables_ = sm_.readSetting(
        "Executables",
    #ifdef Q_OS_WIN
        QVariantMap({{COMMAND_DISPLAY_NAME, COMMAND_EXE}, {POWER_SHELL_DISPLAY_NAME, POWER_SHELL_EXE}})
    #elif defined(Q_OS_MAC)
        QVariantMap({{TERMINAL_DISPLAY_NAME, TERMINAL_EXE}})
    #else
        // pass
    #endif // Q_OS_WIN
    ).toMap();
}

Settings& Settings::getInstance()
{
    static Settings instance;
    return instance;
}

QString Settings::getLanguage()
{
    QLocale locale = QLocale::system();
    switch (locale.language())
    {
        case QLocale::Language::English: return getInstance().sm_.readSetting("Language", "EN").toString();
        case QLocale::Language::Chinese: return getInstance().sm_.readSetting("Language", "ZH").toString();
        default: return getInstance().sm_.readSetting("Language", "EN").toString();
    }
}

std::pair<QString, QString> Settings::getCurrentExecutable()
{
#ifdef Q_OS_WIN
    QString displayName = getInstance().sm_.readSetting("CurrentExecutable", COMMAND_DISPLAY_NAME).toString();
#elif defined(Q_OS_MAC)
    QString displayName = getInstance().sm_.readSetting("CurrentExecutable", TERMINAL_DISPLAY_NAME).toString();
#else
    // pass
#endif
    const auto& map = getAllExecutables();
    if (map.contains(displayName))
        return {displayName, map[displayName].toString()};
    return {displayName, ""};
}

QString Settings::getParameter()
{
    return getInstance().sm_.readSetting("Parameter", "").toString();
}

gbhk::KeyCombination Settings::getKeyCombination()
{
    QString kcStr =
        getInstance().sm_.readSetting("Hotkey", DEFAULT_HOTKEY).toString();
    return gbhk::KeyCombination::fromString(kcStr.toStdString());
}

bool Settings::getIsAutoRunOnStartUp()
{
    return getInstance().sm_.readSetting("AutoRunOnStartUp", false).toBool();
}

void Settings::setLanguage(const QString& value)
{
    getInstance().sm_.writeSetting("Language", value);
}

void Settings::setCurrentExecutable(const QString& value)
{
    getInstance().sm_.writeSetting("CurrentExecutable", value);
}

void Settings::setParameter(const QString& value)
{
    if (value.isEmpty())
        getInstance().sm_.removeSetting("Parameter");
    else
        getInstance().sm_.writeSetting("Parameter", value);
}

void Settings::setKeyCombination(const gbhk::KeyCombination& value)
{
    QString kcStr = QString::fromStdString(value.toString());
    auto& sm = getInstance().sm_;
    sm.writeSetting("Hotkey", kcStr);
}

void Settings::setAutoRunOnStartUp(bool value)
{
    getInstance().sm_.writeSetting("AutoRunOnStartUp", value);
}

QVariantMap Settings::getAllExecutables()
{
    QMutexLocker<QMutex> locker(&getInstance().mtx_);
    return getInstance().executables_;
}

void Settings::addExecutable(const QString& displayName, const QString& filepath)
{
    {
        QMutexLocker<QMutex> locker(&getInstance().mtx_);
        getInstance().executables_[displayName] = filepath;
    }
    getInstance().sm_.writeSetting("Executables", getAllExecutables());
}

void Settings::removeExecutable(const QString& displayName)
{
    {
        QMutexLocker<QMutex> locker(&getInstance().mtx_);
        getInstance().executables_.remove(displayName);
    }
    getInstance().sm_.writeSetting("Executables", getAllExecutables());
    // 如果删除的是当前Executable，则尝试回退当前Executable。
    if (getCurrentExecutable().first == displayName)
    {
        auto exes = getAllExecutables();
        if (!exes.isEmpty())
            setCurrentExecutable(exes.begin().key());
        else
            setCurrentExecutable("");
    }
}
