@echo off
setlocal

set "PROJECT=%~dp0"
set "BUILD=%PROJECT%build"
set "TOOLS=C:\Tools"
set "EXE_SRC=%BUILD%\Release\widefetch.exe"
set "EXE_DST=%TOOLS%\widefetch.exe"

cmake --build "%BUILD%" --config Release
if errorlevel 1 exit /b 1

if not exist "%TOOLS%" mkdir "%TOOLS%"

if not exist "%EXE_SRC%" exit /b 1
copy /Y "%EXE_SRC%" "%EXE_DST%" >nul

exit /b 0
