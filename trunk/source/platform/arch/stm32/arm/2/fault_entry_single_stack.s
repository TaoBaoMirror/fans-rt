        IF      :LNOT::DEF:CONFIG_ARCH_SUPPORT_KSTACK
    EXPORT  UsageFault_Handler
    EXPORT  BusFault_Handler
    EXPORT  MemManage_Handler
    EXPORT  HardFault_Handler

    IMPORT  CORE_SwEnterIRQ
    IMPORT  CORE_SwLeaveIRQ
    IMPORT  STM32_FaultHandler

    PRESERVE8

    AREA    |.text|, CODE, READONLY
    ALIGN 4
    THUMB

Fault_Handler PROC
    CPSID   I
    PUSH    {LR}
    PUSH    {R4-R12}
    MRS     R0,     PSP                     ; ��ջָ����Ϊ���
    MOV     R4,     R0
    MOV     R5,     R1
    BL      CORE_SwEnterIRQ                   ; �����ջָ��
    MSR     PSP,    R0                      ; ʹ���ں˶�ջ
    CPSIE   I

    MOV     R1,     R5
    MOV     R0,     R4
    BL      STM32_FaultHandler              ; �����жϴ������

    CPSID   I
    MRS     R0,     PSP                     ; ��ջָ����Ϊ���
    BL      CORE_SwLeaveIRQ                   ; �����µĶ�ջָ��
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