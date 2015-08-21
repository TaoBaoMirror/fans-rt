@echo off
echo. > %2.tmp
type %1 | find "[" > %1.section
type %1 | find "=" > %1.value
set macro_suffix_name=_ENUM_H
set enum_perfix_name=E_
set enum_suffix_name=_DEFINE
for /f "eol=; tokens=1 delims=[]" %%i in (%1.section) do set section_name=%%i
echo #ifndef __%section_name%%macro_suffix_name% >> %2.tmp
echo #define __%section_name%%macro_suffix_name% >> %2.tmp
echo typedef enum{ >> %2.tmp
for /f "eol=; tokens=1,2 delims== " %%i in (%1.value) do echo    %%i = %%j, >> %2.tmp
echo }%enum_perfix_name%%section_name%%enum_suffix_name%; >> %2.tmp
echo. >> %2.tmp
echo #endif >> %2.tmp
if not exist %2 goto copy_file
fc %2.tmp %2 > NUL
if "%ERRORLEVEL%"=="0" goto finish_break
:copy_file
echo Now update file %2 ...
copy /y %2.tmp %2 > NUL
:finish_break
del /q %2.tmp
del /q %1.section
del /q %1.value