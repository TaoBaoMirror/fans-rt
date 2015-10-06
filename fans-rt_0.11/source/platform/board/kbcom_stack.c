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
 *    2015-06-4     JiangYong       new file
 */
#include "config.h"
#include <stm32f10x.h>
#include <facore.h>
#include <fadefs.h>
#include <faerror.h>
#include <fatypes.h>

STATIC CHAR RW_IDLE_STACK g_SystemIdleTaskStack[CONFIG_IDLE_STACK_SIZE] = {0};

PUBLIC RO_CORE_CODE LPVOID CORE_GetIdleStackBuffer(DWORD CpuID)
{
    if (0 != CpuID)
    {
        return NULL;
    }
    
    return g_SystemIdleTaskStack;
}

PUBLIC RO_CORE_CODE SIZE_T CORE_GetIdleStackLength(VOID)
{
    SIZE_T Adjust = ((DWORD)g_SystemIdleTaskStack % sizeof(U64));
    return Adjust ? (CONFIG_IDLE_STACK_SIZE - Adjust) : CONFIG_IDLE_STACK_SIZE;
}

