;文 件 名： libstm32.s
;说    明： 提供平台支撑
;=================================================================
    EXPORT  FW_SystemCall
	EXPORT	FW_StackPosition
    EXPORT  FW_TestFault
    EXPORT  FW_SystemIdle
    
    PRESERVE8

    AREA    |.text|, CODE, READONLY
    ALIGN 4
    THUMB


FW_SystemIdle   PROC
    WFI
    BX      LR
    ENDP

FW_SystemCall   PROC
    SVC     #0
    BX      LR
    ENDP

FW_StackPosition PROC
    MOV     R0,    R13
    BX      LR
    ENDP

FW_TestFault        PROC
    MOV     R0,     #0
    MOV     R1,     #1
    MOV     R2,     #2
    MOV     R3,     #3
    MOV     R4,     #4
    MOV     R5,     #5
    MOV     R6,     #6
    MOV     R7,     #7
    MOV     R8,     #8
    MOV     R9,     #9
    MOV     R10,    #10
    MOV     R11,    #11
    MOV     R12,    #12
    MOV     R14,    #14
    STR     R1,     [R0]
    B       .
    ENDP

ALIGN
            END