@echo off
echo. > %2
for /f "eol=; skip=1 tokens=1,2 delims== " %%i in (%1) do echo %%i EQU %%j >> %2
echo    END >> %2
echo. >> %2