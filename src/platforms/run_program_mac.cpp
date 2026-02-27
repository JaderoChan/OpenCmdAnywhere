#include "run_program.h"

#include <cstdlib>

static bool runCmd(
    const QString& program,
    const QString& parameterStr,
    const QString& workDirectory)
{
    QString cmd;
    if (parameterStr.isEmpty())
        cmd = QString("open -a %1 %2").arg(program, workDirectory);
    else
        cmd = QString("open -a %1 --args %2 %3").arg(program, parameterStr, workDirectory);
    system(cmd.toUtf8().constData());
}

bool runProgram(
    const QString& program,
    const QString& parameter,
    const QString& workDirectory)
{
    runCmd(program, parameter, workDirectory);
}
