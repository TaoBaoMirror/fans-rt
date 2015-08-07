    EXPORT  CORE_ActiveSwitchIRQ
    EXPORT  CORE_SystemHang
    EXPORT  CORE_RestoreIRQ
    EXPORT  CORE_DisableIRQ
    EXPORT  CORE_EnableIRQ
    EXPORT  CORE_SaveIRQFlags
    EXPORT  CORE_SpinLock
    EXPORT  CORE_SpinUnlock
    EXPORT  CORE_SpinLockIRQ
    EXPORT  CORE_SpinUnlockIRQ

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

;PUBLIC VOID CORE_SpinLock(LPSPIN_LOCK_T Lock);
CORE_SpinLock       PROC
    PUSH    {R0 - R3}                               ;
    MRS     R3,     PRIMASK                         ;
    CPSID   I                                       ;   DisableIRQ
LSL_0                                               ;
    LDREX   R2,     [R0]                            ;   R2 = *Lock
    AND     R1,     R2,     #1                      ;   R1 = R2 & 1
    CMP     R1,     #1                              ;   IF R1 == 1 THEN
    BEQ     LSL_0                                   ;       GOTO LSL_0
    ORR     R2,     #1                              ;   R2 = R2 | 1
    STREX   R1,     R2,     [R0]                    ;   IF R2 ==> [R0] is not successfully then R1 = 1
    CMP     R1,     #1                              ;   IF R1 == 1 THEN
    BEQ     LSL_0                                   ;       GOTO LSL_0
    DSB                                             ;   Flush cache
    MSR     PRIMASK,    R3                          ;   RestoreIRQ
    POP     {R0 - R3}                               ;   
    BX      LR                                      ;   return
    ENDP

;PUBLIC VOID CORE_SpinUnlock(LPSPIN_LOCK_T Lock);
CORE_SpinUnlock       PROC
    PUSH    {R0 - R2}                               ;
    MRS     R1,     PRIMASK                         ;
    CPSID   I                                       ;   DisableIRQ
    LDR     R2,     [R0]                            ;
    BIC     R2,     #1                              ;   R2 = R2 & (~1)
    STR     R2,     [R0]                            ;   *Lock = R2
    DSB                                             ;
    MSR     PRIMASK,    R1                          ;   RestoreIRQ
    POP     {R0 - R2}                               ;
    CPSIE   I                                       ;
    BX      LR                                      ;
    ENDP
    
;PUBLIC VOID CORE_SpinLockIRQ(LPSPIN_LOCK_T Lock, DWORD_PTR dwFlags);
CORE_SpinLockIRQ       PROC
    PUSH    {R0 - R2}                               ;
    MRS     R2,     PRIMASK                         ;
    CPSID   I                                       ;   DisableIRQ
    STR     R2,     [R1]                            ;   *Flags = R2
LSLI_0                                              ;
    LDREX   R2,     [R0]                            ;   R2 = *Lock
    AND     R1,     R2,     #1                      ;   R1 = R2 & 1
    CMP     R1,     #1                              ;   IF R1 == 1 THEN
    BEQ     LSLI_0                                  ;       GOTO LSLI_0
    ORR     R2,     R2,     #1                      ;   R2 = R2 | 1
    STREX   R1,     R2,     [R0]                    ;   IF R2 ==> [R0] is not successfully then R1 = 1
    CMP     R1,     #1                              ;   IF R1 == 1 THEN
    BEQ     LSLI_0                                  ;       GOTO LSLI_0
    DSB                                             ;   Flush cache
    POP     {R0 - R2}                               ;   
    BX      LR                                      ;   return
    ENDP

;PUBLIC VOID CORE_SpinUnlockIRQ(LPSPIN_LOCK_T Lock, DWORD_PTR dwFlags);
CORE_SpinUnlockIRQ       PROC
    PUSH    {R0, R2}                                ;
    LDR     R2,     [R0]                            ;
    BIC     R2,     #1                              ;   R2 = R2 & (~1)
    STR     R2,     [R0]                            ;   *Lock = R2
    DSB                                             ;
    LDR     R2,     [R1]                            ;   R2 = *Flags;
    MSR     PRIMASK,    R2                          ;   RestoreIRQ
    POP     {R0, R2}                                ;
    CPSIE   I                                       ;
    BX      LR                                      ;
    NOP
    ENDP
ALIGN
            END