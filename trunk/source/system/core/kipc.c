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
#include "libcal.h"

typedef struct tagIPC_BASE_OBJECT IPC_BASE_OBJECT;
typedef struct tagIPC_BASE_OBJECT * PIPC_BASE_OBJECT;
typedef struct tagIPC_BASE_OBJECT FAR * LPIPC_BASE_OBJECT;

struct tagIPC_BASE_OBJECT{
    KOBJECT_HEADER      Header;
    LIST_HEAD           WaitQueue;
};

#define     GetIPCWaitQueue(lpObject)               (&(((LPIPC_BASE_OBJECT)(lpObject))->WaitQueue))
#define     GetFirstWaitNode(lpObject)              LIST_NEXT_NODE(GetIPCWaitQueue(lpObject))
#define     GetFirstWaitTask(lpObject)              GetContextByIPCNode(GetFirstWaitNode(lpObject))

/************************************************************************************
                                    Event Class
************************************************************************************/

typedef struct tagIPC_EVENT_OBJECT IPC_EVENT_OBJECT;
typedef struct tagIPC_EVENT_OBJECT * PIPC_EVENT_OBJECT;
typedef struct tagIPC_EVENT_OBJECT * LPIPC_EVENT_OBJECT;

struct tagIPC_EVENT_OBJECT{
    IPC_BASE_OBJECT                 Base;
    VOLATILE IPC_EVENT_MARKS        Marks;
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


STATIC E_STATUS IPC_MallocEvent(LPKOBJECT_HEADER lpHeader, LPVOID lpParam)
{
    return STATE_SUCCESS;
}

STATIC E_STATUS IPC_ActiveEvent(LPKOBJECT_HEADER lpHeader, LPVOID lpParam)
{
    LPIPC_EVENT_CREATE_PARAM lpPacket = lpParam;
    
    if (0 != (lpPacket->Marks.Value & (~MARK_EVENT_BITS_MASK)))
    {
        CORE_ERROR(TRUE, "Invalid value to create event '%s'.", GetObjectName(lpHeader));
        return STATE_INVALID_VALUE;
    }
    
    LIST_HEAD_INIT(GetIPCWaitQueue(lpHeader));
    SetEventMarks(lpHeader, lpPacket->Marks.Value);

    return STATE_SUCCESS;
}

STATIC E_STATUS IPC_TakeEvent(LPKOBJECT_HEADER lpHeader, LPVOID lpParam)
{
    return STATE_SUCCESS;
}

STATIC E_STATUS IPC_WaitEvent(LPKOBJECT_HEADER lpHeader, LONG WaitTime)
{
    CORE_INFOR(TRUE, "wait event '%s', state '%d'...",
            GetObjectName(lpHeader), GetEventSignal(lpHeader));
    
    if (TRUE != GetEventSignal(lpHeader))
    {
        CORE_TaskAttach2WaitQueue(GetIPCWaitQueue(lpHeader), WaitTime);
    }
    
    return STATE_TIME_OUT;
}

STATIC E_STATUS IPC_PostEvent(LPKOBJECT_HEADER lpHeader, LPVOID lpParam)
{
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
            LPTASK_CONTEXT lpContext = GetContextByIPCNode(lpList);

            CORE_TaskWakeup(lpContext, STATE_SUCCESS);
            
            CORE_INFOR(TRUE, "Post '%s' and wakeup task '%s' ...",
                GetObjectName(lpHeader), GetContextTaskName(lpContext));
        }

        CORE_DEBUG(TRUE, "Event %s automatic is '%d', state is '%d' ...",
                GetObjectName(lpHeader), GetEventAutomatic(lpHeader), GetEventSignal(lpHeader));
    }
    
    return STATE_SUCCESS;
}

STATIC E_STATUS IPC_ResetEvent(LPKOBJECT_HEADER lpHeader, LPVOID lpParam)
{
    SetEventSignal(lpHeader, FALSE);
    
    return STATE_SUCCESS;
}


STATIC E_STATUS IPC_FreeEvent(LPKOBJECT_HEADER lpHeader)
{
    if (TRUE != GetEventSignal(lpHeader))
    {
        LPLIST_HEAD lpList = NULL;
        LPLIST_HEAD lpNode = NULL;

        LIST_FOR_EACH_SAFE(lpList, lpNode, GetIPCWaitQueue(lpHeader))
        {
            LPTASK_CONTEXT lpContext = GetContextByIPCNode(lpList);

            CORE_TaskWakeup(lpContext, STATE_REMOVED);
            
            CORE_INFOR(TRUE, "Remove %s and wakeup task '%s' ...",
                GetObjectName(lpHeader), GetContextTaskName(lpContext));
        }

        CORE_DEBUG(TRUE, "Event %s automatic is '%d', state is '%d' ...",
                GetObjectName(lpHeader), GetEventAutomatic(lpHeader), GetEventSignal(lpHeader));
    }
    
    return STATE_SUCCESS;
}


DEFINE_CLASS(EVT_MAGIC, EventClass, sizeof(IPC_EVENT_OBJECT),
            IPC_MallocEvent,
            IPC_ActiveEvent,
            IPC_TakeEvent,
            IPC_WaitEvent,
            IPC_PostEvent,
            IPC_ResetEvent,
            IPC_FreeEvent);


/************************************************************************************
                                    Mutex Class
************************************************************************************/

typedef struct tagIPC_MUTEX_OBJECT IPC_MUTEX_OBJECT;
typedef struct tagIPC_MUTEX_OBJECT * PIPC_MUTEX_OBJECT;
typedef struct tagIPC_MUTEX_OBJECT * LPIPC_MUTEX_OBJECT;

#define     MUTEX_VALUE_BITS        OBJECT_SID_BITS

typedef struct tagMUTEX_ATTRIBUTE{
    S32                 MutexValue: MUTEX_VALUE_BITS;              /**< 唯一识别码 */
    HANDLE              hOnwerTask: 32 - MUTEX_VALUE_BITS;
}MUTEX_ATTRIBUTE;

struct tagIPC_MUTEX_OBJECT{
    IPC_BASE_OBJECT                 Base;
    union{
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
            (((LPIPC_MUTEX_OBJECT)(lpObject))->un.Bits.MutexValue ++)
#define     DecMutexValue(lpObject)                                                                 \
            (-- ((LPIPC_MUTEX_OBJECT)(lpObject))->un.Bits.MutexValue)
            
#define     SetMutexAttribute(lpObject, Handle, Value)                                              \
            do {                                                                                    \
                (((LPIPC_MUTEX_OBJECT)(lpObject))->un.Attribute) =                                  \
                    ((Handle & (~HANDLE_OBJECT_SID_MASK)) | (Value & HANDLE_OBJECT_SID_MASK));      \
            } while(0)

STATIC E_STATUS IPC_MallocMutex(LPKOBJECT_HEADER lpHeader, LPVOID lpParam)
{
    return STATE_SUCCESS;
}

STATIC E_STATUS IPC_ActiveMutex(LPKOBJECT_HEADER lpHeader, LPVOID lpParam)
{
    LPIPC_MUTEX_CREATE_PARAM lpPacket = lpParam;
    
    if (NULL == lpParam)
    {
        CORE_ERROR(TRUE, "Invalid parameter to create mutex '%s'.", GetObjectName(lpHeader));
        return STATE_INVALID_PARAMETER;
    }
        
    if (0 != (lpPacket->Marks.Value & (~MARK_MUTEX_BITS_MASK)))
    {
        CORE_ERROR(TRUE, "Invalid value to create mutex '%s'.", GetObjectName(lpHeader));
        return STATE_INVALID_VALUE;
    }
    
    LIST_HEAD_INIT(GetIPCWaitQueue(lpHeader));
    
    if (lpPacket->Marks.Bits.Signal)
    {
        SetMutexValue(lpHeader, 1);
    }
    else
    {
        SetMutexAttribute(lpHeader, GetCurrentTaskHandle(), 0);
        SetContextLockedMutex(CORE_GetCurrentTask(), GetObjectHandle(lpHeader));
    }

    return STATE_SUCCESS;
}

STATIC E_STATUS IPC_TakeMutex(LPKOBJECT_HEADER lpHeader, LPVOID lpParam)
{
    return STATE_SUCCESS;
}

STATIC E_STATUS IPC_LockMutex(LPKOBJECT_HEADER lpHeader, LONG WaitTime)
{
    LPTASK_CONTEXT lpOnwerContext;

    CORE_INFOR(TRUE, "lock mutex '%s', value '%d'...",
        GetObjectName(lpHeader), GetMutexValue(lpHeader));

    /* 不需要屏蔽中断，mutex 不能在中断中使用 */
    /* 该函数已经在内核中执行，没有任务切换的可能 */
    if (0 == DecMutexValue(lpHeader))
    {
        lpOnwerContext = CORE_GetCurrentTask();

        SetMutexOnwer(lpHeader, GetContextHandle(lpOnwerContext));
        SetContextLockedMutex(lpOnwerContext, GetObjectHandle(lpHeader));

        return STATE_SUCCESS;
    }

    lpOnwerContext = CORE_Handle2TaskContextCheck(GetMutexOnwer(lpHeader), FALSE);

    CORE_ASSERT(lpOnwerContext, SYSTEM_CALL_OOPS(),
        "BUG: Mutex(%s) invalid onwer task context.", GetObjectName(lpHeader));
    
    CORE_ASSERT(CORE_GetCurrentTask() != lpOnwerContext, SYSTEM_CALL_OOPS(),
        "BUG: Daed lock %s as task %s.", GetObjectName(lpHeader), GetContextTaskName(lpOnwerContext));
    
    CORE_ASSERT(GetContextLockedMutex(lpOnwerContext) == GetObjectHandle(lpHeader),
        SYSTEM_CALL_OOPS(), "BUG: Mutex(%s) no lock as '%s', .",
        GetObjectName(lpHeader), GetContextTaskName(lpOnwerContext));

    CORE_PriorityUpsideCheck(lpOnwerContext);
    CORE_TaskAttach2WaitQueue(GetIPCWaitQueue(lpHeader), WaitTime);

    return STATE_TIME_OUT;
}

STATIC E_STATUS IPC_UnlockMutex(LPKOBJECT_HEADER lpHeader, LPVOID lpParam)
{
    LPTASK_CONTEXT lpThisContext, lpNextContext;

    /* 未锁或没有任务等待解锁 */
    if (GetMutexValue(lpHeader) > 0 || IncMutexValue(lpHeader) > 0)
    {
        CORE_INFOR(TRUE, "Unlock mutex '%s', value is %d.",
            GetContextTaskName(CORE_GetCurrentTask()),
            GetObjectName(lpHeader), GetMutexValue(lpHeader));
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

STATIC E_STATUS IPC_ResetMutex(LPKOBJECT_HEADER lpHeader, LPVOID lpParam)
{
    return STATE_NOT_SUPPORT;
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


DEFINE_CLASS(MTX_MAGIC, MutexClass, sizeof(IPC_MUTEX_OBJECT),
            IPC_MallocMutex,
            IPC_ActiveMutex,
            IPC_TakeMutex,
            IPC_LockMutex,
            IPC_UnlockMutex,
            IPC_ResetMutex,
            IPC_FreeMutex);

PUBLIC E_STATUS initCoreInterProcessCommunicationManager(VOID)
{
    CORE_INFOR(TRUE, "IPC_BASE_OBJECT: %d,   IPC_EVENT_OBJECT: %d",
        sizeof(IPC_BASE_OBJECT), sizeof(IPC_EVENT_OBJECT));

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
    
    //LPC_INSTALL(&LPCService, "inter process communication(IPC) manager starting");

    return STATE_SUCCESS;
}


