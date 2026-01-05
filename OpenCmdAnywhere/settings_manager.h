#pragma once

#include <qapplication.h>
#include <qmutex.h>
#include <qobject.h>
#include <qstring.h>
#include <qsettings.h>
#include <qvariant.h>

// thread-safe
class SettingsManager : public QObject
{
public:
    SettingsManager(
        const QString& organization = QApplication::organizationName(),
        const QString& application = QApplication::applicationName(),
        QObject* parent = nullptr
    );

    void writeSetting(const QString& key, const QVariant& value);
    void writeSettings(const QVariantMap& settings);

    QVariant readSetting(const QString& key, const QVariant& defaultValue);
    QVariantMap readSettings();

    void removeSetting(const QString& key);
    void clearSettings();

    bool has(const QString& key);

private:
    mutable QMutex mtx_;
    QSettings settings_;
};
