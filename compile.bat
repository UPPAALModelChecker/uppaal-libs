@echo off

erase /F /S /Q libtable.dll libtable-dbg.dll > NUL

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
copy %cd%\%BUILD_DIR%\src\%BUILD_TYPE%\table.dll %cd%\libtable.dll
echo SUCCESS building release into libtable.dll


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
copy %cd%\%BUILD_DIR%\src\%BUILD_TYPE%\table.dll %cd%\libtable-dbg.dll
echo SUCCESS building debug into libtable-dbg.dll
start "" "%cd%"

pause
exit 0

:Failure
echo FAILED
pause
exit 1
