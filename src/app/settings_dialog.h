#pragma once

#include <qdialog.h>
#include <qevent.h>

#include <global_hotkey/key_combination.hpp>

#include "ui_settings_dialog.h"
#include "settings.h"

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    SettingsDialog(const QString& parameter, const gbhk::KeyCombination& hotkey, QWidget* parent = nullptr);

signals:
    void parameterChanged(QString parameter);
    void hotkeyChanged(gbhk::KeyCombination hotkey);

protected:
    virtual void updateText();
    void changeEvent(QEvent* event) override;

private:
    Ui::SettingsDialog ui;
};
