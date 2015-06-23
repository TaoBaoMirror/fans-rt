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
#include <stdio.h>
#include <string.h>
#include <fadefs.h>
#include <faerror.h>
#include <fatypes.h>

#include "klpc.h"
#include "ktask.h"
#include "khash.h"
#include "kpool.h"
#include "klist.h"
#include "kcore.h"
#include "kboard.h"
#include "kdebug.h"
#include "schedule.h"

#define     BOOT_TASK_ID                0
#define     IDLE_TASK_ID                1
#define     MakeCoreStackHandle(id)                                                             \
            MakeCoreHandle(Magic2ClassID(STK_MAGIC), 0, KOBJECT_STATE_ACTIVE, id, 0)

EXPORT E_STATUS caIdleEntry(LPVOID lpParam);

STATIC DWORD g_SystemTaskCount = 0;

#define Handle2TaskContext(hTask)       CORE_Handle2TaskContextCheck(hTask, TRUE)
#define GetGlobalTaskContextCount()     g_SystemTaskCount
#define IncGlobalTaskContextCount()     (g_SystemTaskCount ++)
#define DecGlobalTaskContextCount()     (-- g_SystemTaskCount)

EXPORT LPTASK_CONTEXT CORE_Handle2TaskContextCheck(HANDLE hTask, BOOL Check)
{
    return (LPTASK_CONTEXT) CORE_Handle2HeaderCheck(hTask, Check);
}

/************************************************************************************************
                               
************************************************************************************************/
PUBLIC E_STATUS ScheduleStartup(VOID)
{
    SetScheduleRunState(TRUE);
    SetInterruptNestLayer(0);
    return CORE_EnableKernelStack(CORE_GetCoreStackButtom());
}

STATIC E_STATUS SetTaskError(E_STATUS emCode)
{
    LPTASK_CONTEXT lpCurrentContext = GetCurrentTaskContext();

    if (NULL != lpCurrentContext)
    {
        SetContextTaskError(lpCurrentContext, emCode);
        return STATE_SUCCESS;
    }

    SetSystemGlobalError(emCode);

    return STATE_INVALID_TASK;
}

STATIC E_STATUS GetTaskError(VOID)
{
    LPTASK_CONTEXT lpCurrentContext = GetCurrentTaskContext();
    
    if (NULL != lpCurrentContext)
    {
        return GetContextTaskError(lpCurrentContext);
    }

    return GetSystemGlobalError();
}

STATIC VOID SetContextParam(LPTASK_CONTEXT lpTaskContext, LPKTASK_CREATE_PARAM lpTaskParam)
{
#ifdef SYSTEM_HAVE_TICK64
    SetContextResumeTick(lpTaskContext, TASK_SLICE_INFINITE);
#else
    SetContextResumeRemain(lpTaskContext, TASK_SLICE_INFINITE);
#endif
    SetContextSliceRemain(lpTaskContext, MILLI_SECOND_TO_TICK(lpTaskParam->Param.SliceLength));
    SetContextSliceLength(lpTaskContext, MILLI_SECOND_TO_TICK(lpTaskParam->Param.SliceLength));
    SetContextMiscBits(lpTaskContext, 0, TASK_STATE_DETACH, FALSE, 0, 0);
    SetContextThisPriority(lpTaskContext, lpTaskParam->Param.Priority);
    SetContextInitPriority(lpTaskContext, lpTaskParam->Param.Priority);
    SetContextTaskError(lpTaskContext, STATE_SUCCESS);
    SetContextStackBuffer(lpTaskContext, NULL);

    return;
}

STATIC VOID AttachStack2Context(LPTASK_CONTEXT lpTaskContext,
                    LPKOBJECT_HEADER StackHeader, LPKTASK_CREATE_PARAM lpTaskParam)
{
    KOBJTABLE_ID_T Tid = GetHandleObjectTid(GetObjectHandle(StackHeader));
    KCONTAINER_ID_T Pid = GetHandleObjectPid(GetObjectHandle(StackHeader));
    
    lpTaskParam->lpStackBuffer   = (LPVOID) StackHeader;
    lpTaskParam->lpStackPosition = (LPVOID) (lpTaskParam->lpStackBuffer + lpTaskParam->Param.StackSize);
    lpTaskParam->lpStackPosition = CORE_FillStack(lpTaskParam->lpStackPosition, lpTaskParam->Param.fnTaskMain,
                                     lpTaskParam->Param.lpArgument, GetContextHandle(lpTaskContext));

    CORE_SetArchContextParam(&lpTaskContext->ArchContext, lpTaskParam);
    SetContextStackID(lpTaskContext, Pid, Tid);
    CORE_DEBUG(TRUE, "Task '%s' stack pid is %u, tid is %u.",
        GetContextTaskName(lpTaskContext), Pid, Tid);
    CORE_DEBUG(TRUE, "Task '%s' stack pid is %u, tid is %u.",
        GetContextTaskName(lpTaskContext), GetContextStackPid(lpTaskContext),
        GetContextStackTid(lpTaskContext));
}

STATIC VOID AttachStack2BootTask(LPTASK_CONTEXT lpTaskContext, LPKOBJECT_HEADER StackHeader)
{
    LPBYTE lpStackBuffer = (LPVOID) StackHeader;
    KOBJTABLE_ID_T Tid = GetHandleObjectTid(GetObjectHandle(StackHeader));
    KCONTAINER_ID_T Pid = GetHandleObjectPid(GetObjectHandle(StackHeader));
    
    SetContextStackBuffer(lpTaskContext, lpStackBuffer); 
    SetContextStackID(lpTaskContext, Pid, Tid);
}

STATIC E_STATUS AttachQueue(LPTASK_CONTEXT lpTaskContext, TASK_STATUS TaskState)
{
    switch(TaskState)
    {
    case TASK_STATE_SLEEP:
        CORE_DEBUG(TRUE, "Task '%s' attach to suspend queue.", GetContextTaskName(lpTaskContext));
        Attach2SuspendQueue(lpTaskContext, TICK_INFINITE);
        break;
    case TASK_STATE_READY:
        CORE_DEBUG(TRUE, "Task '%s' attach to ready queue.", GetContextTaskName(lpTaskContext));
        Attach2ReadyQueue(lpTaskContext);
        break;
    default:
        return STATE_NOT_READY;
    }
    
    return STATE_SUCCESS;
}

STATIC E_STATUS AttachContext2System(LPTASK_CONTEXT lpTaskContext, LPKTASK_CREATE_PARAM lpTaskParam)
{
    E_STATUS State;
    TASK_STATUS TaskState = lpTaskParam->Param.AutoStartup
                          ? TASK_STATE_READY
                          : TASK_STATE_SLEEP;
    DWORD dwFlags = CORE_DisableIRQ();

    TaskContextReadyNodeInit(lpTaskContext);
    TaskContextIPCNodeInit(lpTaskContext);
    
    if (STATE_SUCCESS == (State = AttachQueue(lpTaskContext, TaskState)))
    {
        Attach2SystemTaskTable(lpTaskContext);
        SetContextWorkTimes(lpTaskContext, 0);
        if (IncGlobalTaskContextCount())
        {
            SetContextStartTick(lpTaskContext, INVALID_TICK);
        }
        else
        {
            SetContextState(lpTaskContext, TASK_STATE_WORKING);
            SetContextStartTick(lpTaskContext, CORE_GetSystemTick());
        }
    }
    
    CORE_RestoreIRQ(dwFlags);
    
    return State;
}

STATIC E_STATUS DetachContextFromSystem(LPTASK_CONTEXT lpTaskContext)
{
    DWORD dwFlags = CORE_DisableIRQ();
    
    /* 任务 Cancel 标志必须被置位 */
    if (TRUE != GetContextCancel(lpTaskContext))
    {
        CORE_RestoreIRQ(dwFlags);
        return STATE_NOT_READY;
    }
#if 0
    /* 任务堆栈必须已删除 */
    if (NULL != GetContextStackBuffer(lpTaskContext))
    {
        CORE_RestoreIRQ(dwFlags);
        return STATE_SYSTEM_BUSY;
    }
#endif
    switch(GetContextState(lpTaskContext))
    {
    case TASK_STATE_READY:
    case TASK_STATE_WORKING:
    case TASK_STATE_SLEEP:
        DecGlobalTaskContextCount();
        DetechFromSystem(lpTaskContext);
        break;
    default: /*TASK_STATE_DETACH*/
        ;
    }

    CORE_RestoreIRQ(dwFlags);

    return STATE_SUCCESS;
}

STATIC E_STATUS CloseTaskContext(LPTASK_CONTEXT lpTaskContext)
{
    LPKOBJECT_HEADER lpStackHeader = GetContextStackBuffer(lpTaskContext);
    
    if (IsCoreHandle(GetContextHandle(lpTaskContext)))
    {
        return STATE_SYSTEM_BUSY;
    }
    
    SetContextCancel(lpTaskContext, TRUE);
    
    if (NULL != lpStackHeader)
    {
#if (CONFIG_DYNAMIC_STACK_ENABLE == FALSE)
        KOBJTABLE_ID_T Tid = GetContextStackTid(lpTaskContext);
        KCONTAINER_ID_T Pid = GetContextStackPid(lpTaskContext);
        
        CORE_FreeObjectByID(lpStackHeader, STK_MAGIC, Tid, Pid);
#else
#error "Not support dynamic stack !"
#endif
        SetContextStackBuffer(lpTaskContext, NULL);
    }
    
    return CORE_FreeObject(GetContextHeader(lpTaskContext));
}




STATIC SMLT_KEY_T MallocSmltKey(LPTASK_CONTEXT lpTaskContext)
{
    DWORD MarkBits = GetContextSmltMarkBits(lpTaskContext);
    SMLT_KEY_T SmltKey = (SMLT_KEY_T) GetDwordLowestBit(MarkBits);
    
    if (SmltKey >= SMLT_ARRAY_SIZE)
    {
        return TASK_SMLTKEY_INVALID;
    }

    return SmltKey;
}

STATIC E_STATUS FreeSmltKey(LPTASK_CONTEXT lpTaskContext, SMLT_KEY_T SmltKey)
{
    if (SmltKey >= SMLT_ARRAY_SIZE)
    {
        return STATE_OVER_RANGE;
    }
    
    if (TRUE == GetContextSmltKeyIsFree(lpTaskContext, SmltKey))
    {
        return STATE_MEMORY_OVERFLOW;
    }
    
    SetContextSmltKeyToFree(lpTaskContext, SmltKey);
    
    return STATE_SUCCESS;
}

STATIC E_STATUS SetSmltKeyValue(LPTASK_CONTEXT lpTaskContext, SMLT_KEY_T SmltKey, DWORD Value)
{
    if (SmltKey >= SMLT_ARRAY_SIZE)
    {
        return STATE_OVER_RANGE;
    }

    if (TRUE == GetContextSmltKeyIsFree(lpTaskContext, SmltKey))
    {
        return STATE_MEMORY_OVERFLOW;
    }

    SetContextSmltValue(lpTaskContext, SmltKey, Value);

    return STATE_SUCCESS;
}

STATIC E_STATUS GetSmltKeyValue(LPTASK_CONTEXT lpTaskContext, SMLT_KEY_T SmltKey, LPDWORD lpValue)
{
    if (SmltKey >= SMLT_ARRAY_SIZE)
    {
        return STATE_OVER_RANGE;
    }

    if (TRUE == GetContextSmltKeyIsFree(lpTaskContext, SmltKey))
    {
        return STATE_MEMORY_OVERFLOW;
    }

    *lpValue = GetContextSmltValue(lpTaskContext, SmltKey);

    return STATE_SUCCESS;
}

/************************************************************************************************
                               Some none object functions
************************************************************************************************/
STATIC E_STATUS NONE_ActiveObject(LPKOBJECT_HEADER lpHeader, LPVOID lpParam)
{
    return STATE_NOT_SUPPORT;
}

STATIC E_STATUS NONE_TakeObject(LPKOBJECT_HEADER lpHeader, LPVOID lpParam)
{
    return STATE_NOT_SUPPORT;
}

STATIC E_STATUS NONE_WaitObject(LPKOBJECT_HEADER lpHeader, LONG WaitTime)
{
    return STATE_NOT_SUPPORT;
}

STATIC E_STATUS NONE_PostObject(LPKOBJECT_HEADER lpHeader, LPVOID lpParam)
{
    return STATE_NOT_SUPPORT;
}

STATIC E_STATUS NONE_ResetObject(LPKOBJECT_HEADER lpHeader, LPVOID lpParam)
{
    return STATE_NOT_SUPPORT;
}

/************************************************************************************************
                               Some stack object functions
************************************************************************************************/
STATIC E_STATUS STM_MallocStack(LPKOBJECT_HEADER lpHeader, LPVOID lpParam)
{
    LPTASK_CONTEXT lpTaskContext;
    LPKTASK_CREATE_PARAM lpPacket = lpParam;

    if (NULL == lpPacket)
    {
        CORE_ERROR(TRUE, "Invalid parameter for malloc stack");
        return STATE_INVALID_PARAMETER;
    }
    
    if (NULL == (lpTaskContext = Handle2TaskContext(lpPacket->hTask)))
    {
        CORE_ERROR(TRUE, "Invalid task context(handle: 0x%08x) for malloc stack", lpPacket->hTask);
        return STATE_INVALID_TASK;
    }

    CORE_DEBUG(TRUE, "Malloc stack '%s' for task 0x%P handle 0x%08X ...",
            GetObjectName(lpHeader), lpTaskContext, lpPacket->hTask);
    
    AttachStack2Context(lpTaskContext, lpHeader, lpPacket);
    
    return STATE_SUCCESS;
}

STATIC E_STATUS STM_FreeStack(LPKOBJECT_HEADER lpHeader)
{
    return STATE_SUCCESS;
}


DEFINE_CLASS(STK_MAGIC, StackClass, CONFIG_DEFAULT_STACK_SIZE,
            STM_MallocStack,
            NONE_ActiveObject,
            NONE_TakeObject,
            NONE_WaitObject,
            NONE_PostObject,
            NONE_ResetObject,
            STM_FreeStack);

/************************************************************************************************
                               Some task object functions
************************************************************************************************/
STATIC E_STATUS STM_MallocContext(LPKOBJECT_HEADER lpHeader, LPVOID lpParam)
{
    LPKTASK_CREATE_PARAM lpTaskParam = lpParam;
    LPTASK_CONTEXT lpTaskContext = (LPVOID) lpHeader;

    if (NULL == lpTaskParam)
    {
        CORE_ERROR(TRUE, "Invalid parameter for malloc task context");
        return STATE_INVALID_PARAMETER;
    }
        
    if (0 != GetGlobalTaskContextCount() && NULL == lpTaskParam->Param.fnTaskMain)
    {
        CORE_ERROR(TRUE, "Invalid entry to create task '%s' !", GetContextTaskName(lpTaskContext));
        return STATE_INVALID_POINTER;
    }

#if 0
    if (CONFIG_DEFAULT_STACK_SIZE != lpTaskParam->Param.StackSize)
    {
        CORE_ERROR(TRUE, "Invalid stack size %d.", lpTaskParam->Param.StackSize);
        return STATE_INVALID_SIZE;
    }
#endif
    if (lpTaskParam->Param.Priority == TASK_PRIORITY_IDLE)
    {
        if (TRUE == CheckAllIdleTaskCreateFinished())
        {
            CORE_ERROR(TRUE, "Idle task was create finished !");
            return STATE_INVALID_PRIORITY;
        }
        
        if (TASK_SLICE_INFINITE != lpTaskParam->Param.SliceLength)
        {
            CORE_ERROR(TRUE, "Invalid slice(%d) time to create task '%s' !",
                lpTaskParam->Param.SliceLength, GetContextTaskName(lpTaskContext));
            return STATE_INVALID_TIME;
        }
    }
    else if (lpTaskParam->Param.Priority < CONFIG_TASK_PRIORITY_MAX)
    {
        if (CONFIG_TIME_SLICE_NORMAL > lpTaskParam->Param.SliceLength)
        {
            CORE_ERROR(TRUE, "Invalid slice(%d) time to create task '%s' !",
                lpTaskParam->Param.SliceLength, GetContextTaskName(lpTaskContext));
            return STATE_INVALID_TIME;
        }
    }
#if (CONFIG_TASK_PRIORITY_MAX != 256)
    else if (lpTaskParam->Param.Priority >= CONFIG_TASK_PRIORITY_MAX)
    {
        CORE_ERROR(TRUE, "Invalid priority(%d) to create task '%s'!",
            lpTaskParam->Param.Priority, GetContextTaskName(lpTaskContext));
        return STATE_INVALID_PRIORITY;
    }
#endif

    CORE_DEBUG(TRUE, "Malloc 0x%P task '%s' ...", lpHeader, GetObjectName(lpHeader));

    SetContextParam(lpTaskContext, lpTaskParam);

    return STATE_SUCCESS;
}

STATIC E_STATUS STM_ActiveContext(LPKOBJECT_HEADER lpHeader, LPVOID lpParam)
{
    LPKTASK_CREATE_PARAM lpTaskParam = lpParam;
    LPTASK_CONTEXT lpTaskContext = (LPVOID) lpHeader;
    
    CORE_DEBUG(TRUE, "Active task '%s' ...", GetObjectName(lpHeader));

    return AttachContext2System(lpTaskContext, lpTaskParam);
}

STATIC E_STATUS STM_FreeContext(LPKOBJECT_HEADER lpHeader)
{
    LPTASK_CONTEXT lpTaskContext = (LPVOID) lpHeader;

    CORE_DEBUG(TRUE, "Task '%s' stack pid is %u, tid is %u.",
        GetContextTaskName(lpTaskContext), GetContextStackPid(lpTaskContext),
        GetContextStackTid(lpTaskContext));
    
    CORE_DEBUG(TRUE, "Free 0x%P task '%s' ...", lpHeader, GetObjectName(lpHeader));
    
    return DetachContextFromSystem(lpTaskContext);
}


DEFINE_CLASS(TSK_MAGIC, TaskClass, sizeof(TASK_CONTEXT),
            STM_MallocContext,
            STM_ActiveContext,
            NONE_TakeObject,
            NONE_WaitObject,
            NONE_PostObject,
            NONE_ResetObject,
            STM_FreeContext);

STATIC E_STATUS SVC_GetTaskError(LPVOID lpPrivate, LPVOID lpParam)
{
    return GetTaskError();
}

STATIC E_STATUS SVC_SetTaskError(LPVOID lpPrivate, LPVOID lpParam)
{
    LPLPC_REQUEST_PACKET lpPacket = lpParam;
    
    return SetTaskError((E_STATUS) lpPacket->u0.dParam);
}

STATIC E_STATUS SVC_GetCurrentTask(LPVOID lpPrivate, LPVOID lpParam)
{
    LPLPC_REQUEST_PACKET lpPacket = lpParam;
    LPTASK_CONTEXT lpCurrentTask = GetCurrentTaskContext();

    CORE_ASSERT(lpCurrentTask, SYSTEM_CALL_OOPS();, "BUG: Invalid current task context.");
    
    lpPacket->u0.hParam = GetContextHandle(lpCurrentTask);
    
    return STATE_SUCCESS;
}

STATIC E_STATUS SVC_GetTaskState(LPVOID lpPrivate, LPVOID lpParam)
{
    LPLPC_REQUEST_PACKET lpPacket = lpParam;
    LPTASK_CONTEXT lpCurrentTask = GetCurrentTaskContext();

    CORE_ASSERT(lpCurrentTask, SYSTEM_CALL_OOPS();, "BUG: Invalid current task context.");
    
    lpPacket->u0.dParam = GetContextState(lpCurrentTask);
    
    return STATE_SUCCESS;
}

STATIC E_STATUS SVC_GetTaskStartTick(LPVOID lpPrivate, LPVOID lpParam)
{
    LPTASK_CONTEXT lpTaskContext;
    LPLPC_REQUEST_PACKET lpPacket = lpParam;
    
    lpTaskContext = TASK_SELF_HANDLE == lpPacket->u0.hParam
                  ? GetCurrentTaskContext()
                  : Handle2TaskContext(lpPacket->u0.hParam);
    
    if (NULL == lpTaskContext)
    {
        return STATE_INVALID_OBJECT;
    }

    lpPacket->u1.dParam = ((GetContextStartTick(lpTaskContext)>>0) & 0xffffffffL);
    lpPacket->u2.dParam = ((GetContextStartTick(lpTaskContext)>>32) & 0xffffffffL);

    return STATE_SUCCESS;
}

STATIC E_STATUS SVC_TaskSchedule(LPVOID lpPrivate, LPVOID lpParam)
{
    E_STATUS State;
    DWORD dwFlags = CORE_DisableIRQ();
    LPLPC_REQUEST_PACKET lpPacket = lpParam;
    LPTASK_CONTEXT lpCurrentTask = GetCurrentTaskContext();

    CORE_ASSERT(lpCurrentTask, CORE_DisableIRQ(); SYSTEM_CALL_OOPS();,
           "BUG: Invalid current task context.");
    
    State = SuspendTask(lpCurrentTask, lpPacket->u0.lParam);
    
    CORE_RestoreIRQ(dwFlags);

    return State;
}

STATIC E_STATUS SVC_TaskWakeup(LPVOID lpPrivate, LPVOID lpParam)
{
    LPTASK_CONTEXT lpTaskContext;
    LPLPC_REQUEST_PACKET lpPacket = lpParam;

    lpTaskContext = Handle2TaskContext(lpPacket->u0.hParam);

    if (NULL == lpTaskContext)
    {
        return STATE_INVALID_PARAMETER;
    }
    
    SetContextCancel(lpTaskContext, TRUE);
    
    return WakeupTask(lpTaskContext, STATE_SUCCESS);
}

STATIC E_STATUS SVC_TestCancel(LPVOID lpPrivate, LPVOID lpParam)
{
    LPLPC_REQUEST_PACKET lpPacket = lpParam;
    LPTASK_CONTEXT lpCurrentTask = GetCurrentTaskContext();
    
    CORE_ASSERT(lpCurrentTask, SYSTEM_CALL_OOPS();, "BUG: Invalid current task context.");

    lpPacket->u0.dParam = GetContextCancel(lpCurrentTask);
    
    if (TRUE == lpPacket->u0.dParam)
    {
        SetTaskError(STATE_REMOVED);
    }
    
    return STATE_SUCCESS;
}

STATIC E_STATUS SVC_PostCancel(LPVOID lpPrivate, LPVOID lpParam)
{
    LPTASK_CONTEXT lpTaskContext;
    LPLPC_REQUEST_PACKET lpPacket = lpParam;

    lpTaskContext = Handle2TaskContext(lpPacket->u0.hParam);

    if (NULL == lpTaskContext)
    {
        return STATE_INVALID_PARAMETER;
    }
    
    SetContextCancel(lpTaskContext, TRUE);
    
    return STATE_SUCCESS;
}


STATIC E_STATUS SVC_GetTaskPriority(LPVOID lpPrivate, LPVOID lpParam)
{
    LPTASK_CONTEXT lpTaskContext;
    LPLPC_REQUEST_PACKET lpPacket = lpParam;
    
    lpTaskContext = TASK_SELF_HANDLE == lpPacket->u0.hParam
                  ? GetCurrentTaskContext()
                  : Handle2TaskContext(lpPacket->u0.hParam);
    
    if (NULL == lpTaskContext)
    {
        return STATE_INVALID_PARAMETER;
    }
    
    lpPacket->u1.dParam = GetContextThisPriority(lpTaskContext);
    
    return STATE_SUCCESS;
}

STATIC E_STATUS SVC_SetTaskPriority(LPVOID lpPrivate, LPVOID lpParam)
{
    DWORD dwFlags;
    E_STATUS State;
    LPTASK_CONTEXT lpTaskContext;
    LPLPC_REQUEST_PACKET lpPacket = lpParam;
    
    lpTaskContext = TASK_SELF_HANDLE == lpPacket->u0.hParam
                  ? GetCurrentTaskContext()
                  : Handle2TaskContext(lpPacket->u0.hParam);
    
    if (NULL == lpTaskContext)
    {
        return STATE_INVALID_PARAMETER;
    }
    
    dwFlags = CORE_DisableIRQ();
    State = SetTaskPriority(lpTaskContext, (TASK_PRIORITY)lpPacket->u1.dParam);
    CORE_RestoreIRQ(dwFlags);
    
    return State;
}


STATIC E_STATUS SVC_GetSmltKey(LPVOID lpPrivate, LPVOID lpParam)
{
    LPLPC_REQUEST_PACKET lpPacket = lpParam;
    LPTASK_CONTEXT lpCurrentTask = GetCurrentTaskContext();
    
    CORE_ASSERT(lpCurrentTask, SYSTEM_CALL_OOPS();, "BUG: Invalid current task context.");

    lpPacket->u0.dParam = MallocSmltKey(lpCurrentTask);
    
    if (TASK_SMLTKEY_INVALID == (SMLT_KEY_T) lpPacket->u0.dParam)
    {
        SetTaskError(STATE_OUT_OF_MEMORY);
        return STATE_OUT_OF_MEMORY;
    }
    
    return STATE_SUCCESS;
}

STATIC E_STATUS SVC_PutSmltKey(LPVOID lpPrivate, LPVOID lpParam)
{
    LPLPC_REQUEST_PACKET lpPacket = lpParam;
    LPTASK_CONTEXT lpCurrentTask = GetCurrentTaskContext();
    
    CORE_ASSERT(lpCurrentTask, SYSTEM_CALL_OOPS();, "BUG: Invalid current task context.");

    return FreeSmltKey(lpCurrentTask, (SMLT_KEY_T)lpPacket->u0.dParam);
}

STATIC E_STATUS SVC_GetSmltValue(LPVOID lpPrivate, LPVOID lpParam)
{
    LPLPC_REQUEST_PACKET lpPacket = lpParam;
    LPTASK_CONTEXT lpCurrentTask = GetCurrentTaskContext();
    
    CORE_ASSERT(lpCurrentTask, SYSTEM_CALL_OOPS();, "BUG: Invalid current task context.");
    
    return GetSmltKeyValue(lpCurrentTask, (SMLT_KEY_T)lpPacket->u0.dParam, (LPDWORD)&lpPacket->u1.dParam);
}

STATIC E_STATUS SVC_SetSmltValue(LPVOID lpPrivate, LPVOID lpParam)
{
    LPLPC_REQUEST_PACKET lpPacket = lpParam;
    LPTASK_CONTEXT lpCurrentTask = GetCurrentTaskContext();
    
    CORE_ASSERT(lpCurrentTask, SYSTEM_CALL_OOPS();, "BUG: Invalid current task context.");
    
    return SetSmltKeyValue(lpCurrentTask, (SMLT_KEY_T)lpPacket->u0.dParam, lpPacket->u1.dParam);
}

STATIC E_STATUS SVC_CloseTask(LPVOID lpPrivate, LPVOID lpParam)
{
    LPTASK_CONTEXT lpTaskContext;
    LPLPC_REQUEST_PACKET lpPacket = lpParam;
    
    lpTaskContext = TASK_SELF_HANDLE == lpPacket->u0.hParam
                  ? GetCurrentTaskContext()
                  : Handle2TaskContext(lpPacket->u0.hParam);
    
    if (NULL == lpTaskContext)
    {
        return STATE_INVALID_PARAMETER;
    }

    return CloseTaskContext(lpTaskContext);
}

STATIC E_STATUS SVC_GetTaskInfo(LPVOID lpPrivate, LPVOID lpParam)
{
    return STATE_NOT_IMPLEMENTED;
}

STATIC E_STATUS SVC_EnumTask(LPVOID lpPrivate, LPVOID lpParam)
{
    return STATE_NOT_IMPLEMENTED;
}


STATIC E_STATUS SVC_SystemPerformance(LPVOID lpPrivate, LPVOID lpParam)
{
    return STATE_NOT_IMPLEMENTED;
}

STATIC E_STATUS SVC_MallocStack(LPVOID lpPrivate, LPVOID lpParam)
{
    return STATE_NOT_IMPLEMENTED;
}

STATIC E_STATUS SVC_FreeStack(LPVOID lpPrivate, LPVOID lpParam)
{
    return STATE_NOT_IMPLEMENTED;
}


STATIC CONST REQUEST_HANDLER fnHandlers[] = {
    SVC_GetTaskError,               /* 00.LPC_TSS_GET_TASKERROR */
    SVC_SetTaskError,               /* 01.LPC_TSS_SET_TASKERROR */
    SVC_GetCurrentTask,             /* 02.LPC_TSS_GET_CURRENT */
    SVC_GetTaskState,               /* 03.LPC_TSS_GET_TASKSTATE */
    SVC_GetTaskStartTick,           /* 04.LPC_TSS_GET_STARTTICK */
    SVC_GetTaskPriority,            /* 05.LPC_TSS_GET_PRIORITY */
    SVC_SetTaskPriority,            /* 06.LPC_TSS_SET_PRIORITY */
    SVC_GetSmltKey,                 /* 07.LPC_TSS_GET_SMLTKEY */
    SVC_PutSmltKey,                 /* 08.LPC_TSS_PUT_SMLTKEY */
    SVC_GetSmltValue,               /* 09.LPC_TSS_GET_SMLTVALUE */
    SVC_SetSmltValue,               /* 10.LPC_TSS_SET_SMLTVALUE */
    SVC_TaskSchedule,               /* 11.LPC_TSS_SCHEDULE_TIMEOUT */
    SVC_TaskWakeup,                 /* 12.LPC_TSS_WAKE_UP */
    SVC_TestCancel,                 /* 13.LPC_TSS_TEST_CANCEL */
    SVC_PostCancel,                 /* 14.LPC_TSS_POST_CANCEL */
    SVC_CloseTask,                  /* 15.LPC_TSS_CLOSE_TASK */
    SVC_GetTaskInfo,                /* 16.LPC_TSS_GET_TASKINFO */
    SVC_EnumTask,                   /* 17.LPC_TSS_SYS_ENUMTASK */
    SVC_SystemPerformance,          /* 18.LPC_TSS_PERFORMANCE */
    SVC_MallocStack,                /* 19.LPC_TSS_STACK_MALLOC */
    SVC_FreeStack,                  /* 20.LPC_TSS_STACK_FREE */
};

DEFINE_LPC_SERVICE(LPCService, STM_MAGIC, SIZEOF_ARRAY(fnHandlers), NULL, fnHandlers);

EXPORT CODE_TEXT LPTASK_CONTEXT CORE_CreateTaskEx(LPCSTR lpTaskName, LPTASK_CREATE_PARAM lpParam)
{
    E_STATUS State;
    KTASK_CREATE_PARAM TaskParam;
    LPTASK_CONTEXT lpTaskContext = NULL;
#if (CONFIG_DYNAMIC_STACK_ENABLE == FALSE)
    CHAR StackName[OBJECT_NAME_MAX];
#endif
    
    if (NULL == lpTaskName || NULL == lpParam)
    {
        CORE_SetError(STATE_INVALID_PARAMETER);
        return NULL;
    }
    
    TaskParam.Param = *lpParam;

#if (CONFIG_TASK_PRIORITY_MAX != 256)
    if (TaskParam.Param.Priority >= CONFIG_TASK_PRIORITY_MAX)
    {
        CORE_ERROR(TRUE, "Invalid priority(%d) to create task '%s'!",
                        TaskParam.Param.Priority, lpTaskName);
        CORE_SetError(STATE_INVALID_PRIORITY);
        return NULL;
    }
#endif

    if (TASK_PRIORITY_IDLE == TaskParam.Param.Priority)
    {
        TaskParam.Param.SliceLength = TASK_SLICE_INFINITE;
        TaskParam.Param.StackSize = CORE_GetIdleStackCapacity();
    }
    else
    {
        if (TaskParam.Param.SliceLength < CONFIG_TIME_SLICE_NORMAL)
        {
            TaskParam.Param.SliceLength = CONFIG_TIME_SLICE_NORMAL;
        }
    }
    
    TaskParam.Param.StackSize   =   0 == TaskParam.Param.StackSize
                                ?   CONFIG_DEFAULT_STACK_SIZE
                                :   TaskParam.Param.StackSize;
    TaskParam.Param.lpTaskPath  =   NULL == TaskParam.Param.lpTaskPath
                                ?   CONFIG_DEFAULT_PATH
                                :   TaskParam.Param.lpTaskPath;

    lpTaskContext = (LPVOID) CORE_MallocObject(TSK_MAGIC, lpTaskName, &TaskParam);

    if (NULL == lpTaskContext)
    {
        CORE_ERROR(TRUE, "Malloc task '%s' failed!", lpTaskName);
        return NULL;
    }
    
    TaskParam.hTask = GetContextHandle(lpTaskContext);
    SetTaskPermission(lpTaskContext, TASK_PERMISSION_CORE);
    
#if (CONFIG_DYNAMIC_STACK_ENABLE == FALSE)
    memcpy(StackName, lpTaskName, OBJECT_NAME_MAX);
    SetStackObjectName(StackName, '*');
#endif
    if (0 == GetGlobalTaskContextCount())
    {
        LPKOBJECT_HEADER StackHeader = CORE_GetBootStackBuffer();
        SetObjectHandle(StackHeader, MakeCoreStackHandle(BOOT_TASK_ID));
        AttachStack2BootTask(lpTaskContext, StackHeader);
        SetCurrentPriority(TASK_PRIORITY_NORMAL);
        SetCurrentTaskContext(lpTaskContext);               /* boot task is current task */
        SetSwitch2TaskContext(lpTaskContext);
    }
    else if (TASK_PRIORITY_IDLE == TaskParam.Param.Priority)
    {
        LPKOBJECT_HEADER StackHeader = (LPVOID) CORE_GetIdleStackBuffer(GetFreeIdleCPUID());

        if (NULL == StackHeader)
        {
            CORE_ERROR(TRUE, "No memory to create stack for task '%s' !", lpTaskName);
            CloseTaskContext(lpTaskContext);
            CORE_SetError(STATE_INVALID_STACK);
            return NULL;
        }

        SetObjectHandle(StackHeader, MakeCoreStackHandle(IDLE_TASK_ID));
        AttachStack2Context(lpTaskContext, StackHeader, &TaskParam);
    }
#if (CONFIG_DYNAMIC_STACK_ENABLE == FALSE)
    else if (NULL == CORE_MallocObject(STK_MAGIC, StackName, &TaskParam))
    {
        State = CORE_GetError();
        CORE_ERROR(TRUE, "No memory to create stack for task '%s' !", lpTaskName);
        CloseTaskContext(lpTaskContext);
        CORE_SetError(State);
        return NULL;
    }
#else
#error "Can not dynamic stack."
#endif

    if (STATE_SUCCESS != (State = CORE_ActiveObject(GetContextHeader(lpTaskContext), &TaskParam)))
    {
        CORE_ERROR(TRUE, "Active object failed to create task '%s' !", lpTaskName);
        CloseTaskContext(lpTaskContext);
        CORE_SetError(State);
    }
    
    return lpTaskContext;
}
EXPORT_SYMBOL(CORE_CreateTaskEx);


/**
 * Create new task for core.
 * @param The name of new task.
 * @param The entry function of new task.
 * @param The argument of new task.
 * @param The priority of new task.
 * @return The context of new task.
 *
 * date           author          notes
 * 2015-06-21     JiangYong       new function
 */
EXPORT CODE_TEXT LPTASK_CONTEXT CORE_CreatePriorityTask(LPCSTR __IN lpTaskName, FNTASKMAIN fnMain,
                                  LPVOID lpArgument, TASK_PRIORITY Priority)
{
    TASK_CREATE_PARAM TaskParam;

    TaskParam.AutoStartup   =   TRUE;
    TaskParam.lpTaskPath    =   NULL;
    TaskParam.fnTaskMain    =   fnMain;
    TaskParam.lpArgument    =   lpArgument;
    TaskParam.Priority      =   Priority;
    TaskParam.SliceLength   =   CONFIG_TIME_SLICE_NORMAL;
    TaskParam.StackSize     =   CONFIG_DEFAULT_STACK_SIZE;

    return CORE_CreateTaskEx(lpTaskName, &TaskParam);
}
EXPORT_SYMBOL(CORE_CreatePriorityTask);

/************************************************************************************************
                               Some stack object functions
************************************************************************************************/
PUBLIC E_STATUS initCoreSystemTaskScheduleManager(VOID)
{
    DWORD CpuID = 0;
    E_STATUS State;

    CORE_INFOR(TRUE, "ArchContext offset of TaskContext is %d.", OFFSET_OF(TASK_CONTEXT, ArchContext));
    CORE_INFOR(TRUE, "Max priority is %d, Show the size of tss type for debug:", CONFIG_TASK_PRIORITY_MAX);
    CORE_INFOR(TRUE, "POOL_MAP_T: %d  MANA_MAP_T: %d  TASK_CONTEXT: %d   KOBJECT_HEADER: %d",
        sizeof(POOL_MAP_T), sizeof(MANA_MAP_T), sizeof(TASK_CONTEXT), sizeof(KOBJECT_HEADER));
    CORE_INFOR(TRUE, "TASK_STATUS: %d  TIME_SLICE_T: %d", sizeof(TASK_STATUS), sizeof(TIME_SLICE_T));
    
    SystemSchedulerInitialize();

    /* 注册Task context对象类 */
    State = CORE_RegisterClass(&TaskClass);

    CORE_ASSERT(STATE_SUCCESS == State, SYSTEM_CALL_OOPS(),
        "Register task context class failed, result = %d !", State);
#if (CONFIG_DYNAMIC_STACK_ENABLE == FALSE)
    /* 注册Task stack对象类 */
    State = CORE_RegisterClass(&StackClass);

    CORE_ASSERT(STATE_SUCCESS == State, SYSTEM_CALL_OOPS(),
        "Register stack context class failed, result = %d !", State);
#endif

    LPC_INSTALL(&LPCService, "Task Schedule(TSK) service starting");

    if (NULL == CORE_CreateTask(BOOT_TASK_NAME, NULL, NULL))
    {
        CORE_ERROR(TRUE, "Create boot task failed, result = %d !", CORE_GetError());
        SYSTEM_CALL_OOPS();
    }

    for (CpuID = 0; CpuID < CORE_GetCPUNumbers(); CpuID ++)
    {
        CHAR IdleTaskName[OBJECT_NAME_MAX];

        memset(IdleTaskName, 0, sizeof(IdleTaskName));
        snprintf(IdleTaskName, OBJECT_NAME_MAX - 1, "%s%02d", IDLE_TASK_NAME, CpuID);
        
        if (NULL == CORE_CreatePriorityTask(IdleTaskName, caIdleEntry, NULL, TASK_PRIORITY_IDLE))
        {
            CORE_ERROR(TRUE, "Create idle task(%s) failed, result = %d !", IdleTaskName, CORE_GetError());
        }
    }
        
    ScheduleStartup();

    return STATE_SUCCESS;
}


/**
 * 进入IRQ中断临界状态
 * @return DWORD 返回中断结束后的嵌套层数
 * \par
 *     进入IRQ中断临界状态，在支持内核堆栈和任务切换中断的硬件平台上对中断嵌套层数
 * 自减 1 并返回进入中断前的嵌套层数。
 */
EXPORT DWORD CORE_EnterIRQ(VOID)
{
    return (DWORD)(EnterInterruptCritiacl());
}

/**
 * 退出IRQ中断临界状态
 * @return DWORD 返回中断结束后的嵌套层数
 * \par
 *     退出IRQ中断临界状态，在支持内核堆栈和任务切换中断的硬件平台上，如果检测到
 * 调度标志(线程被挂起或被唤醒之后)，需要启用调度器，然后对中断嵌套层数自减 1。
 */
EXPORT DWORD CORE_LeaveIRQ(VOID)
{
    if (CheckNeedSchedule())
    {
        ScheduleBegin();
    }

    return (DWORD)(LeaveInterruptCritiacl());
}

EXPORT VOID CORE_SetTaskStackPosition(LPVOID StackPosition)
{
    LPTASK_CONTEXT lpCurrentTask = GetCurrentTaskContext();

    SetContextStackPosition(lpCurrentTask, StackPosition);
}

EXPORT LPVOID CORE_GetTaskStackPosition(VOID)
{
    LPTASK_CONTEXT lpCurrentTask = GetCurrentTaskContext();

    return GetContextStackPosition(lpCurrentTask);
}

EXPORT LPVOID CORE_GetCoreStackPosition(LPVOID StackPosition)
{
    return CORE_GetTaskCoreStackPosition(GetCurrentTaskContext(), StackPosition);
}


#if (CONFIG_ARCH_SUPPORT_SCHEDULE == TRUE)
/**
 * 切换任务(硬件任务切换中断，硬件内核堆栈)
 * @param StackPosition 当前任务用户栈指针
 * @return LPVOID 新任务用户栈指针
 * \par
 *     保存当前任务用户栈指针并返回新任务用户栈指针，调整当前优先级为新任务
 * 优先级更，改当前任务上下文指针为新任务指针，并设置新任务为WORKING状态。
 */
EXPORT LPVOID CORE_SwitchTask(LPVOID StackPosition)
{
    LPTASK_CONTEXT lpCurrentTask = GetCurrentTaskContext();
    LPTASK_CONTEXT lpSwitch2Task = GetSwitch2TaskContext();
    
    SetContextStackPosition(lpCurrentTask, StackPosition);
    SetCurrentPriority(GetContextThisPriority(lpSwitch2Task));
    SetContextState(lpSwitch2Task, TASK_STATE_WORKING);
    SetCurrentTaskContext(lpSwitch2Task);
    StackPosition = GetContextStackPosition(lpSwitch2Task);
    
    if (INVALID_TICK == GetContextStartTick(lpSwitch2Task))
    {
        SetContextStartTick(lpSwitch2Task, CORE_GetSystemTick());
    }
    
    return StackPosition;
}
#endif


EXPORT VOID CORE_TaskScheduling(VOID)
{
    if (GetScheduleRunState())
    {
        ScheduleSliceHandler();
        ScheduleResumeHandler();
        AddSystemScheduleCount();
    }
}
EXPORT_SYMBOL(CORE_TaskScheduling)

EXPORT E_STATUS CORE_SetError(E_STATUS emCode)
{
    DWORD dwFlags = CORE_DisableIRQ();

    E_STATUS State = SetTaskError(emCode);

    CORE_RestoreIRQ(dwFlags);
    
    return State;
}
EXPORT_SYMBOL(CORE_SetError);

EXPORT E_STATUS CORE_GetError(VOID)
{
    DWORD dwFlags = CORE_DisableIRQ();
    
    E_STATUS State = GetTaskError();
    
    CORE_RestoreIRQ(dwFlags);
    
    return State;
}
EXPORT_SYMBOL(CORE_GetError);


EXPORT LPTASK_CONTEXT CORE_GetCurrentTask(VOID)
{
    DWORD dwFlags = CORE_DisableIRQ();
    LPTASK_CONTEXT lpCurrentTask = GetCurrentTaskContext();
    CORE_ASSERT(lpCurrentTask,
        CORE_RestoreIRQ(dwFlags); SYSTEM_CALL_OOPS(),
        "Current task context not found ?");
    CORE_RestoreIRQ(dwFlags);
    
    return lpCurrentTask;
}
EXPORT_SYMBOL(CORE_GetCurrentTask);

EXPORT VOID CORE_SetCurrentTaskLPCPacket(LPVOID lpPacket)
{
    DWORD dwFlags = CORE_DisableIRQ();
    
    if (!GetScheduleRunState());
    else
    {
        CORE_ASSERT(GetCurrentTaskContext(),
            CORE_RestoreIRQ(dwFlags); SYSTEM_CALL_OOPS(),
            "Current task context not found ?");
        SetContextLPCPacket(GetCurrentTaskContext(), lpPacket);
    }
    
    CORE_RestoreIRQ(dwFlags);
}

EXPORT E_STATUS CORE_TaskWakeup(LPTASK_CONTEXT lpTaskContext, E_STATUS Result)
{
    DWORD dwFlags = CORE_DisableIRQ();
    E_STATUS State = WakeupTask(lpTaskContext, Result);
    CORE_RestoreIRQ(dwFlags);

    return State;
}

EXPORT E_STATUS CORE_TaskSuspend(LONG Timeout)
{
    DWORD dwFlags = CORE_DisableIRQ();
    LPTASK_CONTEXT lpCurrentTask = GetCurrentTaskContext();

    CORE_ASSERT(lpCurrentTask,
        CORE_RestoreIRQ(dwFlags); SYSTEM_CALL_OOPS(),
        "Current task context not found ?");
    
    SuspendTask(GetCurrentTaskContext(), Timeout);

    CORE_RestoreIRQ(dwFlags);

    return STATE_SUCCESS;
}

EXPORT E_STATUS CORE_TaskAttach2WaitQueue(LPLIST_HEAD lpWaitQueue, LONG Timeout)
{
    DWORD dwFlags = CORE_DisableIRQ();
    LPTASK_CONTEXT lpCurrentTask = GetCurrentTaskContext();

    CORE_ASSERT(lpCurrentTask,
        CORE_RestoreIRQ(dwFlags); SYSTEM_CALL_OOPS(),
        "Current task context not found ?");
    
    SuspendTask(lpCurrentTask, Timeout);
    
    Insert2WaitQueue(lpWaitQueue, lpCurrentTask);
    
    CORE_RestoreIRQ(dwFlags);

    return STATE_SUCCESS;
}


EXPORT E_STATUS CORE_PriorityUpsideCheck(LPTASK_CONTEXT lpOnwerContext)
{
    DWORD dwFlags = CORE_DisableIRQ();
    LPTASK_CONTEXT lpCurrentTask = GetCurrentTaskContext();
    TASK_PRIORITY Priority = GetContextThisPriority(lpCurrentTask);

    CORE_ASSERT(lpCurrentTask,
        CORE_RestoreIRQ(dwFlags); SYSTEM_CALL_OOPS(),
        "Current task context not found ?");
    
    /* 如果 onwer 的优先级大于当前任务优先级，则调整 onwer 的优先级，以防止优先级倒挂 */
    if (Priority < GetContextThisPriority(lpOnwerContext))
    {
        SetTaskThisPriority(lpOnwerContext, Priority);
    }
    
    CORE_RestoreIRQ(dwFlags);
    
    return STATE_SUCCESS;
}

EXPORT E_STATUS CORE_ResetTaskPriority(LPTASK_CONTEXT lpTaskContext)
{
    DWORD dwFlags = CORE_DisableIRQ();
    TASK_PRIORITY Priority = GetContextInitPriority(lpTaskContext);
    
    SetTaskThisPriority(lpTaskContext, Priority);
    
    CORE_RestoreIRQ(dwFlags);

    return STATE_SUCCESS;
}

EXPORT DWORD CORE_GetInterruptLayer(VOID)
{
    DWORD dwFlags = CORE_DisableIRQ();
    DWORD Layer = GetInterruptNestLayer();
    CORE_RestoreIRQ(dwFlags);

    return Layer;
}
