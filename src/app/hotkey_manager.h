#pragma once

#include <qobject.h>

#include <global_hotkey/global_hotkey.hpp>

class HotkeyManager : public QObject
{
    Q_OBJECT

public:
    explicit HotkeyManager(QObject* parent = nullptr);
    ~HotkeyManager();

    void setHotkey(const gbhk::KeyCombination& hotkey);

signals:
    void hotkeyTriggered();

private:
    gbhk::GlobalHotkeyManager& ghm_;
    gbhk::KeyCombination hotkey_;
};
