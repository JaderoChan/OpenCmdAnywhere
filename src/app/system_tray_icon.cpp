#include "system_tray_icon.h"

#include <easy_translate.hpp>

#include "config.h"
#include "logo_icon.h"
#include "utils/file_icon_getter.h"
#include "utils/logging.h"

SystemTrayIcon::SystemTrayIcon(const Settings& settings, QObject* parent)
    : QSystemTrayIcon(QIcon(getLogoIcon()), parent), settings_(settings)
{
    setupLanguagesMenu_();
    setupExecutablesMenu_();

    menu_.addMenu(&languagesMenu_);
    menu_.addSeparator();
    menu_.addMenu(&executablesMenu_);
    menu_.addSeparator();

    autoRunOnStartUpAction_.setCheckable(true);
    autoRunOnStartUpAction_.setChecked(settings_.autoRunOnStartUp);

    menu_.addAction(&autoRunOnStartUpAction_);
    menu_.addSeparator();
    menu_.addAction(&settingsDialogAction_);
    menu_.addAction(&aboutDialogAction_);
    menu_.addAction(&openLogDirAction_);
    menu_.addAction(&exitAppAction_);

    setContextMenu(&menu_);

    connect(this, &QSystemTrayIcon::activated, this, &SystemTrayIcon::onActivated);
    connect(&autoRunOnStartUpAction_, &QAction::triggered, this, [=](bool checked)
    { emit autoRunOnStartUpChanged(checked); });
    connect(&settingsDialogAction_, &QAction::triggered, this, [=]()
    { emit settingsDialogActionTriggered(); });
    connect(&aboutDialogAction_, &QAction::triggered, this, [=]()
    { emit aboutDialogActionTriggered(); });
    connect(&openLogDirAction_, &QAction::triggered, this, [=]()
    { emit openLogDirActionTriggered(); });
    connect(&exitAppAction_, &QAction::triggered, this, [=]()
    { emit exitAppActionTriggered(); });

    show(); // 先进行显示再更新文本可以防止 Tool Tip 设置失败。
    updateText();
}

void SystemTrayIcon::updateText()
{
    setToolTip(EASYTR(APP_TITLE));

    languagesMenu_.setTitle(EASYTR("Language"));
    executablesMenu_.setTitle(EASYTR("Run With"));
    for (int i = 0; i < languagesMenu_.actions().size(); ++i)
        languagesMenu_.actions()[i]->setText(EASYTR(easytr::languages().getIds()[i]));

    autoRunOnStartUpAction_.setText(EASYTR("Auto Run On Start Up"));
    settingsDialogAction_.setText(EASYTR("Settings"));
    aboutDialogAction_.setText(EASYTR("About"));
    openLogDirAction_.setText(EASYTR("Open Log Directory"));
    exitAppAction_.setText(EASYTR("Exit App"));
}

void SystemTrayIcon::onActivated(ActivationReason reason)
{
#ifndef Q_OS_MAC
    switch (reason)
    {
        case Trigger:   // Fallthrough
        case Context:
            contextMenu()->popup(QCursor::pos());
            break;
        default:
            break;
    }
#endif // !Q_OS_MAC
}

void SystemTrayIcon::setupLanguagesMenu_()
{
    languagesMenu_.setIcon(QIcon(":/icons/language.png"));

    auto languagesGroup = new QActionGroup(&menu_);
    languagesGroup->setExclusive(true);

    for (const auto& langId : easytr::languages().getIds())
    {
        auto action = new QAction(&menu_);

        action->setCheckable(true);
        const auto qLangId = QString::fromStdString(langId);
        if (qLangId == settings_.language)
            action->setChecked(true);

        languagesGroup->addAction(action);
        languagesMenu_.addAction(action);

        connect(action, &QAction::triggered, this, [=]() { emit languageChanged(qLangId); });
    }
}

void SystemTrayIcon::setupExecutablesMenu_()
{
    auto executablesGroup = new QActionGroup(&menu_);
    executablesGroup->setExclusive(true);

    int idx = 0;
    for (const auto& executableItem : defaultExecutables())
    {
        auto action = new QAction(executableItem.name, &menu_);

        action->setToolTip(executableItem.filepath);
        action->setCheckable(true);
        if (idx == settings_.currentExecutableIdx)
        {
            action->setChecked(true);
            setExecutablesMenuIcon_(executableItem.filepath);
        }

        executablesGroup->addAction(action);
        executablesMenu_.addAction(action);

        connect(action, &QAction::triggered, this, [=]()
        {
            setExecutablesMenuIcon_(executableItem.filepath);
            emit currentExecutableChanged(idx);
        });

        ++idx;
    }
}

void SystemTrayIcon::setExecutablesMenuIcon_(const QString& exePath)
{
    QIcon icon = getFileIcon(exePath);
    executablesMenu_.setIcon(icon); // 即使获取图标失败，也仍然设置图标，可示置空。
    if (icon.isNull())
        debugOut(qWarning(),
            "[File Icon Getter] Failed to get the executable icon, the executable path is: %1.",
            exePath);
}
