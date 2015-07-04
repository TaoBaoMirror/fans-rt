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
    MOV     R0,     #0
    MSR     PRIMASK, R0
    BX      LR
    ENDP


PendSV_Handler  PROC
    BX      LR
    ENDP

SVC_Handler     PROC
    CPSID   I                           ;  Why to disable IRQ ? Guess !
    PUSH    {LR}                        ;
    PUSH    {R4, R0-R3}                 ;  Why to push R4? Guess !
    BL      CORE_EnterIRQ               ;  Set current interrupt nest layer
    POP     {R0-R3}                     ;  Where is R4?
    CPSIE   I                           ;
    BL      CORE_HandlerLPC             ;  Call system service
    CPSID   I                           ;
    POP     {R4}                        ;  Stack must be aligned to 64 bit, so push R4
    BL      CORE_LeaveIRQ               ;  Set and get current interrupt nest layer
    CBNZ    R0,     SV_LE               ;  Nest layer != 0 then leave this interrupt
    BL      CORE_CheckMustbeSchedule    ;  Check need schedule
    CBZ     R0,     SV_LE               ;  Must schedule = FALSE then leave this interrupt
    PUSH    {R4-R12}                    ;  nest layer = 0 and Must schedule = TRUE then scheduling
    MRS     R0,     MSP                 ;  R0 = Core stack for old task
    MRS     R1,     PSP                 ;  R1 = User stack for old task
    BL      CORE_SwitchTask             ;  CORE_SwitchTask(CoreStack, UserStack);
                                        ;  No need check the task permssion
    BL      CORE_GetTaskStackPosition   ;
    MOV     R1,     R0                  ;  R1 = User stack for new task
    BL      CORE_GetCoreStackPosition   ;  R0 = Core stack for new task
    MSR     PSP,    R1                  ;  Update user stack
    MSR     MSP,    R0                  ;  Update core stack
    POP     {R4-R12}                    ;  Restore new task registers
SV_LE                                   ;
    CPSIE   I                           ;
    POP     {PC}                        ;  Return to task break point
    ENDP

SysTick_Handler PROC
    CPSID   I                           ;  Why to disable IRQ ? Guess !
    PUSH    {R0, LR}                    ;  Why to push R0?
    BL      CORE_EnterIRQ               ;  Set current interrupt nest layer
    BL      CORE_TickHandler            ;  Inc the system tick
    CPSIE   I                           ;
    BL      CORE_TaskScheduling         ;  Find the new task will be scheduling
    CPSID   I                           ;
    POP     {R0}                        ;  Stack must be aligned to 64 bit, so push R0
    BL      CORE_LeaveIRQ               ;  Set and get current interrupt nest layer
    CBNZ    R0,     ST_LE               ;  Nest layer != 0 then leave this interrupt
    BL      CORE_CheckMustbeSchedule    ;  Check need schedule
    CBZ     R0,     ST_LE               ;  Must schedule = FALSE then leave this interrupt
    PUSH    {R4-R12}                    ;  nest layer = 0 and Must schedule = TRUE then scheduling
    MRS     R0,     MSP                 ;  R0 = Core stack for old task
    MRS     R1,     PSP                 ;  R1 = User stack for old task
    BL      CORE_SwitchTask             ;  CORE_SwitchTask(CoreStack, UserStack);
                                        ;  No need check the task permssion
    BL      CORE_GetTaskStackPosition   ;
    MOV     R1,     R0                  ;  R1 = User stack for new task
    BL      CORE_GetCoreStackPosition   ;  R0 = Core stack for new task
    MSR     PSP,    R1                  ;  Update user stack
    MSR     MSP,    R0                  ;  Update core stack
    POP     {R4-R12}                    ;  Restore new task registers
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