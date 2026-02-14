#pragma once

#include <qmap.h>
#include <qmutex.h>
#include <qstring.h>

#include <global_hotkey/key_combination.hpp>

#include "settings_manager.h"

// Singleton, hungry run, thread-safe
class Settings
{
public:
    static Settings& getInstance();

    static QString getLangugae();
    // Return: <display name : executable filepath>, the executable filepath may be invalid.
    static std::pair<QString, QString> getCurrentExecutable();
    // The return value may be empty.
    static QString getParameter();
    static gbhk::KeyCombination getKeyCombination();
    static bool getIsAutoRunOnStartUp();

    static void setLanguage(const QString& value);
    static void setCurrentExecutable(const QString& value);
    static void setParameter(const QString& value);
    static void setKeyCombination(const gbhk::KeyCombination& value);
    static void setAutoRunOnStartUp(bool value);

    // Return: <display name : executable filepath>
    static QVariantMap getAllExecutables();
    static void addExecutable(const QString& displayName, const QString& filepath);
    static void removeExecutable(const QString& displayName);

private:
    Settings();
    ~Settings() = default;
    Settings(const Settings&) = delete;
    Settings& operator=(const Settings&) = delete;

    mutable QMutex mtx_;
    SettingsManager sm_;
    QVariantMap executables_;
};
