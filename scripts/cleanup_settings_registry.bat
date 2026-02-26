:REM 用于清理程序注册表/本地缓存条目
:REM Used for cleaning up program registry entries

@echo off
reg delete "HKCU\Software\JaderoChan\OpenCmdAnywhere" /f
reg delete "HKCU\Software\Microsoft\Windows\CurrentVersion\Run" /v "OpenCmdAnywhere" /f
set "PROGRAM_DATA_DIR=%APPDATA%/JaderoChan/OpenCmdAnywhere"
rd /s /q "%PROGRAM_DATA_DIR%"
pause
