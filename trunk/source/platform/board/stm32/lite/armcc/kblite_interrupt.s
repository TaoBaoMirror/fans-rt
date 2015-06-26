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

    PRESERVE8

    AREA    |.text|, CODE, READONLY
    ALIGN 4
    THUMB

NVIC_CONTROL    EQU     0xE000ED04
NVIC_PEND_SET   EQU     0x10000000

CORE_Switch2UserMode   PROC
    PUSH    {LR}
    MRS     R0,     MSP
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
    CPSID   I
    MRS     R0,     PSP
    STMFD	R0!,    {LR, R4 - R12}
    BL      CORE_SwitchTask
    LDMFD	R0!,    {LR, R4 - R12}
    MSR     PSP,    R0
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