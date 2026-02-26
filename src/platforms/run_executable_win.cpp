#include "run_executable.h"

#include <stdexcept>

#include <windows.h>
#include <userenv.h>

bool runExecutable(
    const QString& exeFilepath,
    const QString& workDirectory,
    const QString& parameter)
{
    QString cmdLine = "\"" + exeFilepath + "\"";
    if (!parameter.isEmpty())
        cmdLine += (" " + parameter);

    // 获取用户Token
    HANDLE userToken = NULL;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_DUPLICATE, &userToken))
        return false;
    HANDLE duplicateToken = NULL;
    if (!DuplicateTokenEx(
        userToken,
        TOKEN_ALL_ACCESS,
        nullptr,
        SecurityImpersonation,
        TokenPrimary,
        &duplicateToken))
    {
        CloseHandle(userToken);
        return false;
    }
    CloseHandle(userToken);

    // 创建环境变量块并设置进程创建标志。
    DWORD dwCreationFlags = 0;
    LPVOID lpEnvironment = NULL;
    if (CreateEnvironmentBlock(&lpEnvironment, duplicateToken, FALSE))
    {
        dwCreationFlags |= CREATE_UNICODE_ENVIRONMENT;
    }
    else
    {
        CloseHandle(duplicateToken);
        lpEnvironment = NULL;
    }

    dwCreationFlags |= (CREATE_NEW_CONSOLE | CREATE_DEFAULT_ERROR_MODE | NORMAL_PRIORITY_CLASS);

    // 创建进程
    STARTUPINFOW si = {0};
    si.cb = sizeof(si);
    PROCESS_INFORMATION pi = {0};

    std::wstring wstrCmdLine = cmdLine.toStdWString();
    std::wstring wstrWorkDir = workDirectory.toStdWString();
    BOOL res = CreateProcessAsUserW(
        duplicateToken,
        NULL,
        wstrCmdLine.data(),
        NULL,
        NULL,
        FALSE,
        dwCreationFlags,
        lpEnvironment,
        wstrWorkDir.c_str(),
        &si,
        &pi
    );

    // 清理资源
    if (lpEnvironment)
        DestroyEnvironmentBlock(lpEnvironment);
    CloseHandle(duplicateToken);

    if (res)
    {
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }

    return res;
}
