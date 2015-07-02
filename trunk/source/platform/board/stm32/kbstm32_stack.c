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

#if (CONFIG_CORE_STACK_SIZE != 0)
STATIC RW_CORE_STACK CHAR g_SystemCoreStack[CONFIG_CORE_STACK_SIZE] = {0};

PUBLIC LPVOID CORE_GetCoreStackBuffer(VOID)
{
    return g_SystemCoreStack;
}

PUBLIC SIZE_T CORE_GetCoreStackLength(VOID)
{
    SIZE_T Adjust = ((DWORD)g_SystemCoreStack % sizeof(U64));
    return Adjust ? (CONFIG_CORE_STACK_SIZE - Adjust) : CONFIG_CORE_STACK_SIZE;
}
#endif

PUBLIC LPVOID CORE_GetBootStackBuffer(VOID)
#if (defined(__CC_ARM) && defined(__MICROLIB))
{
    extern BYTE __initial_sp;
    return (&__initial_sp) - 0x400;
}

#elif ((defined(__CC_ARM) && (!defined(__MICROLIB))) || defined(_GNUC_))
{
    extern LPVOID CORE_BootStackAddress(VOID);
    return CORE_BootStackAddress();
}
#else
#error "Can not support this complier !"
#endif
