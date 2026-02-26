#include <qapplication.h>
#include <qdir.h>
#include <qlockfile.h>

#include <easy_translate.hpp>

#include "config.h"
#include "file_logger.h"
#include "hotkey_handler.h"
#include "language.h"
#include "require_permission_dialog.h"
#include "settings.h"
#include "systemtray.h"
#include "logo_icon.h"
#include "platforms/permission_manager.h"
#include "platforms/auto_run_on_startup.h"
#include "utils/logging.h"

int main(int argc, char* argv[])
{
    QLockFile lock(QDir::temp().absoluteFilePath(APP_LOCK_FILEPATH));
    if (lock.isLocked() || !lock.tryLock(200))
    {
        debugOut(qCritical(), "[Start] Another instance is already running.");
        return 1;
    }

    // 设置程序全局属性
    QApplication a(argc, argv);
    a.setOrganizationDomain(APP_ORGANIZATION_DOMAIN);
    a.setOrganizationName(APP_ORGANIZATION);
    a.setApplicationName(APP_TITLE);
    a.setApplicationVersion(APP_VERSION);
    a.setWindowIcon(getLogoIcon());
    a.setQuitOnLastWindowClosed(false);

    FileLogger& fileLogger = FileLogger::getInstance();
    if (!fileLogger.setup(APP_LOG_FILEPATH))
        debugOut(qWarning(), "[Start] Failed to setup file logger.");

    // 设置语言
    setLanguage(Settings::getLanguage());

    // 检查应用权限
    if (!PermissionManager::hasPermission())
    {
        debugOut(qInfo(), "[Start] No permission, try request.");

        RequirePermissionDialog dlg;
        int ret = dlg.exec();
        switch (ret)
        {
            case RequirePermissionDialog::GotPermission:
            case RequirePermissionDialog::ForceContinue:
                break;
            case RequirePermissionDialog::Exit:
                return 0;
            default:
                break;
        }
    }

    HotkeyHandler::setHotkey(Settings::getKeyCombination());

    if (Settings::getIsAutoRunOnStartUp() != isAutoRunOnStartUp())
        setAutoRunOnStartUp(Settings::getIsAutoRunOnStartUp());

    SystemTray st;
    a.installEventFilter(&st);

    int ret = a.exec();

    easytr::updateTranslationsFiles();

    return ret;
}
