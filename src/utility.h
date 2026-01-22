#pragma once

#include <qicon.h>
#include <qstring.h>

bool isRunOnStartup();

bool setRunOnStartup(bool enable);

QIcon getExecutableIcon(const QString& exePath);
