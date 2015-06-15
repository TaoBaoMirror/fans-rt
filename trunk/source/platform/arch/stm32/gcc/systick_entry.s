.include "birq.inc"
.include "saveirq.inc"

.global     SysTick_Handler

.extern     CORE_EnterIRQ
.extern     CORE_LeaveIRQ
.extern     CORE_TickHandler
.extern     CORE_Scheduler
.extern     EXTI_Disable

  .syntax unified
	.cpu cortex-m3
	.fpu softvfp
	.thumb
    .section .text
    .align 4
    .type	SysTick_Handler, %function

SysTick_Handler:
    cpsid   i
    push    {lr}
    push    {r4-r11}
    push    {r12}

    M_SAVE_IRQ        #E_CORE_SYSTICK_IRQ_ID
    bl      CORE_TickHandler
    mov     r0,    r13
    bl      CORE_EnterIRQ
    mov     r13,   r0
    cpsie   i

    bl      CORE_Scheduler

    cpsid   i
    mov     r0,    r13
    bl      CORE_LeaveIRQ
    mov     r13,  r0
    pop		{r12}
    pop     {r4-r11}
    cpsie   i
    pop     {pc}

.end

