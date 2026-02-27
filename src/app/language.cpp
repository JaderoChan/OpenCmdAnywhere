#include "language.h"

#include <qapplication.h>
#include <qdir.h>
#include <qevent.h>
#include <qlocale.h>

#include <easy_translate.hpp>

#include "config.h"
#include "utils/logging.h"

class DirectoryScope
{
public:
    explicit DirectoryScope(const QString& path)
    {
        originDir_ = QDir::currentPath();
        QDir::setCurrent(path);
    }

    ~DirectoryScope()
    {
        QDir::setCurrent(originDir_);
    }

private:
    QString originDir_;
};

QString currentSystemLang()
{
    switch (QLocale::system().language())
    {
        case QLocale::Language::English:    return "EN";
        case QLocale::Language::Chinese:    return "ZH";
        default:                            return "EN";
    }
}

bool setLanguage(const QString& langId)
{
    {
        DirectoryScope dirScope(APP_RESOURCE_DIRPATH);
        easytr::setLanguages(easytr::Languages::fromFile(APP_LANG_FILEPATH));
    }
    if (easytr::languages().empty())
    {
        debugOut(qWarning(), "[Language] Failed to load languages or languages list is empty.");
        return false;
    }

    std::string id = langId.toStdString();
    if (!easytr::hasLanguage(id))
    {
        debugOut(qWarning(), "[Language] Language %1 is not available.", langId);
        return false;
    }

    {
        DirectoryScope dirScope(APP_RESOURCE_DIRPATH);
        if (!easytr::setCurrentLanguage(id))
        {
            debugOut(qWarning(), "[Language] Failed to set the current language to %1.", langId);
            return false;
        }
    }

    QEvent event(QEvent::Type::LanguageChange);
    qApp->sendEvent(qApp, &event);

    return true;
}
