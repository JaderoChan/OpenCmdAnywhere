#include "run_executable.h"

#include <string>
#include <vector>

#include <qstringlist.h>

#include <unistd.h>
#include <sys/wait.h>

bool runExecutable(
    const QString& exeFilename,
    const QString& workDirectory,
    const QString& parameter)
{
    pid_t pid = fork();
    if (pid < 0)
        return false;

    // 子进程
    if (pid == 0)
    {
        if (!workDirectory.isEmpty())
        {
            if (chdir(workDirectory.toStdString().c_str()) != 0)
                _exit(1);
        }

        std::string exeFileStr = exeFilename.toStdString();
        QStringList args = parameter.split(' ', Qt::SkipEmptyParts);

        std::vector<std::string> argsStdStr;
        argsStdStr.push_back(exeFileStr);
        for (const QString& arg : args)
            argsStdStr.push_back(arg.toStdString());

        std::vector<char*> argv;
        for (auto& arg : argsStdStr)
            argv.push_back(const_cast<char*>(arg.c_str()));
        argv.push_back(nullptr);

        execvp(exeFileStr.c_str(), argv.data());
        _exit(1);
    }

    // 父进程
    return true;
}
