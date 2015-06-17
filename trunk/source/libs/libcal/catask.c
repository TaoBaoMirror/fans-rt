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
#include <stdio.h>
#include <string.h>
#include <fadefs.h>
#include <faerror.h>
#include <fatypes.h>
#include <fapi.h>

#include "libfw.h"
#include "libcal.h"
#include "ktask.h"
#include "request.h"
#include "cadebug.h"

EXPORT E_STATUS caIdleEntry(LPVOID lpParam)
{
    LOG_INFOR(TRUE, "System idle task working now ....");

    do{
        FW_SystemIdle();
    }while(FALSE == caTestCancel());
    
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
EXPORT VOID caTaskEntry(FNTASKMAIN fnMain, LPVOID lpArgument, HANDLE hTask)
{
    CHAR Name[OBJECT_NAME_MAX];
    
    caGetTaskName(hTask, Name);
    
    LOG_DEBUG(TRUE, "Task 0x%08X name '%s' main 0x%P starting(%lld) now ...",
        hTask, Name, fnMain, caGetTaskStartTick(hTask));

    fnMain(lpArgument);
    
    caCloseTask(hTask);
    
    LOG_ERROR(TRUE, "BUG: ****The task has been died ****");
    
    while(1) caScheduleTimeout(1000);
}

EXPORT E_STATUS caGetError(VOID)
{
    LPC_REQUEST_PACKET Packet;
    
    memset(&Packet, 0, sizeof(LPC_REQUEST_PACKET));
    
    return caSystemCall(&Packet, STM_MAGIC, LPC_TSS_GET_TASKERROR);
}

EXPORT E_STATUS caSetError(E_STATUS emCode)
{
    LPC_REQUEST_PACKET Packet;

    memset(&Packet, 0, sizeof(LPC_REQUEST_PACKET));
    
    Packet.u0.dParam = emCode;

    return caSystemCall(&Packet, STM_MAGIC, LPC_TSS_SET_TASKERROR);
}

EXPORT HANDLE caGetCurrentTask(VOID)
{
    E_STATUS Result;
    LPC_REQUEST_PACKET Packet;
    
    memset(&Packet, 0, sizeof(LPC_REQUEST_PACKET));
    
    if (STATE_SUCCESS != (Result = caSystemCall(&Packet, STM_MAGIC, LPC_TSS_GET_CURRENT)))
    {
        SetError(Result);
        return INVALID_HANDLE_VALUE;
    }

    return Packet.u0.hParam;
}

EXPORT TASK_STATUS caGetTaskState(HANDLE hTask)
{
    E_STATUS Result;
    LPC_REQUEST_PACKET Packet;
    
    memset(&Packet, 0, sizeof(LPC_REQUEST_PACKET));
    
    if (STATE_SUCCESS != (Result = caSystemCall(&Packet, STM_MAGIC, LPC_TSS_GET_TASKSTATE)))
    {
        SetError(Result);
        return TASK_STATE_MAX;
    }

    return (TASK_STATUS) Packet.u0.dParam;
}

EXPORT E_STATUS caScheduleTimeout(LONG Timeout)
{
    LPC_REQUEST_PACKET Packet;
    
    memset(&Packet, 0, sizeof(LPC_REQUEST_PACKET));
    
    Packet.u0.lParam = Timeout;

    return caSystemCall(&Packet, STM_MAGIC, LPC_TSS_SCHEDULE_TIMEOUT);
}

EXPORT E_STATUS caTaskWakeup(HANDLE hTask)
{
    LPC_REQUEST_PACKET Packet;
    
    memset(&Packet, 0, sizeof(LPC_REQUEST_PACKET));

    Packet.u0.hParam = hTask;

    return caSystemCall(&Packet, STM_MAGIC, LPC_TSS_WAKE_UP);
}

EXPORT BOOL caTestCancel(VOID)
{
    LPC_REQUEST_PACKET Packet;

    memset(&Packet, 0, sizeof(LPC_REQUEST_PACKET));
    
    Packet.u0.dParam = TRUE;
    
    (VOID) caSystemCall(&Packet, STM_MAGIC, LPC_TSS_TEST_CANCEL);
    
    return Packet.u0.dParam;
}

EXPORT E_STATUS caPostCancel(HANDLE hTask)
{
    LPC_REQUEST_PACKET Packet;
    
    memset(&Packet, 0, sizeof(LPC_REQUEST_PACKET));

    Packet.u0.hParam = hTask;

    return caSystemCall(&Packet, STM_MAGIC, LPC_TSS_POST_CANCEL);
}

EXPORT E_STATUS caGetTaskName(HANDLE hTask, CHAR Name[OBJECT_NAME_MAX])
{
    LPC_REQUEST_PACKET Packet;

    memset(&Packet, 0, sizeof(LPC_REQUEST_PACKET));

    Packet.u0.hParam = hTask;
    Packet.u1.pParam = Name;

    return caSystemCall(&Packet, STM_MAGIC, LPC_TSS_GET_TASKNAME);
}

EXPORT TICK caGetTaskStartTick(HANDLE hTask)
{
    TICK Tick = (TICK) 0;
    LPC_REQUEST_PACKET Packet;

    memset(&Packet, 0, sizeof(LPC_REQUEST_PACKET));

    Packet.u0.hParam = hTask;
    
    if (STATE_SUCCESS != caSystemCall(&Packet, STM_MAGIC, LPC_TSS_GET_TASKTICK))
    {
        return INVALID_TICK;
    }

#ifdef SYSTEM_HAVE_TICK64
    Tick = Packet.u2.dParam;
    Tick = (Tick << 32);
#endif
    Tick += Packet.u1.dParam;

    return Tick;
}

EXPORT TASK_PRIORITY caGetPriority(HANDLE hTask)
{
    LPC_REQUEST_PACKET Packet;
    
    Packet.u0.hParam = hTask;
    Packet.u1.dParam = TASK_PRIORITY_INVALID;
    
    if (STATE_SUCCESS != caSystemCall(&Packet, STM_MAGIC, LPC_TSS_GET_PRIORITY))
    {
        return TASK_PRIORITY_INVALID;
    }
    
    return (TASK_PRIORITY) Packet.u1.dParam;
}

EXPORT E_STATUS caSetPriority(HANDLE hTask, TASK_PRIORITY Priority)
{
    LPC_REQUEST_PACKET Packet;
    
    Packet.u0.hParam = hTask;
    Packet.u1.dParam = Priority;
    
    return caSystemCall(&Packet, STM_MAGIC, LPC_TSS_SET_PRIORITY);
}
EXPORT_CORE_SYMBOL(caSetPriority);

EXPORT E_STATUS caCloseTask(HANDLE hTask)
{
    LPC_REQUEST_PACKET Packet;

    memset(&Packet, 0, sizeof(LPC_REQUEST_PACKET));

    Packet.u0.hParam = hTask;
    
    return caSystemCall(&Packet, STM_MAGIC, LPC_TSS_CLOSE_TASK);
}

EXPORT E_STATUS caStackMalloc(HANDLE hTask, LPVOID lpParam)
{
    LPC_REQUEST_PACKET Packet;

    memset(&Packet, 0, sizeof(LPC_REQUEST_PACKET));

    Packet.u0.hParam = hTask;
    Packet.u1.pParam = lpParam;
    
    return caSystemCall(&Packet, STM_MAGIC, LPC_TSS_STACK_MALLOC);
}
