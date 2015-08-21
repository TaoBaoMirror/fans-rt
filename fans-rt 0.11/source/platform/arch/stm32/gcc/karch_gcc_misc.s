
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
    .global CORE_ActiveSwitchIRQ
    .global  CORE_InitArchFirst

.equ    NVIC_CONTROL,   0xE000ED04
.equ    NVIC_PEND_SET,  0x10000000

    .type	CORE_ActiveSwitchIRQ, %function

CORE_ActiveSwitchIRQ:
    PUSH    {R4, R5}
    LDR     R4,     =NVIC_CONTROL
    LDR     R5,     =NVIC_PEND_SET
    STR     R5,     [R4]
    ISB
    POP     {R4, R5}
    BX      LR
    NOP

    .type	CORE_SystemHang, %function
CORE_SystemHang:
    B       .

    .type	CORE_SaveIRQFlags, %function
CORE_SaveIRQFlags:
    MRS     R0,    PRIMASK
    BX      LR

    .type	CORE_DisableIRQ, %function
CORE_DisableIRQ:
    MRS     R0,    PRIMASK
    CPSID   I
    BX      LR

    .type	CORE_RestoreIRQ, %function
CORE_RestoreIRQ:
    MSR     PRIMASK, R0
    BX      LR

    .type	CORE_EnableIRQ, %function
CORE_EnableIRQ:
    MOV     R0,     #0
    MSR     PRIMASK, R0
    BX      LR

    .type	CORE_InitArchFirst, %function
CORE_InitArchFirst:
    bx  lr
.end

