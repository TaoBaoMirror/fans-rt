
	INCLUDE birq.inc
	INCLUDE saveirq.inc
        IF      :LNOT::DEF:CONFIG_ARCH_SUPPORT_KSTACK
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
    PUSH    {R4-R12}
	M_SAVE_IRQ 		#E_CORE_SYSTICK_IRQ_ID
    BL      CORE_TickHandler
    MOV     R0,    R13                      ; ��ջָ����Ϊ���
    BL      CORE_SwEnterIRQ                 ; �����ջָ��
    MOV     R13,   R0                       ; ʹ���ں˶�ջ
    CPSIE   I

    BL      CORE_TaskScheduling             ; �����жϴ������

    CPSID   I
    MOV     R0,    R13                      ; ��ջָ����Ϊ���
    BL      CORE_SwLeaveIRQ                   ; �����ջָ��
    MOV     R13,   R0                       ; ʹ���ں˶�ջ
    POP     {R4-R12}
    CPSIE   I
    POP     {PC}
    ENDP

ALIGN
        ENDIF
            END