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
    IMPORT  CORE_GetCoreStackPosition
    IMPORT  CORE_GetTaskStackPosition
    IMPORT  CORE_GetCurrentPermission

    PRESERVE8

    AREA    |.text|, CODE, READONLY
    ALIGN 4
    THUMB

NVIC_CONTROL    EQU     0xE000ED04
NVIC_PEND_SET   EQU     0x10000000

CORE_Switch2UserMode   PROC
    PUSH    {LR}
    BL      CORE_GetCoreStackPosition
    MRS     R1,     MSP
    MSR     PSP,    R1
    MSR     MSP,    R0
    MOV     R0,     #0
    MSR     PRIMASK, R0
    MOV     R0,     #0x3
    MSR     CONTROL,    R0
    ISB
    POP     {PC}
    ENDP

PendSV_Handler  PROC
    CPSID   I                           ;  Why to disable IRQ ? Guess !
    PUSH    {LR}
    BL      CORE_GetCurrentPermission
    MOV     R3,     R0                  ;  R3 = Permission
    POP     {LR}
    MRS     R0,     MSP                 ;  R0 = Core stack for old task
    MRS     R1,     PSP                 ;  R1 = User stack for old task
    CBNZ    R3,     PV_L1               ;  R3 = 0  Current task is core task
    STMFD	R0!,    {LR, R4 - R12}      ;  Save core stack for the old task
    MSR     MSP,    R0                  ;  Adjust core stack
    B       PV_L2
PV_L1
    STMFD	R1!,    {LR, R4 - R12}      ;  Save user stack for the old task
PV_L2    
    BL      CORE_SwitchTask             ;  CORE_SwitchTask(CoreStack, UserStack);
    MOV     R4,     R0                  ;  R4 = Permission
    BL      CORE_GetTaskStackPosition
    MOV     R1,     R0                  ;  R1 = User stack for new task
    BL      CORE_GetCoreStackPosition   ;  R0 = Core stack for new task
    CBNZ    R4,     PV_L3               ;  R4 = 0  Current task is core task
    LDMFD	R0!,    {LR, R4 - R12}      ;  Load core stack for the new task(core)
    B       PV_L4
PV_L3
    LDMFD	R1!,    {LR, R4 - R12}      ;  Load user stack for the new task(user)
PV_L4
    MSR     PSP,    R1                  ;  Update user stack
    MSR     MSP,    R0                  ;  Update core stack
    CPSIE   I
    BX      LR
    ENDP

SVC_Handler     PROC
    CPSID   I
    PUSH    {LR}
    PUSH    {R4}
    PUSH    {R0, R1, R2, R3}
    BL      CORE_EnterIRQ
    POP     {R0, R1, R2, R3}
    CPSIE   I

    BL      CORE_HandlerLPC                 ; 进入中断处理程序

    CPSID   I
    BL      CORE_LeaveIRQ
    POP     {R4}
    CPSIE   I
    POP     {PC}
    ENDP
    
SysTick_Handler PROC
    CPSID   I
    PUSH    {LR}
    PUSH    {R4}
    BL      CORE_EnterIRQ
    BL      CORE_TickHandler
    CPSIE   I
    BL      CORE_TaskScheduling             ; 进入中断处理程序
    CPSID   I
    BL      CORE_LeaveIRQ
    POP     {R4}
    CPSIE   I
    POP     {PC}
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