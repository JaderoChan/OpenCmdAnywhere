#pragma once

#include <qdialog.h>
#include <qevent.h>

#include "ui_executable_item_dialog.h"

class ExecutableItemDialog : public QDialog
{
public:
    explicit ExecutableItemDialog(QWidget* parent = nullptr);
    explicit ExecutableItemDialog(const std::pair<QString, QString>& defaultValue, QWidget* parent = nullptr);

    std::pair<QString, QString> data();

protected:
    virtual void updateText();
    void changeEvent(QEvent* event) override;

    void onSelectFileBtnClicked();
    void onConfirmBtnClicked();
    void onCancelBtnClicked();

private:
    Ui::ExecutableItemDialog ui;
    std::pair<QString, QString> defaultValue_;
};
