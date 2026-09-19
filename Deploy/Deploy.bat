```bat
@echo off
setlocal EnableDelayedExpansion

echo ==========================================
echo   Image Processing - DLL Deployment
echo ==========================================
echo.

REM --------------------------------------------------
REM Paths
REM --------------------------------------------------

REM Folder containing deploy.bat and Image_Processing.exe
set "APP_DIR=%~dp0"

REM Executable
set "EXE=%APP_DIR%Image_Processing.exe"

REM DLL destination
set "DLL_DIR=%APP_DIR%bin"

REM MSYS2 MINGW64
set "MINGW_BIN=C:\msys64\mingw64\bin"

REM MSYS2 ldd
set "LDD=C:\msys64\usr\bin\ldd.exe"

REM --------------------------------------------------
REM Check files
REM --------------------------------------------------

if not exist "%EXE%" (
    echo ERROR: Image_Processing.exe was not found.
    echo.
    pause
    exit /b 1
)

if not exist "%LDD%" (
    echo ERROR: ldd.exe was not found.
    echo.
    pause
    exit /b 1
)

if not exist "%MINGW_BIN%" (
    echo ERROR: MSYS2 MINGW64 bin folder was not found.
    echo.
    pause
    exit /b 1
)

REM --------------------------------------------------
REM Create bin folder
REM --------------------------------------------------

if not exist "%DLL_DIR%" (
    echo Creating bin folder...
    mkdir "%DLL_DIR%"
)

echo.

REM --------------------------------------------------
REM Analyze dependencies
REM --------------------------------------------------

echo Executable:
echo %EXE%
echo.

echo Analyzing DLL dependencies...
echo.

"%LDD%" "%EXE%" > "%TEMP%\image_processing_ldd.txt" 2>&1

REM --------------------------------------------------
REM Copy DLL dependencies
REM --------------------------------------------------

echo Copying required DLLs...
echo.

for /f "tokens=1" %%D in ('findstr /R /C:"/mingw64/bin/.*\.dll" "%TEMP%\image_processing_ldd.txt"') do (

    set "DLL_PATH=%%D"

    for %%F in ("!DLL_PATH!") do set "DLL_NAME=%%~nxF"

    if exist "%MINGW_BIN%\!DLL_NAME!" (
        echo   !DLL_NAME!
        copy /Y "%MINGW_BIN%\!DLL_NAME!" "%DLL_DIR%\" >nul
    )
)

echo.

REM --------------------------------------------------
REM Copy Qt Windows platform plugin
REM --------------------------------------------------

echo Copying Qt platform plugin...

if not exist "%DLL_DIR%\platforms" (
    mkdir "%DLL_DIR%\platforms"
)

if exist "%MINGW_BIN%\..\share\qt6\plugins\platforms\qwindows.dll" (

    copy /Y "%MINGW_BIN%\..\share\qt6\plugins\platforms\qwindows.dll" "%DLL_DIR%\platforms\" >nul

    echo   platforms\qwindows.dll

) else (

    echo WARNING: qwindows.dll was not found.

)

echo.

REM --------------------------------------------------
REM Remove temporary file
REM --------------------------------------------------

del "%TEMP%\image_processing_ldd.txt" >nul 2>&1

REM --------------------------------------------------
REM Finished
REM --------------------------------------------------

echo ==========================================
echo   Deployment Complete
echo ==========================================
echo.
echo DLLs copied to:
echo %DLL_DIR%
echo.
echo Image_Processing.exe was NOT copied or modified.
echo.
echo ==========================================

pause
endlocal
```
