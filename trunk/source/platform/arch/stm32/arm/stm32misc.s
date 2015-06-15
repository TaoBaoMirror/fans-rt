;文 件 名： stm32misc.s
;说    明： 提供平台支撑
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
;函数名：DWORD CORE_SaveIRQFlags(VOID)
;说  明：返回中断标志寄存器值
;参  数NONE
;返回值：DWORD 标志寄存器值
;备  注：无
;=================================================================================
CORE_SaveIRQFlags  PROC
    MRS     R0,    PRIMASK
    BX      LR
    ENDP
;=================================================================================
;函数名：DWORD CORE_DisableIRQ(VOID)
;说  明：返回中断标志寄存器值，并关闭中断
;参  数NONE
;返回值：DWORD 标志寄存器值
;备  注：无
;=================================================================================
CORE_DisableIRQ  PROC
    MRS     R0,    PRIMASK
    CPSID   I
    BX      LR
    ENDP
    
;=================================================================================
;函数名：VOID CORE_RestoreIRQ(DWORD dwFlags);
;说  明：恢复标志寄存器(解锁IRQ)
;参  数：DWORD dwFlags                新的标志寄存器值
;返回值：无
;备  注：无
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