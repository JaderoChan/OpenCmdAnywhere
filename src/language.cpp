#include "language.h"

#include <qapplication.h>
#include <qdir.h>
#include <qevent.h>

#include <easy_translate.hpp>

#include "config.h"

QString setLanguage(const QString& langId)
{
#if defined(Q_OS_MAC) && defined(IS_MACOSX_BUNDLE)
    QDir::setCurrent(QApplication::applicationDirPath() + "/../Resources");
#else
    QDir::setCurrent(QApplication::applicationDirPath());
#endif
    easytr::setLanguages(APP_LANG_FILEPATH);
    if (easytr::languages().empty())
        qDebug() << "Invalid or empty Languages file";

    std::string id = langId.toStdString();
    if (easytr::hasLanguage(id))
    {
        if (easytr::setCurrentLanguage(id))
            qDebug() << "Successfully set language to:" << id.c_str();
        else
            qDebug() << "Failed to set language to:" << id.c_str();
    }
    // Fallthrough, fallback
    else
    {
        qDebug() << "Expected language is missing, try fall back to the default language";
        if (easytr::languages().empty())
        {
            qDebug() << "Not find any language";
            return easytr::currentLanguage();
        }
        else
        {
            id = easytr::languages().getIds().front();
            if (easytr::setCurrentLanguage(id))
                qDebug() << "Successfully fall back to language:" << id.c_str();
            else
                qDebug() << "Failed to fall back to language:" << id.c_str();
        }
    }

    QEvent event(QEvent::Type::LanguageChange);
    qApp->sendEvent(qApp, &event);

    return easytr::currentLanguage();
}
