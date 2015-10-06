@echo off
pushd %~dp0
path=%path%;%cd%
if not "%7"=="" goto error_count
if "%6"=="" goto error_count
set %1=%2 > NUL
set %3=%4 > NUL
set %5=%6 > NUL
set ERROR_REASON=ARCH
set ERROR_EXAMPLE=stm32
if "%ARCH%"=="" goto error_param
set ERROR_REASON=BOARD
set ERROR_EXAMPLE=tiny
if "%BOARD%"=="" goto error_param
set ERROR_REASON=COMPILER
set ERROR_EXAMPLE=armcc
if "%COMPILER%"=="" goto error_param
for %%i in (%ARCH%) do set ARCH=%%i
for %%i in (%BOARD%) do set BOARD=%%i
for %%i in (%COMPILER%) do set COMPILER=%%i
set path_source=..\..\platform\board\%ARCH%\%BOARD%\kirq_define.ini
set path_enum_inc=..\..\platform\board\%ARCH%\%BOARD%\%COMPILER%\kirq_define_enum.inc
set path_enum_h=..\..\platform\board\%ARCH%\%BOARD%\kirq_define_enum.h
set path_name_h=..\..\platform\board\%ARCH%\%BOARD%\kirq_define_name.h
set path_version_ini=..\..\version.ini
set path_version_h=..\..\include\version.h
set path_board_name_h=..\..\platform\board\%ARCH%\%BOARD%\kboard_name.h

call ini2inc.cmd %path_source% %path_enum_inc%
call ini2enum.cmd %path_source% %path_enum_h%
call ini2name.cmd %path_source% %path_name_h%
call ini2ver.cmd %path_version_ini% %path_version_h%
if not exist %path_board_name_h% echo #define    BOARD_NAME_STRING    "%ARCH%-%BOARD%" > %path_board_name_h%
goto finish_break

:error_file
echo *****ERROR: Could not found file %path_source%
goto show_infor
:error_param
echo *****ERROR: Please assign a %ERROR_REASON% to build, Eg. %~nx0 %ERROR_REASON%=%ERROR_EXAMPLE%
echo.
goto show_infor 
:error_count
echo *****ERROR: The command line parameter invalid.
echo.
:show_infor
echo   Keil make script for Fans-rt.
echo   Author : JiangYong
echo   Date   : 2015.07.19
echo   Usage  : %~nx0 ARCH=cpu name BOARD=board name COMPILER=compiler name
:finish_break
popd
set ERROR_REASON=
set ERROR_EXAMPLE=
set ARCH=
set BOARD=
set COMPILER=
