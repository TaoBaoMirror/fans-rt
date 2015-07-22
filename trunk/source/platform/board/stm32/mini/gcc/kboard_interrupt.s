/*
*    Copyright(C) 2013-2015, Fans-rt development team.
*
*    All rights reserved.
*
*    This is open source software.
*    Learning and research can be unrestricted to  modification, use and dissemination.
*    If you need for commercial purposes, you should get the author's permission.
*
*    Configuration:
*     System global core stack                  NO
*     The local core stack of general task      YES
*     The loacl core stack of kernel task       YES
*     The local user stack of general task      NO
*     Hardware supported task switch IRQ        YES
*     Hardware supported double stack           NO
*
*    date           author          notes
*    2015-07-19     JiangYong       new file
*/
.include "kirq_define_enum.inc"
.extern  CORE_EnterIRQ
.extern  CORE_LeaveIRQ
.extern  CORE_TickHandler
.extern  CORE_TaskScheduling
.extern  CORE_HandlerLPC
.extern  CORE_SwitchTask
.extern  CORE_GetCoreStackPosition
.extern  CORE_GetTaskStackPosition
.extern  CORE_GetCurrentPermission


  .syntax unified
	.cpu cortex-m3
	.fpu softvfp
	.thumb
    .section .text
    .align 4

.equ    NVIC_CONTROL,   0xE000ED04
.equ    NVIC_PEND_SET,  0x10000000

    .global CORE_Switch2UserMode
    .type	CORE_Switch2UserMode, %function
CORE_Switch2UserMode:
    MOV     R0,     #0
    MSR     PRIMASK, R0
    BX      LR

    .global PendSV_Handler
    .type	PendSV_Handler, %function
PendSV_Handler:
    CPSID   I                           /*  Why to disable IRQ ? Guess ! */
    PUSH    {R4-R12, LR}                /*  Why to push R12 ? */
    MRS     R0,     MSP                 /*  R0 = Core stack for old task */
    MRS     R1,     PSP                 /*  R1 = User stack for old task */
    BL      CORE_SwitchTask             /*  CORE_SwitchTask(CoreStack, UserStack) */
                                        /*  No need check the task permssion */
    BL      CORE_GetTaskStackPosition   /*  */
    MOV     R1,     R0                  /*  R1 = User stack for new task */
    BL      CORE_GetCoreStackPosition   /*  R0 = Core stack for new task */
    MSR     PSP,    R1                  /*  Update user stack */
    MSR     MSP,    R0                  /*  Update core stack */
    POP     {R4-R12, LR}                /*  Stack must be aligned to 64 bit, so push R12 */
    CPSIE   I                           /*  */
    BX      LR                          /*  Return to the break point of the new task  */

    .global SVC_Handler
    .type	SVC_Handler, %function
SVC_Handler:
    CPSID   I                           /*  Why to disable IRQ ? Guess ! */
    PUSH    {R4, LR}                    /*  Why to push R4 ? Guess ! */
    PUSH    {R0-R3}                     /*  Save the service parameter to stack */
    BL      CORE_EnterIRQ               /*  Enter Interrupt Critiacl */
    POP     {R0-R3}                     /*  Restore the service permeter from stack */
    CPSIE   I                           /*  */
                                        /*  */
    BL      CORE_HandlerLPC             /*  Call system service */
                                        /*  */
    CPSID   I                           /*  */
    BL      CORE_LeaveIRQ               /*  Leave Interrupt Critiacl */
    POP     {R4, LR}                    /*  Stack must be aligned to 64 bit, so push R4 */
    CPSIE   I                           /*  */
    BX      LR                          /*  Return to the break point of this task  */

    .global SysTick_Handler
    .type	SysTick_Handler, %function
SysTick_Handler:
    CPSID   I                           /*  Why to disable IRQ ? Guess ! */
    PUSH    {R4, LR}                    /*  The LR must be push, but the */
                                        /*  stack is not aligned to 64 bit */
    BL      CORE_EnterIRQ               /*  Enter Interrupt Critiacl */
    BL      CORE_TickHandler            /*  Inc the system tick */
    CPSIE   I                           /*  */
    BL      CORE_TaskScheduling         /*  Find the new task will be scheduling */
    CPSID   I                           /*  */
    BL      CORE_LeaveIRQ               /*  Leave Interrupt Critiacl */
    POP     {R4, LR}                    /*  Pop registers */
    CPSIE   I                           /*  */
    BX      LR                          /*  Return to the break point of this task  */

    .global HardFault_Handler
    .type	HardFault_Handler, %function
HardFault_Handler:
    B       .

    .global MemManage_Handler
    .type	MemManage_Handler, %function
MemManage_Handler:
    B       .
 
    .global BusFault_Handler
    .type	BusFault_Handler, %function
BusFault_Handler:
    B       .

    .global UsageFault_Handler
    .type	UsageFault_Handler, %function
UsageFault_Handler:
    B       .

.end
