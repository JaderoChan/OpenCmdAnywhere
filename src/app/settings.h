#pragma once

#include <qstring>
#include <qlist.h>

#include <global_hotkey/key_combination.hpp>

struct ExecutableItem
{
    QString name;
    QString filepath;
};

struct Settings
{
    bool autoRunOnStartUp;
    int currentExecutableIdx;
    QString language;
    QString parameter;
    gbhk::KeyCombination hotkey;
};

const QList<ExecutableItem>& defaultExecutables();

Settings loadSettings();

void saveSettings(const Settings& settings);
