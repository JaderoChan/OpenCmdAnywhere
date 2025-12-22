#pragma once

#include <string>

#include <windows.h>

std::wstring getWindowExePath(HWND window);

std::wstring getWindowExeDirectory(HWND window);

std::wstring getFocusedWindowDirectory();

bool runExecutable(
    const std::wstring& exeFilename,
    const std::wstring& workDirectory,
    const std::wstring& parameter,
    bool isAdmin
);
