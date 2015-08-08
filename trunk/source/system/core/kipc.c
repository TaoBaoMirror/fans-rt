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
#include <string.h>
#include <fadefs.h>
#include <faerror.h>
#include <fatypes.h>

#include "klpc.h"
#include "kipc.h"
#include "ktask.h"
#include "kpool.h"
#include "klist.h"
#include "kcore.h"
#include "kdebug.h"
#include "ktable.h"
//#define IPC_DEBUG_ENABLE TRUE

#if (IPC_DEBUG_ENABLE == TRUE)
#define     IPC_DEBUG(Enter, ...)                            CORE_DEBUG(Enter, __VA_ARGS__)
#define     IPC_INFOR(Enter, ...)                            CORE_INFOR(Enter, __VA_ARGS__)
#define     IPC_ERROR(Enter, ...)                            CORE_ERROR(Enter, __VA_ARGS__)
#ifndef _MSC_VER
#define     IPC_ASSERT(condition, code, ...)                 CORE_ASSERT(condition, code, __VA_ARGS__)
#else
#define     IPC_ASSERT(condition, code, ...)
#endif
#else
#define     IPC_DEBUG(...)
#define     IPC_INFOR(...)
#define     IPC_ERROR(...)
#define     IPC_ASSERT(condition, code, ...)
#endif

#define     IPC_MallocEvent                         IPC_DummyOperation
#define     IPC_TakeEvent                           IPC_DummyOperation

#define     IPC_MallocMutex                         IPC_DummyOperation
#define     IPC_TakeMutex                           IPC_DummyOperation
#define     IPC_ResetMutex                          IPC_DummyOperation

#define     IPC_MallocSemaphore                     IPC_DummyOperation
#define     IPC_TakeSemaphore                       IPC_DummyOperation
#define     IPC_ResetSemaphore                      IPC_DummyOperation

#define     IPC_MallocSemset                        IPC_DummyOperation
#define     IPC_TakeSemset                          IPC_DummyOperation
#define     IPC_ResetSemset                         IPC_DummyOperation

#define     IPC_DetachSemaphore                     IPC_DetachDefault
#define     IPC_DetachSemset                        IPC_DetachDefault
#define     IPC_DetachEvent                         IPC_DetachDefault

/************************************************************************************
                                    Event Class
************************************************************************************/

typedef struct tagIPC_EVENT_OBJECT IPC_EVENT_OBJECT;
typedef struct tagIPC_EVENT_OBJECT * PIPC_EVENT_OBJECT;
typedef struct tagIPC_EVENT_OBJECT * LPIPC_EVENT_OBJECT;

struct tagIPC_EVENT_OBJECT{
    KIPC_CLASS_HEADER               Base;
    VOLATILE EVENT_ATTRIBUTE        Attribute;
};

#define     GetEventAttribute(lpHeader)                                                             \
            (((LPIPC_EVENT_OBJECT)(lpHeader))->Attribute.Value)
#define     SetEventAttribute(lpHeader, data)                                                       \
            do { (((LPIPC_EVENT_OBJECT)(lpHeader))->Attribute.Value) = (data); } while(0)
#define     GetEventAutomatic(lpHeader)                                                             \
            (((LPIPC_EVENT_OBJECT)(lpHeader))->Attribute.Byte.Automatic)
#define     SetEventAutomatic(lpHeader, value)                                                      \
            do { ((LPIPC_EVENT_OBJECT)(lpHeader))->Attribute.Byte.Automatic = value; } while(0)
#define     GetEventSignal(lpHeader)                                                                \
            (((LPIPC_EVENT_OBJECT)(lpHeader))->Attribute.Byte.Signal)
#define     SetEventSignal(lpHeader, value)                                                         \
            do { ((LPIPC_EVENT_OBJECT)(lpHeader))->Attribute.Byte.Signal = value; } while(0)
#define     IncEventBlockedTasks(lpHeader)                                                               \
            (++ ((LPIPC_EVENT_OBJECT)(lpHeader))->Attribute.Bits.Blocked)
#define     DecEventBlockedTasks(lpHeader)                                                               \
            (-- ((LPIPC_EVENT_OBJECT)(lpHeader))->Attribute.Bits.Blocked)
/************************************************************************************
                                    Mutex Class
************************************************************************************/

typedef struct tagIPC_MUTEX_OBJECT IPC_MUTEX_OBJECT;
typedef struct tagIPC_MUTEX_OBJECT * PIPC_MUTEX_OBJECT;
typedef struct tagIPC_MUTEX_OBJECT * LPIPC_MUTEX_OBJECT;

struct tagIPC_MUTEX_OBJECT{
    KIPC_CLASS_HEADER               Base;
    VOLATILE MUTEX_ATTRIBUTE        Attribute;
};

#define     CheckIsOnwerTask(lpHeader, lpTask)                                                      \
            (GetMutexOnwer(lpHeader) == (GetContextHandle(lpTask) & MUTEX_ONWER_MASK))
#define     GetMutexOnwer(lpHeader)                                                                 \
            (((LPIPC_MUTEX_OBJECT)(lpHeader))->Attribute.Handle & MUTEX_ONWER_MASK)
#define     SetMutexOnwer(lpHeader, hOnwer)                                                         \
            do {                                                                                    \
                (((LPIPC_MUTEX_OBJECT)(lpHeader))->Attribute.Bits.hOnwerTask)                       \
                                    = ((hOnwer)>> MUTEX_VALUE_BITS);                                \
            } while(0)

#define     GetMutexValue(lpHeader)                                                                 \
            (((LPIPC_MUTEX_OBJECT)(lpHeader))->Attribute.Bits.MutexValue)
#define     SetMutexValue(lpHeader, data)                                                           \
            do { (((LPIPC_MUTEX_OBJECT)(lpHeader))->Attribute.Bits.MutexValue) = (data); } while(0)
#define     IncMutexValue(lpHeader)                                                                 \
            (++ ((LPIPC_MUTEX_OBJECT)(lpHeader))->Attribute.Bits.MutexValue)
#define     DecMutexValue(lpHeader)                                                                 \
            (-- ((LPIPC_MUTEX_OBJECT)(lpHeader))->Attribute.Bits.MutexValue)


/************************************************************************************
                                 Semaphore Class
************************************************************************************/
typedef struct tagIPC_SEMAPHORE_OBJECT IPC_SEMAPHORE_OBJECT;
typedef struct tagIPC_SEMAPHORE_OBJECT * PIPC_SEMAPHORE_OBJECT;
typedef struct tagIPC_SEMAPHORE_OBJECT * LPIPC_SEMAPHORE_OBJECT;

struct tagIPC_SEMAPHORE_OBJECT{
    KIPC_CLASS_HEADER               Base;
    VOLATILE SEMAPHORE_ATTRIBUTE    Attribute;
};

#define     GetSemaphoreValue(lpHeader)                                                                     \
                (((LPIPC_SEMAPHORE_OBJECT)(lpHeader))->Attribute.Value)
#define     SetSemaphoreValue(lpHeader, lpAttribute)                                                        \
                do { (((LPIPC_SEMAPHORE_OBJECT)(lpHeader))->Attribute.Value) = (lpAttribute)->Value & (~0x1); } while(0)
#define     GetSemaphoreSignals(lpHeader)                                                                   \
                (((LPIPC_SEMAPHORE_OBJECT)(lpHeader))->Attribute.Bits.Signal)
#define     SetSemaphoreSignals(lpHeader, data)                                                             \
                do { (((LPIPC_SEMAPHORE_OBJECT)(lpHeader))->Attribute.Bits.Signal) = (data); } while(0)
#define     IncSemaphoreSignals(lpHeader)                                                                   \
                (++ ((LPIPC_SEMAPHORE_OBJECT)(lpHeader))->Attribute.Bits.Signal)
#define     DecSemaphoreSignals(lpHeader)                                                                   \
                (-- ((LPIPC_SEMAPHORE_OBJECT)(lpHeader))->Attribute.Bits.Signal)
#define     GetSemaphoreMaxCount(lpHeader)                                                                  \
                (((LPIPC_SEMAPHORE_OBJECT)(lpHeader))->Attribute.Bits.MaxCount)
#define     SetSemaphoreMaxCount(lpHeader, data)                                                            \
                do { (((LPIPC_SEMAPHORE_OBJECT)(lpHeader))->Attribute.Bits.MaxCount) = (data); } while(0)

/****************************************************************************
                               Semset Class
****************************************************************************/

typedef struct tagIPC_SEMSET_OBJECT IPC_SEMSET_OBJECT;
typedef struct tagIPC_SEMSET_OBJECT * PIPC_SEMSET_OBJECT;
typedef struct tagIPC_SEMSET_OBJECT * LPIPC_SEMSET_OBJECT;

struct tagIPC_SEMSET_OBJECT{
    KIPC_CLASS_HEADER               Base;
    VOLATILE SEMSET_ATTRIBUTE       Attribute;
};


#define     GetSemsetMaskValue(lpHeader)                                                            \
                (((LPIPC_SEMSET_OBJECT)(lpHeader))->Attribute.Bits.LightMask)
#define     SetSemsetMaskValue(lpHeader, data)                                                      \
                do { (((LPIPC_SEMSET_OBJECT)(lpHeader))->Attribute.Bits.LightMask) = (data); } while(0)
#define     SetSemsetMaskBit(lpHeader, shift, boolean)                                              \
                do { SET_BIT_VALUE(GetSemsetMaskValue(lpHeader), shift, boolean); } while(0)
#define     GetSemsetWaitFull(lpHeader)                                                             \
                 (((LPIPC_SEMSET_OBJECT)(lpHeader))->Attribute.Bits.Full)
#define     GetSemsetFullMask(lpHeader)                                                             \
                 GetBitsMaskValue(((LPIPC_SEMSET_OBJECT)(lpHeader))->Attribute.Bits.Lights - 1)
#define     GetSemsetBlockedTasks(lpHeader)                                                           \
                 (((LPIPC_SEMSET_OBJECT)(lpHeader))->Attribute.Bits.Blocked)
#define     IncSemsetBlockedTasks(lpHeader)                                                           \
                 (++ ((LPIPC_SEMSET_OBJECT)(lpHeader))->Attribute.Bits.Blocked)
#define     DecSemsetBlockedTasks(lpHeader)                                                           \
                 (-- ((LPIPC_SEMSET_OBJECT)(lpHeader))->Attribute.Bits.Blocked)
/**
 * Task insert to the IPC queue.
 * @param The context of the task.
 * @return VOID
 * \par
 * If need wait an IPC object, the task will be insert to IPC wait queue
 * (Best appropriate priority in the queue).
 *
 * 如果需要等待一个 IPC 对象，任务需要插入到 IPC 对象的等待队列，插入队列
 * 的位置根据队列中任务的优先级进行排列。
 *
 * date           author          notes
 * 2015-06-13     JiangYong       first version
 */
STATIC VOID Insert2WaitQueue(LPLIST_HEAD lpHead, LPTASK_CONTEXT lpTaskContext)
{
    LPLIST_HEAD lpList = lpHead;

    IPC_INFOR(TRUE, "Insert 2 wait queue task '%s' priority %u.",
            GetContextTaskName(lpTaskContext), GetContextThisPriority(lpTaskContext));
    
    LIST_FOR_EACH(lpList, lpHead)
    {
        LPTASK_CONTEXT lpWaitContext = GetContextByIPCNode(lpList);
        
        IPC_INFOR(TRUE, "Wait queue task '%s-%p' priority %u.",
            GetContextTaskName(lpWaitContext), lpWaitContext, 
            GetContextThisPriority(lpWaitContext));
        
        if (GetContextThisPriority(lpWaitContext) >
            GetContextThisPriority(lpTaskContext))
        {
            break;
        }
    }

    AttachIPCNode(lpList, lpTaskContext);
    SetContextState(lpTaskContext, TASK_STATE_WAITING);
}


STATIC E_STATUS IPC_DummyOperation(LPKOBJECT_HEADER lpHeader, LPVOID lpParam)
{
    return STATE_SUCCESS;
}


STATIC E_STATUS IPC_BlockTask(LPKOBJECT_HEADER lpHeader, LPVOID lpParam)
{
    E_STATUS State = STATE_SUCCESS;
    LONG WaitTime = GetWaitTime4mParam(lpParam);
    LPTASK_CONTEXT lpCurrentTask = CORE_GetCurrentTask();

    if (STATE_SUCCESS == (State = CORE_TaskSuspend(lpCurrentTask, WaitTime)))
    {
        /* 将当前任务插入到阻塞队列 */
        SetREQPrivate(GetContextLPCPacket(lpCurrentTask), lpHeader);
        Insert2WaitQueue(GetIPCWaitQueue(lpHeader), lpCurrentTask);
        return STATE_TIME_OUT;
    }
    
    return State;
}



STATIC E_STATUS IPC_WakeupTask(LPKOBJECT_HEADER lpHeader, SHORT SignalID, BOOL IsMutex)
{
    LPKIPC_WAIT_PARAM lpWaitParam;
    LPLPC_REQUEST_PACKET lpPacket;
    E_STATUS State = STATE_SUCCESS;
    LPTASK_CONTEXT lpTaskContext = GetFirstWaitTask(lpHeader);
    /* 阻塞队列为空，表明有 BUG */
    IPC_ASSERT(NULL != GetFirstWaitNode(lpHeader), SYSTEM_CALL_OOPS(),
        "BUG: Bad wait task queue, semaphore'%s'.",  GetObjectName(lpHeader));

    lpPacket = GetContextLPCPacket(lpTaskContext);

    IPC_ASSERT(lpPacket, SYSTEM_CALL_OOPS(),
        "BUG: Post semaphore '%s' failed, invalid request packet.", GetObjectName(lpHeader));

    if (KOBJECT_STATE_ACTIVE != GetObjectState(lpHeader))
    {
        SetREQResult(lpPacket, STATE_REMOVED);
        
        State = CORE_TaskWakeup(lpTaskContext);

        IPC_INFOR(TRUE, "Mutex '%s' remove, task '%s' will be wakeup ...",
                   GetObjectName(lpHeader), GetContextTaskName(lpTaskContext));
    }
    else
    {
    
        SetREQResult(lpPacket, STATE_SUCCESS);

        lpWaitParam = REQpParam(lpPacket, u1);

        IPC_ASSERT(NULL != lpWaitParam, SYSTEM_CALL_OOPS(), "Invalid IPC param for task(%s)!",
                    GetContextTaskName(lpTaskContext));

        SetSignalID2Param(lpWaitParam, SignalID);

        if (IsMutex)
        {
            SetMutexOnwer(lpHeader, GetContextHandle(lpTaskContext));
        }

        State = CORE_TaskWakeup(lpTaskContext);

        IPC_INFOR(TRUE, "Post '%s' and wakeup task '%s', task state %d, result %d...",
            GetObjectName(lpHeader), GetContextTaskName(lpTaskContext),
            GetContextState(lpTaskContext), State);
    }
        
    return State;
}

STATIC E_STATUS IPC_DetachDefault(LPKOBJECT_HEADER lpHeader, LPVOID lpParam)
{
    if (NULL != lpParam)
    {
        LPTASK_CONTEXT lpTaskContext = lpParam;
        DWORD dwFlags = CORE_DisableIRQ();
 
        DetachIPCNode(lpTaskContext);
        CORE_RestoreIRQ(dwFlags);
 
        return STATE_SUCCESS;
    }
    
    return STATE_INVALID_PARAMETER;
}

/************************************************************************************
                                    Event Class
************************************************************************************/
STATIC SIZE_T IPC_SizeofEvent(LPKCLASS_DESCRIPTOR lpClass, LPVOID lpParam)
{
    return sizeof(IPC_EVENT_OBJECT);
}

STATIC E_STATUS IPC_ActiveEvent(LPKOBJECT_HEADER lpHeader, LPVOID lpParam)
{
    LPEVENT_ATTRIBUTE lpAttribute = lpParam;

    if (NULL == lpParam)
    {
        IPC_ERROR(TRUE, "Invalid parameter to create event '%s'.", GetObjectName(lpHeader));
        return STATE_INVALID_PARAMETER;
    }
    
    if (0 != (lpAttribute->Value & (~EVENT_BITS_MASK)))
    {
        IPC_ERROR(TRUE, "Invalid value to create event '%s'.", GetObjectName(lpHeader));
        return STATE_INVALID_VALUE;
    }
    
    LIST_HEAD_INIT(GetIPCWaitQueue(lpHeader));
    SetEventAttribute(lpHeader, lpAttribute->Value);    

    return STATE_SUCCESS;
}


STATIC E_STATUS IPC_WaitEvent(LPKOBJECT_HEADER lpHeader, LPVOID lpParam)
{
    DWORD dwFlags;

    IPC_INFOR(TRUE, "wait event '%s', state '%d'...",
            GetObjectName(lpHeader), GetEventSignal(lpHeader));
    
    IPC_SPIN_LOCK_IRQ(lpHeader, dwFlags);;

    if (TRUE != GetEventSignal(lpHeader))
    {
        IPC_BlockTask(lpHeader, lpParam);
        IncEventBlockedTasks(lpHeader);
    }
    else
    {
        SetEventSignal(lpHeader, FALSE);
    }
    
    IPC_SPIN_UNLOCK_IRQ(lpHeader, dwFlags);
    
    return STATE_TIME_OUT;
}

STATIC E_STATUS IPC_PostEvent(LPKOBJECT_HEADER lpHeader, LPVOID lpParam)
{
    DWORD dwFlags = 0;
    E_STATUS State = STATE_SUCCESS;

    IPC_SPIN_LOCK_IRQ(lpHeader, dwFlags);

    if (TRUE != GetEventAutomatic(lpHeader))
    {
        SetEventSignal(lpHeader, TRUE);
    }
    else
    {
        SetEventSignal(lpHeader, FALSE);
    }

    if (DecEventBlockedTasks(lpHeader) > 0)
    do {
        State = IPC_WakeupTask(lpHeader, WAIT_SIGNAL_ID_0, FALSE);
    }while(DecEventBlockedTasks(lpHeader) > 0);

    IPC_SPIN_UNLOCK_IRQ(lpHeader, dwFlags);
    
    IPC_DEBUG(TRUE, "Event %s automatic is '%d', state is '%d' ...",
            GetObjectName(lpHeader), GetEventAutomatic(lpHeader), GetEventSignal(lpHeader));

    return State;
}

STATIC E_STATUS IPC_ResetEvent(LPKOBJECT_HEADER lpHeader, LPVOID lpParam)
{
    DWORD dwFlags = 0;
    
    IPC_SPIN_LOCK_IRQ(lpHeader, dwFlags);
    
    SetEventSignal(lpHeader, FALSE);
    
    IPC_SPIN_UNLOCK_IRQ(lpHeader, dwFlags);
    
    return STATE_SUCCESS;
}


STATIC E_STATUS IPC_FreeEvent(LPKOBJECT_HEADER lpHeader)
{
    DWORD dwFlags = 0;
    E_STATUS State = STATE_SUCCESS;

    IPC_SPIN_LOCK_IRQ(lpHeader, dwFlags);

    if (DecEventBlockedTasks(lpHeader) > 0)
    do {
        State = IPC_WakeupTask(lpHeader, WAIT_SIGNAL_ID_0, FALSE);
    }while(DecEventBlockedTasks(lpHeader) > 0);
    
    IPC_SPIN_UNLOCK_IRQ(lpHeader, dwFlags);
    
    IPC_DEBUG(TRUE, "Event %s automatic is '%d', state is '%d' ...",
            GetObjectName(lpHeader), GetEventAutomatic(lpHeader), GetEventSignal(lpHeader));

    return State;
}

DEFINE_KCLASS(KIPC_CLASS_DESCRIPTOR,
              EventClass,
              EVT_MAGIC,
              KIPC_CLASS_METHODS,
              IPC_SizeofEvent,
              IPC_MallocEvent,
              IPC_ActiveEvent,
              IPC_TakeEvent,
              IPC_FreeEvent,
              IPC_WaitEvent,
              IPC_PostEvent,
              IPC_ResetEvent,
              IPC_DetachDefault);



/************************************************************************************
                                    Mutex Class
************************************************************************************/    
STATIC SIZE_T IPC_SizeofMutex(LPKCLASS_DESCRIPTOR lpClass, LPVOID lpParam)
{
    return sizeof(IPC_MUTEX_OBJECT);
}

STATIC E_STATUS IPC_ActiveMutex(LPKOBJECT_HEADER lpHeader, LPVOID lpParam)
{
    LPMUTEX_ATTRIBUTE lpAttribute = lpParam;
    
    if (NULL == lpParam)
    {
        IPC_ERROR(TRUE, "Invalid parameter to create mutex '%s'.", GetObjectName(lpHeader));
        return STATE_INVALID_PARAMETER;
    }

    if (0 == lpAttribute->Bits.MutexValue)
    {
        LPTASK_CONTEXT lpCurrentTask = CORE_GetCurrentTask();
        IPC_ASSERT(lpCurrentTask, SYSTEM_CALL_OOPS(), "Current task context not found ?");
        lpAttribute->Value |= GetContextHandle(lpCurrentTask) & (~HANDLE_OBJECT_SID_MASK);
    }
    else if (1 != lpAttribute->Bits.MutexValue)
    {
        IPC_ERROR(TRUE, "Invalid value to create mutex '%s'.", GetObjectName(lpHeader));
        return STATE_INVALID_VALUE;
    }

    LIST_HEAD_INIT(GetIPCWaitQueue(lpHeader));
    SetIPCAttribute(lpHeader, lpAttribute);
    
    IPC_INFOR(TRUE, "Create mutex (%s - 0x%P) ...", GetObjectName(lpHeader), lpHeader);

    return STATE_SUCCESS;
}

STATIC E_STATUS IPC_LockMutex(LPKOBJECT_HEADER lpHeader, LPVOID lpParam)
{
    DWORD dwFlags;
    LPTASK_CONTEXT lpCurrentTask;
    LPTASK_CONTEXT lpOnwerContext;

    IPC_INFOR(TRUE, "Lock mutex '%s' by '%s', value is %d.",
        GetObjectName(lpHeader), GetContextTaskName(CORE_GetCurrentTask()), 
        GetMutexValue(lpHeader));
    
    if (NULL == lpParam)
    {
        CORE_ERROR(TRUE, "Invalid parameter for lock mutex '%s'.", GetObjectName(lpHeader));
        return STATE_INVALID_PARAMETER;
    }

    /* 如果 MUTEX VALUE - 1 后等于 0 则说明 MUTEX 没有锁 */
    /* 注: MUTEX VALUE 的最大值为 1，不会超过 1*/
    if (0 == DecMutexValue(lpHeader))
    {
        lpOnwerContext = CORE_GetCurrentTask();

        IPC_ASSERT(lpOnwerContext, SYSTEM_CALL_OOPS(), "Current task context not found ?");
        SetMutexOnwer(lpHeader, GetContextHandle(lpOnwerContext));
        SetSignalID2Param(lpParam, WAIT_SIGNAL_ID_0);

        return STATE_SUCCESS;
    }

    IPC_SPIN_LOCK_IRQ(lpHeader, dwFlags);
    lpOnwerContext = CORE_Handle2TaskContextCheck(GetMutexOnwer(lpHeader), FALSE);

    IPC_ASSERT(lpOnwerContext,
        IPC_SPIN_UNLOCK_IRQ(lpHeader, dwFlags); SYSTEM_CALL_OOPS(),
        "BUG: Mutex(%s) invalid onwer task context.", GetObjectName(lpHeader));

    lpCurrentTask = CORE_GetCurrentTask();

    IPC_ASSERT(lpCurrentTask,
        IPC_SPIN_UNLOCK_IRQ(lpHeader, dwFlags); SYSTEM_CALL_OOPS(),
        "BUG: Lock mutex '%s' failed, invalid current task.", GetObjectName(lpHeader));
    
    IPC_ASSERT(lpCurrentTask != lpOnwerContext,
        IPC_SPIN_UNLOCK_IRQ(lpHeader, dwFlags); SYSTEM_CALL_OOPS(),
        "BUG: Daed lock %s as task %s.", GetObjectName(lpHeader), GetContextTaskName(lpOnwerContext));

    /* 倒挂检测 */
    CORE_PriorityUpsideCheck(lpOnwerContext, lpCurrentTask);

    IPC_BlockTask(lpHeader, lpParam);
    
    IPC_SPIN_UNLOCK_IRQ(lpHeader, dwFlags); 

    return STATE_TIME_OUT;
}

STATIC E_STATUS IPC_UnlockMutex(LPKOBJECT_HEADER lpHeader, LPVOID lpParam)
{
    E_STATUS State;
    DWORD dwFlags = 0;
    LPTASK_CONTEXT lpThisContext;

    IPC_INFOR(TRUE, "Unlock mutex '%s' by '%s', value is %d.",
        GetObjectName(lpHeader), CORE_GetCurrentTaskName(), 
        GetMutexValue(lpHeader));

    IPC_SPIN_LOCK_IRQ(lpHeader, dwFlags);
    /* Mutex 的 Onwer 任务*/
    lpThisContext = CORE_Handle2TaskContextCheck(GetMutexOnwer(lpHeader), FALSE);

    /* 如果 Onwer 任务为空，则任务未锁或是BUG，返回未准备好 */
    if (NULL == lpThisContext)
    {
        IPC_SPIN_UNLOCK_IRQ(lpHeader, dwFlags);
        CORE_ERROR(TRUE, "Mutex '%s' haven't onwer task, task '%s' can not unlock it !",
            GetObjectName(lpHeader), CORE_GetCurrentTaskName());
        return STATE_NOT_READY;
    }

    /* 如果当前任务不是 Onwer 则没有解锁权限 */
    if (lpThisContext != CORE_GetCurrentTask())
    {
        IPC_SPIN_UNLOCK_IRQ(lpHeader, dwFlags);
        CORE_ERROR(TRUE, "Mutex '%s' unlock failed, task '%s' can not unlock it !",
            GetObjectName(lpHeader), CORE_GetCurrentTaskName());
        return STATE_INVALID_PERMISSION;
    }

    /* 如果 MUTEX VALUE 大于 0 表示 MUTEX 未锁， 信号数量不需要增加，直接返回成功 */
    /* 如果 MUTEX VALUE + 1 后大于 0 表示，VALUE 自增前值为 0，即没有任务被阻塞 */
    if (GetMutexValue(lpHeader) > 0 || IncMutexValue(lpHeader) > 0)
    {
        SetMutexOnwer(lpHeader, INVALID_HANDLE_VALUE);
        IPC_SPIN_UNLOCK_IRQ(lpHeader, dwFlags);
        return STATE_SUCCESS;
    }
    /* else 否则 MUTEX 中有任务被阻塞 */

    CORE_ResetTaskPriority(lpThisContext);
    State = IPC_WakeupTask(lpHeader, WAIT_SIGNAL_ID_0, TRUE);
    IPC_SPIN_UNLOCK_IRQ(lpHeader, dwFlags);
    
    return State;
}

STATIC E_STATUS IPC_FreeMutex(LPKOBJECT_HEADER lpHeader)
{
    /* If the value is 0, no task has been blocked. */
    /* If the value is less than 0, some task has been blocked */
    /* So if value + 1 < 1, then value less than 0 */
    while(IncMutexValue(lpHeader) < 1)
    {
        IPC_WakeupTask(lpHeader, WAIT_SIGNAL_INVALID, TRUE);
    }

    /* After wakeup all task, the value is 1 or 2. */
    /* If the value is a 1, the mutex was locked, */
    /* so the onwer task will be reset priority. */
    if (1 == GetMutexValue(lpHeader))
    {
        LPTASK_CONTEXT lpOnwerContext;

        lpOnwerContext = CORE_Handle2TaskContextCheck(GetMutexOnwer(lpHeader), FALSE);

        IPC_ASSERT(lpOnwerContext, SYSTEM_CALL_OOPS(),
            "BUG: Mutex(%s) invalid onwer task context.", GetObjectName(lpHeader));

        CORE_ResetTaskPriority(lpOnwerContext);           /* 尝试恢复 ONWER 的初始先级 */
    }

    return STATE_SUCCESS;
}

STATIC E_STATUS IPC_DetachMutex(LPKOBJECT_HEADER lpHeader, LPVOID lpParam)
{
    if (NULL != lpParam)
    {
        LPTASK_CONTEXT lpTaskContext = lpParam;
        DWORD dwFlags = CORE_DisableIRQ();
 
        IncMutexValue(lpHeader);
        DetachIPCNode(lpTaskContext);
        CORE_RestoreIRQ(dwFlags);
 
        return STATE_SUCCESS;
    }

    return STATE_INVALID_PARAMETER;
}

DEFINE_KCLASS(KIPC_CLASS_DESCRIPTOR,
              MutexClass,
              MTX_MAGIC,
              KIPC_CLASS_METHODS,
              IPC_SizeofMutex,
              IPC_MallocMutex,
              IPC_ActiveMutex,
              IPC_TakeMutex,
              IPC_FreeMutex,
              IPC_LockMutex,
              IPC_UnlockMutex,
              IPC_ResetMutex,
              IPC_DetachMutex);

/****************************************************************************
                             Semaphore Class
****************************************************************************/
STATIC SIZE_T IPC_SizeofSemaphore(LPKCLASS_DESCRIPTOR lpClass, LPVOID lpParam)
{
    return sizeof(IPC_SEMAPHORE_OBJECT);
}


STATIC E_STATUS IPC_ActiveSemaphore(LPKOBJECT_HEADER lpHeader, LPVOID lpParam)
{
    LPSEMAPHORE_ATTRIBUTE lpAttribute = lpParam;

    if (NULL == lpParam)
    {
        IPC_ERROR(TRUE, "Invalid parameter to create semaphore '%s'.", GetObjectName(lpHeader));
        return STATE_INVALID_PARAMETER;
    }

    if (lpAttribute->Bits.MaxCount <= 0 || lpAttribute->Bits.Signal > lpAttribute->Bits.MaxCount)
    {
        IPC_ERROR(TRUE, "Invalid value to create semaphore '%s'.", GetObjectName(lpHeader));
        return STATE_INVALID_VALUE;
    }

    LIST_HEAD_INIT(GetIPCWaitQueue(lpHeader));

    SetIPCAttribute(lpHeader, lpAttribute);

    return STATE_SUCCESS;
}

STATIC E_STATUS IPC_WaitSemaphore(LPKOBJECT_HEADER lpHeader, LPVOID lpParam)
{
    DWORD dwFlags;
    E_STATUS State = STATE_SUCCESS;

    IPC_INFOR(TRUE, "Wait semaphore '%s' by '%s', signals is %d.",
        GetObjectName(lpHeader), CORE_GetCurrentTaskName(), 
        GetSemaphoreSignals(lpHeader));

    if (NULL == lpParam)
    {
        CORE_ERROR(TRUE, "Invalid parameter for wait semaphore '%s'.", GetObjectName(lpHeader));
        return STATE_INVALID_PARAMETER;
    }

    IPC_SPIN_LOCK_IRQ(lpHeader, dwFlags);

    /* 如果 SIGNALS - 1 < 0 则说明 SEMAPHORE 没有信号 */
    if (DecSemaphoreSignals(lpHeader) < 0)
    {
        State = IPC_BlockTask(lpHeader, lpParam);
    }
    else
    {
        SetSignalID2Param(lpParam, WAIT_SIGNAL_ID_0);
    }

    IPC_SPIN_UNLOCK_IRQ(lpHeader, dwFlags);
    
    return State;
}

STATIC E_STATUS IPC_PostSemaphore(LPKOBJECT_HEADER lpHeader, LPVOID lpParam)
{
    DWORD dwFlags;
    E_STATUS State = STATE_SUCCESS;
    
    IPC_INFOR(TRUE, "Post semaphore '%s' by '%s', signals is %d.",
        GetObjectName(lpHeader), GetContextTaskName(CORE_GetCurrentTask()), 
        GetSemaphoreSignals(lpHeader));

    IPC_SPIN_LOCK_IRQ(lpHeader, dwFlags);
    /* 如果 SIGNALS + 1 < 0 则说明有任务被 SEMAPHORE 阻塞 */
    if (IncSemaphoreSignals(lpHeader) <= 0)
    {
        State = IPC_WakeupTask(lpHeader, WAIT_SIGNAL_ID_0, FALSE);
    }

    IPC_SPIN_UNLOCK_IRQ(lpHeader, dwFlags);
    
    return State;
}

STATIC E_STATUS IPC_FreeSemaphore(LPKOBJECT_HEADER lpHeader)
{
    /* If the signal is 0, no task has been blocked. */
    /* If the signal is less than 0, some task has been blocked */
    /* So if signal + 1 < 1, then signal less than 0 */
    while (IncSemaphoreSignals(lpHeader) < 1)
    {
        IPC_WakeupTask(lpHeader, WAIT_SIGNAL_INVALID, TRUE);
    }
    
    return STATE_SUCCESS;
}

DEFINE_KCLASS(KIPC_CLASS_DESCRIPTOR,
              SemaphoreClass,
              SEM_MAGIC,
              KIPC_CLASS_METHODS,
              IPC_SizeofSemaphore,
              IPC_MallocSemaphore,
              IPC_ActiveSemaphore,
              IPC_TakeSemaphore,
              IPC_FreeSemaphore,
              IPC_WaitSemaphore,
              IPC_PostSemaphore,
              IPC_ResetSemaphore,
              IPC_DetachSemaphore);

/****************************************************************************
                               Semset Class
****************************************************************************/
STATIC SIZE_T IPC_SizeofSemset(LPKCLASS_DESCRIPTOR lpClass, LPVOID lpParam)
{
    return sizeof(IPC_SEMSET_OBJECT);
}


STATIC E_STATUS IPC_ActiveSemset(LPKOBJECT_HEADER lpHeader, LPVOID lpParam)
{
    LPSEMSET_ATTRIBUTE lpAttribute = lpParam;

    if (NULL == lpParam)
    {
        IPC_ERROR(TRUE, "Invalid parameter to create semset '%s'.", GetObjectName(lpHeader));
        return STATE_INVALID_PARAMETER;
    }

    if (0 != (lpAttribute->Value & (SEMSET_BLOCKS_MASK | SEMSET_SIGNAL_MASK | SEMSET_LOCK_MASK)))
    {
        IPC_ERROR(TRUE, "Invalid attribute to create semset '%s', value 0x%08X, mask 0x%08X.",
            GetObjectName(lpHeader), lpAttribute->Value,
            (SEMSET_BLOCKS_MASK | SEMSET_SIGNAL_MASK | SEMSET_LOCK_MASK));
        return STATE_INVALID_VALUE;
    }
    
    if (0 == lpAttribute->Bits.Lights || lpAttribute->Bits.Lights >= WAIT_SIGNAL_MAX)
    {
        IPC_ERROR(TRUE, "Invalid max lights(%u) to create semset '%s'.",
            lpAttribute->Bits.Lights, GetObjectName(lpHeader));
    }

    LIST_HEAD_INIT(GetIPCWaitQueue(lpHeader));
    
    IPC_INFOR(TRUE, "Create semset value is 0x%08X, full mask is 0x%08X.",
        lpAttribute->Value, GetSemsetFullMask(lpHeader));
    
    SetIPCAttribute(lpHeader, lpAttribute);

    return STATE_SUCCESS;
}

STATIC E_STATUS IPC_WaitSemset(LPKOBJECT_HEADER lpHeader, LPVOID lpParam)
{
    DWORD dwFlags = 0;
    E_STATUS State = STATE_SUCCESS;

    IPC_INFOR(TRUE, "Wait semset '%s' by '%s', mask is 0x%08X.",
        GetObjectName(lpHeader), CORE_GetCurrentTaskName(), 
        GetSemsetMaskValue(lpHeader));

    if (NULL == lpParam)
    {
        CORE_ERROR(TRUE, "Invalid parameter for wait semset '%s'.", GetObjectName(lpHeader));
        return STATE_INVALID_PARAMETER;
    }

    IPC_SPIN_LOCK_IRQ(lpHeader, dwFlags);

    /* Wait for full signal */
    if (GetSemsetWaitFull(lpHeader))
    {
        /* If the single was not full, need blocking current task. */
        if (GetSemsetFullMask(lpHeader) != GetSemsetMaskValue(lpHeader))
        {
            State = IPC_BlockTask(lpHeader, lpParam);
            /* Inc the conter for blocked tasks */
            IncSemsetBlockedTasks(lpHeader);
        }
        else
        {
            /* Reseting the signal bitmap to waiting next cycle */
            SetSemsetMaskValue(lpHeader, SEMSET_SIGNAL_NULL);
        }
    }
    else /* Wait for single signal */
    {
        DWORD SignalID = GetDwordLowestBit(GetSemsetMaskValue(lpHeader));
        
        /* If the signal bitmap value is 0, the lowest bit will be return 32. */
        /* So the signal id can not large than 19, otherwise blocking current task. */
        /* The number of signal id max value is 19, because the signal bitmap has 20 bits. */
        if (SignalID >= WAIT_SIGNAL_MAX)
        {
            State = IPC_BlockTask(lpHeader, lpParam);
            IncSemsetBlockedTasks(lpHeader);
        }
        else
        {
            /* The current task take a signal from bitmap. */
            SetSemsetMaskBit(lpHeader, SignalID, FALSE);
        }
    }
    
    IPC_SPIN_UNLOCK_IRQ(lpHeader, dwFlags);

    return State;
}

STATIC E_STATUS IPC_PostSemset(LPKOBJECT_HEADER lpHeader, LPVOID lpParam)
{
    DWORD dwFlags;
    SHORT SignalID;
    E_STATUS State = STATE_SUCCESS;
    
    IPC_INFOR(TRUE, "Post semset '%s' by '%s', value is 0x%08X, full mask is 0x%08X.",
        GetObjectName(lpHeader), CORE_GetCurrentTaskName(), 
        GetSemsetMaskValue(lpHeader), GetSemsetFullMask(lpHeader));
    
    if (NULL == lpParam)
    {
        CORE_ERROR(TRUE, "Invalid parameter for post semset '%s'.",
            GetObjectName(lpHeader));
        return STATE_INVALID_PARAMETER;
    }
    
    SignalID = * (LPSHORT) lpParam;
    
    if (SignalID < 0 || SignalID >= WAIT_SIGNAL_MAX)
    {
        CORE_ERROR(TRUE, "The signal id(%d) to post '%s' is invalid.",
            SignalID, GetObjectName(lpHeader));
        return STATE_INVALID_VALUE;
    }

    IPC_SPIN_LOCK_IRQ(lpHeader, dwFlags);

    /* Wait for full signal */
    if (GetSemsetWaitFull(lpHeader))
    {
        SetSemsetMaskBit(lpHeader, SignalID, TRUE);

        if (GetSemsetFullMask(lpHeader) == GetSemsetMaskValue(lpHeader))
        {
            /* If the signal was full and some task has been blocked, */
            /* Then wakeup the first block task from the blocked queue. */
            if (GetSemsetBlockedTasks(lpHeader))
            {
                /* Wakeup the first block task. */
                State = IPC_WakeupTask(lpHeader, SignalID, FALSE);
                /* Reseting the signal bitmap to waiting next cycle */
                SetSemsetMaskValue(lpHeader, SEMSET_SIGNAL_NULL);
                /* Dec the conter for blocked tasks */
                DecSemsetBlockedTasks(lpHeader);
            }
        }
    }
    else /* Wait for single signal */
    {
        /* If some task has been blocked, then wakeup the first task. */
        if (GetSemsetBlockedTasks(lpHeader) > 0)
        {
            /* Wakeup the first block task and no need to set signal bitmap. */
            State = IPC_WakeupTask(lpHeader, SignalID, FALSE);
            /* The conter of blocked tasks will be decrement */
            DecSemsetBlockedTasks(lpHeader);
        }
        else
        {
            /* If any task not been blocked, so need to setting the signal bitmap. */
            SetSemsetMaskBit(lpHeader, SignalID, TRUE);
        }
    }

    IPC_SPIN_UNLOCK_IRQ(lpHeader, dwFlags);
    
    return State;
}

STATIC E_STATUS IPC_FreeSemset(LPKOBJECT_HEADER lpHeader)
{
    if (GetSemsetBlockedTasks(lpHeader) > 0)
    do {
        IPC_WakeupTask(lpHeader, WAIT_SIGNAL_INVALID, FALSE);
    }while(DecSemsetBlockedTasks(lpHeader));
    
    return STATE_SUCCESS;
}

DEFINE_KCLASS(KIPC_CLASS_DESCRIPTOR,
              SemsetClass,
              SET_MAGIC,
              KIPC_CLASS_METHODS,
              IPC_SizeofSemset,
              IPC_MallocSemset,
              IPC_ActiveSemset,
              IPC_TakeSemset,
              IPC_FreeSemset,
              IPC_WaitSemset,
              IPC_PostSemset,
              IPC_ResetSemset,
              IPC_DetachSemset);

PUBLIC E_STATUS initCoreInterProcessCommunicationManager(VOID)
{
    CORE_INFOR(TRUE, "KIPC_CLASS_HEADER:    %d,   KIPC_CLASS_BASE:    %d",
        sizeof(KIPC_CLASS_HEADER), sizeof(KIPC_CLASS_BASE));
    CORE_INFOR(TRUE, "IPC_EVENT_OBJECT:     %d,   IPC_MUTEX_OBJECT:   %d",
        sizeof(IPC_EVENT_OBJECT), sizeof(IPC_MUTEX_OBJECT));
    CORE_INFOR(TRUE, "IPC_SEMAPHORE_OBJECT: %d,   IPC_SEMSET_OBJECT:  %d",
        sizeof(IPC_SEMAPHORE_OBJECT), sizeof(IPC_SEMSET_OBJECT));

    /* 注册EVENT对象类 */
    if (STATE_SUCCESS != REGISTER_KCLASS(EventClass))
    {
        IPC_ERROR(TRUE, "Register event class failed !");
        SYSTEM_CALL_OOPS();
    }

    /* 注册MUTEX对象类 */
    if (STATE_SUCCESS != REGISTER_KCLASS(MutexClass))
    {
        IPC_ERROR(TRUE, "Register mutex class failed !");
        SYSTEM_CALL_OOPS();
    }

    /* 注册SEMAPHORE对象类 */
    if (STATE_SUCCESS != REGISTER_KCLASS(SemaphoreClass))
    {
        IPC_ERROR(TRUE, "Register semaphore class failed !");
        SYSTEM_CALL_OOPS();
    }

    /* 注册SEMSET对象类 */
    if (STATE_SUCCESS != REGISTER_KCLASS(SemsetClass))
    {
        IPC_ERROR(TRUE, "Register semset class failed !");
        SYSTEM_CALL_OOPS();
    }    
    //LPC_INSTALL(&LPCService, "inter process communication(IPC) manager starting");

    return STATE_SUCCESS;
}


