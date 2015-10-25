@echo off
if "%MINGW_PATH%" == "" set MINGW_PATH=C:\MinGW\msys\1.0
if not exist %MINGW_PATH%\msys.bat goto mingw_not_found
set ARCH=
set BOARD=
set COMPILER=
set SOURCES_ROOT=
if not "%1"=="-?" goto check_argument

:check_argument
if "%6"=="" (
    set ERROR_REASON=Argument error, you must input argument ARCH, BOARD and COMPILER.
    goto argument_error
)

if not "%7"=="" (
    set ERROR_REASON=Argument error, the argument 7 "%7" is invalid.
    goto argument_error
)

if not "%1"=="ARCH" (
    if not "%1"=="arch" (
        if not "%1"=="BOARD" (
            if not "%1"=="board" (
                if not "%1"=="COMPILER" (
                    if not "%1"=="compiler" (
                        set ERROR_REASON=The argument %1 was not supported.
                        goto argument_error
                    )
                )
            )
        )
    )
)

if not "%3"=="ARCH" (
    if not "%3"=="arch" (
        if not "%3"=="BOARD" (
            if not "%3"=="board" (
                if not "%3"=="COMPILER" (
                    if not "%3"=="compiler" (
                        set ERROR_REASON=The argument %3 was not supported.
                        goto argument_error
                    )
                )
            )
        )
    )
)

if not "%5"=="ARCH" (
    if not "%5"=="arch" (
        if not "%5"=="BOARD" (
            if not "%5"=="board" (
                if not "%5"=="COMPILER" (
                    if not "%5"=="compiler" (
                        set ERROR_REASON=The argument %5 was not supported.
                        goto argument_error
                    )
                )
            )
        )
    )
)


REM **********************************************************
:check_arch
if "%1"=="ARCH" (
    set ERROR_REASON=The parameter of argument ARCH is NULL.
    if "%2"=="" goto argument_error;
    set ARCH=%2
    goto check_board
)

if "%3"=="ARCH" (
    set ERROR_REASON=The parameter of argument ARCH is NULL.
    if "%4"=="" goto argument_error;
    set ARCH=%4
    goto check_board
)

if "%5"=="ARCH" (
    set ERROR_REASON=The parameter of argument ARCH is NULL.
    if "%6"=="" goto argument_error;
    set ARCH=%6
    goto check_board
)

if "%1"=="arch" (
    set ERROR_REASON=The parameter of argument ARCH is NULL.
    if "%2"=="" goto argument_error;
    set ARCH=%2
    goto check_board
)

if "%3"=="arch" (
    set ERROR_REASON=The parameter of argument ARCH is NULL.
    if "%4"=="" goto argument_error;
    set ARCH=%4
    goto check_board
)

if "%5"=="arch" (
    set ERROR_REASON=The parameter of argument ARCH is NULL.
    if "%6"=="" goto argument_error;
    set ARCH=%6
    goto check_board
)

set ERROR_REASON=The argument ARCH was not found.
goto argument_error;

REM **********************************************************
:check_board
if "%1"=="BOARD" (
    set ERROR_REASON=The parameter of argument BOARD is NULL.
    if "%2"=="" goto argument_error;
    set BOARD=%2
    goto check_compiler
)

if "%3"=="BOARD" (
    set ERROR_REASON=The parameter of argument BOARD is NULL.
    if "%4"=="" goto argument_error;
    set BOARD=%4
    goto check_compiler
)

if "%5"=="BOARD" (
    set ERROR_REASON=The parameter of argument BOARD is NULL.
    if "%6"=="" goto argument_error;
    set BOARD=%6
    goto check_compiler
)

if "%1"=="board" (
    set ERROR_REASON=The parameter of argument BOARD is NULL.
    if "%2"=="" goto argument_error;
    set BOARD=%2
    goto check_compiler
)

if "%3"=="board" (
    set ERROR_REASON=The parameter of argument BOARD is NULL.
    if "%4"=="" goto argument_error;
    set BOARD=%4
    goto check_compiler
)

if "%5"=="board" (
    set ERROR_REASON=The parameter of argument BOARD is NULL.
    if "%6"=="" goto argument_error;
    set BOARD=%6
    goto check_compiler
)

set ERROR_REASON=The argument BOARD was not found.
goto argument_error;


REM **********************************************************
:check_compiler
if "%1"=="COMPILER" (
    set ERROR_REASON=The parameter of argument COMPILER is NULL.
    if "%2"=="" goto argument_error;
    set COMPILER=%2
    goto environment_set
)

if "%3"=="COMPILER" (
    set ERROR_REASON=The parameter of argument COMPILER is NULL.
    if "%4"=="" goto argument_error;
    set COMPILER=%4
    goto environment_set
)

if "%5"=="COMPILER" (
    set ERROR_REASON=The parameter of argument COMPILER is NULL.
    if "%6"=="" goto argument_error;
    set COMPILER=%6
    goto environment_set
)

if "%1"=="compiler" (
    set ERROR_REASON=The parameter of argument COMPILER is NULL.
    if "%2"=="" goto argument_error;
    set COMPILER=%2
    goto environment_set
)

if "%3"=="compiler" (
    set ERROR_REASON=The parameter of argument COMPILER is NULL.
    if "%4"=="" goto argument_error;
    set COMPILER=%4
    goto environment_set
)

if "%5"=="compiler" (
    set ERROR_REASON=The parameter of argument COMPILER is NULL.
    if "%6"=="" goto argument_error;
    set COMPILER=%6
    goto environment_set
)
set ERROR_REASON=The argument COMPILER was not found.
goto argument_error;


REM **********************************************************
:environment_set
set SCRIPT_TEMP_PATH=%~f0
set SOURCES_DRIVER=%SCRIPT_TEMP_PATH:~,1%
pushd %~f0\..\..\..\..
set SOURCES_ROOT_TEMP=/%SOURCES_DRIVER%%cd:~2%
set SOURCES_ROOT=%SOURCES_ROOT_TEMP:\=/%
popd
pushd %~f0\..\..\..\..\..\..\tools\Bochs
set BXSHARE=%cd%
popd
set ERROR_REASON=
set SCRIPT_TEMP_PATH=
set SOURCES_DRIVER=
set SOURCES_ROOT_TEMP=
call %MINGW_PATH%\msys.bat
goto task_over

:argument_error
echo %ERROR_REASON%
echo Please input %~nx0 -? to show help.
goto task_over

:mingw_not_found
echo Environment variable MINGW_PATH not defined !
echo Please define it and retry.

:task_over
pause