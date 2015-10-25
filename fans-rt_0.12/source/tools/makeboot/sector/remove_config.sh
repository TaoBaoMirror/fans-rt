if [ -d "../" ]; then
	echo Remove directory [../]
	rmdir --ignore-fail-on-non-empty ../
	echo Remove directory [/G/fans-rt/fans-rt_0.12/source/../targets/x86/gcc/sector]
	rmdir --ignore-fail-on-non-empty /G/fans-rt/fans-rt_0.12/source/../targets/x86/gcc/sector
	echo Remove directory [/G/fans-rt/fans-rt_0.12/source/../depends/x86/gcc/sector]
	rmdir --ignore-fail-on-non-empty /G/fans-rt/fans-rt_0.12/source/../depends/x86/gcc/sector
	echo Remove directory [/G/fans-rt/fans-rt_0.12/source/../targets/tools/sector]
	rmdir --ignore-fail-on-non-empty /G/fans-rt/fans-rt_0.12/source/../targets/tools/sector
fi
