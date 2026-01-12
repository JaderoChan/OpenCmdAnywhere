#include "utility.h"

#include <windows.h>
#include <shellapi.h>

#include <qapplication.h>
#include <qdir.h>
#include <qfileinfo.h>
#include <qsettings.h>

#include <minilog.hpp>

bool isRunOnStartup()
{
    QSettings settings(
        "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run",
        QSettings::NativeFormat);
    QString appName = QCoreApplication::applicationName();
    QString appPath = QDir::toNativeSeparators(QCoreApplication::applicationFilePath());
    if (settings.contains(appName) && settings.value(appName) == appPath)
        return true;
    return false;
}

bool setRunOnStartup(bool enable)
{
    QSettings settings(
        "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run",
        QSettings::NativeFormat);
    QString appName = QCoreApplication::applicationName();
    QString appPath = QDir::toNativeSeparators(QCoreApplication::applicationFilePath());
    if (enable)
        settings.setValue(appName, appPath);
    else
        settings.remove(appName);
    settings.sync();
    return settings.status() == QSettings::NoError;
}

QIcon getExecutableIcon(const QString& exePath)
{
    std::wstring path = QDir::toNativeSeparators(exePath).toStdWString();
    SHFILEINFOW sfi = {0};
    // https://learn.microsoft.com/en-us/windows/win32/api/shellapi/nf-shellapi-shgetfileinfow
    // Remarks: You should call this function from a background thread. Failure to do so could cause the UI to stop responding.
    DWORD_PTR result = SHGetFileInfoW(
        path.c_str(),
        0,
        &sfi,
        sizeof(sfi),
        SHGFI_ICON | SHGFI_LARGEICON
    );

    QIcon icon;
    if (result != 0 && sfi.hIcon != nullptr)
    {
        QPixmap pixmap = QPixmap::fromImage(QImage::fromHICON(sfi.hIcon));
        if (!pixmap.isNull())
            icon.addPixmap(pixmap);
        DestroyIcon(sfi.hIcon);
    }

    return icon;
}
