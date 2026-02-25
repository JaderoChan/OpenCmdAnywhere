@echo off
reg delete "HKCU\Software\JaderoChan\Open Cmd Anywhere" /f
reg delete "HKCU\Software\Microsoft\Windows\CurrentVersion\Run" /v "Open Cmd Anywhere" /f
pause
