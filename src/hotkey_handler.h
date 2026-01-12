#pragma once

#include <global_hotkey/global_hotkey.hpp>

// Singleton
class HotkeyHandler
{
public:
    static HotkeyHandler& getInstance();

    // 如果指定了一个无效KeyCombination则删除此热键。返回设置的热键，如果设置失败或指定了无效KeyCombination则返回无效KeyCombination。
    static gbhk::KeyCombination setHotkey(const gbhk::KeyCombination& kc, bool isAdmin);

protected:
    static void hotkeyTriggered(bool isAdmin);

private:
    HotkeyHandler();
    ~HotkeyHandler();
    HotkeyHandler(const HotkeyHandler&) = delete;
    HotkeyHandler& operator=(const HotkeyHandler&) = delete;

    gbhk::GlobalHotkeyManager& ghm_;
    gbhk::KeyCombination hotkeyAsUserRun_;
    gbhk::KeyCombination hotkeyAsAdminRun_;
};
