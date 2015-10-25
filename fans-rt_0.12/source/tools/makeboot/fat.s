[section .data]
[global	_boot_sectors]

times 512-(($-$$)&511)  db  0
_fat12_start	equ		$
incbin "fat12.bin"
_fat12_length	equ		$-_fat12_start

_fat16_start	equ		$
incbin "fat16.bin"
_fat16_length	equ		$-_fat16_start

_fat32_start	equ		$
incbin "fat32.bin"
_fat32_length	equ		$-_fat32_start

_boot_sectors:
dd	0x00000000,		0
dd	_fat12_start,	_fat12_length
dd	_fat16_start,	_fat16_length
dd	_fat32_start,	_fat32_length
