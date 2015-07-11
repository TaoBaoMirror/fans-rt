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
    
    LIST_FOR_EACH(lpList, lpHead)
    {
        LPTASK_CONTEXT lpWaitContext = GetContextBySleepNode(lpList);
        
        if (GetContextThisPriority(lpWaitContext) >
            GetContextThisPriority(lpTaskContext))
        {
            break;
        }
    }

    AttachIPCNode(lpList, lpTaskContext);
    SetContextState(lpTaskContext, TASK_STATE_WAITING);
}


EXPORT VOID PriorityUpsideCheck(LPTASK_CONTEXT lpOnwerContext, LPTASK_CONTEXT lpCurrentTask)
{
    TASK_PRIORITY Priority = GetContextThisPriority(lpCurrentTask);

    CORE_ASSERT(lpCurrentTask, SYSTEM_CALL_OOPS(),
        "Current task context not found ?");
    
    /* 如果 onwer 的优先级大于当前任务优先级，则调整 onwer 的优先级，以防止优先级倒挂 */
    if (Priority < GetContextThisPriority(lpOnwerContext))
    {
        CORE_SetThisPriority(lpOnwerContext, Priority);
    }
}

STATIC E_STATUS IPC_DummyOperation(LPKOBJECT_HEADER lpHeader, LPVOID lpParam)
{
    return STATE_SUCCESS;
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

typedef struct tagIPC_EVENT_OBJECT IPC_EVENT_OBJECT;
typedef struct tagIPC_EVENT_OBJECT * PIPC_EVENT_OBJECT;
typedef struct tagIPC_EVENT_OBJECT * LPIPC_EVENT_OBJECT;

struct tagIPC_EVENT_OBJECT{
    IPC_BASE_OBJECT                 Base;
    VOLATILE EVENT_ATTRIBUTE        Marks;
    BYTE                            cbReserved[3];
};

#define     GetEventMarks(lpObject)                                                                 \
            (((LPIPC_EVENT_OBJECT)(lpObject))->Marks.Value)
#define     SetEventMarks(lpObject, data)                                                           \
            do { (((LPIPC_EVENT_OBJECT)(lpObject))->Marks.Value) = (data); } while(0)

#define     GetEventAutomatic(lpObject)                                                             \
            (((LPIPC_EVENT_OBJECT)(lpObject))->Marks.Bits.Automatic)
#define     SetEventAutomatic(lpObject, value)                                                      \
            do { ((LPIPC_EVENT_OBJECT)(lpObject))->Marks.Bits.Automatic = value; } while(0)

#define     GetEventSignal(lpObject)                                                                 \
            (((LPIPC_EVENT_OBJECT)(lpObject))->Marks.Bits.Signal)
#define     SetEventSignal(lpObject, value)                                                          \
            do { ((LPIPC_EVENT_OBJECT)(lpObject))->Marks.Bits.Signal = value; } while(0)

STATIC SIZE_T IPC_SizeofEvent(LPKCLASS_DESCRIPTOR lpClass, LPVOID lpParam)
{
    return sizeof(IPC_EVENT_OBJECT);
}

STATIC E_STATUS IPC_ActiveEvent(LPKOBJECT_HEADER lpHeader, LPVOID lpParam)
{
    LPEVENT_ATTRIBUTE lpAttribute = lpParam;
    
    if (0 != (lpAttribute->Value & (~MARK_EVENT_BITS_MASK)))
    {
        CORE_ERROR(TRUE, "Invalid value to create event '%s'.", GetObjectName(lpHeader));
        return STATE_INVALID_VALUE;
    }
    
    LIST_HEAD_INIT(GetIPCWaitQueue(lpHeader));
    SetEventMarks(lpHeader, lpAttribute->Value);
    SetObjectState(lpHeader, KOBJECT_STATE_ACTIVE);

    return STATE_SUCCESS;
}


STATIC E_STATUS IPC_WaitEvent(LPKOBJECT_HEADER lpHeader, LONG WaitTime)
{
    DWORD dwFlags = CORE_DisableIRQ();

    CORE_INFOR(TRUE, "wait event '%s', state '%d'...",
            GetObjectName(lpHeader), GetEventSignal(lpHeader));

    if (TRUE != GetEventSignal(lpHeader))
    {
        LPTASK_CONTEXT lpCurrentTask = CORE_GetCurrentTaskSafe();
        CORE_TaskSuspend(lpCurrentTask, WaitTime);
        Insert2WaitQueue(GetIPCWaitQueue(lpHeader), lpCurrentTask);
    }
    
    CORE_RestoreIRQ(dwFlags);
    
    return STATE_TIME_OUT;
}

STATIC E_STATUS IPC_PostEvent(LPKOBJECT_HEADER lpHeader, LPVOID lpParam)
{
    DWORD dwFlags = CORE_DisableIRQ();
    
    if (FALSE == GetEventSignal(lpHeader))
    {
        LPLIST_HEAD lpList = NULL;
        LPLIST_HEAD lpNode = NULL;

        if (TRUE != GetEventAutomatic(lpHeader))
        {
            SetEventSignal(lpHeader, TRUE);
        }
        
        LIST_FOR_EACH_SAFE(lpList, lpNode, GetIPCWaitQueue(lpHeader))
        {
            LPTASK_CONTEXT lpTaskContext = GetContextByIPCNode(lpList);

            CORE_TaskWakeup(lpTaskContext, STATE_SUCCESS);
            
            CORE_INFOR(TRUE, "Post '%s' and wakeup task '%s' ...",
                GetObjectName(lpHeader), GetContextTaskName(lpTaskContext));
        }

        CORE_DEBUG(TRUE, "Event %s automatic is '%d', state is '%d' ...",
                GetObjectName(lpHeader), GetEventAutomatic(lpHeader), GetEventSignal(lpHeader));
    }
    
    CORE_RestoreIRQ(dwFlags);
    
    return STATE_SUCCESS;
}

STATIC E_STATUS IPC_ResetEvent(LPKOBJECT_HEADER lpHeader, LPVOID lpParam)
{
    DWORD dwFlags = CORE_DisableIRQ();
    
    SetEventSignal(lpHeader, FALSE);
    
    CORE_RestoreIRQ(dwFlags);
    
    return STATE_SUCCESS;
}


STATIC E_STATUS IPC_FreeEvent(LPKOBJECT_HEADER lpHeader)
{
    DWORD dwFlags = CORE_DisableIRQ();
    
    if (TRUE != GetEventSignal(lpHeader))
    {
        LPLIST_HEAD lpList = NULL;
        LPLIST_HEAD lpNode = NULL;

        LIST_FOR_EACH_SAFE(lpList, lpNode, GetIPCWaitQueue(lpHeader))
        {
            LPTASK_CONTEXT lpTaskContext = GetContextByIPCNode(lpList);

            CORE_TaskWakeup(lpTaskContext, STATE_REMOVED);
            
            CORE_INFOR(TRUE, "Remove %s and wakeup task '%s' ...",
                GetObjectName(lpHeader), GetContextTaskName(lpTaskContext));
        }

        CORE_DEBUG(TRUE, "Event %s automatic is '%d', state is '%d' ...",
                GetObjectName(lpHeader), GetEventAutomatic(lpHeader), GetEventSignal(lpHeader));
    }
    
    CORE_RestoreIRQ(dwFlags);
    
    return STATE_SUCCESS;
}


DEFINE_CLASS(EVT_MAGIC, EventClass,
            IPC_SizeofEvent,
            IPC_MallocEvent,
            IPC_ActiveEvent,
            IPC_TakeEvent,
            IPC_WaitEvent,
            IPC_PostEvent,
            IPC_ResetEvent,
            IPC_DetachEvent,
            IPC_FreeEvent);


/************************************************************************************
                                    Mutex Class
************************************************************************************/

typedef struct tagIPC_MUTEX_OBJECT IPC_MUTEX_OBJECT;
typedef struct tagIPC_MUTEX_OBJECT * PIPC_MUTEX_OBJECT;
typedef struct tagIPC_MUTEX_OBJECT * LPIPC_MUTEX_OBJECT;

struct tagIPC_MUTEX_OBJECT{
    IPC_BASE_OBJECT                 Base;
    VOLATILE union{
        MUTEX_ATTRIBUTE                 Bits;
        HANDLE                          Handle;
        DWORD                           Attribute;
    }un;
};

#define     GetMutexOnwer(lpObject)                                                                 \
            (((LPIPC_MUTEX_OBJECT)(lpObject))->un.Handle &(~HANDLE_OBJECT_SID_MASK))
#define     SetMutexOnwer(lpObject, hOnwer)                                                         \
            do {                                                                                    \
                (((LPIPC_MUTEX_OBJECT)(lpObject))->un.Handle)                                       \
                                    = ((hOnwer)&(~HANDLE_OBJECT_SID_MASK));                         \
            } while(0)

#define     GetMutexValue(lpObject)                                                                 \
            (((LPIPC_MUTEX_OBJECT)(lpObject))->un.Bits.MutexValue)
#define     SetMutexValue(lpObject, data)                                                           \
            do { (((LPIPC_MUTEX_OBJECT)(lpObject))->un.Bits.MutexValue) = (data); } while(0)
#define     IncMutexValue(lpObject)                                                                 \
            (++ ((LPIPC_MUTEX_OBJECT)(lpObject))->un.Bits.MutexValue)
#define     DecMutexValue(lpObject)                                                                 \
            (-- ((LPIPC_MUTEX_OBJECT)(lpObject))->un.Bits.MutexValue)
            
#define     SetMutexAttribute(lpObject, Handle, Value)                                              \
            do {                                                                                    \
                (((LPIPC_MUTEX_OBJECT)(lpObject))->un.Attribute) =                                  \
                    ((Handle & (~HANDLE_OBJECT_SID_MASK)) | (Value & HANDLE_OBJECT_SID_MASK));      \
            } while(0)

STATIC SIZE_T IPC_SizeofMutex(LPKCLASS_DESCRIPTOR lpClass, LPVOID lpParam)
{
    return sizeof(IPC_MUTEX_OBJECT);
}

STATIC E_STATUS IPC_ActiveMutex(LPKOBJECT_HEADER lpHeader, LPVOID lpParam)
{
    LPMUTEX_ATTRIBUTE lpAttribute = lpParam;
    
    if (NULL == lpParam)
    {
        CORE_ERROR(TRUE, "Invalid parameter to create mutex '%s'.", GetObjectName(lpHeader));
        return STATE_INVALID_PARAMETER;
    }
        
    if ((0 != (lpAttribute->MutexValue)) && (1 != (lpAttribute->MutexValue)))
    {
        CORE_ERROR(TRUE, "Invalid value to create mutex '%s'.", GetObjectName(lpHeader));
        return STATE_INVALID_VALUE;
    }
    
    LIST_HEAD_INIT(GetIPCWaitQueue(lpHeader));
    
    if (lpAttribute->MutexValue)
    {
        SetMutexValue(lpHeader, 1);
    }
    else
    {
        LPTASK_CONTEXT lpCurrentTask = CORE_GetCurrentTask();
        CORE_ASSERT(lpCurrentTask, SYSTEM_CALL_OOPS(), "Current task context not found ?");
        SetMutexAttribute(lpHeader, GetContextHandle(lpCurrentTask), 0);
        SetContextLockedMutex(lpCurrentTask, GetObjectHandle(lpHeader));
    }
    
    SetObjectState(lpHeader, KOBJECT_STATE_ACTIVE);

    return STATE_SUCCESS;
}

STATIC E_STATUS IPC_LockMutex(LPKOBJECT_HEADER lpHeader, LONG WaitTime)
{
    DWORD dwFlags;
    LPTASK_CONTEXT lpOnwerContext;
    LPTASK_CONTEXT lpCurrentTask;

    CORE_DEBUG(TRUE, "Lock mutex '%s' by '%s', value is %d.",
        GetObjectName(lpHeader), GetContextTaskName(CORE_GetCurrentTask()), 
        GetMutexValue(lpHeader));

    /* 不需要屏蔽中断，mutex 不能在中断中使用 */
    /* 该函数已经在内核中执行，没有任务切换的可能 */
    if (0 == DecMutexValue(lpHeader))
    {
        lpOnwerContext = CORE_GetCurrentTask();

        CORE_ASSERT(lpOnwerContext, SYSTEM_CALL_OOPS(), "Current task context not found ?");
        SetMutexOnwer(lpHeader, GetContextHandle(lpOnwerContext));
        SetContextLockedMutex(lpOnwerContext, GetObjectHandle(lpHeader));

        return STATE_SUCCESS;
    }

    dwFlags = CORE_DisableIRQ();
    lpOnwerContext = CORE_Handle2TaskContextCheck(GetMutexOnwer(lpHeader), FALSE);

    CORE_ASSERT(lpOnwerContext, CORE_RestoreIRQ(dwFlags); SYSTEM_CALL_OOPS(),
        "BUG: Mutex(%s) invalid onwer task context.", GetObjectName(lpHeader));
    
    CORE_ASSERT(CORE_GetCurrentTask() != lpOnwerContext, CORE_RestoreIRQ(dwFlags); SYSTEM_CALL_OOPS(),
        "BUG: Daed lock %s as task %s.", GetObjectName(lpHeader), GetContextTaskName(lpOnwerContext));
    
    CORE_ASSERT(GetContextLockedMutex(lpOnwerContext) == GetObjectHandle(lpHeader),
        CORE_RestoreIRQ(dwFlags); SYSTEM_CALL_OOPS(), "BUG: Mutex(%s) no lock as '%s', .",
        GetObjectName(lpHeader), GetContextTaskName(lpOnwerContext));

    lpCurrentTask = CORE_GetCurrentTask();
    PriorityUpsideCheck(lpOnwerContext, lpCurrentTask);
    Insert2WaitQueue(GetIPCWaitQueue(lpHeader), lpCurrentTask);
    CORE_TaskSuspend(lpCurrentTask, WaitTime);
    CORE_RestoreIRQ(dwFlags); 

    return STATE_TIME_OUT;
}

STATIC E_STATUS IPC_UnlockMutex(LPKOBJECT_HEADER lpHeader, LPVOID lpParam)
{
    LPTASK_CONTEXT lpThisContext, lpNextContext;

    CORE_DEBUG(TRUE, "Unlock mutex '%s' by '%s', value is %d.",
        GetObjectName(lpHeader), GetContextTaskName(CORE_GetCurrentTask()), 
        GetMutexValue(lpHeader));
    /* 未锁或没有任务等待解锁 */
    if (GetMutexValue(lpHeader) > 0 || IncMutexValue(lpHeader) > 0)
    {
        return STATE_SUCCESS;
    }

    CORE_ASSERT(NULL != GetFirstWaitNode(lpHeader), SYSTEM_CALL_OOPS(),
        "BUG: Bad mutex wait task queue.");
    
    lpNextContext = GetFirstWaitTask(lpHeader);
    lpThisContext = CORE_Handle2TaskContextCheck(GetMutexOnwer(lpHeader), FALSE);
    
    CORE_ASSERT(lpThisContext, SYSTEM_CALL_OOPS(),
        "BUG: Mutex(%s) invalid onwer task context.", GetObjectName(lpHeader));
    
    CORE_ResetTaskPriority(lpThisContext);           /* 尝试恢复 ONWER 的初始先级 */
    SetMutexOnwer(lpHeader, GetContextHandle(lpNextContext));
    SetContextLockedMutex(lpNextContext, GetObjectHandle(lpHeader));
    CORE_TaskWakeup(lpNextContext, STATE_SUCCESS);

    CORE_INFOR(TRUE, "Unlock '%s' and wakeup task '%s' ...",
        GetObjectName(lpHeader), GetContextTaskName(lpNextContext));
    
    return STATE_SUCCESS;
}

STATIC E_STATUS IPC_FreeMutex(LPKOBJECT_HEADER lpHeader)
{
    S16 Value = GetMutexValue(lpHeader);

    if (Value <= 0)
    {
        LPTASK_CONTEXT lpOnwerContext = CORE_Handle2TaskContextCheck(GetMutexOnwer(lpHeader), FALSE);
        
        CORE_ASSERT(lpOnwerContext, SYSTEM_CALL_OOPS(),
            "BUG: Mutex(%s) invalid onwer task context.", GetObjectName(lpHeader));
        
        CORE_ResetTaskPriority(lpOnwerContext);           /* 尝试恢复 ONWER 的初始先级 */

        if (Value < 0)
        {
            LPLIST_HEAD lpList = NULL;
            LPLIST_HEAD lpNode = NULL;

            LIST_FOR_EACH_SAFE(lpList, lpNode, GetIPCWaitQueue(lpHeader))
            {
                LPTASK_CONTEXT lpWaitContext = GetContextByIPCNode(lpList);

                CORE_TaskWakeup(lpWaitContext, STATE_REMOVED);
                
                CORE_INFOR(TRUE, "Remove mutex '%s' and wakeup task '%s' ...",
                    GetObjectName(lpHeader), GetContextTaskName(lpWaitContext));
            }

            CORE_DEBUG(TRUE, "Mutex %s automatic is '%d', state is '%d' ...",
                    GetObjectName(lpHeader), GetMutexRecursive(lpHeader), GetMutexSignal(lpHeader));
        }
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

DEFINE_CLASS(MTX_MAGIC, MutexClass,
            IPC_SizeofMutex,
            IPC_MallocMutex,
            IPC_ActiveMutex,
            IPC_TakeMutex,
            IPC_LockMutex,
            IPC_UnlockMutex,
            IPC_ResetMutex,
            IPC_DetachMutex,
            IPC_FreeMutex);

typedef struct tagIPC_SEMAPHORE_OBJECT IPC_SEMAPHORE_OBJECT;
typedef struct tagIPC_SEMAPHORE_OBJECT * PIPC_SEMAPHORE_OBJECT;
typedef struct tagIPC_SEMAPHORE_OBJECT * LPIPC_SEMAPHORE_OBJECT;

struct tagIPC_SEMAPHORE_OBJECT{
    IPC_BASE_OBJECT                 Base;
    VOLATILE SEMAPHORE_ATTRIBUTE    Attribute;
};

STATIC SIZE_T IPC_SizeofSemaphone(LPKCLASS_DESCRIPTOR lpClass, LPVOID lpParam)
{
    return sizeof(IPC_SEMAPHORE_OBJECT);
}


STATIC E_STATUS IPC_ActiveSemaphore(LPKOBJECT_HEADER lpHeader, LPVOID lpParam)
{
    return STATE_NOT_SUPPORT;
}

STATIC E_STATUS IPC_WaitSemaphore(LPKOBJECT_HEADER lpHeader, LONG WaitTime)
{
    return STATE_NOT_SUPPORT;
}

STATIC E_STATUS IPC_PostSemaphore(LPKOBJECT_HEADER lpHeader, LPVOID lpParam)
{
    return STATE_NOT_SUPPORT;
}

STATIC E_STATUS IPC_FreeSemaphore(LPKOBJECT_HEADER lpHeader)
{
    return STATE_NOT_SUPPORT;
}

DEFINE_CLASS(SEM_MAGIC, SemaphoreClass,
            IPC_SizeofSemaphone,
            IPC_MallocSemaphore,
            IPC_ActiveSemaphore,
            IPC_TakeSemaphore,
            IPC_WaitSemaphore,
            IPC_PostSemaphore,
            IPC_ResetSemaphore,
            IPC_DetachSemaphore,
            IPC_FreeSemaphore);

typedef struct tagIPC_SEMSET_OBJECT IPC_SEMSET_OBJECT;
typedef struct tagIPC_SEMSET_OBJECT * PIPC_SEMSET_OBJECT;
typedef struct tagIPC_SEMSET_OBJECT * LPIPC_SEMSET_OBJECT;

struct tagIPC_SEMSET_OBJECT{
    IPC_BASE_OBJECT                 Base;
    VOLATILE SEMSET_ATTRIBUTE    Attribute;
};

STATIC SIZE_T IPC_SizeofSemset(LPKCLASS_DESCRIPTOR lpClass, LPVOID lpParam)
{
    return sizeof(IPC_SEMSET_OBJECT);
}


STATIC E_STATUS IPC_ActiveSemset(LPKOBJECT_HEADER lpHeader, LPVOID lpParam)
{
    return STATE_NOT_SUPPORT;
}

STATIC E_STATUS IPC_WaitSemset(LPKOBJECT_HEADER lpHeader, LONG WaitTime)
{
    return STATE_NOT_SUPPORT;
}

STATIC E_STATUS IPC_PostSemset(LPKOBJECT_HEADER lpHeader, LPVOID lpParam)
{
    return STATE_NOT_SUPPORT;
}

STATIC E_STATUS IPC_FreeSemset(LPKOBJECT_HEADER lpHeader)
{
    return STATE_NOT_SUPPORT;
}

DEFINE_CLASS(SET_MAGIC, SemsetClass,
            IPC_SizeofSemset,
            IPC_MallocSemset,
            IPC_ActiveSemset,
            IPC_TakeSemset,
            IPC_WaitSemset,
            IPC_PostSemset,
            IPC_ResetSemset,
            IPC_DetachSemset,
            IPC_FreeSemset);

PUBLIC E_STATUS initCoreInterProcessCommunicationManager(VOID)
{
    CORE_INFOR(TRUE, "IPC_BASE_OBJECT:   %d,   IPC_EVENT_OBJECT:     %d",
                sizeof(IPC_BASE_OBJECT), sizeof(IPC_EVENT_OBJECT));
    CORE_INFOR(TRUE, "IPC_MUTEX_OBJECT:  %d,   IPC_SEMAPHORE_OBJECT: %d",
        sizeof(IPC_MUTEX_OBJECT), sizeof(IPC_SEMAPHORE_OBJECT));

    /* 注册EVENT对象类 */
    if (STATE_SUCCESS != CORE_RegisterClass(&EventClass))
    {
        CORE_ERROR(TRUE, "Register event class failed !");
        SYSTEM_CALL_OOPS();
    }

    /* 注册MUTEX对象类 */
    if (STATE_SUCCESS != CORE_RegisterClass(&MutexClass))
    {
        CORE_ERROR(TRUE, "Register mutex class failed !");
        SYSTEM_CALL_OOPS();
    }

    /* 注册SEMAPHORE对象类 */
    if (STATE_SUCCESS != CORE_RegisterClass(&SemaphoreClass))
    {
        CORE_ERROR(TRUE, "Register semaphore class failed !");
        SYSTEM_CALL_OOPS();
    }

    /* 注册SEMSET对象类 */
    if (STATE_SUCCESS != CORE_RegisterClass(&SemsetClass))
    {
        CORE_ERROR(TRUE, "Register semset class failed !");
        SYSTEM_CALL_OOPS();
    }    
    //LPC_INSTALL(&LPCService, "inter process communication(IPC) manager starting");

    return STATE_SUCCESS;
}


