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
#include "ktask.h"
#include "kpool.h"
#include "klist.h"
#include "kcore.h"
#include "kdebug.h"
#include "schedule.h"
#include "libcal.h"

STATIC TASK_CONTEXT g_SystemCoreContext[CORE_TASK_MAX];
STATIC CHAR * g_SystemIdleTaskStack[CONFIG_BOOT_STACK_SIZE];

#define Handle2TaskContext(hTask)       CORE_Handle2TaskContextCheck(hTask, TRUE)

EXPORT LPTASK_CONTEXT CORE_Handle2TaskContextCheck(HANDLE hTask, BOOL Check)
{
    if (!IsCoreHandle(hTask))
    {
        return (LPTASK_CONTEXT) CORE_Handle2HeaderCheck(hTask, Check);
    }

    if ((BOOT_TASK_HANDLE & (~ HANDLE_OBJECT_SID_MASK)) == (hTask & (~ HANDLE_OBJECT_SID_MASK)))
    {
        return &g_SystemCoreContext[BOOT_TASK_ID];
    }
    
    if (IDLE_TASK_HANDLE == hTask)
    {
        return &g_SystemCoreContext[IDLE_TASK_ID];
    }
    
    CORE_ERROR(TRUE, "Why to get context failed ? hTask: 0x%08X, hBoot: 0x%08X, hIdle: 0x%08X",
        hTask, BOOT_TASK_HANDLE, IDLE_TASK_HANDLE);
    
    return NULL;
}

/************************************************************************************************
                               
************************************************************************************************/



PUBLIC E_STATUS ScheduleStartup(VOID)
{
    SetScheduleRunState(TRUE);
    SetInterruptNestLayer(0);
    return CORE_EnableKernelStack(GetKernelStackButtom());
}

STATIC E_STATUS TaskSetError(E_STATUS emCode)
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

STATIC E_STATUS TaskGetError(VOID)
{
    LPTASK_CONTEXT lpCurrentContext = GetCurrentTaskContext();
    
    if (NULL != lpCurrentContext)
    {
        return GetContextTaskError(lpCurrentContext);
    }

    return GetSystemGlobalError();
}

STATIC  E_STATUS SetContextParam(LPTASK_CONTEXT lpTaskContext, LPKTASK_CREATE_PARAM lpTaskParam)
{
#if (CONFIG_ENABLE_FAST_SCHEDULE == TRUE)
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
    
    return STATE_SUCCESS;
}

STATIC VOID AttachStack2Context(LPTASK_CONTEXT lpTaskContext,
                    LPKOBJECT_HEADER StackHeader, LPKTASK_CREATE_PARAM lpTaskParam)
{
    LPBYTE lpStackBuffer = (LPVOID) StackHeader;
    LPBYTE lpStackPosition = (LPVOID) (lpStackBuffer + lpTaskParam->Param.StackSize);
    KOBJTABLE_ID_T Tid = GetHandleObjectTid(GetObjectHandle(StackHeader));
    KCONTAINER_ID_T Pid = GetHandleObjectPid(GetObjectHandle(StackHeader));
    
 
    lpStackPosition = CORE_FillStack(lpStackPosition, lpTaskParam->Param.fnTaskMain,
                                     lpTaskParam->Param.lpArgument, GetContextHandle(lpTaskContext));

    SetContextStackCapacity(lpTaskContext, lpTaskParam->Param.StackSize);
    SetContextStackBuffer(lpTaskContext, lpStackBuffer); 
    SetContextStackPosition(lpTaskContext, lpStackPosition);
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
        CORE_INFOR(TRUE, "Task '%s' attach to suspend queue.", GetContextTaskName(lpTaskContext));
        Attach2SuspendQueue(lpTaskContext, TICK_INFINITE);
        break;
    case TASK_STATE_READY:
        CORE_INFOR(TRUE, "Task '%s' attach to ready queue.", GetContextTaskName(lpTaskContext));
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
        SetContextStartTick(lpTaskContext, INVALID_TICK);
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

    /* 任务堆栈必须已删除 */
    if (NULL != GetContextStackBuffer(lpTaskContext))
    {
        CORE_RestoreIRQ(dwFlags);
        return STATE_SYSTEM_BUSY;
    }

    switch(GetContextState(lpTaskContext))
    {
    case TASK_STATE_READY:
    case TASK_STATE_WORKING:
    case TASK_STATE_SLEEP:
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
        
#endif
        SetContextStackBuffer(lpTaskContext, NULL);
    }
    
    return CORE_FreeObject(GetContextHeader(lpTaskContext));
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
    
    CORE_INFOR(TRUE, "Malloc stack '%s' for task 0x%P handle 0x%08X ...",
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
    
    if (TASK_SLICE_INFINITE == lpTaskParam->Param.SliceLength || 
        CONFIG_TIME_SLICE_NORMAL > lpTaskParam->Param.SliceLength)
    {
        CORE_ERROR(TRUE, "Invalid slice(%d) time to create task '%s' !",
            lpTaskParam->Param.SliceLength, GetContextTaskName(lpTaskContext));
        return STATE_INVALID_TIME;
    }

    if (CONFIG_DEFAULT_STACK_SIZE != lpTaskParam->Param.StackSize)
    {
        CORE_ERROR(TRUE, "Invalid stack size %d.", lpTaskParam->Param.StackSize);
        return STATE_INVALID_SIZE;
    }
    
#if (CONFIG_TASK_PRIORITY_MAX != 256)
    if (lpTaskParam->Param.Priority >= CONFIG_TASK_PRIORITY_MAX)
#else
    if (lpTaskParam->Param.Priority >= CONFIG_TASK_PRIORITY_MAX ||
        lpTaskParam->Param.Priority == TASK_PRIORITY_IDLE)
#endif
    {
        CORE_ERROR(TRUE, "Invalid priority(%d) to create task '%s'!",
            lpTaskParam->Param.Priority, GetContextTaskName(lpTaskContext));
        return STATE_INVALID_PRIORITY;
    }
 
    CORE_INFOR(TRUE, "Malloc 0x%P task '%s' ...", lpHeader, GetObjectName(lpHeader));

    return SetContextParam(lpTaskContext,  lpTaskParam);
}

STATIC E_STATUS STM_ActiveContext(LPKOBJECT_HEADER lpHeader, LPVOID lpParam)
{
    LPKTASK_CREATE_PARAM lpTaskParam = lpParam;
    LPTASK_CONTEXT lpTaskContext = (LPVOID) lpHeader;
    
    CORE_INFOR(TRUE, "Active task '%s' ...", GetObjectName(lpHeader));

    return AttachContext2System(lpTaskContext, lpTaskParam);
}

STATIC E_STATUS STM_FreeContext(LPKOBJECT_HEADER lpHeader)
{
    LPTASK_CONTEXT lpTaskContext = (LPVOID) lpHeader;

    CORE_INFOR(TRUE, "Free 0x%P task '%s' ...", lpHeader, GetObjectName(lpHeader));
    
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
    return TaskGetError();
}

STATIC E_STATUS SVC_SetTaskError(LPVOID lpPrivate, LPVOID lpParam)
{
    LPLPC_REQUEST_PACKET lpPacket = lpParam;
    
    return TaskSetError((E_STATUS) lpPacket->u0.dParam);
}

STATIC E_STATUS SVC_GetTaskName(LPVOID lpPrivate, LPVOID lpParam)
{
    LPDWORD lpdName;
    LPTASK_CONTEXT lpTaskContext;
    LPLPC_REQUEST_PACKET lpPacket = lpParam;
    
    lpTaskContext = INVALID_HANDLE_VALUE == lpPacket->u0.hParam
                  ? GetCurrentTaskContext()
                  : Handle2TaskContext(lpPacket->u0.hParam);
    
    if (NULL == lpTaskContext)
    {
        return STATE_INVALID_OBJECT;
    }

    lpdName = lpPacket->u1.pParam;
    
    DumpContextTaskName(lpTaskContext, lpdName);

    return STATE_SUCCESS;
}

STATIC E_STATUS SVC_GetTaskStartTick(LPVOID lpPrivate, LPVOID lpParam)
{
    LPTASK_CONTEXT lpTaskContext;
    LPLPC_REQUEST_PACKET lpPacket = lpParam;
    
    lpTaskContext = INVALID_HANDLE_VALUE == lpPacket->u0.hParam
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
    LPLPC_REQUEST_PACKET lpPacket = lpParam;
    DWORD dwFlags = CORE_DisableIRQ();
    E_STATUS State = SuspendTask(GetCurrentTaskContext(), lpPacket->u0.lParam);
    
    CORE_RestoreIRQ(dwFlags);

    return State;
}

STATIC E_STATUS SVC_TaskWakeup(LPVOID lpPrivate, LPVOID lpParam)
{
    LPTASK_CONTEXT lpTaskContext;
    LPLPC_REQUEST_PACKET lpPacket = lpParam;

    if (INVALID_HANDLE_VALUE == lpPacket->u0.hParam)
    {
        return STATE_INVALID_PARAMETER;
    }

    lpTaskContext = Handle2TaskContext(lpPacket->u0.hParam);

    if (NULL == lpTaskContext)
    {
        return STATE_INVALID_OBJECT;
    }
    
    SetContextCancel(lpTaskContext, TRUE);
    
    return WakeupTask(lpTaskContext, STATE_SUCCESS);
}

STATIC E_STATUS SVC_TestCancel(LPVOID lpPrivate, LPVOID lpParam)
{
    LPTASK_CONTEXT lpTaskContext = GetCurrentTaskContext();
    
    CORE_ASSERT(lpTaskContext, SYSTEM_CALL_OOPS();, "BUG: Invalid current task context.");

    if (TRUE == GetContextCancel(lpTaskContext))
    {
        return STATE_REMOVED;
    }
    
    return STATE_SUCCESS;
}

STATIC E_STATUS SVC_PostCancel(LPVOID lpPrivate, LPVOID lpParam)
{
    LPTASK_CONTEXT lpTaskContext;
    LPLPC_REQUEST_PACKET lpPacket = lpParam;

    if (INVALID_HANDLE_VALUE == lpPacket->u0.hParam)
    {
        return STATE_INVALID_PARAMETER;
    }

    lpTaskContext = Handle2TaskContext(lpPacket->u0.hParam);

    if (NULL == lpTaskContext)
    {
        return STATE_INVALID_OBJECT;
    }
    
    SetContextCancel(lpTaskContext, TRUE);
    
    return STATE_SUCCESS;
}


STATIC E_STATUS SVC_GetTaskPriority(LPVOID lpPrivate, LPVOID lpParam)
{
    LPTASK_CONTEXT lpTaskContext;
    LPLPC_REQUEST_PACKET lpPacket = lpParam;
    
    lpTaskContext = INVALID_HANDLE_VALUE == lpPacket->u0.hParam
                  ? GetCurrentTaskContext()
                  : Handle2TaskContext(lpPacket->u0.hParam);
    
    if (NULL == lpTaskContext)
    {
        return STATE_INVALID_OBJECT;
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
    
    lpTaskContext = INVALID_HANDLE_VALUE == lpPacket->u0.hParam
                  ? GetCurrentTaskContext()
                  : Handle2TaskContext(lpPacket->u0.hParam);
    
    if (NULL == lpTaskContext)
    {
        return STATE_INVALID_OBJECT;
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
    
    lpTaskContext = INVALID_HANDLE_VALUE == lpPacket->u0.hParam
                  ? GetCurrentTaskContext()
                  : Handle2TaskContext(lpPacket->u0.hParam);
    
    if (NULL == lpTaskContext)
    {
        return STATE_INVALID_OBJECT;
    }

    return CloseTaskContext(lpTaskContext);
}

STATIC CONST REQUEST_HANDLER fnHandlers[] = {
    SVC_GetTaskError,
    SVC_SetTaskError,
    SVC_GetTaskName,
    SVC_GetTaskStartTick,
    SVC_GetTaskPriority,
    SVC_SetTaskPriority,
    SVC_TaskSchedule,
    SVC_TaskWakeup,
    SVC_TestCancel,
    SVC_PostCancel,
    SVC_CloseTask,
};

DEFINE_LPC_SERVICE(LPCService, STM_MAGIC, SIZEOF_ARRAY(fnHandlers), NULL, fnHandlers);


PUBLIC E_STATUS CreateCoreTask(LPCSTR lpName, FNTASKMAIN fnMain, LPVOID lpArgument, DWORD Id)
{
    KTASK_CREATE_PARAM TaskParam;
    LPTASK_CONTEXT lpTaskContext;
    LPKOBJECT_HEADER StackHeader = NULL;

    if (Id >= CORE_TASK_MAX)
    {
        return STATE_OUT_OF_MEMORY;
    }
    
    if (TRUE == GetScheduleRunState())
    {
        return STATE_SYSTEM_BUSY;
    }
    
    lpTaskContext = &g_SystemCoreContext[Id];

    memset(lpTaskContext, 0, sizeof(TASK_CONTEXT));

    TaskParam.Param.lpTaskPath    =   CONFIG_DEFAULT_PATH;
    TaskParam.Param.AutoStartup   =   TRUE;
    TaskParam.Param.fnTaskMain    =   fnMain;
    TaskParam.Param.lpArgument    =   lpArgument;

    SetContextHandle(lpTaskContext, MakeCoreTaskHandle(Id));
    SetContextTaskName(lpTaskContext, ((LPDWORD)lpName));
    
    if (Id == BOOT_TASK_ID)
    {
        TaskParam.Param.Priority      =   TASK_PRIORITY_NORMAL;
        TaskParam.Param.SliceLength   =   CONFIG_TIME_SLICE_NORMAL;
        TaskParam.Param.StackSize     =   CONFIG_BOOT_STACK_SIZE;
        StackHeader = CORE_GetBootStackBuffer();
        
        SetObjectHandle(StackHeader, MakeCoreStackHandle(Id));
        SetContextParam(lpTaskContext, &TaskParam);
        AttachStack2BootTask(lpTaskContext, StackHeader);
        SetCurrentPriority(TASK_PRIORITY_NORMAL);
        SetCurrentTaskContext(lpTaskContext);               /* boot task is current task */
        SetSwitch2TaskContext(lpTaskContext);
    }
    else
    {
        TaskParam.Param.Priority      =   TASK_PRIORITY_IDLE;
        TaskParam.Param.SliceLength   =   TASK_SLICE_INFINITE;
        TaskParam.Param.StackSize     =   CONFIG_IDLE_STACK_SIZE;
        StackHeader = (LPVOID) g_SystemIdleTaskStack;
        
        SetObjectHandle(StackHeader, MakeCoreStackHandle(Id));
        SetContextParam(lpTaskContext, &TaskParam);
        AttachStack2Context(lpTaskContext, StackHeader, &TaskParam);
    }

    AttachContext2System(lpTaskContext, &TaskParam);
    
    if (Id == BOOT_TASK_ID)
    {
        SetContextState(lpTaskContext, TASK_STATE_WORKING);
        SetContextStartTick(lpTaskContext, CORE_GetSystemTick());;
    }

    CORE_INFOR(TRUE, "Create core task %p name '%s' handle is 0x%08x.",
        lpTaskContext, GetContextTaskName(lpTaskContext), GetContextHandle(lpTaskContext));

    return STATE_SUCCESS;
}


/************************************************************************************************
                               Some stack object functions
************************************************************************************************/
PUBLIC E_STATUS initCoreSystemTaskScheduleManager(VOID)
{
    E_STATUS State;
    
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

    State = CreateCoreTask("Boot", caIdleTask_Main, NULL, BOOT_TASK_ID);

    CORE_ASSERT(STATE_SUCCESS == State, SYSTEM_CALL_OOPS(),
        "Create boot task failed, result = %d !", State);

    State = CreateCoreTask("Idle", caIdleTask_Main, NULL, IDLE_TASK_ID);

    CORE_ASSERT(STATE_SUCCESS == State, SYSTEM_CALL_OOPS(),
        "Create idle task failed, result %d.", State);
        
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
    return GetKernelStackPosition(StackPosition);
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

    E_STATUS State = TaskSetError(emCode);

    CORE_RestoreIRQ(dwFlags);
    
    return State;
}
EXPORT_CORE_SYMBOL(CORE_SetError);

EXPORT E_STATUS CORE_GetError(VOID)
{
    DWORD dwFlags = CORE_DisableIRQ();
    
    E_STATUS State = TaskGetError();
    
    CORE_RestoreIRQ(dwFlags);
    
    return State;
}
EXPORT_CORE_SYMBOL(CORE_GetError);


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
EXPORT_CORE_SYMBOL(CORE_GetCurrentTask);

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
