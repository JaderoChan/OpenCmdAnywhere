#include "hotkey_handler.h"

#include <thread>

#include <qdir.h>

#include "settings.h"
#include "platforms/front_window_exedir_getter.h"
#include "platforms/run_executable.h"
#include "utils/logging.h"

HotkeyHandler::HotkeyHandler()
#if defined(Q_OS_WIN) || defined(Q_OS_MAC)
    : ghm_(gbhk::RegisterGlobalHotkeyManager::getInstance())
#else
    : ghm_(gbhk::HookGlobalHotkeyManager::getInstance())
#endif
{
    int rc = ghm_.run();
    if (rc != gbhk::RC_SUCCESS)
        debugOut(qCritical(), "[Hotkey Manager] Failed to run the Global Hotkey Manager. Error message: %1.",
            gbhk::getReturnCodeMessage(rc).c_str());
}

HotkeyHandler::~HotkeyHandler()
{
    int rc = ghm_.stop();
    if (rc != gbhk::RC_SUCCESS)
        debugOut(qCritical(), "[Hotkey Manager] Failed to stop the Global Hotkey Manager. Error message: %1.",
            gbhk::getReturnCodeMessage(rc).c_str());
}

HotkeyHandler& HotkeyHandler::getInstance()
{
    static HotkeyHandler instance;
    return instance;
}

void HotkeyHandler::setHotkey(const gbhk::KeyCombination& newHotkey)
{
    auto& instance = getInstance();
    auto& ghm = instance.ghm_;
    auto& oldHotkey = getInstance().hotkey_;

    if (ghm.isHotkeyRegistered(oldHotkey))
    {
        if (newHotkey.isValid())
        {
            int rc = ghm.replaceHotkey(oldHotkey, newHotkey);
            if (rc != gbhk::RC_SUCCESS)
                debugOut(qCritical(), "[Hotkey Manager] Failed to replace hotkey from '%1' to '%2'. Error message: %3.",
                    oldHotkey.toString().c_str(),
                    newHotkey.toString().c_str(),
                    gbhk::getReturnCodeMessage(rc).c_str());
        }
        else
        {
            int rc = ghm.unregisterHotkey(oldHotkey);
            if (rc != gbhk::RC_SUCCESS)
                debugOut(qCritical(), "[Hotkey Manager] Failed to unregister hotkey '%1'. Error message: %2.",
                    oldHotkey.toString().c_str(),
                    gbhk::getReturnCodeMessage(rc).c_str());
        }
    }
    else
    {
        if (newHotkey.isValid())
        {
            int rc = ghm.registerHotkey(newHotkey, [=]() { hotkeyTriggered(); });
            if (rc != gbhk::RC_SUCCESS)
                debugOut(qCritical(), "[Hotkey Manager] Failed to register hotkey '%1'. Error message: %2.",
                    newHotkey.toString().c_str(),
                    gbhk::getReturnCodeMessage(rc).c_str());
        }
    }
}

void HotkeyHandler::hotkeyTriggered()
{
    std::thread th([=]()
    {
        auto executable = QDir::toNativeSeparators(Settings::getCurrentExecutable().second);
        auto parameter = Settings::getParameter();
        if (executable.isEmpty())
        {
            debugOut(qWarning(), "[Hotkey Triggered] The executable filepath is empty.");
            return;
        }

        QString workDir;
        try { workDir = getFrontWindowExeDir(); }
        catch (const std::exception& e)
        {
            debugOut(qWarning(),
                "[Hotkey Triggered] Failed to get front window executable directory. Error message: %1.",
                e.what());
            return;
        }

        if (!runExecutable(executable, workDir, parameter))
        {
            debugOut(qWarning(),
                "[Hotkey Triggered] Failed to run the executable '%1' in directory '%2' with parameters '%3'.",
                executable, workDir, parameter);
        }
    });
    th.detach();
}
