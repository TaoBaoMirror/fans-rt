;
;    Copyright(C) 2013-2015, Fans-rt development team.
;
;    All rights reserved.
;
;    This is open source software.
;    Learning and research can be unrestricted to  modification, use and dissemination.
;    If you need for commercial purposes, you should get the author's permission.
;
;    Configuration:
;     System global core stack                  YES
;     The local core stack of general task      NO
;     The loacl core stack of kernel task       YES
;     The local user stack of general task      YES
;     Hardware supported task switch IRQ        YES
;     Hardware supported double stack           YES
;
;    date           author          notes
;    2015-06-25     JiangYong       new file
;    2015-07-07     JiangYong       rename to kboard_interrupt.s
;
        INCLUDE kirq_define_enum.inc

    EXPORT  UsageFault_Handler
    EXPORT  BusFault_Handler
    EXPORT  MemManage_Handler
    EXPORT  HardFault_Handler
    EXPORT  SysTick_Handler
    EXPORT  PendSV_Handler
    EXPORT  SVC_Handler
    EXPORT  CORE_Switch2UserMode

    IMPORT  CORE_EnterIRQ
    IMPORT  CORE_LeaveIRQ
    IMPORT  CORE_TickHandler
    IMPORT  CORE_TaskScheduling
    IMPORT  CORE_HandlerLPC
    IMPORT  CORE_SwitchTask
    IMPORT  CORE_DebugMonitor
    IMPORT  CORE_GetCoreStackPosition
    IMPORT  CORE_GetTaskStackPosition
    IMPORT  CORE_GetCurrentPermission

    PRESERVE8

    AREA    |.text|, CODE, READONLY
    ALIGN 4
    THUMB

CORE_Switch2UserMode   PROC
    PUSH    {LR}                        ;  Save LR to call sub function
    BL      CORE_GetCoreStackPosition   ;  Get the position of core stack
    MRS     R1,     MSP                 ;
    MSR     PSP,    R1                  ;  User stack is the current stack
    MSR     MSP,    R0                  ;  Set the core stack position
    MOV     R0,     #0                  ;
    MSR     PRIMASK, R0                 ;  Enable IRQ
    MOV     R0,     #0x3                ;
    MSR     CONTROL,    R0              ;  Switch to user mode
    ISB                                 ;
    POP     {PC}                        ;  return
    ENDP

PendSV_Handler  PROC
    CPSID   I                           ;  Why to disable IRQ ? Guess !
    PUSH    {LR}
    BL      CORE_GetCurrentPermission   ;  Check the permission of current task
    MOV     R3,     R0                  ;  R3 = Permission
    POP     {LR}
    MRS     R0,     MSP                 ;  R0 = Core stack for old task
    MRS     R1,     PSP                 ;  R1 = User stack for old task
    CBNZ    R3,     PV_L1               ;  R3 = 0  Current task is core task
    STMFD	R0!,    {R4 - R12, LR}      ;  Save core stack for the old task
    MSR     MSP,    R0                  ;  Adjust core stack
    B       PV_L2
PV_L1
    STMFD	R1!,    {R4 - R12, LR}      ;  Save user stack for the old task
PV_L2    
    BL      CORE_SwitchTask             ;  CORE_SwitchTask(CoreStack, UserStack);
    MOV     R4,     R0                  ;  R4 = Permission
    BL      CORE_GetTaskStackPosition
    MOV     R1,     R0                  ;  R1 = User stack for new task
    BL      CORE_GetCoreStackPosition   ;  R0 = Core stack for new task
    CBNZ    R4,     PV_L3               ;  R4 = 0  Current task is core task
    LDMFD	R0!,    {R4 - R12, LR}      ;  Load core stack for the new task(core)
    B       PV_L4
PV_L3
    LDMFD	R1!,    {R4 - R12, LR}      ;  Load user stack for the new task(user)
PV_L4
    MSR     PSP,    R1                  ;  Update user stack
    MSR     MSP,    R0                  ;  Update core stack
    CPSIE   I
    BX      LR
    ENDP

SVC_Handler     PROC
    CPSID   I
    PUSH    {R4, LR}                    ;  Why to push R4 ? Guess !
    PUSH    {R0 - R3}            ;
    BL      CORE_EnterIRQ               ;  Enter Interrupt Critiacl
    POP     {R0 - R3}
    CPSIE   I

    BL      CORE_HandlerLPC             ;  Call system service

    CPSID   I
    BL      CORE_LeaveIRQ               ;  Leave Interrupt Critiacl
    POP     {R4, LR}                    ;  Pop registers
    CPSIE   I
    BX      LR
    ENDP
    
SysTick_Handler PROC
    CPSID   I
    PUSH    {R4, LR}                    ;  The LR must be push, but the 
                                        ;  stack is not aligned to 64 bit
    BL      CORE_EnterIRQ               ;  Enter Interrupt Critiacl
    BL      CORE_TickHandler            ;  Inc the system tick
    CPSIE   I
    BL      CORE_TaskScheduling         ;  Find the new task will be scheduling
    CPSID   I
    BL      CORE_LeaveIRQ               ;  Leave Interrupt Critiacl
    POP     {R4, LR}                    ;  Pop registers
    CPSIE   I
    BX      LR
    ENDP

HardFault_Handler   PROC
    B       UsageFault_Handler
    ENDP
MemManage_Handler   PROC
    B       UsageFault_Handler
    ENDP
BusFault_Handler    PROC
    B       UsageFault_Handler
    ENDP
UsageFault_Handler  PROC
    CPSID   I
    PUSH    {R4 - R12, LR}
    MOV     R0,     SP
    BL      CORE_DebugMonitor  
    POP     {R4 - R12, LR}
    CPSIE   I
    B       .
    ENDP

DEMCR_OFFSET     EQU     0xe000edfc
DEBUG_ENABLE     EQU     0x010d0000

DebugMon_Handler    PROC
    CPSID   I
    PUSH    {R4 - R12, LR}
    MOV     R0,     SP
    BL      CORE_DebugMonitor
    LDR     R0,     =DEBUG_ENABLE
    LDR     R1,     =DEMCR_OFFSET
    STR     R0,     [R1]     
    POP     {R4 - R12, LR}
    CPSIE   I
    BX      LR
    NOP
    ENDP
	
ALIGN
            END