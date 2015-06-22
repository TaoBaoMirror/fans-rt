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
#include "config.h"
#include <stm32f10x.h>
#include <facore.h>
#include <fadefs.h>
#include <faerror.h>
#include <fatypes.h>

#include "birq.h"
#include "kmem.h"
#include "ktask.h"

STATIC CHAR g_SystemIdleTaskStack[CONFIG_IDLE_STACK_SIZE];
STATIC CHAR g_SystemCoreTaskStack[CONFIG_CORE_STACK_SIZE];

PUBLIC LPVOID CORE_GetIdleStackBuffer(DWORD CpuID)
{
    if (0 != CpuID)
    {
        return NULL;
    }
    
    return g_SystemIdleTaskStack;
}

PUBLIC LPVOID CORE_GetCoreStackButtom(VOID)
{
    return g_SystemCoreTaskStack + CONFIG_CORE_STACK_SIZE;
}

PUBLIC LPVOID CORE_GetTaskCoreStackPosition(LPVOID lpTaskContext, LPVOID StackPosition)
{
    return g_SystemCoreTaskStack + CONFIG_CORE_STACK_SIZE;
}

PUBLIC LPSTR BSP_GetBoardName(VOID)
{
    return "stm32-lite";
}

PUBLIC LPSTR BSP_GetIRQNameString(E_IRQ_ID Id)
{
    switch(Id)
    {
    case E_CORE_SYSTICK_IRQ_ID:
        return _TEXT("SysTick");
    case E_CORE_SYSCALL_IRQ_ID:
        return _TEXT("caSystemCall");
    default:
        return _TEXT("Unknow");
    }
}

#if (CONFIG_MEM_REGION_MAX != 0)

BYTE g_OnChipRegion[CONFIG_MEM_ONCHIP_LENGTH] __attribute__((at(CONFIG_MEM_ONCHIP_ADDRESS)));

PUBLIC E_STATUS BSP_ScanMemoryRegion(VOID)
{
    return CORE_CreateRegion((MMADDR)g_OnChipRegion, sizeof(g_OnChipRegion));;
}
#endif
