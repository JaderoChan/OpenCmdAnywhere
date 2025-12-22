#pragma once

#include <qdialog.h>
#include <qevent.h>

#include "ui_setting_dialog.h"

class SettingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingDialog(QWidget* parent = nullptr);

signals:
    void executablesChanged();

protected:
    virtual void updatetText();
    void changeEvent(QEvent* event) override;

    void updateExecutablesTable();

    void onParameterTextChanged();
    void onHotkeyChanged(QKeyCombination kc, bool isAdmin);
    void onAddExeBtnClicked();
    void onEditExeBtnClicked();
    void onRemoveExeBtnClicked();

private:
    // 获取当前选中的行索引，若未选中行则返回-1；
    int getSelectedRow_();

    Ui::SettingDialog ui;
};
