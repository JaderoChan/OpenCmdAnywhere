#include <qapplication.h>
#include <qdir.h>
#include <qlockfile.h>

#include <easy_translate.hpp>

#include "config.h"
#include "hotkey_handler.h"
#include "language.h"
#include "settings.h"
#include "systemtray.h"
#include "platform/auto_run_on_startup.h"

int main(int argc, char* argv[])
{
    QLockFile lock(QDir::temp().absoluteFilePath(APP_LOCK_FILEPATH));
    if (lock.isLocked() || !lock.tryLock(200))
        return 0;

    QApplication  a(argc, argv);
    a.setOrganizationDomain(APP_ORGANIZATION_DOMAIN);
    a.setOrganizationName(APP_ORGANIZATION);
    a.setApplicationName(APP_TITLE);
    a.setApplicationVersion(APP_VERSION);
    a.setWindowIcon(QIcon(":/icons/app.ico"));
    a.setQuitOnLastWindowClosed(false);

    auto langId = Settings::getLangugae();
    langId = setLanguage(langId);
    Settings::setLanguage(langId);

    auto kc = Settings::getKeyCombination();
    kc = HotkeyHandler::setHotkey(kc);
    Settings::setKeyCombination(kc);

    if (Settings::getIsAutoRunOnStartUp() != isAutoRunOnStartUp())
        setAutoRunOnStartUp(Settings::getIsAutoRunOnStartUp());

    SystemTray st;
    a.installEventFilter(&st);

    int ret = a.exec();

    easytr::updateTranslationsFiles();

    return ret;
}
