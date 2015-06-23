    EXPORT  CORE_ActiveSwitchIRQ

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
    
ALIGN
            END