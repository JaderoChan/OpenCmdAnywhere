#pragma once

#include <global_hotkey/global_hotkey.hpp>

// Singleton
class HotkeyHandler
{
public:
    static HotkeyHandler& getInstance();

    static void setHotkey(const gbhk::KeyCombination& newHotkey);

protected:
    static void hotkeyTriggered();

private:
    HotkeyHandler();
    ~HotkeyHandler();
    HotkeyHandler(const HotkeyHandler&) = delete;
    HotkeyHandler& operator=(const HotkeyHandler&) = delete;

    gbhk::GlobalHotkeyManager& ghm_;
    gbhk::KeyCombination hotkey_;
};
