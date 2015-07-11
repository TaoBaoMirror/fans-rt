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
#include <fauser.h>
#include <fadefs.h>
#include <faerror.h>
#include <fatypes.h>
#include <fapi.h>

#include "libfw.h"
#include "libcal.h"
#include "ktask.h"
#include "request.h"
#include "cadebug.h"

EXPORT RO_CODE E_STATUS caGetError(VOID)
{
    LPC_REQUEST_PACKET Packet;
    
    memset(&Packet, 0, sizeof(LPC_REQUEST_PACKET));
    
    return caSystemCall(&Packet, STM_MAGIC, LPC_TSS_GET_TASKERROR);
}

EXPORT RO_CODE E_STATUS caSetError(E_STATUS emCode)
{
    LPC_REQUEST_PACKET Packet;

    memset(&Packet, 0, sizeof(LPC_REQUEST_PACKET));
    
    Packet.u0.dParam = emCode;

    return caSystemCall(&Packet, STM_MAGIC, LPC_TSS_SET_TASKERROR);
}

EXPORT RO_CODE HANDLE caGetCurrentTask(VOID)
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

EXPORT RO_CODE TASK_STATUS caGetTaskState(HANDLE hTask)
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

EXPORT RO_CODE E_STATUS caScheduleTimeout(LONG Timeout)
{
    LPC_REQUEST_PACKET Packet;
    
    memset(&Packet, 0, sizeof(LPC_REQUEST_PACKET));
    
    Packet.u0.lParam = Timeout;

    return caSystemCall(&Packet, STM_MAGIC, LPC_TSS_SCHEDULE_TIMEOUT);
}

EXPORT RO_CODE E_STATUS caTaskWakeup(HANDLE hTask)
{
    LPC_REQUEST_PACKET Packet;
    
    memset(&Packet, 0, sizeof(LPC_REQUEST_PACKET));

    Packet.u0.hParam = hTask;

    return caSystemCall(&Packet, STM_MAGIC, LPC_TSS_WAKE_UP);
}

EXPORT RO_CODE BOOL caTestCancel(VOID)
{
    LPC_REQUEST_PACKET Packet;

    memset(&Packet, 0, sizeof(LPC_REQUEST_PACKET));
    
    Packet.u0.dParam = TRUE;
    
    (VOID) caSystemCall(&Packet, STM_MAGIC, LPC_TSS_TEST_CANCEL);
    
    return Packet.u0.dParam;
}

EXPORT RO_CODE E_STATUS caPostCancel(HANDLE hTask)
{
    LPC_REQUEST_PACKET Packet;
    
    memset(&Packet, 0, sizeof(LPC_REQUEST_PACKET));

    Packet.u0.hParam = hTask;

    return caSystemCall(&Packet, STM_MAGIC, LPC_TSS_POST_CANCEL);
}

EXPORT RO_CODE TICK caGetTaskStartTick(HANDLE hTask)
{
    TICK Tick = (TICK) 0;
    LPC_REQUEST_PACKET Packet;

    memset(&Packet, 0, sizeof(LPC_REQUEST_PACKET));

    Packet.u0.hParam = hTask;
    
    if (STATE_SUCCESS != caSystemCall(&Packet, STM_MAGIC, LPC_TSS_GET_STARTTICK))
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

EXPORT RO_CODE TASK_PRIORITY caGetPriority(HANDLE hTask)
{
    LPC_REQUEST_PACKET Packet;

    memset(&Packet, 0, sizeof(LPC_REQUEST_PACKET));
    
    Packet.u0.hParam = hTask;
    Packet.u1.dParam = TASK_PRIORITY_INVALID;
    
    if (STATE_SUCCESS != caSystemCall(&Packet, STM_MAGIC, LPC_TSS_GET_PRIORITY))
    {
        return TASK_PRIORITY_INVALID;
    }
    
    return (TASK_PRIORITY) Packet.u1.dParam;
}

EXPORT RO_CODE E_STATUS caSetPriority(HANDLE hTask, TASK_PRIORITY Priority)
{
    LPC_REQUEST_PACKET Packet;

    memset(&Packet, 0, sizeof(LPC_REQUEST_PACKET));
    
    Packet.u0.hParam = hTask;
    Packet.u1.dParam = Priority;
    
    return caSystemCall(&Packet, STM_MAGIC, LPC_TSS_SET_PRIORITY);
}
EXPORT_SYMBOL(caSetPriority);

EXPORT RO_CODE E_STATUS caCloseTask(HANDLE hTask)
{
    LPC_REQUEST_PACKET Packet;

    memset(&Packet, 0, sizeof(LPC_REQUEST_PACKET));

    Packet.u0.hParam = hTask;
    
    return caSystemCall(&Packet, STM_MAGIC, LPC_TSS_CLOSE_TASK);
}

