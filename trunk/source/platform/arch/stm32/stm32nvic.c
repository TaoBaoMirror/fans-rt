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
#include <misc.h>
#include <stm32f10x.h>
#include <fadefs.h>
#include <faerror.h>
#include <fatypes.h>

#define IRQ_PRIORITY(P0, P1)        ((P0 << 2) | P1)

#if (__CM3_CMSIS_VERSION == 0x10030)
#define SetIRQPriority(IRQn, Preemption, Response)      NVIC_SetContextThisPriority(IRQn, IRQ_PRIORITY(Response, Preemption))
#elif (__CM3_CMSIS_VERSION == 0x30001)
#define SetIRQPriority(IRQn, Preemption, Response)      NVIC_SetPriority(IRQn, IRQ_PRIORITY(Response, Preemption))
#else
#error "Can't support this CMSIS version."
#endif



PUBLIC VOID NVIC_EnableFault(VOID)
{
    SCB->CCR |= 0x18;
    SCB->SHCSR |= 0x00007000;
}

PUBLIC VOID NVIC_Configuration(VOID)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    SetIRQPriority (SysTick_IRQn, 0x1, 0x1);
    SetIRQPriority (SVCall_IRQn, 3, 0);     /* 抢占优先级3，响应优先级0 */
    SetIRQPriority (BusFault_IRQn, 0, 0);
    SetIRQPriority (UsageFault_IRQn, 0, 0);
    SetIRQPriority (MemoryManagement_IRQn, 0, 0);
    NVIC_EnableFault();
    
}
