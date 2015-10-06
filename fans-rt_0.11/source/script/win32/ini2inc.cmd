@echo off
echo. > %2.tmp
for /f "eol=; skip=1 tokens=1,2 delims== " %%i in (%1) do if not "%%j" == "" echo %%i EQU %%j >> %2.tmp
echo    END >> %2.tmp
echo. >> %2.tmp
if not exist %2 goto copy_file
fc %2.tmp %2 > NUL
if "%ERRORLEVEL%"=="0" goto finish_break
:copy_file
echo Now update file %2 ...
copy /y %2.tmp %2 > NUL
:finish_break
del /q %2.tmp