#pragma once

#include <qobject.h>
#include <qstring.h>
#include <qsettings.h>
#include <qvariant.h>

class SettingsManager : public QObject
{
public:
    SettingsManager(const QString& organization, const QString& application, QObject* parent = nullptr);

    void writeSetting(const QString& key, const QVariant& value);
    void writeSettings(const QVariantMap& settings);

    QVariant readSetting(const QString& key, const QVariant& defaultValue);
    QVariantMap readSettings();

    void removeSetting(const QString& key);
    void clearSettings();

    bool has(const QString& key);

private:
    QSettings* settings_ = nullptr;
};
