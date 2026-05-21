@echo off
setlocal

cd /d "%~dp0\.."

echo Building Debug...

cmake --build build --config Debug

if %ERRORLEVEL% NEQ 0 (
    echo.
    echo Build failed.
    pause
    exit /b %ERRORLEVEL%
)

echo.
echo Build succeeded.
pause