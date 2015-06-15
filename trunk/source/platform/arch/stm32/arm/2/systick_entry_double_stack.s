
	INCLUDE birq.inc
	INCLUDE saveirq.inc
        IF      :DEF:CONFIG_ARCH_SUPPORT_KSTACK
	EXPORT  SysTick_Handler

    IMPORT  CORE_SwEnterIRQ
    IMPORT  CORE_SwLeaveIRQ
    IMPORT  CORE_TickHandler
    IMPORT  CORE_TaskScheduling

    PRESERVE8

    AREA    |.text|, CODE, READONLY
    ALIGN 4
    THUMB

SysTick_Handler PROC
    CPSID   I
    PUSH    {LR}
    PUSH    {R0}
    BL      CORE_TickHandler
    BL      CORE_TaskScheduling             ; 进入中断处理程序
    POP     {R0}
    CPSIE   I
    POP     {PC}
    ENDP

ALIGN
        ENDIF
            END