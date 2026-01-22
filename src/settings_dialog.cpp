#include "settings_dialog.h"

#include <qheaderview.h>

#include <easy_translate.hpp>

#include "config.h"
#include "hotkey_handler.h"
#include "settings.h"
#include "executable_item_dialog.h"

SettingsDialog::SettingsDialog(QWidget* parent) :
    QDialog(parent)
{
    ui.setupUi(this);

    ui.parameterEdit->setText(Settings::getParameter());
    auto hotkey = Settings::getKeyCombination();
    ui.hotkeyEdit->setKeyCombination(QKeySequence::fromString(hotkey.toString().c_str()));
    ui.executableTable->setColumnCount(2);
    ui.executableTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);

    connect(this, &SettingsDialog::executablesChanged, this, &SettingsDialog::updateExecutablesTable);
    connect(ui.parameterEdit, &QTextEdit::textChanged, this, &SettingsDialog::onParameterTextChanged);
    connect(ui.hotkeyEdit, &KeyCombinationInputer::keyCombinationChanged, this, [=](QKeyCombination kc)
    { onHotkeyChanged(kc); });
    connect(ui.addExeBtn, &QPushButton::clicked, this, &SettingsDialog::onAddExeBtnClicked);
    connect(ui.editExeBtn, &QPushButton::clicked, this, &SettingsDialog::onEditExeBtnClicked);
    connect(ui.removeExeBtn, &QPushButton::clicked, this, &SettingsDialog::onRemoveExeBtnClicked);

    updateExecutablesTable();
    updatetText();
}

void SettingsDialog::updatetText()
{
    setWindowTitle(EASYTR("Setting"));
    ui.parameterLbl->setText(EASYTR("Startup Parameter"));
    ui.parameterEdit->setPlaceholderText(EASYTR("No Parameter"));
    ui.hotkeyLbl->setText(EASYTR("Run Hotkey"));
    ui.hotkeyEdit->setToolTip(EASYTR("Keying the 'ESC' to cancel and keying the 'Delete' to remove hotkey"));
    ui.addExeBtn->setText(EASYTR("Add Executable"));
    ui.editExeBtn->setText(EASYTR("Edit Executable"));
    ui.removeExeBtn->setText(EASYTR("Remove Executable"));
    ui.executableTable->setHorizontalHeaderLabels({EASYTR("Display Name"), EASYTR("Executable Filename")});
}

void SettingsDialog::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::LanguageChange)
        updatetText();
    QDialog::changeEvent(event);
}

void SettingsDialog::updateExecutablesTable()
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

void SettingsDialog::onParameterTextChanged()
{
    Settings::setParameter(ui.parameterEdit->toPlainText());
}

void SettingsDialog::onHotkeyChanged(QKeyCombination kc)
{
    auto kcStr = QKeySequence(kc).toString();
    auto gbhkKc = gbhk::KeyCombination::fromString(kcStr.toStdString());
    // 尝试设置热键，并获取返回结果。
    gbhkKc = HotkeyHandler::setHotkey(gbhkKc);
    Settings::setKeyCombination(gbhkKc);
    kcStr = QString::fromStdString(gbhkKc.toString());
    auto ks = QKeySequence::fromString(kcStr);
    // 更新热键（如果热键设置失败，上面将会返回空热键，也就回退至未设置热键的情况）
    ui.hotkeyEdit->setKeyCombination(ks);
}

void SettingsDialog::onAddExeBtnClicked()
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

void SettingsDialog::onEditExeBtnClicked()
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

void SettingsDialog::onRemoveExeBtnClicked()
{
    int row = getSelectedRow_();
    if (row == -1)
        return;
    QString displayName = ui.executableTable->item(row, 0)->text();
    Settings::removeExecutable(displayName);
    emit executablesChanged();
}

int SettingsDialog::getSelectedRow_()
{
    auto items = ui.executableTable->selectedItems();
    if (items.isEmpty())
        return -1;
    return items.front()->row();
}
