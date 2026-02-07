#include "core.h"

#include <filesystem>
#include <stdexcept>

#include <psapi.h>
#include <shobjidl.h>
#include <shlobj.h>
#include <userenv.h>

std::wstring getWindowExePath(HWND window)
{
    DWORD dwProcessId;
    if (GetWindowThreadProcessId(window, &dwProcessId) == 0)
        throw std::runtime_error("Failed to GetWindowThreadProcessId()");
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwProcessId);
    if (hProcess == NULL)
        throw std::runtime_error("Failed to OpenProcess()");
    wchar_t path[MAX_PATH] = {0};
    DWORD ret = GetModuleFileNameExW(hProcess, NULL, path, MAX_PATH);
    CloseHandle(hProcess);
    if (ret == 0)
        throw std::runtime_error("Failed to GetModuleFileNameExW()");
    return std::wstring(path);
}

std::wstring getWindowExeDirectory(HWND window)
{
    namespace fs = std::filesystem;
    std::wstring path = getWindowExePath(window);
    return fs::path(path).parent_path().wstring();
}

std::wstring getFocusedWindowDirectory()
{
    constexpr const WCHAR* EXPLORER_CLASS_NAME_1    = L"ExploreWClass";
    constexpr const WCHAR* EXPLORER_CLASS_NAME_2    = L"CabinetWClass";
    constexpr const WCHAR* DESKTOP_CLASS_NAME_1     = L"Progman";
    constexpr const WCHAR* DESKTOP_CLASS_NAME_2     = L"WorkerW";

    HWND focusedWindow = GetForegroundWindow();
    if (focusedWindow == nullptr)
        throw std::runtime_error("Failed to GetForegroundWindow()");

    WCHAR classname[MAX_CLASS_NAME];
    if (GetClassNameW(focusedWindow, classname, MAX_CLASS_NAME) == 0)
        throw std::runtime_error("Failed to GetClassName()");

    bool atExplorer1 = wcscmp(classname, EXPLORER_CLASS_NAME_1) == 0;
    bool atExplorer2 = wcscmp(classname, EXPLORER_CLASS_NAME_2) == 0;
    bool atDesktop1 = wcscmp(classname, DESKTOP_CLASS_NAME_1) == 0;
    bool atDesktop2 = wcscmp(classname, DESKTOP_CLASS_NAME_2) == 0;

    if (!atExplorer1 && !atExplorer2 && !atDesktop1 && !atDesktop2)
        return getWindowExeDirectory(focusedWindow);

    if (atDesktop1 || atDesktop2)
    {
        wchar_t path[MAX_PATH] = {0};
        if (!SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_DESKTOP, NULL, SHGFP_TYPE_CURRENT, path)))
            throw std::runtime_error("Failed to SHGetFolderPath()");
        return std::wstring(path);
    }

    IShellWindows* psw = nullptr;
    if (!SUCCEEDED(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE)))
        throw std::runtime_error("Failed to CoInitializeEx()");
    if (!SUCCEEDED(CoCreateInstance(CLSID_ShellWindows, NULL, CLSCTX_ALL, IID_PPV_ARGS(&psw))))
    {
        CoUninitialize();
        throw std::runtime_error("Failed to CoCreateInstance()");
    }

    VARIANT index = {VT_I4};
    if (!SUCCEEDED(psw->get_Count(&index.lVal)))
    {
        psw->Release();
        CoUninitialize();
        throw std::runtime_error("Failed to get_count()");
    }

    LPWSTR path = NULL;
    while (--index.lVal >= 0) {
        IDispatch* pdisp = nullptr;

        if (psw->Item(index, &pdisp) != S_OK)
            continue;

        IWebBrowserApp* pwba = nullptr;
        if (!SUCCEEDED(pdisp->QueryInterface(IID_PPV_ARGS(&pwba))))
        {
            pdisp->Release();
            psw->Release();
            CoUninitialize();
            throw std::runtime_error("Failed to QueryInterface()");
        }

        HWND hwnd = nullptr;
        if (!SUCCEEDED(pwba->get_HWND((SHANDLE_PTR*) &hwnd)))
        {
            pwba->Release();
            pdisp->Release();
            psw->Release();
            CoUninitialize();
            throw std::runtime_error("Failed to get_HWND()");
        }

        if (hwnd != focusedWindow)
        {
            pwba->Release();
            pdisp->Release();
            continue;
        }

        IServiceProvider* psp = nullptr;
        if (!SUCCEEDED(pwba->QueryInterface(IID_PPV_ARGS(&psp))))
        {
            pwba->Release();
            pdisp->Release();
            psw->Release();
            CoUninitialize();
            throw std::runtime_error("Failed to QueryInterface()");
        }

        IShellBrowser* psb = nullptr;
        if (!SUCCEEDED(psp->QueryService(SID_STopLevelBrowser, IID_PPV_ARGS(&psb))))
        {
            psp->Release();
            pwba->Release();
            pdisp->Release();
            psw->Release();
            CoUninitialize();
            throw std::runtime_error("Failed to QueryService()");
        }

        IShellView* psv = nullptr;
        if (!SUCCEEDED(psb->QueryActiveShellView(&psv)))
        {
            psb->Release();
            psp->Release();
            pwba->Release();
            pdisp->Release();
            psw->Release();
            CoUninitialize();
            throw std::runtime_error("Failed to QueryActiveShellView()");
        }

        IFolderView* pfv = nullptr;
        if (!SUCCEEDED(psv->QueryInterface(IID_PPV_ARGS(&pfv))))
        {
            psv->Release();
            psb->Release();
            psp->Release();
            pwba->Release();
            pdisp->Release();
            psw->Release();
            CoUninitialize();
            throw std::runtime_error("Failed to QueryInterface()");
        }

        IShellItemArray* psia = nullptr;
        if (!SUCCEEDED(pfv->GetFolder(IID_PPV_ARGS(&psia))))
        {
            pfv->Release();
            psv->Release();
            psb->Release();
            psp->Release();
            pwba->Release();
            pdisp->Release();
            psw->Release();
            CoUninitialize();
            throw std::runtime_error("Failed to GetFolder()");
        }

        IShellItem* psi = nullptr;
        if (!SUCCEEDED(psia->GetItemAt(0, &psi)))
        {
            psia->Release();
            pfv->Release();
            psv->Release();
            psb->Release();
            psp->Release();
            pwba->Release();
            pdisp->Release();
            psw->Release();
            CoUninitialize();
            throw std::runtime_error("Failed to GetItemAt()");
        }

        if (!SUCCEEDED(psi->GetDisplayName(SIGDN_FILESYSPATH, &path)))
        {
            psi->Release();
            psia->Release();
            pfv->Release();
            psv->Release();
            psb->Release();
            psp->Release();
            pwba->Release();
            pdisp->Release();
            psw->Release();
            CoUninitialize();
            return getWindowExeDirectory(focusedWindow);
        }

        psi->Release();
        psia->Release();
        pfv->Release();
        psv->Release();
        psb->Release();
        psp->Release();
        pwba->Release();
        pdisp->Release();
    }

    psw->Release();
    CoUninitialize();

    if (path)
        return std::wstring(path);
    throw std::runtime_error("Failed to get valid explorer window");
}

bool runExecutable(
    const std::wstring& exeFilename,
    const std::wstring& workDirectory,
    const std::wstring& parameter)
{
    std::wstring cmdLine = L"\"" + exeFilename + L"\"";
    if (!parameter.empty())
        cmdLine += (L" " + parameter);

    // 获取用户Token。
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

    BOOL res = CreateProcessAsUserW(
        duplicateToken,
        NULL,
        cmdLine.data(),
        NULL,
        NULL,
        FALSE,
        dwCreationFlags,
        lpEnvironment,
        workDirectory.c_str(),
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
