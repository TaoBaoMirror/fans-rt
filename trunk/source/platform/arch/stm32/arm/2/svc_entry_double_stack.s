
	INCLUDE birq.inc
	INCLUDE saveirq.inc
        IF      :DEF:CONFIG_ARCH_SUPPORT_KSTACK
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
;************************CONFIG_ARCH_SUPPORT_KSTACK******************************
        
    PUSH    {LR}
    PUSH    {R0}

    BL      CORE_HandlerLPC                 ; 进入中断处理程序

    POP     {R0}
    POP     {PC}
;************************CONFIG_ARCH_SUPPORT_KSTACK******************************
    ENDP

ALIGN
        ENDIF
            END