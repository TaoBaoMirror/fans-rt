
	INCLUDE birq.inc
	INCLUDE saveirq.inc
        IF      :LNOT::DEF:CONFIG_ARCH_SUPPORT_KSTACK
	EXPORT  PendSV_Handler
    EXPORT  SVC_Handler
    IMPORT  CORE_SwEnterIRQ
    IMPORT  CORE_SwLeaveIRQ
    IMPORT  CORE_HandlerLPC

    PRESERVE8

    AREA    |.text|, CODE, READONLY
    ALIGN 4
    THUMB
    
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
    BL      CORE_SwEnterIRQ                   ; 保存堆栈指针
    MOV     R13,   R0
    CPSIE   I

    MOV     R0,    R4                       ; 弹出入参 1
    MOV     R1,    R5                       ; 弹出入参 2
    MOV     R2,    R6                       ; 弹出入参 3
    BL      CORE_HandlerLPC                 ; 进入中断处理程序

    CPSID   I
    MOV     R0,    R13                      ; 堆栈指针作为入参
    BL      CORE_SwLeaveIRQ                   ; 加载新的堆栈指针
    MOV     R13,   R0
    POP     {R4-R12}
    CPSIE   I
    POP     {PC}
    ENDP

ALIGN
        ENDIF
            END