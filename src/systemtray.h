#pragma once

#include <qaction.h>
#include <qactiongroup.h>
#include <qevent.h>
#include <qmenu.h>
#include <qsystemtrayicon.h>

class SystemTray : public QSystemTrayIcon
{
    Q_OBJECT

public:
    explicit SystemTray(QObject* parent = nullptr);
    ~SystemTray();

protected:
    virtual void updateText();
    bool eventFilter(QObject* obj, QEvent* event) override;

protected:
    void onActivated(ActivationReason reason);
    void onRunOnStartupTriggered();
    void onSettingTriggered();
    void onAboutTriggered();
    void onExitAppTriggered();

    void updateExecutableMenu();

private:
    void setupLanguageMenu_();
    void setupExecutableMenu_();
    void setExecutableMenuIcon_(const QString& exePath);
    void setExecutableMenuIcon_(const QIcon& icon);

    QMenu* menu_ = nullptr;
    QMenu* languageMenu_ = nullptr;
    QMenu* executableMenu_ = nullptr;
    QActionGroup* executableGroup_ = nullptr;
    QAction* runOnStartup_ = nullptr;
    QAction* setting_ = nullptr;
    QAction* about_ = nullptr;
    QAction* exitApp_ = nullptr;
};
