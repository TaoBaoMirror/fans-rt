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

PUBLIC LPVOID CORE_FillStack(LPVOID Position, LPVOID fnMain, LPVOID lpArgument,
                LPVOID lpTaskContext, HANDLE hTask, E_TASK_PERMISSION Permission)
{
    LPVOID fnEntry;
    LPVOID fnLeave;
    DWORD TaskObject;
    DWORD IRQLeaveR;
    LPDWORD StackPoint = Position;
    
    if (TASK_PERMISSION_CORE == Permission)
    {
        fnEntry = CORE_TaskEntry;
        fnLeave = CORE_TaskLeave;
        IRQLeaveR  = (DWORD) 0xfffffff9L;
        TaskObject = (DWORD) lpTaskContext;
    }
    else
    {
        fnLeave = USER_TaskLeave;
        fnEntry = USER_TaskEntry;
        IRQLeaveR  = (DWORD) 0xfffffffdL;
        TaskObject = (DWORD) hTask;
    }

    *(--StackPoint)  = (DWORD)0x01000000L;             /* xPSR                                               */
    *(--StackPoint)  = (DWORD)fnEntry;                 /* R15  PC -->Entry Point                             */
    *(--StackPoint)  = (DWORD)fnLeave;                 /* R14 (LR) (init value will cause fault if ever used)*/
    *(--StackPoint)  = (DWORD)0x12121212L;             /* R12                                                */
    *(--StackPoint)  = (DWORD)0x03030303L;             /* R3                                                 */
    *(--StackPoint)  = (DWORD)TaskObject;              /* R2 : Task object                                   */
    *(--StackPoint)  = (DWORD)lpArgument;              /* R1 : Argument                                      */
    *(--StackPoint)  = (DWORD)fnMain;                  /* R0 : Main founction                                */
    
    *(--StackPoint)  = (DWORD)IRQLeaveR;               /* LR : The LR value for IRQ                          */
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


PUBLIC VOID CORE_SetArchContextParam(LPARCH_CONTEXT lpArchContext, LPVOID lpParam)
{
    SetStackCapacity(GetArchUserSD(lpArchContext), 0);
    SetStackBuffer(GetArchUserSD(lpArchContext), NULL); 
    SetStackPosition(GetArchUserSD(lpArchContext), NULL);
    SetStackHandle(GetArchUserSD(lpArchContext), INVALID_HANDLE_VALUE);
    SetStackCapacity(GetArchCoreSD(lpArchContext), 0);
    SetStackBuffer(GetArchCoreSD(lpArchContext), NULL); 
    SetStackPosition(GetArchCoreSD(lpArchContext), NULL);
    SetStackHandle(GetArchCoreSD(lpArchContext), INVALID_HANDLE_VALUE);
}
