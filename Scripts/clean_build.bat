@echo off
setlocal

cd /d "%~dp0\.."

echo Removing build folder...

if exist build (
    rmdir /s /q build
)

echo.
echo Clean done.
pause