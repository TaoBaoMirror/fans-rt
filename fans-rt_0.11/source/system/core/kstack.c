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
 *    2015-07-5     JiangYong       new file
 */
#include "config.h"
#include <stm32f10x.h>
#include <facore.h>
#include <fadefs.h>
#include <faerror.h>
#include <fatypes.h>

#include "ktask.h"
#include "kcore.h"
#include "kstack.h"
#include "kdebug.h"
#include "kboard.h"
#include "kobject.h"

EXPORT VOID CORE_SetTaskStackPosition(LPVOID StackPosition)
{
    LPTASK_CONTEXT lpCurrentTask = CORE_GetCurrentTask();
    LPARCH_CONTEXT lpArchContext = GetContextArchParameter(lpCurrentTask);

    SetStackPosition(GetArchUserSD(lpArchContext), StackPosition);
}

EXPORT LPVOID CORE_GetTaskStackPosition(VOID)
{
    LPTASK_CONTEXT lpCurrentTask = CORE_GetCurrentTask();
    LPARCH_CONTEXT lpArchContext = GetContextArchParameter(lpCurrentTask);

    return GetStackPosition(GetArchUserSD(lpArchContext));
}

EXPORT VOID CORE_SetCoreStackPosition(LPVOID StackPosition)
{
    LPTASK_CONTEXT lpCurrentTask = CORE_GetCurrentTask();
    LPARCH_CONTEXT lpArchContext = GetContextArchParameter(lpCurrentTask);

    SetStackPosition(GetArchCoreSD(lpArchContext), StackPosition);
}

EXPORT LPVOID CORE_GetCoreStackPosition(VOID)
{
    LPTASK_CONTEXT lpCurrentTask = CORE_GetCurrentTask();
    LPARCH_CONTEXT lpArchContext = GetContextArchParameter(lpCurrentTask);
    
    return GetStackPosition(GetArchCoreSD(lpArchContext));
}
