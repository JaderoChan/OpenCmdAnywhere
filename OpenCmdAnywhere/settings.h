#pragma once

#include <qmap.h>
#include <qstring.h>

#include <global_hotkey/key_combination.hpp>

#include "settings_manager.h"

// Singleton, hungry run
class Settings
{
public:
    static Settings& getInstance();

    // The return value may be empty.
    static QString getLangugae();
    // Return: <display name : executable filename>, the executable filename may be invalid.
    static std::pair<QString, QString> getCurrentExecutable();
    // The return value may be empty.
    static QString getParameter();
    static gbhk::KeyCombination getKeyCombination(bool isAdmin);
    static bool getIsRunOnStartup();

    static void setLanguage(const QString& value);
    static void setCurrentExecutable(const QString& value);
    static void setParameter(const QString& value);
    static void setKeyCombination(const gbhk::KeyCombination& value, bool isAdmin);
    static void setIsRunOnStartup(bool value);

    // Return: <display name : executable filename>
    static QVariantMap getAllExecutables();
    static void addExecutable(const QString& displayName, const QString& filename);
    static void removeExecutable(const QString& displayName);

private:
    Settings();
    ~Settings() = default;
    Settings(const Settings&) = delete;
    Settings& operator=(const Settings&) = delete;

    SettingsManager sm_;
    QVariantMap executables_;
};
