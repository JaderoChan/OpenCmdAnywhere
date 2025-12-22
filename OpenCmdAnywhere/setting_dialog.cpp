#include "setting_dialog.h"

#include <qheaderview.h>

#include <easy_translate.hpp>

#include "config.h"
#include "hotkey_handler.h"
#include "settings.h"
#include "executable_item_dialog.h"

SettingDialog::SettingDialog(QWidget* parent) :
    QDialog(parent)
{
    ui.setupUi(this);

    ui.parameterEdit->setText(Settings::getParameter());
    auto runAsUserHotkey = Settings::getKeyCombination(false);
    ui.runAsUserHotkeyEdit->setKeyCombination(QKeySequence::fromString(runAsUserHotkey.toString().c_str()));
    auto runAsAdminHotkey = Settings::getKeyCombination(true);
    ui.runAsAdminHotkeyEdit->setKeyCombination(QKeySequence::fromString(runAsAdminHotkey.toString().c_str()));
    ui.executableTable->setColumnCount(2);
    ui.executableTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);

    connect(this, &SettingDialog::executablesChanged, this, &SettingDialog::updateExecutablesTable);
    connect(ui.parameterEdit, &QTextEdit::textChanged, this, &SettingDialog::onParameterTextChanged);
    connect(ui.runAsUserHotkeyEdit, &KeyCombinationInputer::inputFinished, this, [=](QKeyCombination kc)
    { onHotkeyChanged(kc, false); });
    connect(ui.runAsAdminHotkeyEdit, &KeyCombinationInputer::inputFinished, this, [=](QKeyCombination kc)
    { onHotkeyChanged(kc, true); });
    connect(ui.addExeBtn, &QPushButton::clicked, this, &SettingDialog::onAddExeBtnClicked);
    connect(ui.editExeBtn, &QPushButton::clicked, this, &SettingDialog::onEditExeBtnClicked);
    connect(ui.removeExeBtn, &QPushButton::clicked, this, &SettingDialog::onRemoveExeBtnClicked);

    updateExecutablesTable();
    updatetText();
}

void SettingDialog::updatetText()
{
    setWindowTitle(EASYTR("Setting"));
    ui.parameterLbl->setText(EASYTR("Startup Parameter"));
    ui.parameterEdit->setPlaceholderText(EASYTR("No Parameter"));
    ui.runAsUserHotkeyLbl->setText(EASYTR("Run As User Hotkey"));
    ui.runAsUserHotkeyEdit->setToolTip(EASYTR("Keying the 'ESC' to cancel and keying the 'Delete' to remove hotkey"));
    ui.runAsAdminHotkeyLbl->setText(EASYTR("Run As Admin Hotkey"));
    ui.runAsAdminHotkeyEdit->setToolTip(EASYTR("Keying the 'ESC' to cancel and keying the 'Delete' to remove hotkey"));
    ui.addExeBtn->setText(EASYTR("Add Executable"));
    ui.editExeBtn->setText(EASYTR("Edit Executable"));
    ui.removeExeBtn->setText(EASYTR("Remove Executable"));
    ui.executableTable->setHorizontalHeaderLabels({EASYTR("Display Name"), EASYTR("Executable Filename")});
}

void SettingDialog::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::LanguageChange)
        updatetText();
    QDialog::changeEvent(event);
}

void SettingDialog::updateExecutablesTable()
{
    ui.executableTable->clearContents();
    for (int i = ui.executableTable->rowCount(); i >=0; --i)
        ui.executableTable->removeRow(i);
    auto exes = Settings::getAllExecutables();
    for (auto it = exes.begin(); it != exes.end(); ++it)
    {
        ui.executableTable->insertRow(ui.executableTable->rowCount());
        auto displayNameItem = new QTableWidgetItem(it.key());
        ui.executableTable->setItem(ui.executableTable->rowCount() - 1, 0, displayNameItem);
        auto executableFilenameItem = new QTableWidgetItem(it.value().toString());
        ui.executableTable->setItem(ui.executableTable->rowCount() - 1, 1, executableFilenameItem);
    }
}

void SettingDialog::onParameterTextChanged()
{
    Settings::setParameter(ui.parameterEdit->toPlainText());
}

void SettingDialog::onHotkeyChanged(QKeyCombination kc, bool isAdmin)
{
    auto kcStr = QKeySequence(kc).toString();
    auto gbhkKc = gbhk::KeyCombination::fromString(kcStr.toStdString());
    // 尝试设置热键，并获取返回结果。
    gbhkKc = HotkeyHandler::setHotkey(gbhkKc, isAdmin);
    Settings::setKeyCombination(gbhkKc, isAdmin);
    kcStr = QString::fromStdString(gbhkKc.toString());
    auto ks = QKeySequence::fromString(kcStr);
    // 更新热键（如果热键设置失败，上面将会返回空热键，也就回退至未设置热键的情况）
    if (isAdmin)
        ui.runAsAdminHotkeyEdit->setKeyCombination(ks);
    else
        ui.runAsUserHotkeyEdit->setKeyCombination(ks);
}

void SettingDialog::onAddExeBtnClicked()
{
    ExecutableItemDialog dlg(this);
    int ret = dlg.exec();
    if (ret == QDialog::Accepted)
    {
        auto data = dlg.data();
        Settings::addExecutable(data.first, data.second);
        emit executablesChanged();
    }
}

void SettingDialog::onEditExeBtnClicked()
{
    int row = getSelectedRow_();
    if (row == -1)
        return;
    QString displayName = ui.executableTable->item(row, 0)->text();
    QString filename = ui.executableTable->item(row, 1)->text();

    ExecutableItemDialog dlg({displayName, filename}, this);
    int ret = dlg.exec();
    if (ret == QDialog::Accepted)
    {
        auto data = dlg.data();
        // 如果编辑的条目是当前活动条目，则更新其显示名称。
        if (Settings::getCurrentExecutable().first == displayName)
            Settings::setCurrentExecutable(data.first);
        Settings::removeExecutable(displayName);
        Settings::addExecutable(data.first, data.second);
        emit executablesChanged();
    }
}

void SettingDialog::onRemoveExeBtnClicked()
{
    int row = getSelectedRow_();
    if (row == -1)
        return;
    QString displayName = ui.executableTable->item(row, 0)->text();
    Settings::removeExecutable(displayName);
    emit executablesChanged();
}

int SettingDialog::getSelectedRow_()
{
    auto items = ui.executableTable->selectedItems();
    if (items.isEmpty())
        return -1;
    return items.front()->row();
}
