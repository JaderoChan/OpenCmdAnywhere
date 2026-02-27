#pragma once

#include <qstring.h>

// 如果应用程序不支持当前系统语言则返回`LANG_EN`。
QString currentSystemLang();

bool setLanguage(const QString& langId);
