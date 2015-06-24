	INCLUDE kirq_define_enum.inc

    EXPORT  UsageFault_Handler
    EXPORT  BusFault_Handler
    EXPORT  MemManage_Handler
    EXPORT  HardFault_Handler
    EXPORT  PendSV_Handler
    EXPORT  SVC_Handler
    EXPORT  SysTick_Handler
    EXPORT  CORE_EnableKernelStack

    IMPORT  CORE_EnterIRQ
    IMPORT  CORE_LeaveIRQ
    IMPORT  CORE_HandlerLPC
    IMPORT  CORE_TickHandler
    IMPORT  CORE_TaskScheduling
    IMPORT  STM32_FaultHandler
    IMPORT  CORE_SetTaskStackPosition
    IMPORT  CORE_GetTaskStackPosition
    IMPORT  CORE_GetCoreStackPosition

    PRESERVE8

    AREA    |.text|, CODE, READONLY
    ALIGN 4
    THUMB

CORE_EnableKernelStack   PROC
    MOV     R0,     #0
    MSR     PRIMASK, R0
    BX      LR
    ENDP
    
SysTick_Handler PROC
    CPSID   I
    PUSH    {LR}
    BL      CORE_TickHandler
    BL      CORE_EnterIRQ
    CBNZ    R0,     ST_L1
    PUSH    {R4-R12}
    MOV     R0,     R13                         ; R0 = Stack position
    BL      CORE_SetTaskStackPosition
ST_L1
    CPSIE   I
    BL      CORE_TaskScheduling
    CPSID   I
    BL      CORE_LeaveIRQ
    CBNZ    R0,     ST_L2
    BL      CORE_GetTaskStackPosition
    MOV     R13,    R0
    POP     {R4-R12}
ST_L2
    CPSIE   I
    POP     {PC}
    ENDP

PendSV_Handler  PROC
    BX      LR
    ENDP

SVC_Handler     PROC
    CPSID   I
    PUSH    {LR}
    PUSH    {R8-R12}
    MOV     R8,     R0                          ; 暂存入参 1
    MOV     R9,     R1                          ; 暂存入参 2
    MOV     R10,    R2                          ; 暂存入参 3
    BL      CORE_EnterIRQ
    CBNZ    R0,     SV_L1
    PUSH    {R4-R7}
    MOV     R0,     R13                         ; R0 = Stack position
    BL      CORE_SetTaskStackPosition
    SUB     R13,    #16
SV_L1
    MOV     R0,     R8
    MOV     R1,     R9
    MOV     R2,     R10
    POP     {R8-R11}
    CPSIE   I

    BL      CORE_HandlerLPC
    
    CPSID   I
    BL      CORE_LeaveIRQ
    CBNZ    R0,     SV_L2
    BL      CORE_GetTaskStackPosition
    MOV     R13,    R0
    POP     {R4-R12}
SV_L2
    CPSIE   I
    POP     {PC}
    ENDP
 
Fault_Handler PROC
    CPSID   I
    PUSH    {LR}
    PUSH    {R4-R12}
    MRS     R0,     PSP                     ; 堆栈指针作为入参
    MOV     R4,     R0
    MOV     R5,     R1
    BL      CORE_EnterIRQ                   ; 保存堆栈指针
    MOV     R13,    R0                      ; 使用内核堆栈
    CPSIE   I

    MOV     R1,     R5
    MOV     R0,     R4
    BL      STM32_FaultHandler              ; 进入中断处理程序

    CPSID   I
    MRS     R0,     PSP                     ; 堆栈指针作为入参
    BL      CORE_LeaveIRQ                   ; 加载新的堆栈指针
    MOV     R13,    R0
    POP     {R4-R12}
    CPSIE   I
    POP     {PC}

    ENDP

HardFault_Handler   PROC
    MRS     R1,     CONTROL
    MOV     R1,     #0
    B       Fault_Handler
    ENDP

MemManage_Handler   PROC
    MOV     R1,     #1
    B       Fault_Handler
    ENDP
BusFault_Handler    PROC
    MOV     R1,     #2
    B       Fault_Handler
    ENDP
UsageFault_Handler  PROC
    MOV     R1,     #3
    B       Fault_Handler
    ENDP
	
ALIGN
            END