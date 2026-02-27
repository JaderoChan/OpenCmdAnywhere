#include "app_manager.h"

#include <thread>

#include <qdir.h>
#include <qdesktopservices.h>
#include <qmessagebox.h>
#include <qprocess.h>

#include <easy_translate.hpp>

#include "config.h"
#include "language.h"
#include "platforms/auto_run_on_startup.h"
#include "platforms/front_window_exedir_getter.h"
#include "utils/qwidget_utils.h"
#include "utils/logging.h"

AppManager::AppManager(QObject* parent)
    : QObject(parent), settings_(loadSettings())
{
    hotkeyMgr_ = new HotkeyManager(this);
    hotkeyMgr_->setHotkey(settings_.hotkey);
    sti_ = new SystemTrayIcon(settings_, this);

    if (settings_.autoRunOnStartUp != isAutoRunOnStartUp())
        setAutoRunOnStartUp(settings_.autoRunOnStartUp);

    connect(sti_, &SystemTrayIcon::languageChanged, this, &AppManager::onLanguageChanged);
    connect(sti_, &SystemTrayIcon::currentExecutableChanged, this, &AppManager::onCurrentExecutableChanged);
    connect(sti_, &SystemTrayIcon::autoRunOnStartUpChanged, this, &AppManager::onAutoRunOnStartUpChanged);

    connect(sti_, &SystemTrayIcon::settingsDialogActionTriggered, this, &AppManager::showSettingsDialog);
    connect(sti_, &SystemTrayIcon::aboutDialogActionTriggered, this, &AppManager::showAboutDialog);
    connect(sti_, &SystemTrayIcon::openLogDirActionTriggered, this, &AppManager::openLogDirectory);

    connect(sti_, &SystemTrayIcon::exitAppActionTriggered, qApp, &QApplication::quit);

    connect(hotkeyMgr_, &HotkeyManager::hotkeyTriggered, this, &AppManager::onHotkeyTriggered);
}

void AppManager::showSettingsDialog()
{
    auto dlg = new SettingsDialog(settings_.parameter, settings_.hotkey);
    dlg->setAttribute(Qt::WA_DeleteOnClose);

    connect(dlg, &SettingsDialog::parameterChanged, this, [=](const QString& pararmeter)
    {
        settings_.parameter = pararmeter;
        saveSettings(settings_);
    });

    connect(dlg, &SettingsDialog::hotkeyChanged, this, [=](const gbhk::KeyCombination& hotkey)
    {
        hotkeyMgr_->setHotkey(hotkey);

        settings_.hotkey = hotkey;
        saveSettings(settings_);
    });

    showAndActivate(dlg);
}

void AppManager::showAboutDialog()
{
    auto dlg = new AboutDialog();
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    showAndActivate(dlg);
}

void AppManager::openLogDirectory()
{
    if (!QDesktopServices::openUrl(QUrl::fromLocalFile(APP_LOG_DIRPATH)))
    {
        QMessageBox msgBox(
            QMessageBox::Critical,
            EASYTR("Error"),
            EASYTR("Failed to open log directory")
        );
        msgBox.exec();
    }
}

void AppManager::onLanguageChanged(const QString& language)
{
    setLanguage(language);
    sti_->updateText();
    qApp->setApplicationDisplayName(EASYTR(APP_TITLE));

    settings_.language = language;
    saveSettings(settings_);
}

void AppManager::onCurrentExecutableChanged(int idx)
{
    settings_.currentExecutableIdx = idx;
    saveSettings(settings_);
}

void AppManager::onAutoRunOnStartUpChanged(bool enabled)
{
    setAutoRunOnStartUp(enabled);

    settings_.autoRunOnStartUp = enabled;
    saveSettings(settings_);
}

void AppManager::onHotkeyTriggered()
{
    if (settings_.currentExecutableIdx < 0 || settings_.currentExecutableIdx >= defaultExecutables().size())
    {
        debugOut(qWarning(), "[Hotkey Triggered] The current executable is not exist.");
        return;
    }
    auto executable = defaultExecutables().at(settings_.currentExecutableIdx).filepath;
    if (executable.isEmpty())
    {
        debugOut(qWarning(), "[Hotkey Triggered] The executable filepath is empty.");
        return;
    }

    auto args = QProcess::splitCommand(settings_.parameter);

    QString workDir;
    try { workDir = getFrontWindowExeDir(); }
    catch (const std::exception& e)
    {
        debugOut(qWarning(),
            "[Hotkey Triggered] Failed to get front window executable directory. Error message: %1.",
            e.what());
        return;
    }

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    QProcess process;
    process.setProgram(executable);
    process.setArguments(args);
    process.setWorkingDirectory(workDir);
    process.setProcessEnvironment(env);
    if (!process.startDetached())
    {
        debugOut(qWarning(),
            "[Hotkey Triggered] Failed to start the executable '%1' in directory '%2' with parameters '%3'.",
            executable, workDir, settings_.parameter);
    }

    debugOut(qDebug(),
        ">>> Start Program '%1' in directory '%2' with parameters '%3'.",
        executable, workDir, settings_.parameter);
}
