#include "run_executable.h"

#include <qprocess.h>
#include <qfileinfo.h>
#include <qbytearray.h>

bool runExecutable(
    const QString& exeFilepath,
    const QString& workDirectory,
    const QString& parameter)
{
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    QProcess process;
    process.setProgram(exeFilepath);
    process.setArguments(QProcess::splitCommand(parameter));
    process.setWorkingDirectory(workDirectory);
    process.setProcessEnvironment(env);
    return process.startDetached();
}
