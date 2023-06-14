@echo off
set BUILD_TYPE=Release
set BUILD_DIR=build-release

echo Compiling %BUILD_TYPE% in %BUILD_DIR%
cmake -B %BUILD_DIR% -DCMAKE_BUILD_TYPE=%BUILD_TYPE% .
if %ERRORLEVEL% NEQ 0 goto Failure
cmake --build %BUILD_DIR% --config %BUILD_TYPE%
if %ERRORLEVEL% NEQ 0 goto Failure
cd %BUILD_DIR%
ctest --build-config %BUILD_TYPE% --output-on-failure
if %ERRORLEVEL% NEQ 0 goto Failure
cd ..
echo SUCCESS!
echo Binaries are in %cd%\%BUILD_DIR%\src\%BUILD_TYPE%
start "" "%cd%\%BUILD_DIR%\src\%BUILD_TYPE%"


set BUILD_TYPE=Debug
set BUILD_DIR=build-debug

echo Compiling %BUILD_TYPE% in %BUILD_DIR%
cmake -B %BUILD_DIR% -DCMAKE_BUILD_TYPE=%BUILD_TYPE% .
if %ERRORLEVEL% NEQ 0 goto Failure
cmake --build %BUILD_DIR% --config %BUILD_TYPE%
if %ERRORLEVEL% NEQ 0 goto Failure
cd %BUILD_DIR%
ctest --build-config %BUILD_TYPE% --output-on-failure
if %ERRORLEVEL% NEQ 0 goto Failure
cd ..
echo SUCCESS!
echo Binaries are in %cd%\%BUILD_DIR%\src\%BUILD_TYPE%
start "" "%cd%\%BUILD_DIR%\src\%BUILD_TYPE%"

pause
exit 0

:Failure
echo FAILED
pause
exit 1
