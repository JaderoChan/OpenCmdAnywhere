#pragma once

#include <qstring>

/**
 * @brief 获取前景窗口可执行文件所在的目录路径。此外，如果前景窗口是文件管理器，则获取文件管理器所在目录路径。
 * @return 一个目录路径。
 */
QString getFrontWindowExeDir();
