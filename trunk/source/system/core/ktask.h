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
#ifndef __K_TASK_H
#define __K_TASK_H

#include <fadefs.h>
#include <faerror.h>
#include <fatypes.h>

#include "klpc.h"
#include "klist.h"
#include "karch.h"
#include "kobject.h"
#include "limits.h"


#define     TASK_BOOTSTARTUP_CPUID      0

enum{
    LPC_TSS_GET_TASKERROR,
    LPC_TSS_SET_TASKERROR,
    LPC_TSS_GET_CURRENT,
    LPC_TSS_GET_TASKSTATE,
    LPC_TSS_GET_STARTTICK,
    LPC_TSS_GET_PRIORITY,
    LPC_TSS_SET_PRIORITY,
    LPC_TSS_GET_LSOTHANDLE,
    LPC_TSS_SCHEDULE_TIMEOUT,
    LPC_TSS_WAKE_UP,
    LPC_TSS_TEST_CANCEL,
    LPC_TSS_POST_CANCEL,
    LPC_TSS_CLOSE_TASK,
    LPC_TSS_GET_TASKINFO,
    LPC_TSS_SYS_ENUMTASK,
    LPC_TSS_PERFORMANCE,
    LPC_TSS_REQUEST_MAX
};


#define     TASK_ENTRY(Ptr, Member)     ((LPTASK_CONTEXT)CONTAINER_OF(Ptr, TASK_CONTEXT, Member))

typedef struct tagTASK_CONTEXT TASK_CONTEXT;
typedef struct tagTASK_CONTEXT * PTASK_CONTEXT;
typedef struct tagTASK_CONTEXT FAR * LPTASK_CONTEXT;

struct tagTASK_CONTEXT{
    KOBJECT_HEADER          Header;
    HANDLE                  hLockedMutex;                       /* Mutex handle */
    LIST_HEAD               SystemNode;                         /* 系统任务节点*/
    LIST_HEAD               IPCNode;                            /* IPC对象的任务节点 */
    union{
        LIST_HEAD               ReadyNode;                      /* 就绪任务节点 */
        LIST_HEAD               SleepNode;                      /* 休眠任务节点 */
    }Node;
#ifdef SYSTEM_HAVE_TICK64
    TICK                    StartTick;                          /* 任务的开始时间 */
    TICK                    ResumeTick;                         /* 休眠时的唤醒时间 */
#else
    TICK                    StartTick;                          /* 任务的开始时间 */
    LONG                    ResumeRemain;                       /* 休眠后剩余的唤醒时间 */
    DWORD                   TickReserved[2];
#endif
    ARCH_CONTEXT            ArchContext;
#ifdef SYSTEM_HAVE_QWORD
    QWORD                   WorkTimes;
#else
    DWORD                   WorkTimesL;                         /* 任务被调度的次数，低位 */
    DWORD                   WorkTimesH;                         /* 任务被调度的次数，高位 */
#endif
    union{
        struct {
            DWORD           CPUPercent:8;                       /* 单位时间内任务的CPU占用率 */
            DWORD           TaskState:3;                        /* 任务当前状态: TASK_STATUS*/
            DWORD           FaultBit:1;                         /* 任务异常标记(堆栈溢出) */
            DWORD           CancelBit:1;                        /* 任务取消标志 */
            DWORD           Permission:1;                       /**< The permission of task */
            DWORD           ReservedBits:18;                    /* 保留 18 Bits */
        }Bits;
        DWORD               MiscBits;
    }ub;
    TASK_PRIORITY           ThisPriority;                       /* 当前优先级 */
    TASK_PRIORITY           InitPriority;                       /* 初始优先级 */
    TIME_SLICE_T            SliceRemain;                        /* W时间片剩余TICK数 */
    TIME_SLICE_T            SliceLength;                        /* W时间片最大TICK数 */
    E_STATUS                ErrorCode;                          /* 错误码 */
    BYTE                    bReserved[3];
    LPLPC_REQUEST_PACKET    lpLPCPacket;                        /* LPC 请求包指针 for wait object */
    LPKOBJECT_HEADER        lpLsotObject;                       /* 局部变量对象 */
    DWORD                   Reserved[5];                        /* 保留 */
#if (CONFIG_BUILD_TASK_PATH == TRUE)
    CHAR                    cbTaskPath[MAX_PATH];               /* 任务当前路径 */
#endif
};

#define     GetContextLsotObject(lpTC)                      ((lpTC)->lpLsotObject)
#define     SetContextLsotObject(lpTC, lpHeader)            do {(lpTC)->lpLsotObject = (LPVOID)(lpHeader);} while(0)


#define     TASK_BITS_STACKPID_MASK             OBJECT_PID_MASK
#define     TASK_BITS_STACKTID_MASK             OBJECT_TID_MASK
#define     TASK_BITS_PERCENT_MASK              (0xffUL)
#define     TASK_BITS_STATE_MASK                (0x7UL)
#define     TASK_BITS_FAULT_MASK                (0x1UL)
#define     TASK_BITS_CANCEL_MASK               (0x1UL)
#define     TASK_BITS_PERMISSION_MASK           (0x1UL)

#define     MISCBITS_PERCENT_BITS_SHIFT         0
#define     MISCBITS_PERCENT_BITS_MASK          (TASK_BITS_PERCENT_MASK << MISCBITS_PERCENT_BITS_SHIFT)

#define     MISCBITS_STATE_BITS_SHIFT           8
#define     MISCBITS_STATE_BITS_MASK            (TASK_BITS_STATE_MASK<<MISCBITS_STATE_BITS_SHIFT)

#define     MISCBITS_FAULT_BIT_SHIFT            11
#define     MISCBITS_FAULT_BIT_MASK             (TASK_BITS_FAULT_MASK<<MISCBITS_FAULT_BIT_SHIFT)

#define     MISCBITS_CANCEL_BIT_SHIFT           12
#define     MISCBITS_CANCEL_BIT_MASK            (TASK_BITS_CANCEL_MASK<<MISCBITS_CANCEL_BIT_SHIFT)

#define     MISCBITS_PERMISSION_SHIFT           13
#define     MISCBITS_PERMISSION_MASK            (TASK_BITS_PERMISSION_MASK << MISCBITS_PERMISSION_SHIFT)

#define     MISCBITS_STACKTID_BITS_SHIFT        19
#define     MISCBITS_STACKTID_BITS_MASK         (TASK_BITS_STACKTID_MASK << MISCBITS_STACKTID_BITS_SHIFT)

#define     MISCBITS_STACKPID_BITS_SHIFT        22
#define     MISCBITS_STACKPID_BITS_MASK         (TASK_BITS_STACKPID_MASK << MISCBITS_STACKPID_BITS_SHIFT)

#define     MISCBITS_STACKID_BITS_SHIFT         19
#define     MISCBITS_STACKID_BITS_MASK          (MISCBITS_STACKPID_BITS_MASK + MISCBITS_STACKTID_BITS_MASK)

#define     SetContextMiscBits(lpTC, Percent, State, Fault)                                             \
            do {                                                                                        \
                (lpTC)->ub.MiscBits = (((DWORD)(State)) << MISCBITS_STATE_BITS_SHIFT)                   \
                                    | (((DWORD)(Percent)) << MISCBITS_PERCENT_BITS_SHIFT)               \
                                    | (((DWORD)(Fault)) << MISCBITS_FAULT_BIT_SHIFT)                    \
                                    | (((DWORD)(TASK_PERMISSION_USER)) << MISCBITS_PERMISSION_SHIFT)    \
                                    | (((DWORD)(TRUE)) << MISCBITS_CANCEL_BIT_SHIFT);                   \
            } while(0)

#define     GetContextStartTick(lpTC)                       ((lpTC)->StartTick)
#define     SetContextStartTick(lpTC, Tick)                 do {(lpTC)->StartTick = (Tick);} while(0)

#ifdef SYSTEM_HAVE_QWORD
#define     GetContextWorkTimes(lpTC)                       ((lpTC)->WorkTimes)
#define     GetContextWorkFirst(lpTC)                       (0ULL == GetContextWorkTimes(lpTC))
#define     SetContextWorkTimes(lpTC, Tims)                 do {(lpTC)->WorkTimes = (Tims);} while(0)
#define     AddContextWorkTimes(lpTC)                       ((lpTC)->WorkTimes ++)
#else
#define     GetContextWorkFirst(lpTC)                                                                   \
            (0 == ((lpTC)->WorkTimesL) && 0 == ((lpTC)->WorkTimesH))
#define     GetContextWorkTimesL(lpTC)                      ((lpTC)->WorkTimesL)
#define     GetContextWorkTimesH(lpTC)                      ((lpTC)->WorkTimesH)
#define     SetContextWorkTimes(lpTC, Tims)                                                             \
            do {                                                                                        \
                (lpTC)->WorkTimesL = Tims;                                                              \
                (lpTC)->WorkTimesH = 0;                                                                 \
            } while(0)

#define     AddContextWorkTimes(lpTC)                                                                   \
            do {                                                                                        \
                if (0 == (++ (lpTC)->WorkTimesL))                                                       \
                {                                                                                       \
                    (lpTC)->WorkTimesH ++;                                                              \
                }                                                                                       \
            } while(0)
#endif

#ifdef SYSTEM_HAVE_TICK64
#define     GetContextResumeTick(lpTC)                      ((lpTC)->ResumeTick)
#define     SetContextResumeTick(lpTC, Ticks)                                                           \
            do {                                                                                        \
                if (TASK_SLICE_INFINITE == (Ticks))                                                     \
                {                                                                                       \
                    (lpTC)->ResumeTick = TICK_INFINITE;                                                 \
                }                                                                                       \
                else                                                                                    \
                {                                                                                       \
                    (lpTC)->ResumeTick = CORE_GetSystemTick() + (Ticks);                                \
                }                                                                                       \
            } while(0)

#else
#define     GetContextResumeRemain(lpTC)                    ((lpTC)->ResumeRemain)
#define     DecContextResumeRemain(lpTC)                    (--(lpTC)->ResumeRemain)
#define     SetContextResumeRemain(lpTC, Ticks)             do {(lpTC)->ResumeRemain = (Ticks);} while(0)
#endif

#define     GetContextSliceRemain(lpTC)                     ((lpTC)->SliceRemain)
#define     DecContextSliceRemain(lpTC)                     (-- (lpTC)->SliceRemain)
#define     RelContextSliceRemain(lpTC)                     do {(lpTC)->SliceRemain = (lpTC)->SliceLength;} while(0)
#define     SetContextSliceRemain(lpTC, Ticks)              do {(lpTC)->SliceRemain = (Ticks);} while(0)
#define     GetContextSliceLength(lpTC)                     ((lpTC)->SliceLength)
#define     SetContextSliceLength(lpTC, Ticks)              do {(lpTC)->SliceLength = (Ticks);} while(0)

#define     GetContextCPUPercent(lpTC)                      ((lpTC)->ub.Bits.CPUPercent)
#define     SetContextCPUPercent(lpTC, Percent)             do {(lpTC)->ub.Bits.CPUPercent = (Percent);} while(0)

#define     GetContextState(lpTC)                           ((lpTC)->ub.Bits.TaskState)
#define     SetContextState(lpTC, State)                    do { (lpTC)->ub.Bits.TaskState = (State); } while(0)

#define     GetContextCancel(lpTC)                          ((lpTC)->ub.Bits.CancelBit)
#define     SetContextCancel(lpTC, boolean)                 do { (lpTC)->ub.Bits.CancelBit = (boolean); } while(0)

#define     GetContextFaultBit(lpTC)                        ((lpTC)->ub.Bits.FaultBit)
#define     SetContextFaultBit(lpTC, Fault)                 do {(lpTC)->ub.Bits.FaultBit = (Fault);} while(0)

#define     GetContextPermission(lpTC)                      ((E_TASK_PERMISSION)((lpTC)->ub.Bits.Permission))
#define     SetContextPermission(lpTC, Perm)                do {(lpTC)->ub.Bits.Permission = (Perm);} while(0)

#define     GetContextThisPriority(lpTC)                    ((lpTC)->ThisPriority)
#define     SetContextThisPriority(lpTC, Prio)              do {(lpTC)->ThisPriority = (Prio); } while(0)
#define     GetContextInitPriority(lpTC)                    ((lpTC)->InitPriority)
#define     SetContextInitPriority(lpTC, Prio)              do {(lpTC)->InitPriority = (Prio); } while(0)

#define     GetContextTaskError(lpTC)                       ((lpTC)->ErrorCode)
#define     SetContextTaskError(lpTC, Code)                 do {(lpTC)->ErrorCode = (Code);} while(0)

#define     GetContextArchParameter(lpTC)                   (&(lpTC)->ArchContext)

#define     GetContextLPCPacket(lpTC)                       ((lpTC)->lpLPCPacket)
#define     SetContextLPCPacket(lpTC, Addr)                 do {((lpTC)->lpLPCPacket) = (Addr); } while(0)

#define     GetContextLockedMutex(lpTC)                     ((lpTC)->hLockedMutex)
#define     SetContextLockedMutex(lpTC, hMutex)             do {((lpTC)->hLockedMutex) = (hMutex); } while(0)

#define     TaskContextReadyNodeInit(lpTC)                  LIST_HEAD_INIT(&(lpTC)->Node.ReadyNode);
#define     GetContextByReadyNode(lpNode)                   (lpNode ? TASK_ENTRY(lpNode, Node.ReadyNode) : NULL)
#define     AttachReady(lpHead, lpTC)                       LIST_INSERT_TAIL(lpHead, &lpTC->Node.ReadyNode)
#define     DetachReady(lpTC)                               LIST_REMOVE_NODE(&(lpTC)->Node.ReadyNode)


#define     GetContextBySleepNode(lpNode)                   TASK_ENTRY(lpNode, Node.SleepNode)
#define     GetSleepNode(lpTC)                              (&(lpTC)->Node.SleepNode)
#define     AttachSleep(lpHead, lpTC)                       LIST_INSERT_PREV(lpHead, &(lpTC)->Node.SleepNode)
#define     DetachSleep(lpTC)                               LIST_REMOVE_NODE(&(lpTC)->Node.SleepNode)

#define     GetContextByDeathNode(lpNode)                   TASK_ENTRY(lpNode, Node.ReadyNode)
#define     GetDeathNode(lpTC)                              (&(lpTC)->Node.ReadyNode)
#define     AttachDeath(lpHead, lpTC)                       LIST_INSERT_PREV(lpHead, &(lpTC)->Node.ReadyNode)
#define     DetachDeath(lpTC)                               LIST_REMOVE_NODE(&(lpTC)->Node.ReadyNode)

#define     TaskContextSystemNodeInit(lpTC)                 LIST_HEAD_INIT(&(lpTC)->SystemNode);
#define     GetContextBySystemNode(lpNode)                  TASK_ENTRY(lpNode, SystemNode)
#define     AttachSystem(lpHead, lpTC)                      LIST_INSERT_TAIL(lpHead, &(lpTC)->SystemNode)
#define     DetachSystem(lpTC)                              LIST_REMOVE_NODE(&(lpTC)->SystemNode)
#define     GetSystemNextNode(lpTC)                         LIST_NEXT_NODE(&(lpTC)->SystemNode)
#define     GetNextSystemTask(lpTC)                         GetContextBySystemNode(GetSystemNextNode(lpTC))
#define     GetSystemNode(lpTC)                             (&(lpTC)->SystemNode)

#define     TaskContextIPCNodeInit(lpTC)                    LIST_HEAD_INIT(&(lpTC)->IPCNode);
#define     GetIPCNode(lpTC)                                (&(lpTC)->IPCNode)
#define     GetContextByIPCNode(lpNode)                     TASK_ENTRY(lpNode, IPCNode)
#define     AttachIPCNode(lpHead, lpTC)                     LIST_INSERT_TAIL(lpHead, &(lpTC)->IPCNode)
#define     DetachIPCNode(lpTC)                             LIST_REMOVE_NODE(&(lpTC)->IPCNode)

#define     GetContextHeader(lpTC)                          (&(lpTC)->Header)
#define     GetContextHandle(lpTC)                          GetObjectHandle(GetContextHeader(lpTC))
#define     SetContextHandle(lpTC, H)                       SetObjectHandle(GetContextHeader(lpTC), H)

#define     GetContextTaskName(lpTC)                        GetObjectName(&(lpTC)->Header)
#define     SetContextTaskName(lpTC, lpdName)               SetObjectName(&(lpTC)->Header, lpdName)

#define     DumpContextTaskName(lpTC, lpdName)              DumpObjectName(&(lpTC)->Header, lpdName)

#define     Handle2TaskContext(hTask)                       CORE_Handle2TaskContextCheck(hTask, TRUE)



#ifdef __cplusplus
extern "C" {
#endif
    EXPORT E_STATUS CORE_GetError(VOID);
    EXPORT E_STATUS CORE_SetError(E_STATUS emCode);
#define     CORE_Handle2TaskContextCheck(hTask, Check)      ((LPTASK_CONTEXT) CORE_Handle2HeaderCheck(hTask, Check))

    EXPORT DWORD CORE_EnterIRQ(VOID);
    EXPORT DWORD CORE_LeaveIRQ(VOID);
    EXPORT DWORD CORE_GetGlobalTaskCount(VOID);
    EXPORT LPVOID CORE_GetTaskStackPosition(VOID);
    EXPORT VOID CORE_SetTaskStackPosition(LPVOID StackPosition);
    EXPORT LPVOID CORE_GetCoreStackPosition(VOID);
    EXPORT VOID CORE_SetCoreStackPosition(LPVOID StackPosition);
    EXPORT BOOL CORE_CheckMustbeSchedule(VOID);
    EXPORT E_TASK_PERMISSION CORE_SwitchTask(LPVOID CoreStack, LPVOID UserStack);

    EXPORT LPTASK_CONTEXT CORE_GetCurrentTask(VOID);
    EXPORT LPTASK_CONTEXT CORE_GetCurrentTaskSafe(VOID);
    EXPORT E_TASK_PERMISSION CORE_GetCurrentPermission(VOID);
    EXPORT VOID CORE_SetCurrentTaskRequestPacket(LPVOID lpPacket);
    EXPORT E_STATUS CORE_TaskSuspend(LPTASK_CONTEXT lpTaskContext, LONG Timeout);
    EXPORT E_STATUS CORE_TaskWakeup(LPTASK_CONTEXT lpTaskContext, E_STATUS Result);
    
    EXPORT E_STATUS CORE_TaskAttach(LPTASK_CONTEXT lpTaskContext);
    EXPORT E_STATUS CORE_TaskDetach(LPTASK_CONTEXT lpTaskContext);
    EXPORT E_STATUS CORE_SetThisPriority(LPTASK_CONTEXT lpTaskContext, TASK_PRIORITY Priority);
    EXPORT E_STATUS CORE_ResetTaskPriority(LPTASK_CONTEXT lpTaskContext);   

    EXPORT RO_CODE E_STATUS CORE_IdleMain(LPVOID lpParam);
    EXPORT RO_CODE VOID CORE_TaskEntry(FNTASKMAIN fnMain, LPVOID lpArgument, LPTASK_CONTEXT lpTaskContext);
    EXPORT VOID CORE_TaskLeave(VOID);

#define CORE_CreateTask(lpName, fnMain, lpArgument)                                         \
        CORE_CreatePriorityTask(lpName, fnMain, lpArgument, TASK_PRIORITY_NORMAL)
    EXPORT LPTASK_CONTEXT CORE_CreatePriorityTask(LPCSTR __IN lpTaskName, FNTASKMAIN fnMain,
                          LPVOID lpArgument, TASK_PRIORITY Priority);
    EXPORT LPTASK_CONTEXT CORE_CreateTaskEx(LPCSTR lpTaskName, LPTASK_CREATE_PARAM lpParam);
    
    EXPORT VOID CORE_CloseTask(LPTASK_CONTEXT lpTaskContext);
#ifdef __cplusplus
}
#endif

#endif


