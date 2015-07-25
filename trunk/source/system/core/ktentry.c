/*
 *    Copyright(C) 2013-2015, Fans-rt development team.
 *
 *    All rights reserved.
 *
 *    This is open source software.
 *    Learning and research can be unrestricted to  modification, use and dissemination.
 *    If you need for commercial purposes, you should get the author's permission.
 *
 *    date           author          notes
 *    2015-06-28     JiangYong       new file
 */
#include <stdio.h>
#include <string.h>
#include <fadefs.h>
#include <faerror.h>
#include <fatypes.h>

#include "klpc.h"
#include "ktask.h"
#include "klist.h"
#include "kcore.h"
#include "kstack.h"
#include "kdebug.h"

#include "libfw.h"
#include "libcal.h"

#if 0
STATIC E_STATUS CloseTaskContext(LPTASK_CONTEXT lpTaskContext)
{
    CORE_StackFree(lpTaskContext, TASK_PERMISSION_USER);
    CORE_StackFree(lpTaskContext, TASK_PERMISSION_CORE);
    
    SetContextCancel(lpTaskContext, TRUE);
    
    return CORE_FreeObject(GetContextHeader(lpTaskContext));
}
#endif

EXPORT RO_CODE E_STATUS CORE_IdleMain(LPVOID lpParam)
{
    LPTASK_CONTEXT lpIdleContext = lpParam;
    CORE_DEBUG(TRUE, "System idle task working now ....");

    do{
        FW_SystemIdle();
    }while(FALSE == GetContextCancel(lpIdleContext));
    
    return STATE_SUCCESS;
}


/**
 * 任务入口函数
 * @param fnMain 任务主函数指针
 * @param lpArgument 任务入口参数
 * @param HANDLE hTask 任务句柄
 * @return VOID
 * \par
 *     新任务入口函数需要传入三个参数，在ARCH层实现的任务堆栈填充函数需要将这
 * 三个参数放入恰当的堆栈位置。
 */
EXPORT RO_CODE VOID CORE_TaskEntry(FNTASKMAIN fnMain, LPVOID lpArgument, LPTASK_CONTEXT lpTaskContext)
{
    CORE_ASSERT(fnMain, , "The task main function is NULL.");
    CORE_ASSERT(lpTaskContext, , "The task context object is NULL.");

    CORE_DEBUG(TRUE, "Task 0x%08X name '%s' main 0x%P permission %u starting(%lld) now ...",
            GetContextHandle(lpTaskContext), GetContextTaskName(lpTaskContext), fnMain,
            GetContextPermission(lpTaskContext), GetContextStartTick(lpTaskContext));
    
    if (TASK_PRIORITY_IDLE == GetContextThisPriority(lpTaskContext))
    {
        lpArgument = lpTaskContext;
    }
    
    fnMain(lpArgument);
    
    CORE_ERROR(TRUE, "BUG: ****The task(object: 0x%P) has been died ****", lpTaskContext);
}


EXPORT VOID CORE_TaskLeave(VOID)
{
    CORE_CloseTask(CORE_GetCurrentTask());
    while(1) caScheduleTimeout(1000);
}
