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
#include "kipc.h"
#include "ktask.h"
#include "khash.h"
#include "kpool.h"
#include "klist.h"
#include "kcore.h"
#include "kboard.h"
#include "kstack.h"
#include "kdebug.h"
#include "schedule.h"

#define     BOOT_TASK_ID                0
#define     IDLE_TASK_ID                1

EXPORT DWORD CORE_GetGlobalTaskCount(VOID)
{
    return g_SystemTaskCount;
}

/************************************************************************************************
                               
************************************************************************************************/
PUBLIC E_STATUS ScheduleStartup(VOID)
{
    SetScheduleRunState(TRUE);
    SetInterruptNestLayer(0);
    return CORE_Switch2UserMode();
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

STATIC VOID SetContextParam(LPTASK_CONTEXT lpTaskContext, LPTASK_CREATE_PARAM lpTaskParam)
{
#ifdef SYSTEM_HAVE_TICK64
    SetContextResumeTick(lpTaskContext, TASK_SLICE_INFINITE);
#else
    SetContextResumeRemain(lpTaskContext, TASK_SLICE_INFINITE);
#endif
    SetContextSliceRemain(lpTaskContext, MILLI_SECOND_TO_TICK(lpTaskParam->SliceLength));
    SetContextSliceLength(lpTaskContext, MILLI_SECOND_TO_TICK(lpTaskParam->SliceLength));
    SetContextMiscBits(lpTaskContext, 0, TASK_STATE_CREATE, FALSE);
    SetContextThisPriority(lpTaskContext, lpTaskParam->Priority);
    SetContextInitPriority(lpTaskContext, lpTaskParam->Priority);
    SetContextTaskError(lpTaskContext, STATE_SUCCESS);
    SetContextLsotObject(lpTaskContext, NULL);
    //SetContextPermission(lpTaskContext, TASK_PERMISSION_USER);  /* no need see @SetContextMiscBits*/
    CORE_SetArchContextParam(GetContextArchParameter(lpTaskContext), lpTaskParam);

    return;
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

STATIC E_STATUS AttachContext2System(LPTASK_CONTEXT lpTaskContext, LPTASK_CREATE_PARAM lpTaskParam)
{
    E_STATUS State;
    TASK_STATUS TaskState = lpTaskParam->AutoStartup
                          ? TASK_STATE_READY
                          : TASK_STATE_SLEEP;
    DWORD dwFlags = CORE_DisableIRQ();

    TaskContextReadyNodeInit(lpTaskContext);
    TaskContextIPCNodeInit(lpTaskContext);
    
    if (STATE_SUCCESS == (State = AttachQueue(lpTaskContext, TaskState)))
    {
        Attach2SystemTaskTable(lpTaskContext);
        SetContextWorkTimes(lpTaskContext, 0);
        SetContextCancel(lpTaskContext, FALSE);
        if (IncGlobalTaskContextCount())
        {
            SetContextStartTick(lpTaskContext, INVALID_TICK);
        }
        else
        {
            SetCurrentTaskContext(lpTaskContext);
            SetCurrentPriority(TASK_PRIORITY_NORMAL);
            SetContextState(lpTaskContext, TASK_STATE_WORKING);
            SetContextStartTick(lpTaskContext, CORE_GetSystemTick());
        }
    }
    
    CORE_RestoreIRQ(dwFlags);
    
    return State;
}

STATIC E_STATUS DetachContextFromSystem(LPTASK_CONTEXT lpTaskContext)
{
    E_STATUS Result = STATE_SUCCESS;
    DWORD dwFlags = CORE_DisableIRQ();
    
    /* 任务 Cancel 标志必须被置位，参考 Context2DetachState */
    if (TRUE != GetContextCancel(lpTaskContext))
    {
        CORE_RestoreIRQ(dwFlags);
        return STATE_NOT_READY;
    }

    switch(GetContextState(lpTaskContext))
    {
    case TASK_STATE_DEATH:
        Result = STATE_REMOVED;
        break;
    case TASK_STATE_READY:
    case TASK_STATE_WORKING:
    case TASK_STATE_SLEEP:
    case TASK_STATE_WAITING:
        Result = STATE_SYSTEM_BUSY;
        break;
    case TASK_STATE_DETACH:
        DecGlobalTaskContextCount();
        DetachSystem(lpTaskContext);
        SetContextState(lpTaskContext, TASK_STATE_DEATH);
        break;
    case TASK_STATE_CREATE:
        DecGlobalTaskContextCount();
        SetContextState(lpTaskContext, TASK_STATE_DEATH);
        break;
    default: /*unknow*/
        Result = STATE_INVALID_STATE;
        break;
    }

    CORE_RestoreIRQ(dwFlags);

    return Result;
}

EXPORT E_STATUS DetachFromIPCQueue(LPTASK_CONTEXT lpTaskContext)
{
    LPKOBJECT_HEADER lpHeader = (LPVOID)GetHeaderByWaitQueue(GetIPCNode(lpTaskContext));
    return CORE_DetachIPCQueue(lpHeader, lpTaskContext);
}


STATIC E_STATUS AttachContext2Death(LPTASK_CONTEXT lpTaskContext)
{
    E_STATUS Result = STATE_SUCCESS;
    DWORD dwFlags = CORE_DisableIRQ();

    switch(GetContextState(lpTaskContext))
    {
    case TASK_STATE_CREATE:
    case TASK_STATE_DETACH:
    case TASK_STATE_DEATH:
        Result = STATE_REMOVED;
        break;
    case TASK_STATE_READY:
    case TASK_STATE_WORKING:
    case TASK_STATE_SLEEP:
        break;
    case TASK_STATE_WAITING:
        Result = DetachFromIPCQueue(lpTaskContext);
        break;
    default: /*unknow*/
        Result = STATE_INVALID_STATE;
        break;
    }
    
    if (STATE_SUCCESS == Result)
    {
        Context2DetachState(lpTaskContext);
    }

    CORE_RestoreIRQ(dwFlags);

    return Result;
}

/************************************************************************************************
                               Some task object functions
************************************************************************************************/
STATIC SIZE_T OBJ_SizeofContext(LPKCLASS_DESCRIPTOR lpClass, LPVOID lpParam)
{
    return sizeof(TASK_CONTEXT);
}


STATIC E_STATUS OBJ_MallocContext(LPKOBJECT_HEADER lpHeader, LPVOID lpParam)
{
    LPTASK_CREATE_PARAM lpTaskParam = lpParam;
    LPTASK_CONTEXT lpTaskContext = (LPVOID) lpHeader;

    if (NULL == lpTaskParam)
    {
        CORE_ERROR(TRUE, "Invalid parameter for malloc task context");
        return STATE_INVALID_PARAMETER;
    }
        
    if (0 != GetGlobalTaskContextCount() && NULL == lpTaskParam->fnTaskMain)
    {
        CORE_ERROR(TRUE, "Invalid entry to create task '%s' !", GetContextTaskName(lpTaskContext));
        return STATE_INVALID_POINTER;
    }

    if (lpTaskParam->Priority == TASK_PRIORITY_IDLE)
    {
        if (TRUE == CheckAllIdleTaskCreateFinished())
        {
            CORE_ERROR(TRUE, "Idle task was create finished !");
            return STATE_INVALID_PRIORITY;
        }
        
        if (TASK_SLICE_INFINITE != lpTaskParam->SliceLength)
        {
            CORE_ERROR(TRUE, "Invalid slice(%d) time to create task '%s' !",
                lpTaskParam->SliceLength, GetContextTaskName(lpTaskContext));
            return STATE_INVALID_TIME;
        }
    }
    else if (lpTaskParam->Priority < CONFIG_TASK_PRIORITY_MAX)
    {
        if (CONFIG_TIME_SLICE_NORMAL > lpTaskParam->SliceLength)
        {
            CORE_ERROR(TRUE, "Invalid slice(%d) time to create task '%s' !",
                lpTaskParam->SliceLength, GetContextTaskName(lpTaskContext));
            return STATE_INVALID_TIME;
        }
    }
#if (CONFIG_TASK_PRIORITY_MAX != 256)
    else if (lpTaskParam->Priority >= CONFIG_TASK_PRIORITY_MAX)
    {
        CORE_ERROR(TRUE, "Invalid priority(%d) to create task '%s'!",
            lpTaskParam->Priority, GetContextTaskName(lpTaskContext));
        return STATE_INVALID_PRIORITY;
    }
#endif

    CORE_DEBUG(TRUE, "Malloc 0x%P task '%s' ...", lpHeader, GetObjectName(lpHeader));

    SetContextParam(lpTaskContext, lpTaskParam);

    return STATE_SUCCESS;
}

STATIC E_STATUS OBJ_ActiveContext(LPKOBJECT_HEADER lpHeader, LPVOID lpParam)
{
    LPTASK_CREATE_PARAM lpTaskParam = lpParam;
    LPTASK_CONTEXT lpTaskContext = (LPVOID) lpHeader;
    
    CORE_DEBUG(TRUE, "Active task '%s' ...", GetObjectName(lpHeader));
    
    return AttachContext2System(lpTaskContext, lpTaskParam);
}

STATIC E_STATUS OBJ_TakeContext(LPKOBJECT_HEADER lpHeader, LPVOID lpParam)
{
    return STATE_SUCCESS;
}

STATIC E_STATUS OBJ_FreeContext(LPKOBJECT_HEADER lpHeader)
{
    LPTASK_CONTEXT lpTaskContext = (LPVOID) lpHeader;

    CORE_INFOR(TRUE, "Free 0x%P task '%s' ...", lpHeader, GetObjectName(lpHeader));
    
    return DetachContextFromSystem(lpTaskContext);
}


typedef struct tagKTASK_CLASS_DESCRIPTOR KTASK_CLASS_DESCRIPTOR;
typedef struct tagKTASK_CLASS_DESCRIPTOR * PKTASK_CLASS_DESCRIPTOR;
typedef struct tagKTASK_CLASS_DESCRIPTOR FAR * LPKTASK_CLASS_DESCRIPTOR;

#define     KTASK_CLASS_METHODS        1

struct tagKTASK_CLASS_DESCRIPTOR{
    KCLASS_HEADER               Header;
    FNCLASSMETHOD               fnClassMethods[KTASK_CLASS_METHODS];
};

DEFINE_KCLASS(KTASK_CLASS_DESCRIPTOR,
              TaskClass,
              TSK_MAGIC,
              KTASK_CLASS_METHODS,
              OBJ_SizeofContext,
              OBJ_MallocContext,
              OBJ_ActiveContext,
              OBJ_TakeContext,
              OBJ_FreeContext,
              NULL);

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

STATIC E_STATUS SVC_GetLsotHandle(LPVOID lpPrivate, LPVOID lpParam)
{
    LPKOBJECT_HEADER lpHeader;
    LPTASK_CONTEXT lpTaskContext;

    lpTaskContext = TASK_SELF_HANDLE == REQhParam(lpParam, u0)
                  ? GetCurrentTaskContext()
                  : Handle2TaskContext(REQhParam(lpParam, u0));

    if (NULL == lpTaskContext)
    {
        CORE_ERROR(TRUE, "BUG: Invalid current task context.");
        return STATE_INVALID_OBJECT;
    }
    
    lpHeader = GetContextLsotObject(lpTaskContext);
    
    if (NULL == lpHeader)
    {
        return STATE_NOT_FOUND;
    }

    SetREQhParam(lpParam, u1, GetObjectHandle(lpHeader));

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
    
    return WakeupTask(lpTaskContext);
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

    return AttachContext2Death(lpTaskContext);
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

STATIC CONST REQUEST_HANDLER fnHandlers[] = {
    SVC_GetTaskError,               /* 00.LPC_TSS_GET_TASKERROR */
    SVC_SetTaskError,               /* 01.LPC_TSS_SET_TASKERROR */
    SVC_GetCurrentTask,             /* 02.LPC_TSS_GET_CURRENT */
    SVC_GetTaskState,               /* 03.LPC_TSS_GET_TASKSTATE */
    SVC_GetTaskStartTick,           /* 04.LPC_TSS_GET_STARTTICK */
    SVC_GetTaskPriority,            /* 05.LPC_TSS_GET_PRIORITY */
    SVC_SetTaskPriority,            /* 06.LPC_TSS_SET_PRIORITY */
    SVC_GetLsotHandle,              /* 07.LPC_TSS_GET_LSOTHANDLE */
    SVC_TaskSchedule,               /* 08.LPC_TSS_SCHEDULE_TIMEOUT */
    SVC_TaskWakeup,                 /* 09.LPC_TSS_WAKE_UP */
    SVC_TestCancel,                 /* 10.LPC_TSS_TEST_CANCEL */
    SVC_PostCancel,                 /* 11.LPC_TSS_POST_CANCEL */
    SVC_CloseTask,                  /* 12.LPC_TSS_CLOSE_TASK */
    SVC_GetTaskInfo,                /* 13.LPC_TSS_GET_TASKINFO */
    SVC_EnumTask,                   /* 14.LPC_TSS_SYS_ENUMTASK */
    SVC_SystemPerformance,          /* 15.LPC_TSS_PERFORMANCE */
};

DEFINE_LPC_SERVICE(LPCService, STM_MAGIC, SIZEOF_ARRAY(fnHandlers), NULL, fnHandlers);

EXPORT RO_CODE LPTASK_CONTEXT CORE_CreateTaskEx(LPCSTR lpTaskName, LPTASK_CREATE_PARAM lpParam)
{
    E_STATUS State;
    TASK_CREATE_PARAM TaskParam;
    LPTASK_CONTEXT lpTaskContext = NULL;

    if (NULL == lpTaskName || NULL == lpParam)
    {
        CORE_SetError(STATE_INVALID_PARAMETER);
        return NULL;
    }
    
    TaskParam = *lpParam;

#if (CONFIG_TASK_PRIORITY_MAX != 256)
    if (TaskParam.Priority >= CONFIG_TASK_PRIORITY_MAX)
    {
        CORE_ERROR(TRUE, "Invalid priority(%d) to create task '%s'!",
                        TaskParam.Priority, lpTaskName);
        CORE_SetError(STATE_INVALID_PRIORITY);
        return NULL;
    }
#endif

    if (TASK_PRIORITY_IDLE == TaskParam.Priority)
    {
        TaskParam.SliceLength = TASK_SLICE_INFINITE;
        TaskParam.StackSize = CONFIG_IDLE_STACK_SIZE;
    }
    else
    {
        if (TaskParam.SliceLength < CONFIG_TIME_SLICE_NORMAL)
        {
            TaskParam.SliceLength = CONFIG_TIME_SLICE_NORMAL;
        }
    }
    
    TaskParam.StackSize   =   0 == TaskParam.StackSize
                                ?   CONFIG_KTASK_STACK_SIZE
                                :   TaskParam.StackSize;
    TaskParam.lpTaskPath  =   NULL == TaskParam.lpTaskPath
                                ?   CONFIG_DEFAULT_PATH
                                :   TaskParam.lpTaskPath;

    lpTaskContext = (LPVOID) CORE_MallocObject(TSK_MAGIC, lpTaskName, &TaskParam);

    if (NULL == lpTaskContext)
    {
        CORE_ERROR(TRUE, "Malloc task '%s' failed!", lpTaskName);
        return NULL;
    }

    if (0 == GetGlobalTaskContextCount())
    {
        if (STATE_SUCCESS != (State = CORE_StackMalloc(lpTaskContext, &TaskParam, TASK_PERMISSION_USER)))
        {
            CORE_ERROR(TRUE, "Malloc stack for task '%s' failed!", lpTaskName);
            goto lable0;
        }
        
        if (STATE_SUCCESS != (State = CORE_StackInit(lpTaskContext, &TaskParam, TASK_PERMISSION_USER)))
        {
            CORE_ERROR(TRUE, "Fill stack for task '%s' failed!", lpTaskName);
            goto lable1;
        }
    }
    else
    {
        SetContextPermission(lpTaskContext, TASK_PERMISSION_CORE);
    }

    if (STATE_SUCCESS != (State = CORE_StackMalloc(lpTaskContext, &TaskParam, TASK_PERMISSION_CORE)))
    {
        CORE_ERROR(TRUE, "Malloc stack for task '%s' failed!", lpTaskName);
        goto lable1;
    }
    
    if (STATE_SUCCESS != (State = CORE_StackInit(lpTaskContext, &TaskParam, TASK_PERMISSION_CORE)))
    {
        CORE_ERROR(TRUE, "Fill stack for task '%s' failed!", lpTaskName);
        goto lable2;
    }

    if (STATE_SUCCESS == (State = CORE_ActiveObject(GetContextHeader(lpTaskContext), &TaskParam)))
    {
        CORE_INFOR(TRUE, "Crete task '%s' context '0x%p' successfully!", lpTaskName, lpTaskContext);
        return lpTaskContext;
    }

    CORE_ERROR(TRUE, "Active object failed to create task '%s' !", lpTaskName);
    
lable2:
    CORE_StackFree(lpTaskContext, TASK_PERMISSION_CORE);
lable1:
    CORE_StackFree(lpTaskContext, TASK_PERMISSION_USER);
lable0:
    CORE_FreeObject(GetContextHeader(lpTaskContext));
    CORE_SetError(State);
    
    return NULL;
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
EXPORT RO_CODE LPTASK_CONTEXT CORE_CreatePriorityTask(LPCSTR __IN lpTaskName, FNTASKMAIN fnMain,
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

EXPORT RO_CODE VOID CORE_CloseTask(LPTASK_CONTEXT lpTaskContext)
{
    if (lpTaskContext)
    {
        AttachContext2Death(lpTaskContext);
    }
}

EXPORT RO_CODE BOOL CORE_CheckMustbeSchedule(VOID)
{
    return CheckMustSchedule();
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

/**
 * 切换任务
 * @param CoreStack 当前任务内核栈指针
 * @param UserStack 当前任务用户栈指针
 * @return E_TASK_PERMISSION 目标任务的权限级别
 * \par
 *     保存当前任务用户栈指针并返回新任务用户栈指针，调整当前优先级为新任务
 * 优先级更，改当前任务上下文指针为新任务指针，并设置新任务为WORKING状态。
 */
EXPORT E_TASK_PERMISSION CORE_SwitchTask(LPVOID CoreStack, LPVOID UserStack)
{
    LPTASK_CONTEXT lpSwitch2Task = GetSwitch2TaskContext();
 
    CORE_SetCoreStackPosition(CoreStack);
    CORE_SetTaskStackPosition(UserStack);
    SetCurrentPriority(GetContextThisPriority(lpSwitch2Task));
    SetContextState(lpSwitch2Task, TASK_STATE_WORKING);
    SetCurrentTaskContext(lpSwitch2Task);

    if (INVALID_TICK == GetContextStartTick(lpSwitch2Task))
    {
        SetContextStartTick(lpSwitch2Task, CORE_GetSystemTick());
    }
    
    return GetContextPermission(lpSwitch2Task);
}


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

EXPORT E_TASK_PERMISSION CORE_GetCurrentPermission(VOID)
{
    return GetContextPermission(GetCurrentTaskContext());
}

EXPORT LPTASK_CONTEXT CORE_GetCurrentTask(VOID)
{
    return GetCurrentTaskContext();
}
EXPORT_SYMBOL(CORE_GetCurrentTask);

EXPORT LPCSTR CORE_GetCurrentTaskName(VOID)
{
    LPTASK_CONTEXT lpCurrentTask = GetCurrentTaskContext();
    
    if (NULL == lpCurrentTask)
    {
        return "UnknowTask";
    }
    
    return GetContextTaskName(lpCurrentTask);
}

EXPORT LPTASK_CONTEXT CORE_GetCurrentTaskSafe(VOID)
{
    DWORD dwFlags = CORE_DisableIRQ();
    LPTASK_CONTEXT lpCurrentTask = GetCurrentTaskContext();
    CORE_ASSERT(lpCurrentTask,
        CORE_RestoreIRQ(dwFlags); SYSTEM_CALL_OOPS(),
        "Current task context not found ?");
    CORE_RestoreIRQ(dwFlags);
    
    return lpCurrentTask;
}
EXPORT_SYMBOL(CORE_GetCurrentTaskSafe);

EXPORT VOID CORE_SetCurrentTaskRequestPacket(LPVOID lpPacket)
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

EXPORT E_STATUS CORE_TaskWakeup(LPTASK_CONTEXT lpTaskContext)
{
    return WakeupTask(lpTaskContext);
}

EXPORT E_STATUS CORE_TaskWakeupSafe(LPTASK_CONTEXT lpTaskContext)
{
    DWORD dwFlags = CORE_DisableIRQ();
    E_STATUS State = WakeupTask(lpTaskContext);
    CORE_RestoreIRQ(dwFlags);

    return State;
}

EXPORT E_STATUS CORE_TaskSuspend(LPTASK_CONTEXT lpTaskContext, LONG Timeout)
{
    return SuspendTask(lpTaskContext, Timeout);
}

EXPORT E_STATUS CORE_TaskSuspendSafe(LPTASK_CONTEXT lpTaskContext, LONG Timeout)
{
    DWORD dwFlags = CORE_DisableIRQ();
    E_STATUS Result = SuspendTask(lpTaskContext, Timeout);
    CORE_RestoreIRQ(dwFlags);

    return Result;
}

EXPORT E_STATUS CORE_SetThisPriority(LPTASK_CONTEXT lpTaskContext, TASK_PRIORITY Priority)
{
    DWORD dwFlags = CORE_DisableIRQ();

    SetTaskThisPriority(lpTaskContext, Priority);
    
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

PUBLIC E_STATUS initCoreSystemTaskScheduleManager(VOID)
{
    E_STATUS State;

    CORE_INFOR(TRUE, "ArchContext offset of TaskContext is %d.", OFFSET_OF(TASK_CONTEXT, ArchContext));
    CORE_INFOR(TRUE, "Max priority is %d, Show the size of tss type for debug:", CONFIG_TASK_PRIORITY_MAX);
    CORE_INFOR(TRUE, "ARCH_CONTEXT: %d  MANA_MAP_T: %d  TASK_CONTEXT: %d   KOBJECT_HEADER: %d",
        sizeof(ARCH_CONTEXT), sizeof(MANA_MAP_T), sizeof(TASK_CONTEXT), sizeof(KOBJECT_HEADER));
    CORE_INFOR(TRUE, "TASK_STATUS: %d  TIME_SLICE_T: %d", sizeof(TASK_STATUS), sizeof(TIME_SLICE_T));
    
    SystemSchedulerInitialize();

    /* 注册Task context对象类 */
    State = REGISTER_KCLASS(TaskClass);

    CORE_ASSERT(STATE_SUCCESS == State, SYSTEM_CALL_OOPS(),
        "Register task context class failed, result = %d !", State);

    LPC_INSTALL(&LPCService, "Task Schedule(TSK) service starting");

    if (NULL == CORE_CreateTask(BOOT_TASK_NAME, NULL, NULL))
    {
        CORE_ERROR(TRUE, "Create boot task failed, result %d !", CORE_GetError());
        SYSTEM_CALL_OOPS();
    }

    if (NULL == CORE_CreatePriorityTask(IDLE_TASK_NAME, CORE_IdleMain, NULL, TASK_PRIORITY_IDLE))
    {
        CORE_ERROR(TRUE, "Create idle task(%s) failed, result %d !", IDLE_TASK_NAME, CORE_GetError());
        SYSTEM_CALL_OOPS();
    }

    ScheduleStartup();

    return STATE_SUCCESS;
}

