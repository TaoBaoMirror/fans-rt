if [ -d "/G/fans-rt/fans-rt_0.12/source/../objects/x86/gcc/libtools" ]; then
	echo Remove directory [/G/fans-rt/fans-rt_0.12/source/../objects/x86/gcc/libtools]
	rmdir --ignore-fail-on-non-empty /G/fans-rt/fans-rt_0.12/source/../objects/x86/gcc/libtools
	echo Remove directory [/G/fans-rt/fans-rt_0.12/source/../targets/x86/gcc/libtools]
	rmdir --ignore-fail-on-non-empty /G/fans-rt/fans-rt_0.12/source/../targets/x86/gcc/libtools
	echo Remove directory [/G/fans-rt/fans-rt_0.12/source/../depends/x86/gcc/libtools]
	rmdir --ignore-fail-on-non-empty /G/fans-rt/fans-rt_0.12/source/../depends/x86/gcc/libtools
	echo Remove directory [/G/fans-rt/fans-rt_0.12/source/../targets/tools/libtools]
	rmdir --ignore-fail-on-non-empty /G/fans-rt/fans-rt_0.12/source/../targets/tools/libtools
fi
