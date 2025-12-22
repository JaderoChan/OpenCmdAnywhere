#include "settings_manager.h"

SettingsManager::SettingsManager(const QString& organization, const QString& application, QObject* parent) :
    QObject(parent)
{
    settings_ = new QSettings(
        QSettings::NativeFormat,
        QSettings::UserScope,
        organization,
        application,
        this
    );
}

void SettingsManager::writeSetting(const QString& key, const QVariant& value)
{
    settings_->setValue(key, value);
    settings_->sync();
}

void SettingsManager::writeSettings(const QVariantMap& settings)
{
    QMapIterator<QString, QVariant> it(settings);
    while (it.hasNext())
    {
        it.next();
        settings_->setValue(it.key(), it.value());
    }
    settings_->sync();
}

QVariant SettingsManager::readSetting(const QString& key, const QVariant& defaultValue)
{
    return settings_->value(key, defaultValue);
}

QVariantMap SettingsManager::readSettings()
{
    QVariantMap settings;
    QStringList keys = settings_->allKeys();
    for (const auto& key : keys)
        settings[key] = settings_->value(key);
    return settings;
}

void SettingsManager::removeSetting(const QString& key)
{
    settings_->remove(key);
}

void SettingsManager::clearSettings()
{
    settings_->clear();
}

bool SettingsManager::has(const QString& key)
{
    return settings_->contains(key);
}

