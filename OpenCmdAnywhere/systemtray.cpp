#include "systemtray.h"

#include <qapplication.h>

#include <easy_translate.hpp>
#include <minilog.hpp>

#include "config.h"
#include "language.h"
#include "settings.h"
#include "utility.h"

#include "about_dialog.h"
#include "setting_dialog.h"

SystemTray::SystemTray(QObject* parent) :
    QSystemTrayIcon(QIcon(":/icon/icon.ico"), parent),
    menu_(new QMenu())
{
    setContextMenu(menu_);

    setupLanguageMenu_();
    menu_->addMenu(languageMenu_);
    menu_->addSeparator();

    setupExecutableMenu_();
    menu_->addMenu(executableMenu_);
    menu_->addSeparator();

    runOnStartup_ = new QAction(menu_);
    runOnStartup_->setCheckable(true);
    runOnStartup_->setChecked(isRunOnStartup());
    menu_->addAction(runOnStartup_);
    menu_->addSeparator();

    setting_ = new QAction(menu_);
    menu_->addAction(setting_);

    about_ = new QAction(menu_);
    menu_->addAction(about_);
    menu_->addSeparator();

    exitApp_ = new QAction(menu_);
    menu_->addAction(exitApp_);

    connect(this, &QSystemTrayIcon::activated, this, &SystemTray::onActivated);
    connect(runOnStartup_, &QAction::triggered, this, &SystemTray::onRunOnStartupTriggered);
    connect(setting_, &QAction::triggered, this, &SystemTray::onSettingTriggered);
    connect(about_, &QAction::triggered, this, &SystemTray::onAboutTriggered);
    connect(exitApp_, &QAction::triggered, this, &SystemTray::onExitAppTriggered);

    updateText();
}

SystemTray::~SystemTray()
{
    delete menu_;
    menu_ = nullptr;
}

void SystemTray::updateText()
{
    setToolTip(EASYTR(APP_TITLE));
    languageMenu_->setTitle(EASYTR("Language"));
    executableMenu_->setTitle(EASYTR("Run With"));
    for (int i = 0; i < languageMenu_->actions().size(); ++i)
        languageMenu_->actions()[i]->setText(EASYTR(easytr::languages().getIds()[i]));
    runOnStartup_->setText(EASYTR("Run on Startup"));
    setting_->setText(EASYTR("Setting"));
    about_->setText(EASYTR("About"));
    exitApp_->setText(EASYTR("Exit"));
}

bool SystemTray::eventFilter(QObject* obj, QEvent* event)
{
    Q_UNUSED(obj);
    if (event->type() == QEvent::LanguageChange)
        updateText();
    return QSystemTrayIcon::eventFilter(obj, event);
}

void SystemTray::onActivated(ActivationReason reason)
{
    switch (reason)
    {
        case Trigger:   // Fallthrough
        case Context:
            contextMenu()->popup(QCursor::pos());
            break;
        default:
            break;
    }
}

void SystemTray::onRunOnStartupTriggered()
{
    bool ret = setRunOnStartup(runOnStartup_->isChecked());
    if (!ret)
        mlog::warning("Failed to set to run on startup");
}

void SystemTray::onSettingTriggered()
{
    SettingDialog dlg = SettingDialog();
    connect(&dlg, &SettingDialog::executablesChanged, this, &SystemTray::updateExecutableMenu);
    dlg.exec();
}

void SystemTray::onAboutTriggered()
{
    AboutDialog dlg = AboutDialog();
    dlg.exec();
}

void SystemTray::onExitAppTriggered()
{
    qApp->quit();
}

void SystemTray::updateExecutableMenu()
{
    executableMenu_->clear();
    if (executableGroup_)
    {
        delete executableGroup_;
        executableGroup_ = nullptr;
    }

    executableGroup_ = new QActionGroup(menu_);
    executableGroup_->setExclusive(true);

    setExecutableMenuIcon_(QIcon());

    auto currentExe = Settings::getCurrentExecutable();
    auto exes = Settings::getAllExecutables();

    for (auto it = exes.begin(); it != exes.end(); ++it)
    {
        auto displayName = it.key();
        auto filename = it.value().toString();

        auto action = new QAction(displayName, menu_);
        action->setToolTip(filename);
        action->setCheckable(true);
        if (displayName == currentExe.first)
        {
            action->setChecked(true);
            setExecutableMenuIcon_(filename);
        }
        executableGroup_->addAction(action);
        executableMenu_->addAction(action);

        connect(action, &QAction::triggered, this, [=]()
        {
            if (displayName != Settings::getCurrentExecutable().first)
            {
                Settings::setCurrentExecutable(displayName);
                setExecutableMenuIcon_(filename);
            }
        });
    }
}

void SystemTray::setupLanguageMenu_()
{
    languageMenu_ = new QMenu(menu_);
    languageMenu_->setIcon(QIcon(":/icon/language.ico"));

    auto languageGroup = new QActionGroup(menu_);
    languageGroup->setExclusive(true);

    auto currentLang = Settings::getLangugae();
    for (const auto& id : easytr::languages().getIds())
    {
        auto action = new QAction(menu_);
        action->setCheckable(true);
        QString qstrId = QString::fromStdString(id);
        if (qstrId == currentLang)
            action->setChecked(true);
        languageGroup->addAction(action);
        languageMenu_->addAction(action);

        connect(action, &QAction::triggered, this, [=]()
        {
            setLanguage(qstrId);
            Settings::setLanguage(qstrId);
        });
    }
}

void SystemTray::setupExecutableMenu_()
{
    executableMenu_ = new QMenu(menu_);
    updateExecutableMenu();
}

void SystemTray::setExecutableMenuIcon_(const QString& exePath)
{
    std::thread th([=]()
    {
        QIcon icon = getExecutableIcon(exePath);
        executableMenu_->setIcon(icon);
        if (icon.isNull())
            mlog::warning(
                "Failed to get the executable icon, the executable path is {}",
                exePath.toStdString()
            );
    });
    th.detach();
}

void SystemTray::setExecutableMenuIcon_(const QIcon& icon)
{
    executableMenu_->setIcon(icon);
}
