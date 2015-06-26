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
#include "config.h"
#include <stm32f10x.h>
#include <facore.h>
#include <fadefs.h>
#include <faerror.h>
#include <fatypes.h>

#include "ktask.h"
#include "kcore.h"
#include "kstack.h"
#include "kdebug.h"
#include "kboard.h"
#include "kobject.h"


typedef struct tagKSTACK_HEADER KSTACK_HEADER;
typedef struct tagKSTACK_HEADER * PKSTACK_HEADER;
typedef struct tagKSTACK_HEADER FAR * LPKSTACK_HEADER;

struct tagKSTACK_HEADER{
    KOBJECT_HEADER          Header;
    LPSTR                   Buffer[CONFIG_DEFAULT_STACK_SIZE - sizeof(KOBJECT_HEADER)];
};

#define     SetStackObjectMagic(lpHeader)         SetObjectMagic(GetContextHeader(lpHeader), STK_MAGIC, STK_MAGIC)
#define     GetStackObjectHandle(lpHeader)        GetObjectHandle(GetContextHeader(lpHeader))


EXPORT VOID CORE_SetTaskStackPosition(LPVOID StackPosition)
{
    LPTASK_CONTEXT lpCurrentTask = CORE_GetCurrentTask();
    LPARCH_CONTEXT lpArchContext = GetContextArchParameter(lpCurrentTask);
    E_TASK_PERMISSION TaskPermission = GetContextPermission(lpCurrentTask);

    SetStackPosition(GetArchSD(lpArchContext, TaskPermission), StackPosition);
}

EXPORT LPVOID CORE_GetTaskStackPosition(VOID)
{
    LPTASK_CONTEXT lpCurrentTask = CORE_GetCurrentTask();
    LPARCH_CONTEXT lpArchContext = GetContextArchParameter(lpCurrentTask);
    E_TASK_PERMISSION TaskPermission = GetContextPermission(lpCurrentTask);

    return GetStackPosition(GetArchSD(lpArchContext, TaskPermission));
}

EXPORT LPVOID CORE_GetCoreStackPosition(LPVOID StackPosition)
{
    LPTASK_CONTEXT lpCurrentTask = CORE_GetCurrentTask();
    LPARCH_CONTEXT lpArchContext = GetContextArchParameter(lpCurrentTask);
    
    return GetStackPosition(GetArchCoreSD(lpArchContext));
}



#define     OBJ_MallocStack                         OBJ_DummyOperation
#define     OBJ_ActiveStack                         OBJ_DummyOperation
#define     OBJ_TakeStack                           OBJ_DummyOperation
#define     OBJ_PostStack                           OBJ_DummyOperation
#define     OBJ_ResetStack                          OBJ_DummyOperation

STATIC E_STATUS OBJ_DummyOperation(LPKOBJECT_HEADER lpHeader, LPVOID lpParam)
{
    return STATE_SUCCESS;
}

STATIC E_STATUS OBJ_WaitObject(LPKOBJECT_HEADER lpHeader, LONG WaitTime)
{
    return STATE_NOT_SUPPORT;
}

STATIC E_STATUS OBJ_FreeStack(LPKOBJECT_HEADER lpHeader)
{
    return STATE_SUCCESS;
}

DEFINE_CLASS(STK_MAGIC, StackClass, CONFIG_DEFAULT_STACK_SIZE,
            OBJ_MallocStack,
            OBJ_ActiveStack,
            OBJ_TakeStack,
            OBJ_WaitObject,
            OBJ_PostStack,
            OBJ_ResetStack,
            OBJ_FreeStack);


PUBLIC E_STATUS CORE_CreateStackForTask(LPVOID lpTaskContext, LPVOID lpParam, E_TASK_PERMISSION Permission)
{
    LPKSTACK_HEADER lpHeader = NULL;
    LPTASK_CREATE_PARAM lpTaskParam = lpParam;
    LPTASK_CONTEXT lpCoreContext = lpTaskContext;
    LPARCH_CONTEXT lpArchContext = GetContextArchParameter(lpCoreContext);

    if (NULL == lpTaskParam || Permission >= TASK_PERMISSION_MAX)
    {
        CORE_ERROR(TRUE, "Invalid parameter TaskParam(%P) or Permission(%u).",
            lpTaskParam, Permission);
        return STATE_INVALID_PARAMETER;
    }

    if (KOBJECT_STATE_CREATE != GetObjectState(GetContextHeader(lpCoreContext)))
    {
        CORE_ERROR(TRUE, "Task state(%u) invalid.",
            GetObjectState(GetContextHeader(lpCoreContext)));
        return STATE_INVALID_STATE;
    }

    /* 任务权限和堆栈权限不一致的情况: 普通任务创建内核堆栈和内核任务创建普通堆栈 */
    if (GetContextPermission(lpCoreContext) != Permission)
    {
        /* 内核任务不需要创建用户堆栈 */
        if (TASK_PERMISSION_CORE == GetContextPermission(lpCoreContext))
        {
            return STATE_SUCCESS;
        }
        /* 普通任务创建内核堆栈 */
        if (TRUE == CORE_CpuSupportGlobalCoreStack())
        {
            /* CPU 支持全局内核堆栈 */
            lpHeader = CORE_GetCoreStackButtom();
            SetStackCapacity(GetArchSD(lpArchContext, Permission), CONFIG_CORE_STACK_SIZE);
        }
        else
        {
            /* CPU 不支持全局内核堆栈，每个任务一个独立的内核s堆栈 */
            lpHeader = (LPVOID)CORE_MallocNoNameObject(STK_MAGIC, lpTaskParam);
            SetStackCapacity(GetArchSD(lpArchContext, Permission), CONFIG_DEFAULT_STACK_SIZE);
        }
    }
    else if (0 == CORE_GetGlobalTaskCount())
    {
        /* Boot 任务创建普通堆栈(BOOT任务为普通任务)*/
        lpHeader = CORE_GetBootStackBuffer();
        SetStackCapacity(GetArchSD(lpArchContext, Permission), CORE_GetBootStackCapacity());
    }
    else if (TASK_PRIORITY_IDLE == GetContextThisPriority(lpCoreContext))
    {
        /* Idle 任务创建内核堆栈(Idle任务为内核任务)*/
        lpHeader = CORE_GetIdleStackBuffer(TASK_BOOTSTARTUP_CPUID);
        SetStackCapacity(GetArchSD(lpArchContext, Permission), CORE_GetIdleStackCapacity());
    }
    else
    {
        /* 任务权限和堆栈权限一致的情况：内核任务创建内核堆栈和普通任务创建普通堆栈 */
        if (CONFIG_DEFAULT_STACK_SIZE != lpTaskParam->StackSize)
        {
            CORE_ERROR(TRUE, "Invalid stack size %d.", lpTaskParam->StackSize);
            return STATE_INVALID_SIZE;
        }

        lpHeader = (LPVOID)CORE_MallocNoNameObject(STK_MAGIC, lpTaskParam);
        SetStackCapacity(GetArchSD(lpArchContext, Permission), CONFIG_DEFAULT_STACK_SIZE);
    }
    
    SetStackBuffer(GetArchSD(lpArchContext, Permission), lpHeader);

    if (NULL == lpHeader)
    {
        CORE_ERROR(TRUE, "Malloc stack for task '%s' failed.", GetContextTaskName(lpCoreContext));
        return CORE_GetError();
    }
    
    return STATE_SUCCESS;
}

PUBLIC E_STATUS CORE_FillTaskStack(LPVOID lpTaskContext, LPVOID lpParam, E_TASK_PERMISSION Permission)
{
    LPKSTACK_HEADER lpHeader =  NULL;
    LPTASK_CREATE_PARAM lpTaskParam = lpParam;
    LPTASK_CONTEXT lpCoreContext = lpTaskContext;
    LPARCH_CONTEXT lpArchContext = GetContextArchParameter(lpCoreContext);

    if (NULL == lpTaskParam || Permission >= TASK_PERMISSION_MAX)
    {
        CORE_ERROR(TRUE, "Invalid parameter TaskParam(%P) or Permission(%u).",
            lpTaskParam, Permission);
        return STATE_INVALID_PARAMETER;
    }

    if (KOBJECT_STATE_CREATE != GetObjectState(GetContextHeader(lpCoreContext)))
    {
        CORE_ERROR(TRUE, "Task state(%u) invalid.",
            GetObjectState(GetContextHeader(lpCoreContext)));
        return STATE_INVALID_STATE;
    }
    
    lpHeader = GetStackBuffer(GetArchSD(lpArchContext, Permission));
    
    if (NULL != lpHeader)
    {
        DWORD StackCapacity = GetStackCapacity(GetArchSD(lpArchContext, Permission));
        LPBYTE lpStackBuffer = (LPVOID) lpHeader;
        LPBYTE lpStackPosition = (lpStackBuffer + StackCapacity);
        
        if (0 != CORE_GetGlobalTaskCount() && GetContextPermission(lpCoreContext) == Permission)
        {
            lpStackPosition = CORE_FillStack(lpStackPosition, lpTaskParam->fnTaskMain,
                  lpTaskParam->lpArgument, GetContextHandle(lpCoreContext));
        }

        SetStackHandle(GetArchSD(lpArchContext, Permission), GetStackObjectHandle(lpHeader));
        SetStackPosition(GetArchSD(lpArchContext, Permission), lpStackPosition);
        SetStackObjectMagic(lpHeader);
        
        return STATE_SUCCESS;
    }

    CORE_ERROR(TRUE, "No task stack buffer found.");

    return STATE_INVALID_STACK;
}

PUBLIC E_STATUS CORE_StackFree(LPVOID lpTaskContext, E_TASK_PERMISSION Permission)
{
    LPTASK_CONTEXT lpCoreContext = lpTaskContext;
    
    if (Permission >= TASK_PERMISSION_MAX)
    {
        return STATE_INVALID_PARAMETER;
    }

    if (GetContextPermission(lpCoreContext) != Permission)
    {
        return STATE_SUCCESS;
    }
    
    return STATE_SUCCESS;
}

STATIC E_STATUS SVC_MallocStack(LPVOID lpPrivate, LPVOID lpParam)
{
    LPTASK_CONTEXT lpTaskContext;
    LPLPC_REQUEST_PACKET lpPacket = lpParam;
    
    lpTaskContext = Handle2TaskContext(lpPacket->u0.hParam);
    
    if (NULL == lpTaskContext)
    {
        return STATE_INVALID_PARAMETER;
    }

    return CORE_CreateStackForTask(lpTaskContext, lpPacket->u1.pParam, (E_TASK_PERMISSION) lpPacket->u2.dParam);
}

STATIC E_STATUS SVC_FillStack(LPVOID lpPrivate, LPVOID lpParam)
{
    LPTASK_CONTEXT lpTaskContext;
    LPLPC_REQUEST_PACKET lpPacket = lpParam;
    
    lpTaskContext = Handle2TaskContext(lpPacket->u0.hParam);
    
    if (NULL == lpTaskContext)
    {
        return STATE_INVALID_PARAMETER;
    }

    return CORE_FillTaskStack(lpTaskContext, lpPacket->u1.pParam, (E_TASK_PERMISSION)lpPacket->u2.dParam);
}

STATIC E_STATUS SVC_FreeStack(LPVOID lpPrivate, LPVOID lpParam)
{
    return STATE_NOT_IMPLEMENTED;
}


STATIC CONST REQUEST_HANDLER fnHandlers[] = {
    SVC_MallocStack,                    /* 01. LPC_STK_STACK_MALLOC */
    SVC_FillStack,                      /* 02. LPC_STK_STACK_FILL */
    SVC_FreeStack,                      /* 03. LPC_STK_STACK_FREE */
};

DEFINE_LPC_SERVICE(LPCService, STK_MAGIC, SIZEOF_ARRAY(fnHandlers), NULL, fnHandlers);

PUBLIC E_STATUS initCoreSystemTaskStackManager(VOID)
{
    E_STATUS State;
    /* 注册 stack 对象类 */
    State = CORE_RegisterClass(&StackClass);

    CORE_ASSERT(STATE_SUCCESS == State, SYSTEM_CALL_OOPS(),
        "Register stack class failed, result = %d !", State);
    
    LPC_INSTALL(&LPCService, "Task stack(STK) manager starting");
    
    return STATE_SUCCESS;
}

