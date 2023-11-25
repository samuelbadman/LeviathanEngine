@echo off
rmdir /S /Q Build
cmake -G "Visual Studio 17 2022" -S %cd% -B Build
pause