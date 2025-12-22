#include "hotkey_handler.h"

#include <thread>

#include <minilog.hpp>

#include "settings.h"
#include "core.h"

HotkeyHandler::HotkeyHandler() :
    ghm_(gbhk::RegisterGlobalHotkeyManager::getInstance())
{
    int rc = ghm_.initialize();
    if (rc != gbhk::RC_SUCCESS)
        mlog::warning("Failed to initialize the Global Hotkey Manager, message: {}", gbhk::getReturnCodeMsg(rc));
}

HotkeyHandler::~HotkeyHandler()
{
    int rc = ghm_.uninitialize();
    if (rc != gbhk::RC_SUCCESS)
        mlog::warning("Failed to uninitialize the Global Hotkey Manager, message: {}", gbhk::getReturnCodeMsg(rc));
}

HotkeyHandler& HotkeyHandler::getInstance()
{
    static HotkeyHandler instance;
    return instance;
}

gbhk::KeyCombination HotkeyHandler::setHotkey(const gbhk::KeyCombination& kc, bool isAdmin)
{
    auto& instance = getInstance();
    auto& hotkey = isAdmin ? instance.hotkeyAsUserRun_ : instance.hotkeyAsAdminRun_;

    if (!kc.isValid())
    {
        mlog::info("The setHotkey() got a invalid hotkey parameter");
        if (hotkey.isValid())
        {
            mlog::info("Due to the original hotkey is valid and the setHotkey() got a invalid hotkey so remove the original hotkey");
            int rc = instance.ghm_.remove(hotkey);
            hotkey = {};
            if (rc != gbhk::RC_SUCCESS)
                mlog::warning("Failed to remove the hotkey, message: {}", gbhk::getReturnCodeMsg(rc));
        }
    }
    else
    {
        mlog::info("The setHotkey() got a valid hotkey parameter");
        if (hotkey.isValid())
        {
            mlog::info("Due to the original hotkey is valid and setHotkey() got a valid hotkey so replace the original hotkey to the new hotkey");
            int rc = instance.ghm_.replace(hotkey, kc);
            if (rc != gbhk::RC_SUCCESS)
                hotkey = {};
            else
                hotkey = kc;
            if (rc != gbhk::RC_SUCCESS)
                mlog::warning("Failed to replace the hotkey, message: {}", gbhk::getReturnCodeMsg(rc));
        }
        else
        {
            mlog::info("Due to the original hotkey is invalid and setHotkey() got a valid hotkey so add the new hotkey");
            int rc = instance.ghm_.add(kc, [=]() { hotkeyTriggered(isAdmin); });
            if (rc == gbhk::RC_SUCCESS)
                hotkey = kc;
            if (rc != gbhk::RC_SUCCESS)
                mlog::warning("Failed to add the hotkey, message: {}", gbhk::getReturnCodeMsg(rc));
        }
    }

    return hotkey;
}

void HotkeyHandler::hotkeyTriggered(bool isAdmin)
{
    std::thread th([=]()
    {
        auto executable = Settings::getCurrentExecutable().second.toStdWString();
        auto parameter = Settings::getParameter().toStdWString();
        if (executable.empty())
        {
            mlog::info("The executable filename is empty");
            return;
        }

        try
        {
            auto path = getFocusedWindowDirectory();
            if (!runExecutable(executable, path, parameter, isAdmin))
                throw std::runtime_error("Failed to run the executable");
        } catch (std::exception& e)
        {
            mlog::warning("Error occurred when run the getFocusedWindowDirectory() and runExecutable(), exception: {}", e.what());
        }
    });
    th.detach();
}
