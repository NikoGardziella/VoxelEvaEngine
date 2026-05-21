@echo off
setlocal

cd /d "%~dp0\.."

echo Regenerating Visual Studio 2022 project...

cmake -S . -B build\vs2022 -G "Visual Studio 17 2022" -A x64

if %ERRORLEVEL% NEQ 0 (
    echo.
    echo CMake generation failed.
    pause
    exit /b %ERRORLEVEL%
)

echo.
echo Building Debug...

cmake --build build\vs2022 --config Debug

if %ERRORLEVEL% NEQ 0 (
    echo.
    echo Build failed.
    pause
    exit /b %ERRORLEVEL%
)

echo.
echo Regenerate and build succeeded.
pause