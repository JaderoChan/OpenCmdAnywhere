#include "settings_manager.h"

SettingsManager::SettingsManager(const QString& organization, const QString& application, QObject* parent) :
    QObject(parent),
    settings_(QSettings(QSettings::NativeFormat, QSettings::UserScope, organization, application, this))
{}

void SettingsManager::writeSetting(const QString& key, const QVariant& value)
{
    QMutexLocker<QMutex> locker(&mtx_);
    settings_.setValue(key, value);
    settings_.sync();
}

void SettingsManager::writeSettings(const QVariantMap& settings)
{
    QMapIterator<QString, QVariant> it(settings);
    QMutexLocker<QMutex> locker(&mtx_);
    while (it.hasNext())
    {
        it.next();
        settings_.setValue(it.key(), it.value());
    }
    settings_.sync();
}

QVariant SettingsManager::readSetting(const QString& key, const QVariant& defaultValue)
{
    QMutexLocker<QMutex> locker(&mtx_);
    return settings_.value(key, defaultValue);
}

QVariantMap SettingsManager::readSettings()
{
    QVariantMap settings;
    QMutexLocker<QMutex> locker(&mtx_);
    QStringList keys = settings_.allKeys();
    for (const auto& key : keys)
        settings[key] = settings_.value(key);
    return settings;
}

void SettingsManager::removeSetting(const QString& key)
{
    QMutexLocker<QMutex> locker(&mtx_);
    settings_.remove(key);
}

void SettingsManager::clearSettings()
{
    QMutexLocker<QMutex> locker(&mtx_);
    settings_.clear();
}

bool SettingsManager::has(const QString& key)
{
    QMutexLocker<QMutex> locker(&mtx_);
    return settings_.contains(key);
}
