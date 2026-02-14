#include "file_icon_getter.h"

#include <qfileiconprovider.h>
#include <qfileinfo.h>

QIcon getFileIcon(const QString& filepath)
{
    static QFileIconProvider fileIconProvider;
    QFileInfo fi(filepath);
    if (fi.exists())
        return fileIconProvider.icon(fi);
    return QIcon();
}
