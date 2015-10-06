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

#include "kmem.h"

STATIC BYTE RW_CORE_REGION0 g_OnChipRegion[CONFIG_MEM_ONCHIP_LENGTH] = {0};// __attribute__((at(CONFIG_MEM_ONCHIP_ADDRESS)));

PUBLIC E_STATUS CORE_ScanMemoryRegion(VOID)
{
    return CORE_CreateRegion((MMADDR)g_OnChipRegion, sizeof(g_OnChipRegion));;
}

