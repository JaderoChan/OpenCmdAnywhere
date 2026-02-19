#include "hotkey_handler.h"

#include <thread>

#include <qdir.h>

#include "settings.h"
#include "platform/front_window_exedir_getter.h"
#include "platform/run_executable.h"

HotkeyHandler::HotkeyHandler() :
#ifdef Q_OS_WIN
    ghm_(gbhk::RegisterGlobalHotkeyManager::getInstance())
#elif defined(Q_OS_MAC)
    ghm_(gbhk::HookGlobalHotkeyManager::getInstance())
#else
    ghm_(gbhk::HookGlobalHotkeyManager::getInstance())
#endif
{
    int rc = ghm_.run();
    if (rc != gbhk::RC_SUCCESS)
        qDebug() << "Failed to run the Global Hotkey Manager, message:" << gbhk::getReturnCodeMessage(rc).c_str();
}

HotkeyHandler::~HotkeyHandler()
{
    int rc = ghm_.stop();
    if (rc != gbhk::RC_SUCCESS)
        qDebug() << "Failed to stop the Global Hotkey Manager, message:" << gbhk::getReturnCodeMessage(rc).c_str();
}

HotkeyHandler& HotkeyHandler::getInstance()
{
    static HotkeyHandler instance;
    return instance;
}

gbhk::KeyCombination HotkeyHandler::setHotkey(const gbhk::KeyCombination& kc)
{
    auto& instance = getInstance();
    auto& hotkey = getInstance().hotkey_;

    if (!kc.isValid())
    {
        qDebug() << "The setHotkey() got a invalid hotkey parameter";
        if (hotkey.isValid())
        {
            qDebug() << "Due to the original hotkey is valid and the setHotkey() got a invalid hotkey so unregister the original hotkey";
            int rc = instance.ghm_.unregisterHotkey(hotkey);
            hotkey = {};
            if (rc != gbhk::RC_SUCCESS)
                qDebug() << "Failed to unregister the hotkey, message:" << gbhk::getReturnCodeMessage(rc).c_str();
        }
    }
    else
    {
        qDebug() << "The setHotkey() got a valid hotkey parameter";
        if (hotkey.isValid())
        {
            qDebug() << "Due to the original hotkey is valid and setHotkey() got a valid hotkey so replace the original hotkey to the new hotkey";
            int rc = instance.ghm_.replaceHotkey(hotkey, kc);
            if (rc != gbhk::RC_SUCCESS)
                hotkey = {};
            else
                hotkey = kc;
            if (rc != gbhk::RC_SUCCESS)
                qDebug() << "Failed to replace the hotkey, message:" << gbhk::getReturnCodeMessage(rc).c_str();
        }
        else
        {
            qDebug() << "Due to the original hotkey is invalid and setHotkey() got a valid hotkey so register the new hotkey";
            int rc = instance.ghm_.registerHotkey(kc, [=]() { hotkeyTriggered(); });
            if (rc == gbhk::RC_SUCCESS)
                hotkey = kc;
            if (rc != gbhk::RC_SUCCESS)
                qDebug() << "Failed to register the hotkey, message:" << gbhk::getReturnCodeMessage(rc).c_str();
        }
    }

    return hotkey;
}

void HotkeyHandler::hotkeyTriggered()
{
    std::thread th([=]()
    {
        auto executable = QDir::toNativeSeparators(Settings::getCurrentExecutable().second);
        auto parameter = Settings::getParameter();
        if (executable.isEmpty())
        {
            qDebug() << "The executable filepath is empty";
            return;
        }

        try
        {
            auto workDir = getFrontWindowExeDir();
            if (!runExecutable(executable, workDir, parameter))
                throw std::runtime_error("Failed to run the executable");
        }
        catch (std::exception& e)
        {
            qDebug() << "Error occurred when run the getFrontWindowExeDir() and runExecutable(), exception:" << e.what();
        }
    });
    th.detach();
}
