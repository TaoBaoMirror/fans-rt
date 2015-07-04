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
    IMPORT  CORE_GetCoreStackPosition
    IMPORT  CORE_GetTaskStackPosition
    IMPORT  CORE_GetCurrentPermission

    PRESERVE8

    AREA    |.text|, CODE, READONLY
    ALIGN 4
    THUMB

NVIC_CONTROL    EQU     0xE000ED04
NVIC_PEND_SET   EQU     0x10000000

CORE_Switch2UserMode   PROC
    MOV     R0,     #0
    MSR     PRIMASK, R0
    BX      LR
    ENDP

PendSV_Handler  PROC
    CPSID   I                           ;  Why to disable IRQ ? Guess !
    PUSH    {LR, R4-R12}                ;  Why to push R12 ?
    MRS     R0,     MSP                 ;  R0 = Core stack for old task
    MRS     R1,     PSP                 ;  R1 = User stack for old task
    BL      CORE_SwitchTask             ;  CORE_SwitchTask(CoreStack, UserStack);
                                        ;  No need check the task permssion
    BL      CORE_GetTaskStackPosition   ;
    MOV     R1,     R0                  ;  R1 = User stack for new task
    BL      CORE_GetCoreStackPosition   ;  R0 = Core stack for new task
    MSR     PSP,    R1                  ;  Update user stack
    MSR     MSP,    R0                  ;  Update core stack
    POP     {LR, R4-R12}                ;  Stack must be aligned to 64 bit, so push R12
    CPSIE   I                           ;
    BX      LR                          ;  Return to the break point of the new task 
    ENDP

SVC_Handler     PROC
    CPSID   I                           ;  Why to disable IRQ ? Guess !
    PUSH    {LR, R4}                    ;  Why to push R4 ? Guess !
    PUSH    {R0-R3}                     ;  Save the service parameter to stack
    BL      CORE_EnterIRQ               ;  Enter Interrupt Critiacl
    POP     {R0-R3}                     ;  Restore the service permeter from stack
    CPSIE   I                           ;
                                        ;
    BL      CORE_HandlerLPC             ;  Call system service
                                        ;
    CPSID   I                           ;
    BL      CORE_LeaveIRQ               ;  Leave Interrupt Critiacl
    POP     {LR, R4}                    ;  Stack must be aligned to 64 bit, so push R4
    CPSIE   I                           ;
    BX      LR                          ;  Return to the break point of this task 
    ENDP
    
SysTick_Handler PROC
    CPSID   I                           ;  Why to disable IRQ ? Guess !
    PUSH    {LR, R4}                    ;  The LR must be push, but the 
                                        ;  stack is not aligned to 64 bit
    BL      CORE_EnterIRQ               ;  Enter Interrupt Critiacl
    BL      CORE_TickHandler            ;  Inc the system tick
    CPSIE   I                           ;
    BL      CORE_TaskScheduling         ;  Find the new task will be scheduling
    CPSID   I                           ;
    BL      CORE_LeaveIRQ               ;  Leave Interrupt Critiacl
    POP     {LR, R4}                    ;  Pop registers
    CPSIE   I                           ;
    BX      LR                          ;  Return to the break point of this task 
    ENDP

HardFault_Handler   PROC
    B       .
    ENDP

MemManage_Handler   PROC
    B       .
    ENDP
BusFault_Handler    PROC
    B       .
    ENDP
UsageFault_Handler  PROC
    B       .
    ENDP
	
ALIGN
            END