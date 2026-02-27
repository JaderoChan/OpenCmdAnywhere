#include "settings.h"

#include <qsettings.h>

#include "config.h"
#include "language.h"

#define READ_KC(settings, key, defaultValue) \
gbhk::KeyCombination(settings.value(key, defaultValue).toString().toStdString())

static QList<ExecutableItem> createDefaultExecutables()
{
    QList<ExecutableItem> executables;
#ifdef Q_OS_WIN
    executables.append(ExecutableItem{COMMAND_NAME, COMMAND_FILEPATH});
    executables.append(ExecutableItem{POWERSHELL_NAME, POWERSHELL_FILEPATH});
#elif defined(Q_OS_MAC)
    executables.append(ExecutableItem{TERMINAL_NAME, TERMINAL_FILEPATH});
#else
    // Pass
#endif // Q_OS_WIN
    return executables;
}

const QList<ExecutableItem>& defaultExecutables()
{
    static QList<ExecutableItem> executables = createDefaultExecutables();
    return executables;
}

Settings loadSettings()
{
    Settings settings;
    QSettings qsettings(QSettings::NativeFormat, QSettings::UserScope, APP_ORGANIZATION, APP_TITLE);

    settings.autoRunOnStartUp = qsettings.value("AutoRunOnStartUp", false).toBool();
    settings.currentExecutableIdx = qsettings.value("CurrentExecutableIdx", 0).toInt();
    settings.language = qsettings.value("Language", currentSystemLang()).toString();
    settings.parameter = qsettings.value("Parameter", "").toString();
    settings.hotkey = READ_KC(qsettings, "Hotkey", DEFAULT_HOTKEY);

    return settings;
}

void saveSettings(const Settings& settings)
{
    QSettings qsettings(QSettings::NativeFormat, QSettings::UserScope, APP_ORGANIZATION, APP_TITLE);

    qsettings.setValue("AutoRunOnStartUp", settings.autoRunOnStartUp);
    qsettings.setValue("CurrentExecutableIdx", settings.currentExecutableIdx);
    qsettings.setValue("Language", settings.language);
    qsettings.setValue("Parameter", settings.parameter);
    qsettings.setValue("Hotkey", QString::fromStdString(settings.hotkey.toString()));
}
