#include <qapplication.h>
#include <qdir.h>
#include <qlockfile.h>

#include <easy_translate.hpp>

#include "config.h"
#include "app_manager.h"
#include "file_logger.h"
#include "language.h"
#include "logo_icon.h"
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
    fileLogger.setFilepath(APP_LOG_FILEPATH);
    fileLogger.install();

    // 设置语言
    setLanguage(loadSettings().language);

    a.setApplicationDisplayName(EASYTR(APP_TITLE));

    AppManager appMgr;

    int ret = a.exec();

    easytr::updateTranslationsFiles();

    return ret;
}
