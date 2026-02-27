#include "require_permission_dialog.h"

#include <easy_translate.hpp>

#include "platforms/permission_manager.h"

RequirePermissionDialog::RequirePermissionDialog(QWidget* parent)
    : QDialog(parent)
{
    ui.setupUi(this);

    checkPermissionTimer_.start(50);

    connect(&checkPermissionTimer_, &QTimer::timeout, this, &RequirePermissionDialog::onTimeOut);
    connect(ui.gotoGrantPermissionBtn, &QPushButton::clicked,
        this, &RequirePermissionDialog::onGotoGrantPermissionBtnClicked);
    connect(ui.exitBtn, &QPushButton::clicked, this, &RequirePermissionDialog::reject);
    connect(ui.forceContinueBtn, &QPushButton::clicked,
        this, &RequirePermissionDialog::onForceContinueBtnClicked);

    updateText();
}

void RequirePermissionDialog::accept()
{
    done(RequirePermissionDialog::GotPermission);
}

void RequirePermissionDialog::reject()
{
    done(RequirePermissionDialog::Exit);
}

void RequirePermissionDialog::updateText()
{
    setWindowTitle(EASYTR("Warning"));
#ifdef Q_OS_MAC
    ui.messageTextLbl->setText(EASYTR("Require Permission Text On Mac"));
#else
    ui.messageTextLbl->setText(EASYTR("Require Permission Text"));
#endif // Q_OS_MAC
    ui.gotoGrantPermissionBtn->setText(EASYTR("Goto Grant Permission"));
    ui.exitBtn->setText(EASYTR("Exit"));
    ui.forceContinueBtn->setText(EASYTR("Force Continue"));
    ui.forceContinueBtn->setToolTip(EASYTR("Force Continue ToolTip"));
}

void RequirePermissionDialog::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::LanguageChange)
        updateText();
    QDialog::changeEvent(event);
}

void RequirePermissionDialog::onTimeOut()
{
    if (PermissionManager::hasPermission())
        accept();
}

void RequirePermissionDialog::onGotoGrantPermissionBtnClicked()
{
    PermissionManager::requestPermission();
}

void RequirePermissionDialog::onForceContinueBtnClicked()
{
    done(RequirePermissionDialog::ForceContinue);
}
