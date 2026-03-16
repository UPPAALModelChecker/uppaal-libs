@echo off

erase /F /S /Q libtable.dll libtable-dbg.dll > NUL

set CMAKE_BUILD_PARALLEL_LEVEL=%NUMBER_OF_PROCESSORS%
set CTEST_PARALLEL_LEVEL=%NUMBER_OF_PROCESSORS%
set CTEST_TEST_LOAD=%NUMBER_OF_PROCESSORS%
set CTEST_OUTPUT_ON_FAILURE=1

set CMAKE_BUILD_TYPE=Debug
set BUILD_DIR=build-%CMAKE_BUILD_TYPE%
echo Compiling %CMAKE_BUILD_TYPE% in %BUILD_DIR%
cmake -B %BUILD_DIR% -DCMAKE_BUILD_TYPE=%CMAKE_BUILD_TYPE% .
if %ERRORLEVEL% NEQ 0 goto Failure
cmake --build %BUILD_DIR% --config %CMAKE_BUILD_TYPE%
if %ERRORLEVEL% NEQ 0 goto Failure
ctest --test-dir %BUILD_DIR% --build-config %CMAKE_BUILD_TYPE%
if %ERRORLEVEL% NEQ 0 goto Failure
copy %cd%\%BUILD_DIR%\src\%CMAKE_BUILD_TYPE%\table.dll %cd%\libtable-dbg.dll
echo SUCCESS building debug into libtable-dbg.dll

set CMAKE_BUILD_TYPE=Release
set BUILD_DIR=build-%CMAKE_BUILD_TYPE%
echo Compiling %CMAKE_BUILD_TYPE% in %BUILD_DIR%
cmake -B %BUILD_DIR% -DCMAKE_BUILD_TYPE=%CMAKE_BUILD_TYPE% .
if %ERRORLEVEL% NEQ 0 goto Failure
cmake --build %BUILD_DIR% --config %CMAKE_BUILD_TYPE%
if %ERRORLEVEL% NEQ 0 goto Failure
ctest --test-dir %BUILD_DIR% --build-config %CMAKE_BUILD_TYPE% --output-on-failure
if %ERRORLEVEL% NEQ 0 goto Failure
copy %cd%\%BUILD_DIR%\src\%CMAKE_BUILD_TYPE%\table.dll %cd%\libtable.dll
echo SUCCESS building release into libtable.dll

start "" "%cd%"

pause
exit 0

:Failure
echo FAILED
pause
exit 1
