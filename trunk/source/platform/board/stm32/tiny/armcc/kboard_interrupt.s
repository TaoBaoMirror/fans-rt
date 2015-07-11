;
;    Copyright(C) 2013-2015, Fans-rt development team.
;
;    All rights reserved.
;
;    This is open source software.
;    Learning and research can be unrestricted to  modification, use and dissemination.
;    If you need for commercial purposes, you should get the author's permission.
;
;    Configuration:
;     System global core stack                  NO
;     The local core stack of general task      YES
;     The loacl core stack of kernel task       YES
;     The local user stack of general task      NO
;     Hardware supported task switch IRQ        NO
;     Hardware supported double stack           NO
;
;    date           author          notes
;    2015-06-25     JiangYong       new file
;    2015-07-07     JiangYong       rename to kboard_interrupt.s
;    2015-07-11     JiangYong       code optimization
;
        INCLUDE kirq_define_enum.inc

    EXPORT  UsageFault_Handler
    EXPORT  BusFault_Handler
    EXPORT  MemManage_Handler
    EXPORT  HardFault_Handler
    EXPORT  SysTick_Handler
    EXPORT  PendSV_Handler
    EXPORT  SVC_Handler
    EXPORT  CORE_Switch2UserMode

    IMPORT  CORE_EnterIRQ
    IMPORT  CORE_LeaveIRQ
    IMPORT  CORE_TickHandler
    IMPORT  CORE_TaskScheduling
    IMPORT  CORE_HandlerLPC
    IMPORT  CORE_SwitchTask
    IMPORT  CORE_SetTaskStackPosition
    IMPORT  CORE_GetTaskStackPosition
    IMPORT  CORE_GetCoreStackPosition
    IMPORT  CORE_CheckMustbeSchedule

    PRESERVE8

    AREA    |.text|, CODE, READONLY
    ALIGN 4
    THUMB

CORE_Switch2UserMode   PROC
    MOV     R0,     #0                  ;
    MSR     PRIMASK, R0                 ;  Enable IRQ
    BX      LR                          ;  return
    ENDP


PendSV_Handler  PROC
    BX      LR                          ;  Not support PendSV IRQ
    ENDP

SVC_Handler     PROC
    CPSID   I                           ;  Why to disable IRQ ? Guess !
    PUSH    {LR, R12, R0-R3}            ;  Why to push 12? Guess !
    BL      CORE_EnterIRQ               ;  Set current interrupt nest layer
    POP     {R0 - R3}                   ;  Resume R0 - R3
    CPSIE   I                           ;  Enable IRQ
    BL      CORE_HandlerLPC             ;  Call system service
    B       ST_L1                       ;  The next step same as system tick handler
    ENDP

SysTick_Handler PROC
    CPSID   I                           ;  Why to disable IRQ ? Guess !
    PUSH    {R12, LR}                   ;  Why to push R0?
    BL      CORE_EnterIRQ               ;  Set current interrupt nest layer
    BL      CORE_TickHandler            ;  Inc the system tick
    CPSIE   I                           ;  Enable IRQ
    BL      CORE_TaskScheduling         ;  Find the new task will be scheduling
ST_L1
    CPSID   I                           ;  Disable IRQ
    BL      CORE_LeaveIRQ               ;  Set and get current interrupt nest layer
    CBNZ    R0,     ST_LE               ;  Nest layer != 0 then leave this interrupt
    BL      CORE_CheckMustbeSchedule    ;  Check need schedule
    CBZ     R0,     ST_LE               ;  Must schedule = FALSE then leave this interrupt
    PUSH    {R4 - R11}                  ;  nest layer = 0 and Must schedule = TRUE then scheduling
    MRS     R0,     MSP                 ;  R0 = Core stack for old task
    MRS     R1,     PSP                 ;  R1 = User stack for old task
    BL      CORE_SwitchTask             ;  CORE_SwitchTask(CoreStack, UserStack);
                                        ;  No need check the task permssion
    BL      CORE_GetTaskStackPosition   ;  R0 = User stack for new task(no need)
    MOV     R1,     R0                  ;  R1 = User stack for new task(no need)
    BL      CORE_GetCoreStackPosition   ;  R0 = Core stack for new task
    MSR     PSP,    R1                  ;  Update user stack(no need)
    MSR     MSP,    R0                  ;  Update core stack
    POP     {R4 - R11}                  ;  Restore new task registers
ST_LE
    POP     {R12, LR}                   ;  Resume break point
    CPSIE   I                           ;  Enable IRQ
    BX      LR                          ;  Return to task break point
    ENDP


HardFault_Handler   PROC
    B       .
    ENDP

MemManage_Handler   PROC
    B       .
    ENDP
BusFault_Handler    PROC
    B       .
    ENDP
UsageFault_Handler  PROC
    B       .
    ENDP
	
ALIGN
            END