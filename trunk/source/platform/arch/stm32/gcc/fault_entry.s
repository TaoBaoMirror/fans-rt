
.extern  CORE_EnterIRQ
.extern  CORE_LeaveIRQ
.extern  STM32_FaultHandler

  .syntax unified
	.cpu cortex-m3
	.fpu softvfp
	.thumb
    .section .text
    .align 4
    
    
    .type	Fault_Handler, %function
Fault_Handler:
    cpsid   i
    push    {lr}
    push    {r4-r11}
    push	{r12}
    mov     r0,     r13
    mov     r4,     r0
    mov     r5,     r1
    bl      CORE_EnterIRQ
    mov     r13,    r0
    cpsie   i

    mov     r1,     r5
    mov     r0,     r4
    bl      STM32_FaultHandler

    cpsid   i
    mov     r0,     r13
    bl      CORE_LeaveIRQ
    mov     r13,    r0
    pop		{r12}
    pop     {r4-r11}
    cpsie   i
    pop     {pc}

    .global HardFault_Handler
    .type	HardFault_Handler, %function
HardFault_Handler:
    mov     r1,     #0
    b       Fault_Handler

    .global MemManage_Handler
    .type	MemManage_Handler, %function
MemManage_Handler:
    mov     r1,     #1
    b       Fault_Handler

    .global BusFault_Handler
    .type	BusFault_Handler, %function
BusFault_Handler:
    mov     r1,     #2
    b       Fault_Handler

    .global  UsageFault_Handler
    .type	UsageFault_Handler, %function
UsageFault_Handler:
    mov     r1,     #3
    b       Fault_Handler

.end

