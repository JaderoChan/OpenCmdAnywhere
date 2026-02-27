#include "hotkey_manager.h"

#include "utils/logging.h"

HotkeyManager::HotkeyManager(QObject* parent)
    : QObject(parent), ghm_(gbhk::RegisterGlobalHotkeyManager::getInstance())
{
    int rc = ghm_.run();
    if (rc != gbhk::RC_SUCCESS)
        debugOut(qCritical(), "[Hotkey Manager] Failed to run the Global Hotkey Manager. Error message: %1.",
            gbhk::getReturnCodeMessage(rc).c_str());
}

HotkeyManager::~HotkeyManager()
{
    int rc = ghm_.stop();
    if (rc != gbhk::RC_SUCCESS)
        debugOut(qCritical(), "[Hotkey Manager] Failed to stop the Global Hotkey Manager. Error message: %1.",
            gbhk::getReturnCodeMessage(rc).c_str());
}

void HotkeyManager::setHotkey(const gbhk::KeyCombination& newHotkey)
{
    if (ghm_.isHotkeyRegistered(hotkey_))
    {
        if (newHotkey.isValid())
        {
            int rc = ghm_.replaceHotkey(hotkey_, newHotkey);
            if (rc != gbhk::RC_SUCCESS)
                debugOut(qCritical(), "[Hotkey Manager] Failed to replace hotkey from '%1' to '%2'. Error message: %3.",
                    hotkey_.toString().c_str(),
                    newHotkey.toString().c_str(),
                    gbhk::getReturnCodeMessage(rc).c_str());
        }
        else
        {
            int rc = ghm_.unregisterHotkey(hotkey_);
            if (rc != gbhk::RC_SUCCESS)
                debugOut(qCritical(), "[Hotkey Manager] Failed to unregister hotkey '%1'. Error message: %2.",
                    hotkey_.toString().c_str(),
                    gbhk::getReturnCodeMessage(rc).c_str());
        }
    }
    else
    {
        if (newHotkey.isValid())
        {
            int rc = ghm_.registerHotkey(newHotkey, [=]() { emit hotkeyTriggered(); });
            if (rc != gbhk::RC_SUCCESS)
                debugOut(qCritical(), "[Hotkey Manager] Failed to register hotkey '%1'. Error message: %2.",
                    newHotkey.toString().c_str(),
                    gbhk::getReturnCodeMessage(rc).c_str());
        }
    }
}
