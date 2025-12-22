#include "executable_item_dialog.h"

#include <qdir.h>
#include <qfiledialog.h>
#include <qmessagebox.h>

#include <easy_translate.hpp>

#include "config.h"
#include "settings.h"

ExecutableItemDialog::ExecutableItemDialog(QWidget* parent) :
    QDialog(parent)
{
    ui.setupUi(this);

    connect(ui.selectFileBtn, &QPushButton::clicked, this, &ExecutableItemDialog::onSelectFileBtnClicked);
    connect(ui.confirmBtn, &QPushButton::clicked, this, &ExecutableItemDialog::onConfirmBtnClicked);
    connect(ui.cancelBtn, &QPushButton::clicked, this, &ExecutableItemDialog::onCancelBtnClicked);

    updateText();
}

ExecutableItemDialog::ExecutableItemDialog(const std::pair<QString, QString>& defaultValue, QWidget* parent) :
    ExecutableItemDialog(parent)
{
    defaultValue_ = defaultValue;
    ui.displayNameEdit->setText(defaultValue.first);
    ui.executableFileEdit->setText(defaultValue.second);
}

std::pair<QString, QString> ExecutableItemDialog::data()
{
    return {ui.displayNameEdit->text(), ui.executableFileEdit->text()};
}

void ExecutableItemDialog::updateText()
{
    setWindowTitle(EASYTR("Edit Executable Item"));
    ui.displayNameLbl->setText(EASYTR("Display Name"));
    ui.executableFileLbl->setText(EASYTR("Executable Filename"));
    ui.displayNameEdit->setPlaceholderText(EASYTR("Input the display name"));
    ui.executableFileEdit->setPlaceholderText(EASYTR("Input the executable filename"));
    ui.selectFileBtn->setText(EASYTR("Select File"));
    ui.confirmBtn->setText(EASYTR("Confirm"));
    ui.cancelBtn->setText(EASYTR("Cancel"));
}

void ExecutableItemDialog::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::LanguageChange)
        updateText();
    QDialog::changeEvent(event);
}

void ExecutableItemDialog::onSelectFileBtnClicked()
{
    QString filename = QFileDialog::getOpenFileName(
        this,
        EASYTR("Select a executable file"),
        QDir::rootPath(),
        QString(EASYTR("Executable File")) + " (*.exe)"
    );
    if (!filename.isEmpty())
        ui.executableFileEdit->setText(filename);
}

void ExecutableItemDialog::onConfirmBtnClicked()
{
    if (ui.displayNameEdit->text().isEmpty() || ui.executableFileEdit->text().isEmpty())
    {
        QMessageBox msgBox(
            QMessageBox::Warning,
            EASYTR("Warning"),
            EASYTR("Please input the valid data"),
            QMessageBox::NoButton,
            this
        );
        msgBox.exec();
    }
    else
    {
        auto map = Settings::getAllExecutables();
        if (map.contains(ui.displayNameEdit->text()))
        {
            QMessageBox msgBox(
                QMessageBox::Warning,
                EASYTR("Warning"),
                EASYTR("The given display name is exists"),
                QMessageBox::NoButton,
                this
            );
            msgBox.exec();
        }
        else
        {
            // 如果数据未发生更改，则等效于Reject。
            if (defaultValue_ == data())
                reject();
            else
                accept();
        }
    }
}

void ExecutableItemDialog::onCancelBtnClicked()
{
    reject();
}
