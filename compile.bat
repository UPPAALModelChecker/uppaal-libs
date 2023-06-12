@echo off
echo Compiling for Windows using CMake
cmake -B build-release .
if %ERRORLEVEL% NEQ 0 goto Failure
cmake --build build-release
if %ERRORLEVEL% NEQ 0 goto Failure
cd build-release
ctest --output-on-failure
if %ERRORLEVEL% NEQ 0 goto Failure
cd ..
echo SUCCESS!
echo Binaries are in %cd%\build-release\src
start "" "%cd%\build-release\src"
pause
exit 0

:Failure
echo FAILED
pause
exit 1
