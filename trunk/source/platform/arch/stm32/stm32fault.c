/*
 *    Copyright(C) 2013-2015, Fans-rt development team.
 *
 *    All rights reserved.
 *
 *    This is open source software.
 *    Learning and research can be unrestricted to modification, use and dissemination.
 *    If you need for commercial purposes, you should get the author's permission.
 *
 *    date           author          notes
 *    2014-09-07     JiangYong       new file
 */
#include <string.h>
#include <stdio.h>
#include <stm32f10x.h>

#include <fadefs.h>
#include <faerror.h>
#include <fatypes.h>

#include "ktick.h"
#include "ktask.h"
#include "kcore.h"
#include "kboard.h"
#include "kdebug.h"
#include "stm32hal.h"

#define     CM3_HARDFAULT_REGISTER_ADDRESS          0xe000ed2c
#define     CM3_FAULTSTATUS_REGISTER_ADDRESS        0xe000ed28

#define     CM3_HARDFAULT_BITS                      (*(LPDWORD)(CM3_HARDFAULT_REGISTER_ADDRESS))
#define     CM3_FAULTSTATUS_BITS                    (*(LPDWORD)(CM3_FAULTSTATUS_REGISTER_ADDRESS))


#ifdef DEBUG_IRQ_MONITOR

PUBLIC VOID kDebugShowStack(LPIRQ_REGISTER_STACK lpStack, BOOL IsStack)
{
    kprintf(" R0 = %08X  R1 = %08X  R2 = %08X  R3 = %08X\n",
            lpStack->R0, lpStack->R1, lpStack->R2, lpStack->R3);
    kprintf(" R4 = %08X  R5 = %08X  R6 = %08X  R7 = %08X\n",
            lpStack->R4, lpStack->R5, lpStack->R6, lpStack->R7);
    kprintf(" R8 = %08X  R9 = %08X  R10= %08X  R1 = %08X\n",
            lpStack->R8, lpStack->R9, lpStack->R10, lpStack->R11);
    kprintf(" R12= %08X  R13= %08X  R14= %08X  R15= %08X\n",
            lpStack->R12, IsStack ? lpStack : (LPVOID) lpStack->SP, lpStack->LR0, lpStack->R15);
    kprintf(" PSR= %08X  LR1= %08X\n", lpStack->xPSR, lpStack->LR1);
}
#endif


PUBLIC VOID STM32_FaultHandler(LPVOID lpStackPoint, DWORD Interrupt)
{
#ifdef DEBUG_IRQ_MONITOR
    kprintf("\nEnter hard fault(task %p - %s)(stack %p - int %d)(layer %d)[TICK%ld]:\n",
        CORE_GetCurrentTask(), GetContextTaskName(CORE_GetCurrentTask()), 
        lpStackPoint, Interrupt, CORE_GetInterruptLayer(), (DWORD)CORE_GetSystemTick());
    kprintf("HARD_FAULT %08x   FAULT STATUS  %08x",
        CM3_HARDFAULT_BITS, CM3_FAULTSTATUS_BITS);
    kDebugMonitorIRQ();
#endif
    kprintf("The error interrupt is %d.\n", Interrupt);
    while(1);
}

