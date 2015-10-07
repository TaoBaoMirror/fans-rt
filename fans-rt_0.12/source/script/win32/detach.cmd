if not "%1" == "" set BXSHARE=%1
if "%BXSHARE%" == "" set BXSHARE=g:\fans-rt\tools\bochs
pushd %BXSHARE%\..\..\virtualmachine\images
echo select vdisk file=%cd%\fans-i386.vhd > .\detach.script
echo detach vdisk >> .\detach.script
diskpart /s .\detach.script
del .\detach.script
popd
