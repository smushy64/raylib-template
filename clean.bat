@echo off
setlocal EnableDelayedExpansion

rem Resets build system.
rem Does not affect project files except for generated sources.

set /p response=Are you sure you want to clean all build system files? [Y/n] 

if not "%response%"=="" if /i not "%response%"=="y" exit /b 0

rem ----------------------------------------
rem traced equivalent (basic logging wrapper)
rem ----------------------------------------

:traced
echo " > " %*
%*
exit /b %errorlevel%

rem ----------------------------------------

echo Deleting all build system headers . . .

call :traced del /f /q *.h 2>nul

echo Deleting build system . . .

call :traced del /f /q build.bin 2>nul
call :traced del /f /q build.bin.old 2>nul

echo Deleting build directory . . .

call :traced rmdir /s /q build 2>nul

echo Deleting generated files . . .

call :traced del /f /q src\main.c src\main.cpp 2>nul
call :traced del /f /q src\sources.c src\sources.cpp 2>nul
call :traced del /f /q src\compile_flags.txt 2>nul
call :traced del /f /q include\compile_flags.txt 2>nul

echo All done.

endlocal
