#pragma once

#include <qobject.h>

#include "about_dialog.h"
#include "hotkey_manager.h"
#include "settings.h"
#include "settings_dialog.h"
#include "system_tray_icon.h"

class AppManager : public QObject
{
    Q_OBJECT

public:
    explicit AppManager(QObject* parent = nullptr);

protected:
    void showSettingsDialog();
    void showAboutDialog();
    void openLogDirectory();

    void onLanguageChanged(const QString& language);
    void onCurrentExecutableChanged(int idx);
    void onAutoRunOnStartUpChanged(bool enabled);

    void onHotkeyTriggered();

private:
    Settings settings_;
    HotkeyManager* hotkeyMgr_ = nullptr;
    SystemTrayIcon* sti_ = nullptr;
};
