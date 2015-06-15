.include "birq.inc"
.include "saveirq.inc"

.extern  CORE_EnterIRQ
.extern  CORE_LeaveIRQ
.extern  CORE_HandlerLPC

  .syntax unified
	.cpu cortex-m3
	.fpu softvfp
	.thumb
    .section .text
    .align 4

    .global PendSV_Handler
    .type	PendSV_Handler, %function
PendSV_Handler:
    bx      lr

    .global SVC_Handler
    .type	SVC_Handler, %function
SVC_Handler:
    cpsid   i
    push    {lr}
    push    {r4-r11}
    push	{r12}

    M_SAVE_IRQ 		#E_CORE_SYSCALL_IRQ_ID
    mov     r4,    r0
    mov     r5,    r1
    mov     r6,    r2
    mov     r0,    r13
    bl      CORE_EnterIRQ
    mov     r13,   r0
    cpsie   i

    mov     r0,    r4
    mov     r1,    r5
    mov     r2,    r6
    bl      CORE_HandlerLPC

    cpsid   i
    mov     r0,    r13
    bl      CORE_LeaveIRQ
    mov     r13,   r0
    pop		{r12}
    pop     {r4-r11}
    cpsie   i
    pop     {pc}

.end

