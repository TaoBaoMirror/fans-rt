if not "%1" == "" set BXSHARE=%1
if "%BXSHARE%" == "" set BXSHARE=g:\fans-rt\tools\bochs
pushd %BXSHARE%\..\..\virtualmachine\images
echo select vdisk file=%cd%\fans-i386.vhd > .\attach.script
echo attach vdisk >> .\attach.script
diskpart /s .\attach.script
del .\attach.script
popd
