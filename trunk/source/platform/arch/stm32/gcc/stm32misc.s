
  .syntax unified
	.cpu cortex-m3
	.fpu softvfp
	.thumb
    .section .text
    .align 4

    .global CORE_SystemHang
    .global CORE_SaveIRQFlags
    .global CORE_DisableIRQ
    .global CORE_RestoreIRQ
    .global CORE_EnableIRQ
    .global CORE_IdleEntry
    .type	CORE_SystemHang, %function
CORE_SystemHang:
    b       .

    .type	CORE_SaveIRQFlags, %function
CORE_SaveIRQFlags:
    mrs     r0,    primask
    bx      lr

    .type	CORE_DisableIRQ, %function
CORE_DisableIRQ:
    mrs     R0,    primask
    cpsid   i
    bx      lr

    .type	CORE_RestoreIRQ, %function
CORE_RestoreIRQ:
    msr     primask, r0
    bx      lr

    .type	CORE_EnableIRQ, %function
CORE_EnableIRQ:
    mov     r0,     #0
    MSR     primask, r0
    BX      lr

    .type	CORE_IdleEntry, %function
CORE_IdleEntry:
    wfi
    bx      lr

.end

