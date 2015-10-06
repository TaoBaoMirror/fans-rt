pushd %~dp0
pushd ..\..\platform\board
del /s /q kirq_define_enum.inc
del /s /q kirq_define_name.h
del /s /q kirq_define_enum.h
del /s /q kboard_name.h
del /s /q *.bak
del ..\..\include\version.h
popd
pushd ..\..\
del /s *.bak
del /s /q stm32-mini.gcc
del /s /q stm32-tiny.gcc
del /s /q stm32-huge.gcc
del /s /q stm32-soft.gcc
del /s /q stm32-lite.gcc
popd