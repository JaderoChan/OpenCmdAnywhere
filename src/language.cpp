#include "language.h"

#include <qapplication.h>
#include <qevent.h>

#include <easy_translate.hpp>
#include <minilog.hpp>

#include "config.h"

void initLanguage()
{
    easytr::Languages langs;
    langs.add("EN", (QApplication::applicationDirPath() + "/languages/en.json").toStdString());
    langs.add("ZH", (QApplication::applicationDirPath() + "/languages/zh.json").toStdString());
    easytr::setLanguages(langs);
}

QString setLanguage(const QString& langId)
{
    std::string id = langId.toStdString();
    if (easytr::hasLanguage(id))
    {
        if (easytr::setCurrentLanguage(id))
            mlog::info("Successfully set language to: {}", id.c_str());
        else
            mlog::warning("Failed to set language to: {}", id.c_str());
    }
    // Fallthrough, fallback
    else
    {
        mlog::info("Expected language is missing, try fall back to the default language");
        id = easytr::languages().getIds().front();
        if (easytr::setCurrentLanguage(id))
            mlog::info("Successfully fall back to language: {}", id.c_str());
        else
            mlog::warning("Failed to fall back to language: {}", id.c_str());
    }

    QEvent event(QEvent::Type::LanguageChange);
    qApp->sendEvent(qApp, &event);

    return easytr::currentLanguage();
}
