@echo off
pushd %~dp0
set fans_root_dir=..\..\..
if not exist %fans_root_dir%\libs\nul md %fans_root_dir%\libs
if not exist %fans_root_dir%\libs\thumb\nul md %fans_root_dir%\libs\thumb
if not exist %fans_root_dir%\libs\thumb\keil\nul md %fans_root_dir%\libs\thumb\keil
copy /y %fans_root_dir%\objects\stm32\keil\libfw\libfw.lib %fans_root_dir%\libs\thumb\keil\libfw.a > nul