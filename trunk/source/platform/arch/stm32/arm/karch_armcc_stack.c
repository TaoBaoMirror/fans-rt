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
 *    2015-06-24     JiangYong       new file
 */

#include <facore.h>
#include <fadefs.h>
#include <faerror.h>
#include <fatypes.h>

#if (defined(__CC_ARM))
#ifdef __MICROLIB
extern BYTE __initial_sp;

PUBLIC LPVOID CORE_GetBootStackBuffer(VOID)
{
    return (&__initial_sp) - CONFIG_BOOT_STACK_SIZE;
}
#else           //__MICROLIB

    PUBLIC LPVOID CORE_BootStackAddress(VOID);
    PUBLIC DWORD  CORE_BootStackLength(VOID);
    PUBLIC LPVOID CORE_HeapAddress(VOID);
    PUBLIC DWORD CORE_HeapLength(VOID);

PUBLIC LPVOID CORE_GetBootStackBuffer(VOID)
{
    return CORE_BootStackAddress();
}
#endif          //__MICROLIB
#else           //__ARM_CC
#error "not support this complier..."
#endif          //__ARM_CC
