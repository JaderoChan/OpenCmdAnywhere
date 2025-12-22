#include <qapplication.h>
#include <qdir.h>
#include <qlockfile.h>

#include <easy_translate.hpp>
#include <minilog.hpp>

#include "config.h"
#include "hotkey_handler.h"
#include "language.h"
#include "settings.h"
#include "systemtray.h"

int main(int argc, char* argv[])
{
    QLockFile lock(QDir::current().absoluteFilePath(APP_LOCK_FILENAME));
    if (lock.isLocked() || !lock.tryLock(500))
        return 0;

#ifdef OCAW_OUTLOG
    mlog::addOs("Deafult", std::clog);
#endif // OCAW_OUTLOG

    QApplication a(argc, argv);
    a.setOrganizationName(APP_ORGANIZATION);
    a.setApplicationName(APP_TITLE);
    a.setQuitOnLastWindowClosed(false);

    auto langId = Settings::getLangugae();
    langId = setLanguage(langId);
    Settings::setLanguage(langId);

    auto runAsUserKc = Settings::getKeyCombination(false);
    auto runAsAdminKc = Settings::getKeyCombination(true);
    runAsUserKc = HotkeyHandler::setHotkey(runAsUserKc, false);
    runAsAdminKc = HotkeyHandler::setHotkey(runAsAdminKc, true);
    Settings::setKeyCombination(runAsUserKc, false);
    Settings::setKeyCombination(runAsAdminKc, true);

    SystemTray st;
    st.show();
    a.installEventFilter(&st);

    int ret = a.exec();

    easytr::updateTranslationsFiles();

    return ret;
}
