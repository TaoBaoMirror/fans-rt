    EXPORT  CORE_ActiveSwitchIRQ
    EXPORT  CORE_SystemHang
    EXPORT  CORE_RestoreIRQ
    EXPORT  CORE_DisableIRQ
    EXPORT  CORE_EnableIRQ
    EXPORT  CORE_SaveIRQFlags

    PRESERVE8

    AREA    |.text|, CODE, READONLY
    ALIGN 4
    THUMB

NVIC_CONTROL    EQU     0xE000ED04
NVIC_PEND_SET   EQU     0x10000000

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

CORE_SystemHang   PROC
    B       .
    ENDP

CORE_SaveIRQFlags  PROC
    MRS     R0,    PRIMASK
    BX      LR
    ENDP

CORE_DisableIRQ  PROC
    MRS     R0,    PRIMASK
    CPSID   I
    BX      LR
    ENDP

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