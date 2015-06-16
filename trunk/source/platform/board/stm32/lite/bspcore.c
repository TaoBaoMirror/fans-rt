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
//#include <string.h>
//#include <stdio.h>
#include <stm32f10x.h>
#include <fadefs.h>
#include <faerror.h>
#include <fatypes.h>

#include "kcore.h"
#include "libcal.h"
#include "birq.h"

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
