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

#define     GetStackObjectHeader(lpHeader)          (&(lpHeader)->Header)
#define     SetStackObjectMagic(lpHeader)           SetObjectMagic(GetContextHeader(lpHeader), STK_MAGIC, STK_MAGIC)
#define     GetStackObjectHandle(lpHeader)          GetObjectHandle(GetContextHeader(lpHeader))


EXPORT VOID CORE_SetTaskStackPosition(LPVOID StackPosition)
{
    LPTASK_CONTEXT lpCurrentTask = CORE_GetCurrentTask();
    LPARCH_CONTEXT lpArchContext = GetContextArchParameter(lpCurrentTask);

    SetStackPosition(GetArchUserSD(lpArchContext), StackPosition);
}

EXPORT LPVOID CORE_GetTaskStackPosition(VOID)
{
    LPTASK_CONTEXT lpCurrentTask = CORE_GetCurrentTask();
    LPARCH_CONTEXT lpArchContext = GetContextArchParameter(lpCurrentTask);

    return GetStackPosition(GetArchUserSD(lpArchContext));
}

EXPORT VOID CORE_SetCoreStackPosition(LPVOID StackPosition)
{
    LPTASK_CONTEXT lpCurrentTask = CORE_GetCurrentTask();
    LPARCH_CONTEXT lpArchContext = GetContextArchParameter(lpCurrentTask);

    SetStackPosition(GetArchCoreSD(lpArchContext), StackPosition);
}

EXPORT LPVOID CORE_GetCoreStackPosition(VOID)
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
#define     OBJ_DetachStack                         OBJ_DummyOperation

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

DEFINE_CLASS(SUU_MAGIC,
            UserStackForUserTask,
            CONFIG_DEFAULT_STACK_SIZE,
            OBJ_MallocStack,
            OBJ_ActiveStack,
            OBJ_TakeStack,
            OBJ_WaitObject,
            OBJ_PostStack,
            OBJ_ResetStack,
            OBJ_DetachStack,
            OBJ_FreeStack);

#if (CONFIG_ARCH_SUPPORT_KSTACK == FALSE)
DEFINE_CLASS(SCU_MAGIC,
            CoreStackForUserTask,
            CONFIG_CORE_STACK_SIZE,
            OBJ_MallocStack,
            OBJ_ActiveStack,
            OBJ_TakeStack,
            OBJ_WaitObject,
            OBJ_PostStack,
            OBJ_ResetStack,
            OBJ_DetachStack,
            OBJ_FreeStack);
#endif

DEFINE_CLASS(SCC_MAGIC,
            CoreStackForCoreTask,
            CONFIG_KTASK_STACK_SIZE,
            OBJ_MallocStack,
            OBJ_ActiveStack,
            OBJ_TakeStack,
            OBJ_WaitObject,
            OBJ_PostStack,
            OBJ_ResetStack,
            OBJ_DetachStack,
            OBJ_FreeStack);

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
    /* ����Ȩ�޺Ͷ�ջȨ�޲�һ�µ����: ��ͨ���񴴽��ں˶�ջ���ں����񴴽���ͨ��ջ */
    if (GetContextPermission(lpCoreContext) != Permission)
    {
        /* ����Ȩ�޺Ͷ�ջȨ�޲�һ�£���ջȨ��ΪUSER��������Ȩ��Ϊ CORE: �ں�������Ҫ�����û���ջ */
        if (TASK_PERMISSION_USER == Permission)
        {
            SetStackCapacity(GetArchSD(lpArchContext, Permission), 0);
            SetStackBuffer(GetArchSD(lpArchContext, Permission), NULL);
            CORE_INFOR(TRUE, "Task '%s' no need malloc core stack.",
                GetContextTaskName(lpCoreContext), Permission, lpHeader);
            return STATE_SUCCESS;
        }
        else /* ��ͨ���񴴽��ں˶�ջ */
        {
#if (CONFIG_ARCH_SUPPORT_KSTACK == TRUE)
            /* CPU ֧��ȫ���ں˶�ջ */
            lpHeader = CORE_GetCoreStackBuffer();
            SetStackCapacity(GetArchSD(lpArchContext, Permission), CORE_GetCoreStackLength());
            CORE_INFOR(TRUE, "Malloc stack(%d) buffer 0x%P for task '%s' successfully.",
                Permission, lpHeader, GetContextTaskName(lpCoreContext));
#else
            /* CPU ��֧��ȫ���ں˶�ջ��ÿ������һ���������ں˶�ջ */
            lpHeader = (LPVOID)CORE_MallocNoNameObject(SCU_MAGIC, lpTaskParam);
            SetStackCapacity(GetArchSD(lpArchContext, Permission), CONFIG_CORE_STACK_SIZE);
#endif
        }
    }
    else if (0 == CORE_GetGlobalTaskCount())
    {
        /* Boot ���񴴽���ͨ��ջ(BOOT����Ϊ��ͨ����)*/
        lpHeader = CORE_GetBootStackBuffer();
        SetStackCapacity(GetArchSD(lpArchContext, Permission), CORE_GetBootStackLength());
        CORE_INFOR(TRUE, "Malloc stack(%d) buffer 0x%P for task '%s' successfully.",
            Permission, lpHeader, GetContextTaskName(lpCoreContext));
    }
    else if (TASK_PRIORITY_IDLE == GetContextThisPriority(lpCoreContext))
    {
        /* Idle ���񴴽��ں˶�ջ(Idle����Ϊ�ں�����)*/
        lpHeader = CORE_GetIdleStackBuffer(TASK_BOOTSTARTUP_CPUID);
        SetStackCapacity(GetArchSD(lpArchContext, Permission), CORE_GetIdleStackLength());
    }
    else
    {
        /* ����Ȩ�޺Ͷ�ջȨ��һ�µ�������ں����񴴽��ں˶�ջ����ͨ���񴴽���ͨ��ջ */
        if (TASK_PERMISSION_USER == Permission)
        {
            if (CONFIG_DEFAULT_STACK_SIZE != lpTaskParam->StackSize)
            {
                CORE_ERROR(TRUE, "Invalid user stack size %d.", lpTaskParam->StackSize);
                return STATE_INVALID_SIZE;
            }
            /* �� SUU �з��� */
            lpHeader = (LPVOID)CORE_MallocNoNameObject(SUU_MAGIC, lpTaskParam);
            SetStackCapacity(GetArchSD(lpArchContext, Permission), CONFIG_DEFAULT_STACK_SIZE);
        }
        else
        {
            if (CONFIG_KTASK_STACK_SIZE != lpTaskParam->StackSize)
            {
                CORE_ERROR(TRUE, "Invalid core task stack size %d.", lpTaskParam->StackSize);
                return STATE_INVALID_SIZE;
            }
            
            lpHeader = (LPVOID)CORE_MallocNoNameObject(SCC_MAGIC, lpTaskParam);
            SetStackCapacity(GetArchSD(lpArchContext, Permission), CONFIG_KTASK_STACK_SIZE);
        }
    }
    
    SetStackBuffer(GetArchSD(lpArchContext, Permission), lpHeader);

    if (NULL == lpHeader)
    {
        CORE_ERROR(TRUE, "Malloc stack for task '%s' failed.", GetContextTaskName(lpCoreContext));
        return CORE_GetError();
    }
    
    CORE_INFOR(TRUE, "Task '%s' malloc stack(%d) buffer 0x%P.",
        GetContextTaskName(lpCoreContext), Permission, lpHeader);
    
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
        LPBYTE lpStackBuffer = (LPVOID) lpHeader;
        LPBYTE lpStackPosition = (lpStackBuffer + StackCapacity);
        
        if (0 != CORE_GetGlobalTaskCount() && GetContextPermission(lpCoreContext) == Permission)
        {
            lpStackPosition = CORE_FillStack(lpStackPosition, lpTaskParam->fnTaskMain,
                              lpTaskParam->lpArgument, lpTaskContext,
                              GetContextHandle(lpCoreContext), Permission);
        }

        SetStackHandle(GetArchSD(lpArchContext, Permission), GetStackObjectHandle(lpHeader));
        SetStackPosition(GetArchSD(lpArchContext, Permission), lpStackPosition);
        SetStackObjectMagic(lpHeader);
        
        CORE_INFOR(TRUE, "Task '%s' fill stack(%d) buffer 0x%p buttom 0x%p position 0x%p.",
            GetContextTaskName(lpCoreContext), Permission, lpHeader,
            lpStackBuffer + StackCapacity, lpStackPosition);
        
        return STATE_SUCCESS;
    }
    else
    {
        if (TASK_PERMISSION_CORE == GetContextPermission(lpCoreContext) && 
            TASK_PERMISSION_USER == Permission)
        {
            CORE_DEBUG(TRUE, "Task '%s' no need fill user stack.", GetContextTaskName(lpCoreContext));
            
            return STATE_SUCCESS;
        }
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

        /* ����Ȩ��Ϊ�û�����ͨ���� */
        if (TASK_PERMISSION_USER == GetContextPermission(lpCoreContext))
        {
#if (CONFIG_ARCH_SUPPORT_KSTACK == TRUE)
            /* �ͷŵĶ�ջΪ�ں�ջ */
            if (TASK_PERMISSION_CORE == Permission)
            {
                    /* ��ͨ�����ͷ��ں�ջ������CPU֧��ȫ���ں�ջ */
                    /* �򲻻�����ں�ջ��ֱ��ʹ��ȫ���ں�ջ������ */
                    /* ����Ҫ�ͷ�Ҳ�����ͷ� */
                    break;

            }
#endif
        }
        else /* �ں����� */
        {
            if (TASK_PERMISSION_USER == Permission)
            {
                /* �ں����񲻻�����û�ջ������ֱ�ӷ��سɹ� */
                break;
            }
        }

        Result = STATE_INVALID_OBJECT;
        /*�ͷų���:*/
        /*1.��ͨ�����ͷ��û�ջ */
        /*2.��ͨ�����ͷ��ں�ջ������CPU��֧��ȫ���ں�ջ */
        if (NULL != lpHeader)
        {
            SetObjectHandle(GetStackObjectHeader(lpHeader),
                    GetStackHandle(GetArchSD(lpArchContext, Permission)));
            SetObjectState(GetStackObjectHeader(lpHeader), KOBJECT_STATE_ACTIVE);
            
            initSelfHashNode(GetStackObjectHeader(lpHeader));

            Result = CORE_FreeObject(GetStackObjectHeader(lpHeader));
        }
 
    } while(0);

    SetStackBuffer(GetArchSD(lpArchContext, Permission), NULL);
    SetStackHandle(GetArchSD(lpArchContext, Permission), INVALID_HANDLE_VALUE);
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
    E_STATUS State;
    /* ע�� �û�ջ ������ */
    State = CORE_RegisterClass(&UserStackForUserTask);

    CORE_ASSERT(STATE_SUCCESS == State, SYSTEM_CALL_OOPS(),
        "Register stack class failed, result %d !", State);
#if (CONFIG_ARCH_SUPPORT_KSTACK == FALSE)
    /* ע�� �û�������ں�ջ ������ */
    State = CORE_RegisterClass(&CoreStackForUserTask);

    CORE_ASSERT(STATE_SUCCESS == State, SYSTEM_CALL_OOPS(),
        "Register stack class failed, result %d !", State);
#endif
    /* ע�� �ں�����ջ ������ */
    State = CORE_RegisterClass(&CoreStackForCoreTask);

    CORE_ASSERT(STATE_SUCCESS == State, SYSTEM_CALL_OOPS(),
        "Register stack class failed, result %d !", State);
    
    LPC_INSTALL(&LPCService, "Task stack(STK) manager starting");
    
    return STATE_SUCCESS;
}

