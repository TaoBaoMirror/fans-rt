  .syntax unified
	.cpu cortex-m3
	.fpu softvfp
	.thumb

    .section .text
    .align 4
    
    .global FW_SystemCall
    .type	FW_SystemCall, %function
FW_SystemCall:
    svc     #0
    bx      lr

    .global FW_StackPosition
    .type	FW_StackPosition, %function
FW_StackPosition:
    mov     r0,    r13
    bx      lr

    .global FW_SystemIdle
    .type	FW_SystemIdle, %function
FW_SystemIdle:
    wfi
    bx      lr

.end

