#include "front_window_exedir_getter.h"

#include <cstdio>   // FILE, popen, pclose, fgets
#include <stdexcept>
#include <string>

#include <sys/types.h>
#include <sys/sysctl.h>
#include <libproc.h>

#include <qfile.h>
#include <qfileinfo.h>

#include "config.h"
#include "utils/file_io.h"

static std::string runCommand(const std::string& cmd)
{
    char buf[256] = {0};
    std::string out;

    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe)
        throw std::runtime_error("Failed to open pipe");
    while (fgets(buf, sizeof(buf), pipe))
        out += buf;
    pclose(pipe);

    return out;
}

QString getFocusedFileManagerDir()
{
    static const std::string cmd =
        readAllFromFile(APP_RESOURCE_DIRPATH + "/scripts/get_focused_finder_dir.sh").toStdString();
    std::string out = runCommand(cmd);

    QString path = QString::fromStdString(out);
    // 去除首尾空白字符。
    path = path.trimmed();
    // 如果成功返回路径，其会被单引号包裹。
    if (path.size() < 2 || !path.startsWith('\'') || !path.endsWith('\''))
        throw std::runtime_error(std::string("Failed to get directory path from Finder: ") + out);
    // 去除首尾引号。
    path.remove(0, 1);
    path.chop(1);

    if (!QFileInfo(path).isAbsolute())
        throw std::runtime_error(std::string("Failed to get directory path from Finder: ") + out);

    return path;
}

QString getFrontWindowExeDir()
{
    try
    {
        // 尝试获取处于焦点状态的Finder所在目录路径。
        return getFocusedFileManagerDir();
    }
    catch (const std::exception& e)
    {
        // 如果获取Finder目录路径出现错误则回退至获取前景窗口可执行文件所在目录路径。

        int frontPid = 0;
        size_t pidSize = sizeof(frontPid);

        // 获取前景进程的PID。
        int mib[2] = {CTL_KERN, KERN_ARGMAX};
        if (sysctl(mib, 2, &frontPid, &pidSize, nullptr, 0) == -1)
            throw std::runtime_error("Failed to get front window PID");

        if (frontPid > 0)
        {
            // 获取可执行文件路径。
            char pathBuf[PROC_PIDPATHINFO_MAXSIZE];
            int ret = proc_pidpath(frontPid, pathBuf, sizeof(pathBuf));

            if (ret > 0)
            {
                auto path = std::filesystem::path(pathBuf).parent_path().string();
                return QString::fromStdString(path);
            }
            else
            {
                throw std::runtime_error("Failed to get executable path from front window PID");
            }
        }
        else
        {
            throw std::runtime_error("Invalid front window PID");
        }
    }
}
