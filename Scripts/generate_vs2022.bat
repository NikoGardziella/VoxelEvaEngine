@echo off
setlocal

cd /d "%~dp0\.."

echo Generating Visual Studio 2022 project...

cmake -S . -B build -G "Visual Studio 17 2022" -A x64

if %ERRORLEVEL% NEQ 0 (
    echo.
    echo CMake generation failed.
    pause
    exit /b %ERRORLEVEL%
)

echo.
echo Done.
pause