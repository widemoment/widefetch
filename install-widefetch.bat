@echo off
setlocal
:: change if files names differ
set "PROJECT=%~dp0"
set "BUILD=%PROJECT%build"
set "TOOLS=C:\Tools"
set "EXE_SRC=%BUILD%\Release\widefetch.exe"
set "EXE_DST=%TOOLS%\widefetch.exe"

cmake -S "%PROJECT%" -B "%BUILD%"
if errorlevel 1 exit /b 1

cmake --build "%BUILD%" --config Release
if errorlevel 1 exit /b 1

if not exist "%TOOLS%" mkdir "%TOOLS%"

if not exist "%EXE_SRC%" exit /b 1
copy /Y "%EXE_SRC%" "%EXE_DST%" >nul

for /f "tokens=2,*" %%A in ('reg query HKCU\Environment /v Path 2^>nul ^| find /i "Path"') do set "USERPATH=%%B"

echo %USERPATH% | find /i "%TOOLS%" >nul
if errorlevel 1 (
  set "NEWPATH=%USERPATH%;%TOOLS%"
  reg add HKCU\Environment /v Path /t REG_EXPAND_SZ /d "%NEWPATH%" /f >nul
)

exit /b 0
