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
 *    2014-09-07     JiangYong       new file
 */
#include <string.h>
#include <fadefs.h>
#include <faerror.h>
#include <fatypes.h>

#include "klpc.h"
#include "klist.h"
#include "kcore.h"
#include "kdebug.h"
#include "kobject.h"

STATIC VOLATILE TICK SystemTick = 0;

EXPORT VOID CORE_TickHandler(VOID)
{
    SystemTick ++;
}

EXPORT TICK CORE_GetSystemTick(VOID)
{
    return SystemTick;
}
EXPORT_SYMBOL(CORE_GetSystemTick);

STATIC E_STATUS SVC_GetSystemTick(LPVOID lpPrivate, LPVOID lpParam)
{
    TICK Tick = CORE_GetSystemTick();
    LPLPC_REQUEST_PACKET lpPacket = lpParam;

    lpPacket->u0.dParam = ((Tick >> 0) & 0xffffffff);
    lpPacket->u1.dParam = ((Tick >> 32) & 0xffffffff);

    return STATE_SUCCESS;;
}

STATIC CONST REQUEST_HANDLER fnHandlers[] = {
    SVC_GetSystemTick,              /* LPC_SCM_GET_SYSTICK */
};


DEFINE_LPC_SERVICE(ScmService, SCM_MAGIC, SIZEOF_ARRAY(fnHandlers), NULL, fnHandlers);


PUBLIC E_STATUS initCoreSystemTimerManager(VOID)
{
    LPC_INSTALL(&ScmService, "System clock manager(SCM) starting");

    return STATE_SUCCESS;
}


#if 0
#define TIMER_ENTRY(Ptr, Member)                                \
        CONTAINER_OF(Ptr, TIMER_CLOCK, Member)

struct tagTIMER_CLOCK
{
    KOBJECT_HEADER      Header;
    LIST_HEAD           ClockList;              /**< 闹钟链表 */
    DWORD               dwTimeStep;             /**< 响铃间隔 */
    TICK                tkRingTick;             /**< 响铃时间 */
    LPVOID              lpPrivate;              /**< 私有数据*/
    CLOCK_HANDLER       fnHandler;              /**< 处理函数 */
};

DEFINE_LIST_HEAD(g_ClockList);

STATIC VOLATILE TICK SystemTick = 0;

STATIC INLINE VOID DetachClock(LPTIMER_CLOCK lpTimer)
{
    LIST_REMOVE_NODE(&lpTimer->ClockList);
}

STATIC INLINE VOID AttachClock(LPTIMER_CLOCK lpTimer)
{
    LIST_INSERT_TAIL(&g_ClockList, &lpTimer->ClockList);
}

STATIC VOID Timer_Handler(TICK CurrentTick)
{
    LPLIST_HEAD lpList = NULL;
    LPLIST_HEAD lpNode = NULL;
    DWORD dwFlags = CORE_DisableIRQ();
    
    LIST_FOR_EACH_SAFE(lpList, lpNode, &g_ClockList)
    {
        LPTIMER_CLOCK lpTimer = TIMER_ENTRY(lpList, ClockList);

        switch (OBJECT_GetState(&lpTimer->Header))
        {
        case KOBJECT_STATE_REMOVE:
            DetachClock(lpTimer);
            OBJECT_SetState(&lpTimer->Header, KOBJECT_STATE_DEATH);
            break;
        case KOBJECT_STATE_ACTIVE:
            if (CurrentTick >= lpTimer->tkRingTick)
            {
                LPVOID lpPrivate = lpTimer->lpPrivate;
                CLOCK_HANDLER fnHandler = lpTimer->fnHandler;
                /* 计算下一次响铃时间 */
                if (0 != lpTimer->dwTimeStep)
                {
                    lpTimer->tkRingTick = CurrentTick + lpTimer->dwTimeStep;
                }
                else
                {
                    OBJECT_SetState(&lpTimer->Header, KOBJECT_STATE_REMOVE);
                }
                CORE_RestoreIRQ(dwFlags);
                fnHandler(lpPrivate);
                dwFlags = CORE_DisableIRQ();
            }
            break;
        default:
            ;
        }
    }

    CORE_RestoreIRQ(dwFlags);
    
    return;
}



typedef struct tagTIMER_PARAM TIMER_PARAM;
typedef struct tagTIMER_PARAM * PTIMER_PARAM;
typedef struct tagTIMER_PARAM FAR * LPTIMER_PARAM;

struct tagTIMER_PARAM{
    DWORD               dwTimeout;              /**< 超时时间: 闹钟使用 */
    DWORD               dwTimeStep;             /**< 响铃间隔: 定时器使用 */
    LPVOID              lpPrivate;              /**< 私有数据*/
    CLOCK_HANDLER       fnHandler;              /**< 处理函数 */
};

PUBLIC VOID ActiveAlarmClock(LPTIMER_CLOCK lpTimer, DWORD Ticks)
{
    DWORD dwFlags = 0;

    lpTimer->tkRingTick = GetSystemTick() + Ticks;
    OBJECT_SetState(&lpTimer->Header, KOBJECT_STATE_ACTIVE);
    
    dwFlags = CORE_DisableIRQ();
    
    AttachClock(lpTimer);

    CORE_RestoreIRQ(dwFlags);
}

STATIC E_STATUS Timer_CreateObject(LPKOBJECT_HEADER lpHeader, LPVOID lpParam)
{
    DWORD dwFlags = 0;
    LPTIMER_PARAM lpTParam = lpParam;
    LPTIMER_CLOCK lpTimer = (LPVOID) lpHeader;

    lpTimer->dwTimeStep =   lpTParam->dwTimeStep;
    lpTimer->tkRingTick =   (0 != lpTParam->dwTimeStep)
                        ?   (GetSystemTick() + lpTimer->dwTimeStep)
                        :   (GetSystemTick() + lpTParam->dwTimeout);
    lpTimer->lpPrivate  =   lpTParam->lpPrivate;
    lpTimer->fnHandler  =   lpTParam->fnHandler;
    
    dwFlags = CORE_DisableIRQ();
    
    AttachClock(lpTimer);

    CORE_RestoreIRQ(dwFlags);
    
    return STATE_SUCCESS;
}

STATIC E_STATUS Timer_DeleteObject(LPKOBJECT_HEADER lpHeader)
{
    CORE_TRACE(TRUE, "Delete object '%s', class name '%s', class id %d.",
        OBJECT_GetName(lpHeader), OBJECT_GetClassName(lpHeader), OBJECT_GetClassID(lpHeader));
    
    while (KOBJECT_STATE_DEATH != OBJECT_GetState(lpHeader))
    {
        CORE_INFOR(TRUE, "Object state is %d !", OBJECT_GetState(lpHeader));
    }
    
    return STATE_SUCCESS;
}


OBJECT_METHODS TimerMethods = {
    Timer_CreateObject,
    Timer_DeleteObject,
};

DEFINE_CLASS(TimerClass, CLOCK_MAGIC, sizeof(TIMER_CLOCK), &TimerMethods);

STATIC HANDLE CreateClock(DWORD dwTimeout, DWORD dwInterval, LPVOID lpData, CLOCK_HANDLER fnHandler)
{
    TIMER_PARAM Param = { 0 };
    CHAR caName[OBJECT_NAME_MAX] = { 0 };
    LPTIMER_CLOCK lpTimer = NULL;

    Param.dwTimeStep = dwInterval;
    Param.dwTimeout  = dwTimeout;
    Param.lpPrivate  = lpData;
    Param.fnHandler  = fnHandler;

    ChooseName(caName, "CLK_");

//  lpTimer = CreateObject(NULL, CLOCK_MAGIC, caName, &Param);

    if (NULL == lpTimer)
    {
        return INVALID_HANDLE_VALUE;
    }
    
    return ((HANDLE) lpTimer);
}
EXPORT_SYMBOL(CreateClock)

FANSAPI HANDLE CreateTimerClock(DWORD dwInterval, LPVOID lpData, CLOCK_HANDLER fnHandler)
{
    return CreateClock( 0, dwInterval, lpData, fnHandler);
}
EXPORT_SYMBOL(CreateTimerClock)

FANSAPI HANDLE CreateAlarmClock(DWORD dwTimeout, LPVOID lpData, CLOCK_HANDLER fnHandler)
{
    return CreateClock(dwTimeout, 0, lpData, fnHandler);
}
EXPORT_SYMBOL(CreateAlarmClock)
#endif
