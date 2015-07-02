        IF      :LNOT::DEF:__MICROLIB
    EXPORT  CORE_BootStackAddress
    EXPORT  CORE_BootStackLength
    EXPORT  CORE_HeapAddress
    EXPORT  CORE_HeapLength
    EXPORT  CORE_InitArchFirst
    IMPORT  __user_initial_stackheap

    PRESERVE8

    AREA    |.text|, CODE, READONLY
    ALIGN 4
    THUMB

CORE_BootStackAddress PROC
    PUSH    {R1-R3, LR}
    BL      __user_initial_stackheap
    MOV     R0,     R3
    POP     {R1-R3, PC}
    ENDP

CORE_BootStackLength PROC
    PUSH    {R1-R3, LR}
    BL      __user_initial_stackheap
    SUB     R0, R1, R3
    POP     {R1-R3, PC}
    ENDP
    
CORE_HeapAddress PROC
    PUSH    {R1-R3, LR}
    BL      __user_initial_stackheap
    POP     {R1-R3, PC}
    ENDP
    
CORE_HeapLength PROC
    PUSH    {R1-R3, LR}
    BL      __user_initial_stackheap
    SUB     R0, R2, R0
    POP     {R1-R3, PC}
    ENDP

CORE_InitArchFirst PROC
    MOV     R4,     LR
    BL      __user_initial_stackheap
    MSR     MSP,    R2
    MOV     LR,     R4
    BX      LR
    ENDP
ALIGN   
        ENDIF
            END 
