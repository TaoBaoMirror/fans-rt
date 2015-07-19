        IF      :DEF:__MICROLIB
    EXPORT  CORE_InitArchFirst
    IMPORT  __heap_limit

    PRESERVE8

    AREA    |.text|, CODE, READONLY
    ALIGN 4
    THUMB

CORE_InitArchFirst PROC
    LDR     R0,     =__heap_limit
    MSR     MSP,    R0
    BX      LR
    ENDP
ALIGN   
        ENDIF
            END 
