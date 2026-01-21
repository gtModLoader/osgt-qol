:: For this script to work right, set OSGT_ROOT env var to the target Growtopia dir
@echo off
setlocal

if not defined OSGT_ROOT (
    echo Error: OSGT_ROOT is not defined in the environment
    exit /b 1
)

set "DLL_SOURCE=%~1"
if "%DLL_SOURCE%"=="" (
    echo Warning: No DLL_SOURCE argument provided, skipping copying
)

:: It'd be nice for this script to actually close the game, but the client seems to set restrictive ACEs on itself, and
:: as a result elevated processes can terminate it. If this gets annoying enough, we can undo this mess in the setup 
:: routine of dev builds. For now, just fail if the game's already running
tasklist /FI "IMAGENAME eq Growtopia.exe" 2>NUL | find /I /N "Growtopia.exe">NUL
if %ERRORLEVEL%==0 (
    echo Error: Growtopia is already running. Close it first!
    exit /b 1
)

if not "%DLL_SOURCE%"=="" (
    echo Copying %DLL_SOURCE% to %OSGT_ROOT%\dinput8.dll
    copy /y "%DLL_SOURCE%" "%OSGT_ROOT%\dinput8.dll"
    if errorlevel 1 (
        echo Error: Copy failed
        exit /b 1
    )
)

echo Starting Growtopia...
start "" /d "%OSGT_ROOT%" "%OSGT_ROOT%\Growtopia.exe"

endlocal
