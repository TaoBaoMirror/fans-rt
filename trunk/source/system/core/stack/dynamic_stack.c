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
 *    2015-07-5     JiangYong       new file
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
#include "kmem.h"

#if (CONFIG_DYNAMIC_STACK_ENABLE==TRUE)

#if (CONFIG_MEM_REGION_MAX == 0)
#error "No memory region found !!"
#endif

typedef struct tagKSTACK_HEADER KSTACK_HEADER;
typedef struct tagKSTACK_HEADER * PKSTACK_HEADER;
typedef struct tagKSTACK_HEADER FAR * LPKSTACK_HEADER;

struct tagKSTACK_HEADER{
    LPSTR                   Buffer[CONFIG_DEFAULT_STACK_SIZE];
};


PUBLIC E_STATUS CORE_StackMalloc(LPVOID lpTaskContext, LPVOID lpParam, E_TASK_PERMISSION Permission)
{
    LPKSTACK_HEADER lpHeader = NULL;
    LPARCH_CONTEXT lpArchContext = NULL;
    LPTASK_CREATE_PARAM lpTaskParam = lpParam;
    LPTASK_CONTEXT lpCoreContext = lpTaskContext;

    if (NULL == lpTaskContext)
    {
        CORE_ERROR(TRUE, "Invalid task context.");
        return STATE_INVALID_TASK;
    }
    
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

    lpArchContext = GetContextArchParameter(lpCoreContext);
    /* 任务权限和堆栈权限不一致的情况: 普通任务创建内核堆栈和内核任务创建普通堆栈 */
    if (GetContextPermission(lpCoreContext) != Permission)
    {
        /* 任务权限和堆栈权限不一致，堆栈权限为USER，则任务权限为 CORE: 内核任务不需要创建用户堆栈 */
        if (TASK_PERMISSION_USER == Permission)
        {
            SetStackCapacity(GetArchSD(lpArchContext, Permission), 0);
            SetStackBuffer(GetArchSD(lpArchContext, Permission), NULL);
            CORE_DEBUG(TRUE, "Task '%s' no need malloc core stack.",
                GetContextTaskName(lpCoreContext), Permission, lpHeader);
            return STATE_SUCCESS;
        }
        else /* 普通任务创建内核栈 */
        {
#if (CONFIG_CORE_STACK_SIZE == 0)
            /* 任务权限和堆栈权限不一致，如果内核栈大小为0 */
            /* 则普通任务分配用户栈，但是以局部内核栈的方式使用，具备内核任务权限 */
            if (0 == CORE_GetGlobalTaskCount())
            {
                /* Boot 任务创建普通堆栈(BOOT任务为普通任务)*/
                lpHeader = CORE_GetBootStackBuffer();
                SetStackCapacity(GetArchSD(lpArchContext, Permission), CORE_GetBootStackLength());
                CORE_DEBUG(TRUE, "Malloc stack(%d) buffer 0x%P for task '%s' successfully.",
                    Permission, lpHeader, GetContextTaskName(lpCoreContext));
            }
            else /* 创建非 BOOT 任务内核栈 */
            {
                lpHeader = (LPVOID)CORE_PageAlloc(CONFIG_CORE_STACK_SIZE);
                SetStackCapacity(GetArchSD(lpArchContext, Permission), CONFIG_CORE_STACK_SIZE);
                CORE_DEBUG(TRUE, "Malloc stack(%d) buffer 0x%P for task '%s' successfully.",
                Permission, lpHeader, GetContextTaskName(lpCoreContext));
            }
#elif (CONFIG_ARCH_SUPPORT_KSTACK == TRUE)
            /* CPU 支持全局内核堆栈 */
            lpHeader = CORE_GetCoreStackBuffer();
            SetStackCapacity(GetArchSD(lpArchContext, Permission), CORE_GetCoreStackLength());
            CORE_DEBUG(TRUE, "Malloc stack(%d) buffer 0x%P for task '%s' successfully.",
                Permission, lpHeader, GetContextTaskName(lpCoreContext));
#else
            /* CPU 不支持全局内核堆栈，每个任务一个独立的内核堆栈 */
            lpHeader = (LPVOID)CORE_PageAlloc(lpTaskParam->StackSize);
            SetStackCapacity(GetArchSD(lpArchContext, Permission), CONFIG_CORE_STACK_SIZE);
            CORE_DEBUG(TRUE, "Malloc stack(%d) buffer 0x%P for task '%s' successfully.",
                Permission, lpHeader, GetContextTaskName(lpCoreContext));
#endif
        }
    }
    else if (TASK_PRIORITY_IDLE == GetContextThisPriority(lpCoreContext))
    {
        /* Idle 任务创建内核堆栈(Idle任务为内核任务)*/
        lpHeader = CORE_GetIdleStackBuffer(TASK_BOOTSTARTUP_CPUID);
        SetStackCapacity(GetArchSD(lpArchContext, Permission), CORE_GetIdleStackLength());
        CORE_DEBUG(TRUE, "Malloc stack(%d) buffer 0x%P for task '%s' successfully.",
                Permission, lpHeader, GetContextTaskName(lpCoreContext));
    }
    else
    {
        /* 任务权限和堆栈权限一致的情况：内核任务创建内核堆栈和普通任务创建普通堆栈 */
#if (CONFIG_CORE_STACK_SIZE == 0)
        if (TASK_PERMISSION_USER == Permission)
        {
            /* 任务权限和堆栈权限一致，但是没有全局内核栈，则任务不指定用户栈 */
            SetStackCapacity(GetArchSD(lpArchContext, Permission), 0);
            SetStackBuffer(GetArchSD(lpArchContext, Permission), NULL);
            CORE_DEBUG(TRUE, "Task '%s' no need malloc user stack.",
                GetContextTaskName(lpCoreContext), Permission, lpHeader);

            return STATE_SUCCESS;
        }
#endif
        lpHeader = (LPVOID)CORE_PageAlloc(lpTaskParam->StackSize);
        SetStackCapacity(GetArchSD(lpArchContext, Permission), lpTaskParam->StackSize);
        CORE_DEBUG(TRUE, "Malloc stack(%d) buffer 0x%P for task '%s' successfully.",
                Permission, lpHeader, GetContextTaskName(lpCoreContext));

    }
    
    SetStackBuffer(GetArchSD(lpArchContext, Permission), lpHeader);

    if (NULL == lpHeader)
    {
        CORE_ERROR(TRUE, "Malloc stack(%d) for task '%s' failed.",
            Permission, GetContextTaskName(lpCoreContext));
        return CORE_GetError();
    }

    CORE_DEBUG(TRUE, "Task '%s' malloc stack(%d) buffer 0x%P for task '%s' successfully.",
                GetContextTaskName(lpCoreContext), Permission, lpHeader, GetContextTaskName(lpCoreContext));
    return STATE_SUCCESS;
}

PUBLIC E_STATUS CORE_StackInit(LPVOID lpTaskContext, LPVOID lpParam, E_TASK_PERMISSION Permission)
{
    LPKSTACK_HEADER lpHeader =  NULL;
    LPARCH_CONTEXT lpArchContext = NULL;
    LPTASK_CREATE_PARAM lpTaskParam = lpParam;
    LPTASK_CONTEXT lpCoreContext = lpTaskContext;

    if (NULL == lpTaskContext)
    {
        CORE_ERROR(TRUE, "Invalid task context.");
        return STATE_INVALID_TASK;
    }
    
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

    lpArchContext = GetContextArchParameter(lpCoreContext);
    lpHeader = GetStackBuffer(GetArchSD(lpArchContext, Permission));
    
    if (NULL != lpHeader)
    {
        DWORD StackCapacity = GetStackCapacity(GetArchSD(lpArchContext, Permission));
        BYTE_PTR lpStackBuffer = (LPVOID) lpHeader;
        BYTE_PTR lpStackPosition = (lpStackBuffer + StackCapacity);

        /* 对于 boot task(Boot task 的用户栈不能 Fill)*/
#if (CONFIG_CORE_STACK_SIZE == 0)
        /* 如果内核栈大小为0，则不填充普通用户的用户栈(内核任务和普通任务均只填充内核栈)*/
        if (0 != CORE_GetGlobalTaskCount() && (TASK_PERMISSION_CORE == Permission))
#elif (CONFIG_ARCH_SUPPORT_KSTACK == TRUE)
        /* 如果 CPU 支持全局内核栈，则不填充普通用户的内核栈(只填充内核任务的内核栈和用户任务的用户栈)*/
        if ((0 != CORE_GetGlobalTaskCount()) && (GetContextPermission(lpCoreContext) == Permission))
#else
        /* 如果 CPU 不支持全局内核栈，则不填充内核任务的用户栈(填充内核任务的内核栈，用户任务的所有栈)*/
        if ((0 != CORE_GetGlobalTaskCount()) &&
            ((TASK_PERMISSION_USER == GetContextPermission(lpCoreContext)) ||
            TASK_PERMISSION_CORE == Permission))
#endif
        {
            lpStackPosition = CORE_FillStack(lpStackPosition, lpTaskParam->fnTaskMain,
                              lpTaskParam->lpArgument, lpTaskContext,
                              GetContextHandle(lpCoreContext), Permission);
        }

        SetStackPosition(GetArchSD(lpArchContext, Permission), lpStackPosition);
        
        CORE_DEBUG(TRUE, "Task '%s' fill stack(%d) buffer 0x%p buttom 0x%p position 0x%p.",
            GetContextTaskName(lpCoreContext), Permission, lpHeader,
            lpStackBuffer + StackCapacity, lpStackPosition);
        
        return STATE_SUCCESS;
    }
    else
    {
#if (CONFIG_CORE_STACK_SIZE == 0)
        /* 如果内核栈大小为0，所有任务均没有用户栈 */
        if (TASK_PERMISSION_USER == Permission)
        {
            CORE_DEBUG(TRUE, "Task '%s' no need fill user stack.",
                GetContextTaskName(lpCoreContext));
            
            return STATE_SUCCESS;
        }
#elif (CONFIG_ARCH_SUPPORT_KSTACK == TRUE)
        /* 如果 CPU 支持全局内核栈，则内核任务的用户栈为 NULL*/
        if (TASK_PERMISSION_CORE != GetContextPermission(lpCoreContext) &&
            TASK_PERMISSION_USER == Permission)
        {
            CORE_DEBUG(TRUE, "Task '%s' no need fill core stack.",
                GetContextTaskName(lpCoreContext));
            
            return STATE_SUCCESS;
        }
#else
        /* 如果 CPU 不支持全局内核栈，则内核任务的用户栈为NULL*/
        if (TASK_PERMISSION_CORE != GetContextPermission(lpCoreContext) &&
            TASK_PERMISSION_USER == Permission)
        {
            CORE_DEBUG(TRUE, "Task '%s' no need fill user stack.",
                GetContextTaskName(lpCoreContext));
            
            return STATE_SUCCESS;
        }
#endif
    }

    CORE_ERROR(TRUE, "No task stack buffer found, permission is %d.", Permission);

    return STATE_INVALID_STACK;
}

PUBLIC E_STATUS CORE_StackFree(LPVOID lpTaskContext, E_TASK_PERMISSION Permission)
{
    E_STATUS Result = STATE_SUCCESS;
    LPTASK_CONTEXT lpCoreContext = lpTaskContext;
    LPARCH_CONTEXT lpArchContext = GetContextArchParameter(lpCoreContext);

    if (Permission >= TASK_PERMISSION_MAX)
    {
        return STATE_INVALID_PARAMETER;
    }

    if (KOBJECT_STATE_FREE == GetObjectState(GetContextHeader(lpCoreContext)))
    {
        CORE_ERROR(TRUE, "Task state(%u) invalid.",
            GetObjectState(GetContextHeader(lpCoreContext)));
        return STATE_INVALID_STATE;
    }
    
    do
    {
        LPKSTACK_HEADER lpHeader = GetStackBuffer(GetArchSD(lpArchContext, Permission));

        /* 任务权限为用户级普通任务 */
        if (TASK_PERMISSION_USER == GetContextPermission(lpCoreContext))
        {
#if (CONFIG_CORE_STACK_SIZE == 0)
            /* 没有全局内核栈，则普通任务也没有用户栈 */
            if (TASK_PERMISSION_USER == Permission)
            {
                CORE_DEBUG(TRUE, "Task '%s' no need free user stack.",
                    GetContextTaskName(lpCoreContext));
                break;
            }            
#elif (CONFIG_ARCH_SUPPORT_KSTACK == TRUE)
            /* 释放的堆栈为内核栈 */
            if (TASK_PERMISSION_CORE == Permission)
            {
                /* 普通任务释放内核栈，并且CPU支持全局内核栈 */
                /* 则不会分配内核栈，直接使用全局内核栈，所以 */
                /* 不需要释放也不能释放 */
                CORE_DEBUG(TRUE, "Task '%s' no need free core stack.",
                    GetContextTaskName(lpCoreContext));
                break;

            }
#else
#endif
        }
        else /* 内核任务 */
        {
            if (TASK_PERMISSION_USER == Permission)
            {
                /* 内核任务不会分配用户栈，所以直接返回成功 */
                CORE_DEBUG(TRUE, "Task '%s' no need free user stack",
                    GetContextTaskName(lpCoreContext));
                break;
            }
        }

        Result = STATE_INVALID_OBJECT;
        /*释放场景:*/
        /*1.普通任务释放用户栈 */
        /*2.普通任务释放内核栈，并且CPU不支持全局内核栈 */
        if (NULL != lpHeader)
        {
            Result = CORE_PageFree(lpHeader);
        }
 
    } while(0);

    SetStackBuffer(GetArchSD(lpArchContext, Permission), NULL);
    SetStackPosition(GetArchSD(lpArchContext, Permission), NULL);
    SetStackCapacity(GetArchSD(lpArchContext, Permission), 0);

    return Result;
}

STATIC E_STATUS SVC_MallocStack(LPVOID lpPrivate, LPVOID lpParam)
{
    LPLPC_REQUEST_PACKET lpPacket = lpParam;
    
    return CORE_StackMalloc(Handle2TaskContext(lpPacket->u0.hParam),
               lpPacket->u1.pParam, (E_TASK_PERMISSION) lpPacket->u2.dParam);
}

STATIC E_STATUS SVC_InitStack(LPVOID lpPrivate, LPVOID lpParam)
{
    LPLPC_REQUEST_PACKET lpPacket = lpParam;

    return CORE_StackInit(Handle2TaskContext(lpPacket->u0.hParam),
               lpPacket->u1.pParam, (E_TASK_PERMISSION)lpPacket->u2.dParam);
}

STATIC E_STATUS SVC_FreeStack(LPVOID lpPrivate, LPVOID lpParam)
{
    LPLPC_REQUEST_PACKET lpPacket = lpParam;
    return CORE_StackFree(Handle2TaskContext(lpPacket->u0.hParam),
              (E_TASK_PERMISSION)lpPacket->u1.dParam);
}


STATIC CONST REQUEST_HANDLER fnHandlers[] = {
    SVC_MallocStack,                    /* 01. LPC_STK_STACK_MALLOC */
    SVC_InitStack,                      /* 02. LPC_STK_STACK_INIT */
    SVC_FreeStack,                      /* 03. LPC_STK_STACK_FREE */
};

DEFINE_LPC_SERVICE(LPCService, STK_MAGIC, SIZEOF_ARRAY(fnHandlers), NULL, fnHandlers);

PUBLIC E_STATUS initCoreSystemTaskStackManager(VOID)
{
    LPC_INSTALL(&LPCService, "Task stack(STK) manager starting");
    
    return STATE_SUCCESS;
}
#endif
