        IF      :DEF:CONFIG_ARCH_SUPPORT_KSTACK
	INCLUDE birq.inc
	INCLUDE saveirq.inc
    
    EXPORT  UsageFault_Handler
    EXPORT  BusFault_Handler
    EXPORT  MemManage_Handler
    EXPORT  HardFault_Handler
    EXPORT  SysTick_Handler
    EXPORT  PendSV_Handler
    EXPORT  SVC_Handler
    EXPORT  CORE_ActiveSwitchIRQ
    EXPORT  CORE_EnableKernelStack

    IMPORT  CORE_EnterIRQ
    IMPORT  CORE_LeaveIRQ
    IMPORT  CORE_TickHandler
    IMPORT  CORE_TaskScheduling
    IMPORT  CORE_HandlerLPC
    IMPORT  CORE_SwitchTask
    IMPORT  STM32_FaultHandler

    PRESERVE8

    AREA    |.text|, CODE, READONLY
    ALIGN 4
    THUMB

NVIC_CONTROL    EQU     0xE000ED04
NVIC_PEND_SET   EQU     0x10000000

CORE_EnableKernelStack   PROC
    PUSH    {R0, R1}
    MRS     R1,     MSP
    MSR     PSP,    R1
    MSR     MSP,    R0
    MOV     R0,     #0
    MSR     PRIMASK, R0
    MOV     R0,     #0x3
    MSR     CONTROL,    R0
    ISB
    POP     {R0, R1}
    BX      LR
    ENDP
    
CORE_ActiveSwitchIRQ  PROC
    PUSH    {R4, R5}
    LDR     R4,     =NVIC_CONTROL
    LDR     R5,     =NVIC_PEND_SET
    STR     R5,     [R4]
    ISB
    POP     {R4, R5}
    BX      LR
    NOP
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
    

Fault_Handler PROC
    CPSID   I
    PUSH    {LR}
    PUSH    {R4-R12}
    MRS     R0,     PSP                     ; 堆栈指针作为入参
    MOV     R4,     R0
    MOV     R5,     R1
    BL      CORE_EnterIRQ                   ; 保存堆栈指针
    MSR     PSP,    R0                      ; 使用内核堆栈
    CPSIE   I

    MOV     R1,     R5
    MOV     R0,     R4
    BL      STM32_FaultHandler              ; 进入中断处理程序

    CPSID   I
    MRS     R0,     PSP                     ; 堆栈指针作为入参
    BL      CORE_LeaveIRQ                   ; 加载新的堆栈指针
    MSR     PSP,    R0
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
        ENDIF
            END