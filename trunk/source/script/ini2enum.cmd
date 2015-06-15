@echo off
echo. > %2
type %1 | find "[" > %1.section
type %1 | find "=" > %1.value
for /f "eol=; tokens=1 delims=[]" %%i in (%1.section) do echo #ifndef __H_%%i >> %2
for /f "eol=; tokens=1 delims=[]" %%i in (%1.section) do echo #define __H_%%i >> %2
echo typedef enum{ >> %2
for /f "eol=; tokens=1,2 delims== " %%i in (%1.value) do echo     %%i = %%j, >> %2
for /f "eol=; tokens=1 delims=[]" %%i in (%1.section) do echo }%%i; >> %2
echo. >> %2
echo #endif >> %2
del /q %1.section
del /q %1.value
