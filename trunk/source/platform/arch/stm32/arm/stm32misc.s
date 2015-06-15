;�� �� ���� stm32misc.s
;˵    ���� �ṩƽ̨֧��
;=================================================================
    EXPORT  CORE_SystemHang
    EXPORT  CORE_RestoreIRQ
    EXPORT  CORE_DisableIRQ
    EXPORT  CORE_EnableIRQ
    EXPORT  CORE_SaveIRQFlags

    PRESERVE8

    AREA    |.text|, CODE, READONLY
    ALIGN 4
    THUMB

CORE_SystemHang   PROC
    B       .
    ENDP
;=================================================================================
;��������DWORD CORE_SaveIRQFlags(VOID)
;˵  ���������жϱ�־�Ĵ���ֵ
;��  ���NNONE
;����ֵ��DWORD ��־�Ĵ���ֵ
;��  ע����
;=================================================================================
CORE_SaveIRQFlags  PROC
    MRS     R0,    PRIMASK
    BX      LR
    ENDP
;=================================================================================
;��������DWORD CORE_DisableIRQ(VOID)
;˵  ���������жϱ�־�Ĵ���ֵ�����ر��ж�
;��  ���NNONE
;����ֵ��DWORD ��־�Ĵ���ֵ
;��  ע����
;=================================================================================
CORE_DisableIRQ  PROC
    MRS     R0,    PRIMASK
    CPSID   I
    BX      LR
    ENDP
    
;=================================================================================
;��������VOID CORE_RestoreIRQ(DWORD dwFlags);
;˵  �����ָ���־�Ĵ���(����IRQ)
;��  ����DWORD dwFlags                �µı�־�Ĵ���ֵ
;����ֵ����
;��  ע����
;=================================================================================
CORE_RestoreIRQ       PROC
    MSR     PRIMASK, R0
    BX      LR
    ENDP
    
CORE_EnableIRQ        PROC
    MOV     R0,     #0
    MSR     PRIMASK, R0
    BX      LR
    ENDP

ALIGN
            END