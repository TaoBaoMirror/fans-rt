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
#ifndef __K_SCHEDULE_H
#define __K_SCHEDULE_H

#include "libfw.h"
#include "klpc.h"
#include "ktask.h"
#include "ktick.h"
#include "klist.h"
#include "kcore.h"
#include "karch.h"
#include "ktable.h"
#include "kdebug.h"

#define     SCHEDULE_STATE_MASK         0x1
#define     SCHEDULE_NEED_MASK          0x2
#define     SCHEDULE_CONDITION          (SCHEDULE_STATE_MASK | SCHEDULE_NEED_MASK)

typedef union tagSCHEDULER_FLAGS{
    struct {
        BOOL                SchedulerRunState:1;
        BOOL                NeedSchedule:1;
        BOOL                MustSchedule:1;
    }Bits;
    BYTE                    Value;
}SCHEDULER_FLAGS;

STATIC  RW_CORE_DATA    LIST_HEAD       g_SystemTaskTable;                                                      /**< 系统任务表 */
//STATIC  RW_CORE_DATA    LIST_HEAD       g_SystemDeathQueue;                                                     /**< 任务死亡队列 */
STATIC  RW_CORE_DATA    LIST_HEAD       g_TaskSuspendQueue;                                                     /**< 任务休眠队列 */
STATIC  RW_CORE_DATA    LIST_HEAD       g_TaskReadyQueue[CONFIG_TASK_PRIORITY_MAX];                             /**< 优先级就绪队列 */
PUBLIC  RW_CORE_DATA    VOLATILE        LPTASK_CONTEXT      g_CurrentContext        =       NULL;               /**< 当前任务上下文指针 */
STATIC  RW_CORE_DATA    VOLATILE        LPTASK_CONTEXT      g_Switch2Context        =       NULL;               /**< 切换目标任务上下文指针  */

#if (CONFIG_PROFILER_CYCLE != 0)
STATIC  RW_CORE_DATA    VOLATILE        DWORD               g_SystemScheduleCount   =       0;                  /**< 任务调度次数 */
#endif

STATIC  RW_CORE_DATA    VOLATILE        WORD                g_SystemTaskCount       =       0;
STATIC  RW_CORE_DATA    VOLATILE        E_STATUS            g_SystemGlobalError     =       STATE_SUCCESS;
STATIC  RW_CORE_DATA    VOLATILE        SCHEDULER_FLAGS     g_SchedulerFlags        =       {.Value = 0};
STATIC  RW_CORE_DATA    VOLATILE        E_TASK_PERMISSION   g_CurrentPermission     =       TASK_PERMISSION_USER;
STATIC  RW_CORE_DATA    VOLATILE        TASK_PRIORITY       g_CurrentPriority       =       TASK_PRIORITY_IDLE; /**< 当前任务优先级 */
STATIC  RW_CORE_DATA    VOLATILE        BYTE                g_InterruptNestLayer    =       2;                  /**< 中断嵌套 */

#if (CONFIG_TASK_PRIORITY_MAX <= 8)
STATIC  RW_CORE_DATA    VOLATILE        BYTE                g_PriorityReadyBitmap;                              /**< 任务就绪位图 */
#elif (CONFIG_TASK_PRIORITY_MAX <= 16)
STATIC  RW_CORE_DATA    VOLATILE        WORD                g_PriorityReadyBitmap;                              /**< 任务就绪位图 */
#elif (CONFIG_TASK_PRIORITY_MAX <= 32)
STATIC  RW_CORE_DATA    VOLATILE        DWORD               g_PriorityReadyBitmap;                              /**< 任务就绪位图 */
#else
STATIC  RW_CORE_DATA    VOLATILE        BYTE                g_PriorityReadyBitmap0;                             /**< 0级任务就绪位图 */
STATIC  RW_CORE_DATA    VOLATILE        BYTE                g_PriorityReadyBitmap1[4];                          /**< 1级任务就绪位图 */
STATIC  RW_CORE_DATA    VOLATILE        BYTE                g_PriorityReadyBitmap2[32];                         /**< 2级任务就绪位图 */
#endif

#define     GetGlobalTaskContextCount()                 g_SystemTaskCount
#define     IncGlobalTaskContextCount()                 (g_SystemTaskCount ++)
#define     DecGlobalTaskContextCount()                 (-- g_SystemTaskCount)
#define     GetContextPriorityReadyQueue(lpC)           (&g_TaskReadyQueue[GetContextThisPriority(lpC)])
#define     Attach2SystemTaskTable(lpTaskContext)       AttachSystem(&g_SystemTaskTable, lpTaskContext)
#define     GetPriorityFirstContext(Priority)           GetContextByReadyNode(LIST_FIRST_NODE(&g_TaskReadyQueue[Priority]))

#define     GetIdleContext()                            GetPriorityFirstContext(TASK_PRIORITY_IDLE)
#define     CheckAllIdleTaskCreateFinished()            (NULL != GetIdleContext())
#define     GetFreeIdleCPUID()                          0

#define     CheckPriorityIsFree(Priority)               LIST_EMPTY(&g_TaskReadyQueue[Priority])
#define     CheckTaskPriorityIsFree(lpTC)               CheckPriorityIsFree(GetContextThisPriority(lpTC))
#define     ClearTaskReadyBitmap(lpTC)                  do {                                                                \
                                                            if (CheckTaskPriorityIsFree(lpTC))                              \
                                                            {                                                               \
                                                                ClrReadyBitmap(GetContextThisPriority(lpTC), 1);            \
                                                            }                                                               \
                                                        } while(0)
#define     EnterInterruptCritiacl()                    (g_InterruptNestLayer ++)
#define     LeaveInterruptCritiacl()                    (-- g_InterruptNestLayer)
#define     GetInterruptNestLayer()                     (g_InterruptNestLayer)
#define     SetInterruptNestLayer(Number)               do { g_InterruptNestLayer = (Number); } while(0)

#define     CheckNeedSchedule()                         (SCHEDULE_CONDITION == (g_SchedulerFlags.Value & SCHEDULE_CONDITION))
#define     SetNeedSchedule()                           do { g_SchedulerFlags.Bits.NeedSchedule = TRUE; } while(0)
#define     ClrNeedSchedule()                           do { g_SchedulerFlags.Bits.NeedSchedule = FALSE; } while(0)

#define     CheckMustSchedule()                         (g_SchedulerFlags.Bits.MustSchedule)
#define     SetMustSchedule()                           do { g_SchedulerFlags.Bits.MustSchedule = TRUE; } while(0)
#define     ClrMustSchedule()                           do { g_SchedulerFlags.Bits.MustSchedule = FALSE; } while(0)

#define     GetScheduleRunState()                       (g_SchedulerFlags.Bits.SchedulerRunState)
#define     SetScheduleRunState(Boolean)                do { g_SchedulerFlags.Bits.SchedulerRunState = (BOOL)(Boolean); } while(0)

#define     SetCurrentTaskContext(Context)              do { g_CurrentContext = (Context); ClrMustSchedule();} while(0)
#define     GetCurrentTaskContext()                     g_CurrentContext

#define     GetSwitch2TaskContext()                     g_Switch2Context
#define     SetSwitch2TaskContext(Context)              do { g_Switch2Context = (Context); } while(0)

#define     SetCurrentPriority(Priority)                do { g_CurrentPriority = (Priority); } while(0)
#define     GetCurrentPriority()                        g_CurrentPriority
#define     GetCurrentStackFault()                      GetContextFaultBit(GetCurrentTaskContext())
#define     GetSystemGlobalError()                      (g_SystemGlobalError)
#define     SetSystemGlobalError(state)                 (g_SystemGlobalError = (state))
#define     GetSystemScheduleCount()                    (g_SystemScheduleCount)
#if (CONFIG_PROFILER_CYCLE != 0)
#define     AddSystemScheduleCount()                    (g_SystemScheduleCount ++)
#define     SetSystemScheduleCount(x)                   (g_SystemScheduleCount = (x))
#endif

#if 0
STATIC E_STATUS StackOverflowCheck(LPTASK_CONTEXT lpTaskContext)
{
    return STATE_SUCCESS;
}
#endif

/**
 * Find the highest priority in the ready bitmap
 * @return The highest priority of ready task in system
 * \par
 * This system supports up to 256 priority, the value is higher, the priority is lower. 
 * When a ready task attach to the priority of ready queue, the corresponding
 * bit in the ready bitmap will be set an one. When a priority of ready queue becomes 
 * empty, the corresponding bit in the ready bitmap will be set a zero. When search for  
 * the highest priority ready task, the value is 1 of the lowset bit in the ready bitmap
 * is the highest priority, the first task in this priority ready queue is the new schedule
 * task.
 *
 * 系统最多支持256级优先级，优先级 数值越大，优先级越低。当一个就绪任务被关联到对应优先
 * 级就绪队列时，就绪位图对应位会被置1。当某个优先级就绪队列变为空时，就绪位图的对应位
 * 会被置0。在查找最高优先级的就绪任务时，就绪位图值为1的最低位即为当前需要调度的最高优
 * 先级，该优先级就绪队列中的第一个任务即为需要调度的任务。
 *
 * date           author          notes
 * 2014-09-07     JiangYong       first version
 */
#if (CONFIG_TASK_PRIORITY_MAX <= 8)
STATIC INLINE TASK_PRIORITY SearchHighestPriority(VOID)
{
    return GetByteLowestBit(g_PriorityReadyBitmap);
}
#elif (CONFIG_TASK_PRIORITY_MAX <= 16)
STATIC INLINE TASK_PRIORITY SearchHighestPriority(VOID)
{
    return GetWordLowestBit(g_PriorityReadyBitmap);
}
#elif (CONFIG_TASK_PRIORITY_MAX <= 32)
STATIC INLINE TASK_PRIORITY SearchHighestPriority(VOID)
{
    return GetDwordLowestBit(g_PriorityReadyBitmap);
}
#else
STATIC TASK_PRIORITY SearchHighestPriority(VOID)
{
    DWORD Offs0 = GetByteLowestBit(g_PriorityReadyBitmap0);
    DWORD Offs1 = GetByteLowestBit(g_PriorityReadyBitmap1[Offs0]);
    DWORD Offs2 = GetByteLowestBit(g_PriorityReadyBitmap2[(Offs0 * 8) + Offs1]);
    
    return (Offs0 * 64) + (Offs1 * 8) + Offs2;
}
#endif

#if (CONFIG_TASK_PRIORITY_MAX <= 32)
/**
 * Set the priority of the ready list to ready state(<=32 priority level).
 * @param The value of the priority
 * \par
 * The corresponding bit in the priority bitmap will be set an one.
 *
 * 将优先级就绪位图中对应的位设置为1。
 *
 * date           author          notes
 * 2014-09-07     JiangYong       first version
 */
STATIC INLINE VOID SetReadyBitmap(TASK_PRIORITY Priority)
{
    g_PriorityReadyBitmap  |=  (1 << Priority);
}

/**
 * Set the priority of the ready list to free state(<=32 priority level).
 * @param The value of the priority
 * \par
 * The corresponding bit in the priority bitmap will be set an zero.
 *
 * 将优先级就绪位图中对应的位设置为0。
 *
 * date           author          notes
 * 2014-09-07     JiangYong       first version
 */
STATIC INLINE VOID ClrReadyBitmap(TASK_PRIORITY Priority, BOOL Clear)
{
    g_PriorityReadyBitmap  &= (~(Clear << Priority));
}
#else
/**
 * Set the priority of the ready list to ready state(>32 priority level).
 * @param The value of the priority
 * \par
 * The corresponding bit in the priority of ready bitmap will be set an one.
 *
 * 将优先级就绪位图中对应的位设置为1。
 *
 * date           author          notes
 * 2014-09-07     JiangYong       first version
 */
STATIC INLINE VOID SetReadyBitmap(TASK_PRIORITY Priority)
{
    g_PriorityReadyBitmap0
        |=  (1 << (Priority / (BYTE_WIDTH * BYTE_WIDTH)));
    g_PriorityReadyBitmap1[Priority/(BYTE_WIDTH * BYTE_WIDTH)]
        |=  (1 << ((Priority / BYTE_WIDTH) % BYTE_WIDTH));
    g_PriorityReadyBitmap2[Priority/BYTE_WIDTH]
        |=  (1 << (Priority % BYTE_WIDTH));
}

/**
 * Set the priority of the ready list to free state(>32 priority level).
 * @param The value of the priority
 * \par
 * The corresponding bit in the priority of ready bitmap will be set an zero.
 * 将优先级就绪位图中对应的位设置为0。
 *
 * date           author          notes
 * 2014-09-07     JiangYong       first version
 */
STATIC INLINE VOID ClrReadyBitmap(TASK_PRIORITY Priority, BOOL Clear)
{
    g_PriorityReadyBitmap2[Priority/BYTE_WIDTH]
        &=  ~(Clear << (Priority % BYTE_WIDTH));
    g_PriorityReadyBitmap1[Priority/(BYTE_WIDTH * 8)]
        &=  ~((!g_PriorityReadyBitmap2[Priority/BYTE_WIDTH])
        << ((Priority / BYTE_WIDTH) % BYTE_WIDTH));
    g_PriorityReadyBitmap0
        &=  ~((!g_PriorityReadyBitmap1[Priority/(BYTE_WIDTH * 8)])
        << (Priority / (BYTE_WIDTH * BYTE_WIDTH)));
}

#endif

/**
 * Begin to schedule(The CPU support scheduling interrupt).
 * @return VOID
 * \par
 * If the new task is not the current task, then the scheduling interrupt will be
 * activation, return from current interrupt will be enter to the scheduling interrupt.
 * 
 * CPU支持调度中断时，如果新任务与当前任务不是同一任务，则激活任务调度中断，从中断返
 * 回后进入任务调度中断进行任务调度。
 *
 * date           author          notes
 * 2015-06-11     JiangYong       first version
 */
STATIC VOID ScheduleBegin(VOID)
{
    TASK_PRIORITY Priority = SearchHighestPriority();
    LPTASK_CONTEXT lpOldTask = GetCurrentTaskContext();
    LPTASK_CONTEXT lpNewTask = GetPriorityFirstContext(Priority);

    if (lpOldTask != lpNewTask)
    {
        if (lpNewTask != GetSwitch2TaskContext())
        {
            CORE_DEBUG(TRUE, "Schedule to '%p-%s', Priority is %d, %p, %p.",
               lpNewTask, GetContextTaskName(lpNewTask), Priority, &g_TaskReadyQueue[Priority],
               LIST_FIRST_NODE(&g_TaskReadyQueue[Priority]), lpOldTask, GetContextTaskName(lpOldTask));
            CORE_DEBUG(TRUE, "Last task is '%p-%s', Priority is %d->%d, state is %d.",
               lpOldTask, GetContextTaskName(lpOldTask),
                GetContextInitPriority(lpOldTask),
                GetContextThisPriority(lpOldTask),
                GetContextState(lpOldTask));
            
            SetSwitch2TaskContext(lpNewTask);
            SetMustSchedule();
            CORE_ActiveSwitchIRQ();
        }
    }
    else
    {
        SetContextState(lpNewTask, TASK_STATE_WORKING);
    }
}


/**
 * The slice of current task was finished.
 * @param The context of the finish task.
 * @return VOID
 * \par
 * The slice of current task was decrease to zero, the system will be scheduling the other
 * task of current priority.
 * The system will be reset the slice of this task, this task will be reattach to ready
 * queue, and the state of this task will be change from working to ready. The corresponding 
 * bit in the priority of ready bitmap will be set to one.
 * 
 * 当前任务的时间片自动递减为 0 时，系统将强行调度当优先级的其他任务(抢占)。
 * 系统会重置该任务的时间片，并且将该任务重新关联到就绪队列，该任务的状态也将由工作态切换为
 * 就绪态，对应优先级就绪位图会被重新置位。
 *
 * date           author          notes
 * 2015-06-11     JiangYong       first version
 */
STATIC VOID ScheduleFinish(LPTASK_CONTEXT lpTaskContext)
{
    DetachReady(lpTaskContext);
    AttachReady(GetContextPriorityReadyQueue(lpTaskContext), lpTaskContext);
    SetContextState(lpTaskContext, TASK_STATE_READY);
    SetContextSliceRemain(lpTaskContext, GetContextSliceLength(lpTaskContext));
    SetReadyBitmap(GetContextThisPriority(lpTaskContext));
    AddContextWorkTimes(lpTaskContext);
    SetNeedSchedule();
    CORE_DEBUG(TRUE, "Task '%s' schedule finish.",
        GetContextTaskName(lpTaskContext), GetContextSliceRemain(lpTaskContext));
}


/**
 * Task attach to the ready queue.
 * @param The context of the task.
 * @return VOID
 * \par
 * If need wakeup any task, the task will be detach from suspend queue and detach from ipc
 * object and attach to ready queue. the state of this task will be change from sleep to ready. 
 * The system will be reset the slice of this task, The corresponding bit in the priority of
 * ready bitmap will be set to one.
 * 
 * 唤醒任何任务，该任务都将从挂起队列和 ipc 对象中摘除，任务状态将从休眠切换到就绪，任务时间片
 * 会被系统重置，对应优先级的就绪位图也会被置位。
 *
 * date           author          notes
 * 2015-06-11     JiangYong       first version
 */
STATIC VOID Attach2ReadyQueue(LPTASK_CONTEXT lpTaskContext)
{
    DetachSleep(lpTaskContext);
    DetachIPCObject(lpTaskContext);
    AttachReady(GetContextPriorityReadyQueue(lpTaskContext), lpTaskContext);
    SetContextState(lpTaskContext, TASK_STATE_READY);
    RelContextSliceRemain(lpTaskContext);
    SetReadyBitmap(GetContextThisPriority(lpTaskContext));
    SetNeedSchedule();
}

#ifdef SYSTEM_HAVE_TICK64
/**
 * Task insert to the suspend queue(only for have system tick64).
 * @param The context of the task.
 * @return VOID
 * \par
 * If need suspend any task, the task will be insert to the appropriate location of
 * the suspend queue(Best wakeup tick and the most appropriate priority).
 *
 * 如果需要挂起一个任务，该任务将被插入到休眠队列中最恰当的位置
 *
 * date           author          notes
 * 2015-06-11     JiangYong       first version
 */
STATIC VOID Insert2SuspendQueue(LPTASK_CONTEXT lpThisContext)
{
    LPLIST_HEAD lpList = &g_TaskSuspendQueue;
    TICK ThisResume = GetContextResumeTick(lpThisContext);
    
    if (TICK_INFINITE != ThisResume)
    {
        LIST_FOR_EACH(lpList, &g_TaskSuspendQueue)
        {
            LPTASK_CONTEXT lpTaskContext = GetContextBySleepNode(lpList);
            
            TICK LastResume = GetContextResumeTick(lpTaskContext);

            if (TICK_INFINITE == LastResume)
            {
                break;
            }
            
            if (ThisResume < LastResume)
            {
                /* 根据优先级选择队列位置 */
                if (GetContextThisPriority(lpThisContext) <
                    GetContextThisPriority(lpTaskContext))
                {
                    break;
                }
            }
        }
    }
    
    AttachSleep(lpList, lpThisContext);
}
#endif

/**
 * Task attach to the suspend queue.
 * @param The context of the task.
 * @param The ticks of suspend.
 * @return VOID
 * \par
 * If need suspend any task, the task will be attach to the ready queue, and detach from the
 * suspend queue, and change the task to suspend state, and clears the ready bitmap.
 *
 * 将任务从就绪队列中删除，并关联到休眠队列，然后将任务修改为休眠状态，并清除就绪位图
 *
 * date           author          notes
 * 2015-06-11     JiangYong       first version
 */
STATIC VOID Attach2SuspendQueue(LPTASK_CONTEXT lpTaskContext, TICK Ticks)
{
    DetachReady(lpTaskContext);
    SetContextState(lpTaskContext, TASK_STATE_SLEEP);
#ifdef SYSTEM_HAVE_TICK64
    SetContextResumeTick(lpTaskContext, Ticks);
    Insert2SuspendQueue(lpTaskContext);
#else
    SetContextResumeRemain(lpTaskContext, Ticks);
    AttachSleep(&g_TaskSuspendQueue, lpTaskContext);
#endif
    ClearTaskReadyBitmap(lpTaskContext);
}

/**
 * Task will be detach state.
 * @param The context of the task.
 * @return VOID
 *
 * date           author          notes
 * 2015-06-28     JiangYong       first version
 */
STATIC VOID Context2DetachState(LPTASK_CONTEXT lpTaskContext)
{
    DetachReady(lpTaskContext);
    DetachIPCNode(lpTaskContext);
    SetContextCancel(lpTaskContext, TRUE);
    SetContextState(lpTaskContext, TASK_STATE_DETACH);
    ClearTaskReadyBitmap(lpTaskContext);
}
/**
 * The slice handler for the scheduler.
 * \par
 * The function will check the current task is finished using the scheduling of all 
 * time. if time runs out, but the task will still be working, it will be placed 
 * in the tail of the ready queue of tasks waiting for the next scheduling. This
 * called rotation.
 *
 * 该函数会检查当前任务是否已经使用完本次调度的所有时间片，如果时间片被用完，但是任务
 * 依然为活动状态，则会将任务放入就绪队列的尾部等待下次调度，这种情况称为轮转.
 *
 * date           author          notes
 * 2015-06-11     JiangYong       first version
 */
STATIC VOID ScheduleSliceHandler(VOID)
{
    DWORD dwFlags = CORE_DisableIRQ();
    LPTASK_CONTEXT lpCurrentTask = GetCurrentTaskContext();

    CORE_ASSERT(TASK_STATE_WORKING == GetContextState(lpCurrentTask),
        CORE_RestoreIRQ(dwFlags); SYSTEM_CALL_OOPS();,
        "Task %p name '%s' state %d not valid",
        lpCurrentTask, GetContextTaskName(lpCurrentTask),
        GetContextState(lpCurrentTask));

    if (TASK_SLICE_INFINITE != GetContextSliceRemain(lpCurrentTask))
    {
        if (0 == DecContextSliceRemain(lpCurrentTask))
        {
            ScheduleFinish(lpCurrentTask);
        }
    }

    CORE_RestoreIRQ(dwFlags);
}

#ifdef SYSTEM_HAVE_TICK64
/**
 * Task resume handler(have system tick64).
 * \par
 * The task resume handler will be periodically checks the suspend queue to the system
 * tick frequency. The first task of the suspend queue will be wakeup when the system
 * time exceeds this task suspend time.
 *
 * 任务唤醒处理器会以系统滴答的频率定期检查休眠队列，当系统时间超过队列中第一个任务的
 * 休眠时间后，这个任务将被唤醒。
 *
 * date           author          notes
 * 2015-06-11     JiangYong       first version
 */
STATIC VOID ScheduleResumeHandler(VOID)
{
    DWORD dwFlags = 0;
    LPLIST_HEAD lpList = NULL;
    LPLIST_HEAD lpNode = NULL;
    
    dwFlags = CORE_DisableIRQ();

    LIST_FOR_EACH_SAFE(lpList, lpNode, &g_TaskSuspendQueue)
    {
        LPTASK_CONTEXT lpTaskContext = GetContextBySleepNode(lpList);
        TICK ResumeTick = GetContextResumeTick(lpTaskContext);

        /* If the first task is infinite or insufficient, there is no need to wake up.*/
        /* If the first task need to wake up, then should be detected the next task, */
        /* until no task needs to wake up. */
        if (TICK_INFINITE == ResumeTick || ResumeTick > CORE_GetSystemTick())
        {
            break;
        }
        
        Attach2ReadyQueue(lpTaskContext);
    }

    CORE_RestoreIRQ(dwFlags);
}
#else
/**
 * Task resume handler(no system tick64).
 * \par
 * The task resume handler will be periodically checks the suspend queue to the system
 * tick frequency. When sleep timer is 0, the task will be attach to the ready queue.
 *
 * 任务唤醒处理器会以系统滴答的频率定期检查休眠队列，当系统时间超过队列中第一个任务的
 * 休眠时间后，这个任务将被唤醒。
 *
 * date           author          notes
 * 2015-06-11     JiangYong       first version
 */
STATIC VOID ScheduleResumeHandler(VOID)
{
    DWORD dwFlags = 0;
    LPLIST_HEAD lpList = NULL;
    LPLIST_HEAD lpNode = NULL;

    dwFlags = CORE_DisableIRQ();

    LIST_FOR_EACH_SAFE(lpList, lpNode, &g_TaskSuspendQueue)
    {
        LPTASK_CONTEXT lpTaskContext = GetContextBySleepNode(lpList);

        if (WAIT_INFINITE != GetContextResumeRemain(lpTaskContext))
        {
            if (0 == DecContextResumeRemain(lpTaskContext))
            {
                Attach2ReadyQueue(lpTaskContext);
            }
        }
    }

    CORE_RestoreIRQ(dwFlags);
}
#endif

/**
 * Suspend specified task
 * @param The context of the task.
 * @param The time of suspend
 * @return STATE_SUCCESS successfully
 * \par
 * Task will be attach to suspend queue, and schedule to a new task.
 *
 * 任务将被关联到休眠队列，并调度一个新的任务。
 *
 * date           author          notes
 * 2015-06-11     JiangYong       first version
 */
STATIC E_STATUS SuspendTask(LPTASK_CONTEXT lpTaskContext, LONG Timeout)
{
    TICK Ticks;

    CORE_ASSERT(lpTaskContext, SYSTEM_CALL_OOPS(), "BUG: Invalid task context to suspend !");
    
    if (Timeout < 0)
    {
        if (WAIT_INFINITE != Timeout)
        {
            return STATE_INVALID_VALUE;
        }
        
        Ticks = TICK_INFINITE;
    }
    else
    {
        Ticks = MILLI_SECOND_TO_TICK(Timeout);
    }

    Attach2SuspendQueue(lpTaskContext, Ticks);

    SetNeedSchedule();

    return STATE_SUCCESS;
}

/**
 * Wakeup specified task
 * @param The context of the task.
 * @return STATE_NOT_READY Task not ready.
 * @return STATE_SUCCESS successfully.
 * \par
 * Wake up a task, the task will be awakened specified return value back to 
 * the application layer.
 *
 * 唤醒一个任务，被唤醒的任务将以指定的返回值返回应用层。
 *
 * date           author          notes
 * 2015-06-11     JiangYong       first version
 * 2015-07-30     JiangYong       Add wake routine
 */
STATIC E_STATUS WakeupTask(LPTASK_CONTEXT lpTaskContext)
{
    TASK_STATUS State = GetContextState(lpTaskContext);

    if (TASK_STATE_SLEEP == State ||
        TASK_STATE_WAITING == State)
    {
        Attach2ReadyQueue(lpTaskContext);
        SetNeedSchedule();

        return STATE_SUCCESS;
    }

    return STATE_NOT_READY;
}

/**
 * Set the current priority for tasks in the ready queue
 * @param The context of the task.
 * @param The new priority for this task
 * \par
 * Set the proority for the task in the ready queue needs:
 *   1 detach task from the ready queue
 *   2 cleared ready bitmap
 *   3 changes the current priority value in the task context object
 *   4 the task reattach to the ready queue
 *   5 update the ready bitmap according to the new priority task
 *   6 set schedule flag
 *   7 current priority of the system will be change if the priority 
 *     of current task was changed
 *
 * 就绪队列中的任务调整优先级需要完成如下流程:
 *   1.从就绪队列中摘除任务
 *   2.清除就绪位图
 *   3.修改任务上下文对象中当前优先级的值
 *   4.重新将任务关联到就绪队列
 *   5.根据任务的新优先级更新就绪位图
 *   6.设置调度标志
 *   7.如果当前任务发生优先级变更则修改系统的当前优先级
 *
 * date           author          notes
 * 2015-06-11     JiangYong       first version
 */
STATIC VOID SetReadyTaskPriority(LPTASK_CONTEXT lpTaskContext, TASK_PRIORITY Priority)
{
    DetachReady(lpTaskContext);
    ClearTaskReadyBitmap(lpTaskContext);
    SetContextThisPriority(lpTaskContext, Priority);
    AttachReady(GetContextPriorityReadyQueue(lpTaskContext), lpTaskContext);
    SetReadyBitmap(GetContextThisPriority(lpTaskContext));
    SetNeedSchedule();
    if (lpTaskContext == GetCurrentTaskContext())
    {
        SetCurrentPriority(Priority);
    }
}

/**
 * Set current priority for the task
 * @param The context of the task.
 * @param The new priority for this task
 * \par
 * If the task is suspend, directly modify the priority value.
 * If the task is ready or working must be reattach to the ready queue.
 *
 * 如果任务为休眠状态，直接修改任务优先级，如果任务为就绪状态或活动状态
 * 则需要重新关联就绪队列。
 *
 * date           author          notes
 * 2015-06-11     JiangYong       first version
 */
STATIC E_STATUS SetTaskThisPriority(LPTASK_CONTEXT lpTaskContext, TASK_PRIORITY Priority)
{
    E_STATUS State = STATE_SUCCESS;

    if (Priority == GetContextThisPriority(lpTaskContext))
    {
        return STATE_SUCCESS;
    }
    
    switch(GetContextState(lpTaskContext))
    {
    case TASK_STATE_SLEEP:
    case TASK_STATE_WAITING:
        SetContextThisPriority(lpTaskContext, Priority);
#ifdef SYSTEM_HAVE_TICK64
        DetachSleep(lpTaskContext);
        Insert2SuspendQueue(lpTaskContext);
#endif
        break;
    case TASK_STATE_READY:
    case TASK_STATE_WORKING:
        SetReadyTaskPriority(lpTaskContext, Priority);
        break;
    default:
        State = STATE_NOT_READY;
        break;
    }

    return State;
}

/**
 * Set all priority for the task
 * @param The context of the task.
 * @param The new priority for this task
 * \par
 * The task priority include initial priority and current priority.
 * The current priority of the task currently running show priority in dealing with the priority
 * inversion may require temporary adjustment
 * The current priority of the task is completed after the inversion reset task priority to the
 * initial priority.
 *
 * 任务优先级包括初始优先级和当前优先级
 * 当前优先级表明任务当前运行的优先级，在处理优先级倒挂时可能需要临时调整
 * 任务的当前优先级，倒挂任务执行完毕后优先级复位到初始优先级。
 *
 * date           author          notes
 * 2015-06-11     JiangYong       first version
 */
STATIC E_STATUS SetTaskPriority(LPTASK_CONTEXT lpTaskContext, TASK_PRIORITY Priority)
{
    SetContextInitPriority(lpTaskContext, Priority);
    return SetTaskThisPriority(lpTaskContext, Priority);
}

PUBLIC VOID SystemSchedulerInitialize(VOID)
{
    DWORD i = 0;
    
    for (i = 0; i < CONFIG_TASK_PRIORITY_MAX; i ++)
    {
        LIST_HEAD_INIT(&g_TaskReadyQueue[i]);
    }

    LIST_HEAD_INIT(&g_TaskSuspendQueue);
    LIST_HEAD_INIT(&g_SystemTaskTable);
#if (CONFIG_TASK_PRIORITY_MAX <= 32)
    g_PriorityReadyBitmap = 0;
#else
    g_PriorityReadyBitmap0 = 0;
    memset((LPVOID) g_PriorityReadyBitmap1, 0,sizeof(g_PriorityReadyBitmap1));
    memset((LPVOID) g_PriorityReadyBitmap2, 0,sizeof(g_PriorityReadyBitmap2));
#endif
}

#endif

