@echo off
setlocal

cmake -S . -B build-editor ^
  -DVE_BUILD_EDITOR=ON ^
  -DVE_BUILD_GAME=ON ^
  -DVE_BUILD_SERVER=OFF

cmake --build build-editor --config Debug

endlocal