pushd %~dp0
pushd ..\..\platform\board
del /s /q kirq_define_enum.inc
del /s /q kirq_define_name.h
del /s /q kirq_define_enum.h
del /s /q kboard_name.h
del ../../include/version.h
popd
popd