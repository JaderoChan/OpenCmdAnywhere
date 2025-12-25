#include "settings.h"

#include <qapplication.h>
#include <qlocale.h>

#include "config.h"

Settings::Settings()
    : sm_(QApplication::organizationName(), QApplication::applicationName())
{
    executables_ = sm_.readSetting(
        "Executables",
        QVariantMap({{COMMAND_DISPLAY_NAME, COMMAND_EXE}, {POWER_SHELL_DISPLAY_NAME, POWER_SHELL_EXE}})
    ).toMap();
}

Settings& Settings::getInstance()
{
    static Settings instance;
    return instance;
}

QString Settings::getLangugae()
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
    QString displayName = getInstance().sm_.readSetting("CurrentExecutable", COMMAND_DISPLAY_NAME).toString();
    const auto& map = getAllExecutables();
    if (map.contains(displayName))
        return {displayName, map[displayName].toString()};
    return {displayName, ""};
}

QString Settings::getParameter()
{
    return getInstance().sm_.readSetting("Parameter", "").toString();
}

gbhk::KeyCombination Settings::getKeyCombination(bool isAdmin)
{
    if (isAdmin)
    {
        QString kcStr =
            getInstance().sm_.readSetting("RunAsAdminHotkey", RUN_AS_ADMIN_HOTKEY).toString();
        return gbhk::KeyCombination::fromString(kcStr.toStdString());
    }
    else
    {
        QString kcStr =
            getInstance().sm_.readSetting("RunAsUserHotkey", RUN_AS_USER_HOTKEY).toString();
        return gbhk::KeyCombination::fromString(kcStr.toStdString());
    }
}

bool Settings::getIsRunOnStartup()
{
    return getInstance().sm_.readSetting("RunOnStartup", false).toBool();
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

void Settings::setKeyCombination(const gbhk::KeyCombination& value, bool isAdmin)
{
    QString kcStr = QString::fromStdString(value.toString());
    auto& sm = getInstance().sm_;
    if (isAdmin)
        sm.writeSetting("RunAsAdminHotkey", kcStr);
    else
        sm.writeSetting("RunAsUserHotkey", kcStr);
}

void Settings::setIsRunOnStartup(bool value)
{
    getInstance().sm_.writeSetting("RunOnStartup", value);
}

QVariantMap Settings::getAllExecutables()
{
    return getInstance().executables_;
}

void Settings::addExecutable(const QString& displayName, const QString& filename)
{
    getInstance().executables_[displayName] = filename;
    getInstance().sm_.writeSetting("Executables", getInstance().executables_);
}

void Settings::removeExecutable(const QString& displayName)
{
    getInstance().executables_.remove(displayName);
    getInstance().sm_.writeSetting("Executables", getInstance().executables_);
    // 如果删除的是当前Executable，则尝试回退当前Executable
    if (getCurrentExecutable().first == displayName)
    {
        auto exes = getAllExecutables();
        if (!exes.isEmpty())
            setCurrentExecutable(exes.begin().key());
        else
            setCurrentExecutable("");
    }
}
