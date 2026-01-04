#include "language.h"

#include <qapplication.h>
#include <qevent.h>
#include <qstring>

#include <easy_translate.hpp>
#include <minilog.hpp>

#include "config.h"

QString setLanguage(const QString& langId)
{
    easytr::setLanguages(APP_LANG_FILENAME);
    if (easytr::languages().empty())
        mlog::info("Invalid Languages file");

    std::string id = langId.toStdString();
    if (easytr::hasLanguage(id))
    {
        if (easytr::setCurrentLanguage(id))
            mlog::info("Success to change the language to: {}", id.c_str());
        else
            mlog::warning("Failed to change the language to: {}", id.c_str());
    }
    // Fallthrough, fallback
    else
    {
        mlog::info("Expected language is miss, will use the fallback language");
        if (easytr::languages().empty())
        {
            mlog::warning("Not find any language");
            return easytr::currentLanguage();
        }
        else
        {
            id = easytr::languages().getIds().front();
            if (easytr::setCurrentLanguage(id))
                mlog::info("Success to use the fallback language: {}", id.c_str());
            else
                mlog::warning("Failed to use the fallback language: {}", id.c_str());
        }
    }

    QEvent event(QEvent::Type::LanguageChange);
    qApp->sendEvent(qApp, &event);

    return easytr::currentLanguage();
}
