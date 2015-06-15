        IF      :LNOT::DEF:CONFIG_ARCH_SUPPORT_KSTACK
	INCLUDE birq.inc
	INCLUDE saveirq.inc
    
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
    PUSH    {R4-R12}
	M_SAVE_IRQ 		#E_CORE_SYSTICK_IRQ_ID
    BL      CORE_TickHandler
    MOV     R0,    R13                      ; 堆栈指针作为入参
    BL      CORE_EnterIRQ                   ; 保存堆栈指针
    MOV     R13,   R0                       ; 使用内核堆栈
    CPSIE   I

    BL      CORE_TaskScheduling             ; 进入中断处理程序

    CPSID   I
    MOV     R0,    R13                      ; 堆栈指针作为入参
    BL      CORE_LeaveIRQ                   ; 保存堆栈指针
    MOV     R13,   R0                       ; 使用内核堆栈
    POP     {R4-R12}
    CPSIE   I
    POP     {PC}
    ENDP

PendSV_Handler  PROC
    BX      LR
    ENDP

SVC_Handler     PROC
    CPSID   I
    PUSH    {LR}
    PUSH    {R4-R12}
	M_SAVE_IRQ     #E_CORE_SYSCALL_IRQ_ID
    MOV     R4,    R0                       ; 暂存入参 1
    MOV     R5,    R1                       ; 暂存入参 2
    MOV     R6,    R2                       ; 暂存入参 3
    MOV     R0,    R13                      ; 堆栈指针作为入参
    BL      CORE_EnterIRQ                   ; 保存堆栈指针
    MOV     R13,   R0
    CPSIE   I

    MOV     R0,    R4                       ; 弹出入参 1
    MOV     R1,    R5                       ; 弹出入参 2
    MOV     R2,    R6                       ; 弹出入参 3
    BL      CORE_HandlerLPC                 ; 进入中断处理程序

    CPSID   I
    MOV     R0,    R13                      ; 堆栈指针作为入参
    BL      CORE_LeaveIRQ                   ; 加载新的堆栈指针
    MOV     R13,   R0
    POP     {R4-R12}
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
        ENDIF
            END