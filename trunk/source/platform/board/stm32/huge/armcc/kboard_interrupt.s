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
;     Hardware supported double stack           YES
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
    IMPORT  CORE_GetCurrentPermission

    PRESERVE8

    AREA    |.text|, CODE, READONLY
    ALIGN 4
    THUMB

CORE_Switch2UserMode   PROC
    PUSH    {LR}                        ;  Save LR to call sub function
    BL      CORE_GetCoreStackPosition   ;  Get the position of core stack
    MRS     R1,     MSP                 ;
    MSR     PSP,    R1                  ;  User stack is the current stack
    MSR     MSP,    R0                  ;  Set the core stack position
    MOV     R0,     #0                  ;
    MSR     PRIMASK, R0                 ;  Enable IRQ
    MOV     R0,     #0x3                ;
    MSR     CONTROL,    R0              ;  Switch to user mode
    ISB                                 ;
    POP     {PC}                        ;  return
    ENDP


PendSV_Handler  PROC
    BX      LR                          ;  Not support PendSV IRQ
    ENDP

SVC_Handler     PROC
    CPSID   I                           ;  Why to disable IRQ ? Guess !
    PUSH    {R4, LR}                    ;  Why to push R4? Guess !
    PUSH    {R0 - R3}                   ;  Save the parameter of service
    BL      CORE_EnterIRQ               ;  Set current interrupt nest layer
    POP     {R0 - R3}                   ;  Reload the parameter of service
    CPSIE   I                           ;
    BL      CORE_HandlerLPC             ;  Call system service
    CPSID   I                           ;
    POP     {R4}                        ;  Stack must be aligned to 64 bit, so push R4
    B       ST_L0                       ;  The next step same as systick
    ENDP

SysTick_Handler PROC
    CPSID   I                           ;  Why to disable IRQ ? Guess !
    PUSH    {R4, LR}                    ;  Why to push R0?
    BL      CORE_EnterIRQ               ;  Set current interrupt nest layer
    BL      CORE_TickHandler            ;  Inc the system tick
    CPSIE   I                           ;
    BL      CORE_TaskScheduling         ;  Find the new task will be scheduling
    CPSID   I                           ;
    POP     {R4}                        ;  Stack must be aligned to 64 bit, so push R0
ST_L0
    BL      CORE_LeaveIRQ               ;  Set and get current interrupt nest layer
    CBNZ    R0,     ST_LE               ;  Nest layer != 0 then leave this interrupt
    BL      CORE_CheckMustbeSchedule    ;  Check need schedule
    CBZ     R0,     ST_LE               ;  Must schedule = FALSE then leave this interrupt
    BL      CORE_GetCurrentPermission   ;  Check the permission of current task
    MOV     R2,     R0                  ;  R3 = Permission
    POP     {LR}
    MRS     R0,     MSP                 ;  R0 = Core stack for old task
    MRS     R1,     PSP                 ;  R1 = User stack for old task
    CBNZ    R2,     ST_L1               ;  if Permission != CORE then goto ST_L1
    STMFD   R0!,    {LR, R4 - R12}      ;  Save core stack for the old task
    MSR     MSP,    R0                  ;  Update core stack
    B       ST_L2
ST_L1
    STMFD   R1!,    {LR, R4 - R12}      ;  Save user stack for the old task
    MSR     PSP,    R1                  ;  Update user stack
ST_L2
    BL      CORE_SwitchTask             ;  CORE_SwitchTask(CoreStack, UserStack);
    MOV     R4,     R0                  ;  R4 = The permission of the new task
    BL      CORE_GetTaskStackPosition   ;
    MOV     R1,     R0                  ;  R1 = User stack for new task
    BL      CORE_GetCoreStackPosition   ;  R0 = Core stack for new task
    CBNZ    R4,     PV_L3               ;  R4 = 0  Current task is core task
    LDMFD   R0!,    {LR, R4 - R12}      ;  Load core stack for the new task(core)
    B       PV_L4
PV_L3
    LDMFD   R1!,    {LR, R4 - R12}      ;  Load user stack for the new task(user)
PV_L4
    MSR     PSP,    R1                  ;  Update user stack
    MSR     MSP,    R0                  ;  Update core stack
    PUSH    {LR}                        ;  Because POP PC to return
ST_LE                                   ;
    CPSIE   I                           ;
    POP     {PC}                        ;  Return to task break point
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