#pragma once

#include <qaction.h>
#include <qactiongroup.h>
#include <qmenu.h>
#include <qsystemtrayicon.h>

#include "settings.h"

class SystemTrayIcon : public QSystemTrayIcon
{
    Q_OBJECT

public:
    explicit SystemTrayIcon(const Settings& settings, QObject* parent);

    void updateText();

signals:
    void languageChanged(QString language);
    void currentExecutableChanged(int idx);
    void autoRunOnStartUpChanged(bool enabled);

    void settingsDialogActionTriggered();
    void aboutDialogActionTriggered();
    void openLogDirActionTriggered();

    void exitAppActionTriggered();

protected:
    void onActivated(ActivationReason reason);

private:
    void setupLanguagesMenu_();
    void setupExecutablesMenu_();
    void setExecutablesMenuIcon_(const QString& exeFilepath);

    QMenu menu_;
    QMenu languagesMenu_;
    QMenu executablesMenu_;

    QAction autoRunOnStartUpAction_;
    QAction settingsDialogAction_;
    QAction aboutDialogAction_;
    QAction openLogDirAction_;
    QAction exitAppAction_;

    Settings settings_;
};
