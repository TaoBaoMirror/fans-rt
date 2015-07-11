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
;     The local user stack of general task      YES
;     Hardware supported task switch IRQ        NO
;     Hardware supported double stack           NO
;
;    date           author          notes
;    2015-06-25     JiangYong       new file
;    2015-07-11     JiangYong       soft stack mode finished
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
    IMPORT  CORE_GetCurrentPermission

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
    MOV     R0,     SP                  ;  R0 = Offset of {R0 - R3}
    PUSH    {LR, R12}                   ;  Why to push r12 ?
    MOV     R12,    R0                  ;  R12 = Offset of {R0 - R3}
    BL      CORE_EnterIRQ               ;  Set current interrupt nest layer
    CBNZ    R0,     SV_L0               ;  IRQ nest layer is not 0 then no need switch stack
    PUSH    {R4 - R11}                  ;  Save the other registers of current task
    MOV     R11,    SP                  ;  R11 = Break point
    BL      CORE_GetCurrentPermission   ;  Check the permission of current task
    CBZ     R0,     SV_L0               ;  If is core task no need switch to global core stack
    BL      CORE_GetCoreStackPosition   ;  R0 = Core stack for new task
    MOV     SP,     R0                  ;  switch to global core stack
SV_L0
    LDMFD   R12,    {R0-R3}             ;  Load user stack for the new task(user)
    SUB     R12,    #8                  ;  R12 is the stack point if the IRQ nest layer not 0
    CPSIE   I                           ;  Enable IRQ
    BL      CORE_HandlerLPC             ;  Call system service
    B       ST_L1                       ;  The next step same as system tick handler
    ENDP

SysTick_Handler PROC
    CPSID   I                           ;  Why to disable IRQ ? Guess !
    PUSH    {LR, R12}                   ;  Save the parameter of service
    BL      CORE_EnterIRQ               ;  Set current interrupt nest layer
    MOV     R12,    SP                  ;  SP - 8 = Offset of {R0 - R3}
    CBNZ    R0,     ST_L0               ;  IRQ nest layer is not 0 then no need switch stack
    PUSH    {R4 - R11}                  ;  Save the other registers of current task
    MOV     R11,    SP                  ;  R11 = Break point
    BL      CORE_GetCurrentPermission   ;  Check the permission of current task
    CBZ     R0,     ST_L0               ;  If is core task no need switch to global core stack
    BL      CORE_GetCoreStackPosition   ;  R0 = Core stack for new task
    MOV     SP,     R0                  ;  Switch to global core stack
ST_L0
    BL      CORE_TickHandler            ;  Inc the system tick
    CPSIE   I                           ;  Enable IRQ
    BL      CORE_TaskScheduling         ;  Find the new task will be scheduling
ST_L1
    CPSID   I                           ;  Disable IRQ
    BL      CORE_LeaveIRQ               ;  Set and get current interrupt nest layer
    CBNZ    R0,     ST_LE               ;  IRQ nest layer is not 0 then return
    BL      CORE_CheckMustbeSchedule    ;  Check need schedule
    CBZ     R0,     ST_L2               ;  If no need schedule then return to user stack(R11 = Break point)
    MOV     R1,     R11                 ;  R1 = User stack position
    MOV     R0,     SP                  ;  R0 = Core stack position 
    BL      CORE_SwitchTask             ;  CORE_SwitchTask(CoreStack, UserStack);
    MOV     R4,     R0                  ;  R4 = The permission of new task
    BL      CORE_GetCoreStackPosition   ;  R0 = Core stack for new task
    MOV     R11,    R0                  ;  R11 = Core stack for new task
    CBZ     R4,     ST_L2               ;  If the new task is core task, then scheduling
    BL      CORE_GetTaskStackPosition   ;  R0 = User stack for new task
    MOV     R11,    R0                  ;  R11 = User stack for new task
ST_L2
    MOV     SP,     R11                 ;  Switch stack to new task
    POP     {LR, R4 - R12}              ;  Resume break point
    CPSIE   I                           ;  Enable IRQ
    BX      LR                          ;  Scheduling new task
ST_LE
    MOV     SP,     R12                 ;  Switch to user stack(IRQ nest)
    POP     {LR, R12}                   ;  Resume the registers
    CPSIE   I                           ;  Enable IRQ
    BX      LR                          ;  Return to the original IRQ
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