        IF      :DEF:__MICROLIB
    EXPORT  CORE_InitArchFirst
    IMPORT  __heap_limit

    PRESERVE8

    AREA    |.text|, CODE, READONLY
    ALIGN 4
    THUMB

CORE_InitArchFirst PROC
    MOV     R0,     #0
    MOV     R1,     #1
    MOV     R2,     #2
    MOV     R3,     #3
    PUSH    {R0}
    PUSH    {R1}
    PUSH    {R2}
    PUSH    {R3}
    POP     {R3}
    POP     {R2}
    POP     {R1}
    POP     {R0}
    PUSH    {R0-R3}                     ; »Î’ªÀ≥–Ú R3, R2, R1, R0
    POP     {R0-R3}
    PUSH    {R0, R2-R3}                 ; »Î’ªÀ≥–Ú R3, R2, R1, R0
    POP     {R0, R2-R3}
    PUSH    {R3, R2, R1, R0}
    POP     {R3, R2, R1, R0}
    PUSH    {LR, R10, R12, R1-R3}                     ; »Î’ªÀ≥–Ú R3, R2, R1, R0
    POP     {LR, R10, R12, R1-R3}                     ; »Î’ªÀ≥–Ú R3, R2, R1, R0
    LDR     R0,     =__heap_limit
    MSR     MSP,    R0
    BX      LR
    ENDP
ALIGN   
        ENDIF
            END 
