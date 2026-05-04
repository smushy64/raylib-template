@echo off
setlocal EnableDelayedExpansion

rem Bootstrapper for Raylib Template Build System

set "BUILD_DIR=.\build"
set "ARGS=%*"

rem ----------------------------------------
rem logging functions
rem ----------------------------------------

:log
set "msg=%~1"
set "is_error=%~2"

if "%is_error%"=="1" (
    echo BOOTSTRAPPER ERROR: %msg% 1>&2
) else (
    echo BOOTSTRAPPER: %msg%
)
exit /b 0


:traced
echo BOOTSTRAPPER: running %*
%*
exit /b %errorlevel%


:logged
set "msg=%~1"
shift

call :log "%msg%"

echo BOOTSTRAPPER: running %*
%*
exit /b %errorlevel%

rem ----------------------
rem make sure that GCC is installed.
rem ----------------------

where gcc >nul 2>nul
if %errorlevel%==0 (
) else (
    call :log "gcc not found! Install it through MinGW!" 1
    exit /b 1
)

rem ----------------------------------------
rem ensure build directory
rem ----------------------------------------

set "submodules_loaded=0"

if exist "%BUILD_DIR%" (
    if exist "%BUILD_DIR%\submodules_loaded" (
        set "submodules_loaded=1"
    )
) else (
    call :logged "Build system directory not found, creating . . ." mkdir "%BUILD_DIR%"
)

rem ----------------------------------------
rem submodule check (manual, no arrays in batch)
rem ----------------------------------------

if "%submodules_loaded%"=="0" (

    set "raylib_loaded=0"
    set "cb_loaded=0"
    set "flagship_loaded=0"
    set "ini_parser_loaded=0"

    if exist "extern\raylib\*" set "raylib_loaded=1"
    if exist "extern\cb\*" set "cb_loaded=1"
    if exist "extern\flagship\*" set "flagship_loaded=1"
    if exist "extern\ini-parser\*" set "ini_parser_loaded=1"

    if "%raylib_loaded%"=="0" if "%cb_loaded%"=="0" if "%flagship_loaded%"=="0" if "%ini_parser_loaded%"=="0" (
        set /p response=BOOTSTRAPPER: One or more submodules missing. Initialize? [Y/n] 

        if "!response!"=="" goto init_submodules
        if /i "!response!"=="y" goto init_submodules

        call :log "One or more required submodules has not been initialized!" 1
        exit /b 1

        :init_submodules
        call :log "Initializing submodules . . ." 
        git submodule update --init --recursive
    )

    echo.> "%BUILD_DIR%\submodules_loaded"
)

rem ----------------------------------------
rem build system check
rem ----------------------------------------

if not exist "build.bin" (

    set "missing=0"

    if not exist "cb.h" set "missing=1"
    if not exist "flagship.h" set "missing=1"
    if not exist "ini-parse.h" set "missing=1"

    if "%missing%"=="1" (
        call :log "Copying required files for build system . . ."
        if not exist "cb.h" (
            call :traced copy "extern\cb\cb.h" .
        )
        if not exist "flagship.h" (
            call :traced copy "extern\flagship\flagship.h" .
        )
        if not exist "ini-parse.h" (
            call :traced copy "extern\ini-parser\ini-parse.h" .
        )
    )

    call :log "Building build system . . ." 
    gcc cb.c -o build.bin
)

rem ----------------------------------------
rem run build system
rem ----------------------------------------

build.bin %ARGS%

endlocal
