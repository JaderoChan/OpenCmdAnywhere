#include "systemtray.h"

#include <thread>

#include <qapplication.h>

#include <easy_translate.hpp>

#include "config.h"
#include "language.h"
#include "settings.h"
#include "file_icon_getter.h"
#include "platform/auto_run_on_startup.h"

#include "about_dialog.h"
#include "settings_dialog.h"

SystemTray::SystemTray(QObject* parent) :
    QSystemTrayIcon(QIcon(":/icons/app.ico"), parent),
    menu_(new QMenu())
{
    setContextMenu(menu_);

    setupLanguageMenu_();
    menu_->addMenu(languageMenu_);
    menu_->addSeparator();

    setupExecutableMenu_();
    menu_->addMenu(executableMenu_);
    menu_->addSeparator();

    autoRunOnStartUp_ = new QAction(menu_);
    autoRunOnStartUp_->setCheckable(true);
    autoRunOnStartUp_->setChecked(Settings::getIsAutoRunOnStartUp());
    menu_->addAction(autoRunOnStartUp_);
    menu_->addSeparator();

    setting_ = new QAction(menu_);
    menu_->addAction(setting_);

    about_ = new QAction(menu_);
    menu_->addAction(about_);
    menu_->addSeparator();

    exitApp_ = new QAction(menu_);
    menu_->addAction(exitApp_);

    connect(this, &QSystemTrayIcon::activated, this, &SystemTray::onActivated);
    connect(autoRunOnStartUp_, &QAction::triggered, this, &SystemTray::onAutoRunOnStartupTriggered);
    connect(setting_, &QAction::triggered, this, &SystemTray::onSettingTriggered);
    connect(about_, &QAction::triggered, this, &SystemTray::onAboutTriggered);
    connect(exitApp_, &QAction::triggered, this, &SystemTray::onExitAppTriggered);

    show();
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
    autoRunOnStartUp_->setText(EASYTR("Run on Startup"));
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

void SystemTray::onAutoRunOnStartupTriggered()
{
    bool autoRunOnStartUp = autoRunOnStartUp_->isChecked();
    bool ret = setAutoRunOnStartUp(autoRunOnStartUp);
    if (!ret)
        qDebug() << "Failed to set to run on startup";
    Settings::setAutoRunOnStartUp(autoRunOnStartUp);
}

void SystemTray::onSettingTriggered()
{
    SettingsDialog dlg = SettingsDialog();
    connect(&dlg, &SettingsDialog::executablesChanged, this, &SystemTray::updateExecutableMenu);
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
        auto filepath = it.value().toString();

        auto action = new QAction(displayName, menu_);
        action->setToolTip(filepath);
        action->setCheckable(true);
        if (displayName == currentExe.first)
        {
            action->setChecked(true);
            setExecutableMenuIcon_(filepath);
        }
        executableGroup_->addAction(action);
        executableMenu_->addAction(action);

        connect(action, &QAction::triggered, this, [=]()
        {
            if (displayName != Settings::getCurrentExecutable().first)
            {
                Settings::setCurrentExecutable(displayName);
                setExecutableMenuIcon_(filepath);
            }
        });
    }
}

void SystemTray::setupLanguageMenu_()
{
    languageMenu_ = new QMenu(menu_);
    languageMenu_->setIcon(QIcon(":/icons/language.ico"));

    auto languageGroup = new QActionGroup(menu_);
    languageGroup->setExclusive(true);

    auto currentLang = Settings::getLanguage();
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
        QIcon icon = getFileIcon(exePath);
        executableMenu_->setIcon(icon);
        if (icon.isNull())
            qDebug() << "Failed to get the executable icon, the executable path is:" << exePath;
    });
    th.detach();
}

void SystemTray::setExecutableMenuIcon_(const QIcon& icon)
{
    executableMenu_->setIcon(icon);
}
