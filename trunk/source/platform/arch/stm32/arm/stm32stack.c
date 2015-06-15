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
#include <fadefs.h>
#include <faerror.h>
#include <fatypes.h>

#include "kcore.h"
#include "ktask.h"
#include "kdebug.h"
#include "libcal.h"

#if (defined(__CC_ARM))
#ifdef __MICROLIB
extern BYTE __initial_sp;

PUBLIC LPVOID CORE_GetBootStackBuffer(VOID)
{
    return (&__initial_sp) - CONFIG_IDLE_STACK_SIZE;
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


PUBLIC LPVOID CORE_FillStack(LPVOID Position, LPVOID Entry, LPVOID lpArgument, HANDLE hTask)
{
    LPDWORD StackPoint = Position;

    *(--StackPoint)  = (DWORD)0x01000000L;             /* xPSR                                               */
    *(--StackPoint)  = (DWORD)ntTaskEntry;           /* R15  PC -->Entry Point                             */
    *(--StackPoint)  = (DWORD)0;                       /* R14 (LR) (init value will cause fault if ever used)*/
    *(--StackPoint)  = (DWORD)0x12121212L;             /* R12                                                */
    *(--StackPoint)  = (DWORD)0x03030303L;             /* R3                                                 */
    *(--StackPoint)  = (DWORD)hTask;                   /* R2 : Task handle                                   */
    *(--StackPoint)  = (DWORD)lpArgument;              /* R1 : Argument                                      */
    *(--StackPoint)  = (DWORD)Entry;                   /* R0 : Entry                                         */
#if (CONFIG_ARCH_SUPPORT_KSTACK == TRUE)
    *(--StackPoint)  = (DWORD)0xfffffffdL;             /* Remaining registers saved on process stack         */
#else
    *(--StackPoint)  = (DWORD)0xfffffff9L;             /* Remaining registers saved on process stack         */
#endif
    *(--StackPoint)  = (DWORD)0x12121212L;             /* R12                                                */
    *(--StackPoint)  = (DWORD)0x11111111L;             /* R11                                                */
    *(--StackPoint)  = (DWORD)0x10101010L;             /* R10                                                */
    *(--StackPoint)  = (DWORD)0x09090909L;             /* R9                                                 */
    *(--StackPoint)  = (DWORD)0x08080808L;             /* R8                                                 */
    *(--StackPoint)  = (DWORD)0x07070707L;             /* R7                                                 */
    *(--StackPoint)  = (DWORD)0x06060606L;             /* R6                                                 */
    *(--StackPoint)  = (DWORD)0x05050505L;             /* R5                                                 */
    *(--StackPoint)  = (DWORD)0x04040404L;             /* R4                                                 */

    return (StackPoint);
}

