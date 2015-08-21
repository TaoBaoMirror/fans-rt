@echo off
type %1 | find "[" > %1.section
type %1 | find "=" > %1.value
for /f "eol=; tokens=1 delims=[]" %%i in (%1.section) do set section_name=%%i
for /f "eol=; tokens=1,2* delims==, " %%i in (%1.value) do if "%%i"=="MAJOR_VERSION" set /a %%i=%%j
for /f "eol=; tokens=1,2* delims==, " %%i in (%1.value) do if "%%i"=="MINOR_VERSION" set /a %%i=%%j
for /f "eol=; tokens=1,2* delims==, " %%i in (%1.value) do if "%%i"=="DEBUG_VERSION" set /a %%i=%%j+1

set ERROR_REASON=MAJOR_VERSION
if "%MAJOR_VERSION%"=="" goto error_break
set ERROR_REASON=MINOR_VERSION
if "%MINOR_VERSION%"=="" goto error_break
set ERROR_REASON=DEBUG_VERSION
if "%DEBUG_VERSION%"=="" goto error_break

echo #ifndef __H_%section_name% > %2
echo #define __H_%section_name% >> %2
echo. >>%2
echo #define     MAJOR_VERSION    %MAJOR_VERSION% >> %2
echo #define     MINOR_VERSION    %MINOR_VERSION% >> %2
echo #define     DEBUG_VERSION    %DEBUG_VERSION% >> %2
echo. >> %2
echo #endif >> %2
echo. >> %2
echo [%section_name%] > %1
echo MAJOR_VERSION = %MAJOR_VERSION% >> %1
echo MINOR_VERSION = %MINOR_VERSION% >> %1
echo DEBUG_VERSION = %DEBUG_VERSION% >> %1
echo. >> %1
goto finish_break
:error_break
echo *****ERROR: %ERROR_REASON% not found.
:finish_break
set ERROR_REASON=
set MAJOR_VERSION=
set MINOR_VERSION=
set DEBUG_VERSION=
del /q %1.section
del /q %1.value

