@echo off
setlocal

cmake -S . -B build-game ^
  -DVE_BUILD_EDITOR=OFF ^
  -DVE_BUILD_GAME=ON ^
  -DVE_BUILD_SERVER=OFF

cmake --build build-game --config Release

endlocal