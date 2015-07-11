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
;    2015-07-07     JiangYong       rename to kboard_interrupt.s
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
    PUSH    {LR, R12}                   ;  Save the parameter of service
    BL      CORE_EnterIRQ               ;  Set current interrupt nest layer
    MOV     R12,    SP                  ;  SP - 8 = Offset of {R0 - R3}
    CBNZ    R0,     SV_L0
    PUSH    {R4 - R11}
    MOV     R11,    SP
    BL      CORE_GetCurrentPermission   ;  Core task no need set core stack position
    CBZ     R0,     SV_L0
    BL      CORE_GetCoreStackPosition   ;  R0 = Core stack for new task
    MOV     SP,     R0
SV_L0
    ADD     R12,    #8                  ;  R12 = Offset of {R0 - R3}
    LDMFD   R12,    {R0-R3}             ;  Load user stack for the new task(user)
    SUB     R12,    #8
    CPSIE   I                           ;
    BL      CORE_HandlerLPC             ;  Call system service
    CPSID   I                           ;
    BL      CORE_LeaveIRQ               ;  Set and get current interrupt nest layer
    CBNZ    R0,     SV_LE
    BL      CORE_CheckMustbeSchedule    ;  Check need schedule
    CBZ     R0,     SV_L1
    MOV     R1,     R11                 ;  R11 = User stack position
    MOV     R0,     SP
    BL      CORE_SwitchTask             ;  CORE_SwitchTask(CoreStack, UserStack);
    MOV     R4,     R0                  ;
    BL      CORE_GetCoreStackPosition   ;
    MOV     R11,    R0                  ;
    CBZ     R4,     SV_L1               ;
    BL      CORE_GetTaskStackPosition   ;
    MOV     R11,    R0
SV_L1
    MOV     SP,     R11
    POP     {LR, R4 - R12}
    CPSIE   I
    BX      LR
SV_LE
    MOV     SP,     R12
    POP     {LR, R12}
    CPSIE   I
    BX      LR
    ENDP

SysTick_Handler PROC
    CPSID   I                           ;  Why to disable IRQ ? Guess !
    PUSH    {LR, R12}                   ;  Save the parameter of service
    BL      CORE_EnterIRQ               ;  Set current interrupt nest layer
    MOV     R12,    SP                  ;  SP - 8 = Offset of {R0 - R3}
    CBNZ    R0,     ST_L0
    PUSH    {R4 - R11}
    MOV     R11,    SP
    BL      CORE_GetCurrentPermission   ;  Core task no need set core stack position
    CBZ     R0,     ST_L0
    BL      CORE_GetCoreStackPosition   ;  R0 = Core stack for new task
    MOV     SP,     R0
ST_L0
    BL      CORE_TickHandler            ;  Inc the system tick
    CPSIE   I                           ;
    BL      CORE_TaskScheduling         ;  Find the new task will be scheduling
    CPSID   I                           ;
    BL      CORE_LeaveIRQ               ;  Set and get current interrupt nest layer
    CBNZ    R0,     ST_LE
    BL      CORE_CheckMustbeSchedule    ;  Check need schedule
    CBZ     R0,     ST_L1
    MOV     R1,     R11                 ;  R11 = User stack position
    MOV     R0,     SP
    BL      CORE_SwitchTask             ;  CORE_SwitchTask(CoreStack, UserStack);
    MOV     R4,     R0                  ;
    BL      CORE_GetCoreStackPosition   ;
    MOV     R11,    R0                  ;
    CBZ     R4,     ST_L1               ;
    BL      CORE_GetTaskStackPosition   ;
    MOV     R11,    R0
ST_L1
    MOV     SP,     R11
    POP     {LR, R4 - R12}
    CPSIE   I
    BX      LR
ST_LE
    MOV     SP,     R12
    POP     {LR, R12}
    CPSIE   I
    BX      LR
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