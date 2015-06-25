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
#include "kdebug.h"
#include "kboard.h"
#include "kobject.h"

STATIC CHAR g_SystemCoreTaskStack[CONFIG_CORE_STACK_SIZE];

PUBLIC LPVOID CORE_GetCoreStackButtom(VOID)
{
    return g_SystemCoreTaskStack + CONFIG_CORE_STACK_SIZE;
}

PUBLIC LPVOID CORE_GetTaskCoreStackPosition(LPVOID lpTaskContext, LPVOID StackPosition)
{
    return g_SystemCoreTaskStack + CONFIG_CORE_STACK_SIZE;
}


#define     STM_MallocStack                         STM_DummyOperation
#define     STM_ActiveStack                         STM_DummyOperation
#define     STM_TakeStack                           STM_DummyOperation
#define     STM_PostStack                           STM_DummyOperation
#define     STM_ResetStack                          STM_DummyOperation

STATIC E_STATUS STM_DummyOperation(LPKOBJECT_HEADER lpHeader, LPVOID lpParam)
{
    return STATE_SUCCESS;
}

STATIC E_STATUS STM_WaitObject(LPKOBJECT_HEADER lpHeader, LONG WaitTime)
{
    return STATE_NOT_SUPPORT;
}

STATIC E_STATUS STM_FreeStack(LPKOBJECT_HEADER lpHeader)
{
    return STATE_SUCCESS;
}

DEFINE_CLASS(STK_MAGIC, StackClass, CONFIG_DEFAULT_STACK_SIZE,
            STM_MallocStack,
            STM_ActiveStack,
            STM_TakeStack,
            STM_WaitObject,
            STM_PostStack,
            STM_ResetStack,
            STM_FreeStack);

typedef struct tagKSTACK_HEADER KSTACK_HEADER;
typedef struct tagKSTACK_HEADER * PKSTACK_HEADER;
typedef struct tagKSTACK_HEADER FAR * LPKSTACK_HEADER;

struct tagKSTACK_HEADER{
    KOBJECT_HEADER          Header;
    LPSTR                   Buffer[CONFIG_DEFAULT_STACK_SIZE - sizeof(KOBJECT_HEADER)];
};

#define     SetStackMagic(lpHeader)         SetObjectMagic(GetContextHeader(lpHeader), STK_MAGIC, STK_MAGIC)
#define     GetStackHandle(lpHeader)        GetObjectHandle(GetContextHeader(lpHeader))


PUBLIC E_STATUS CORE_CreateStackForTask(LPVOID lpTaskContext, LPVOID lpParam, BOOL IsCore)
{
    LPKSTACK_HEADER lpHeader = NULL;
    LPTASK_CREATE_PARAM lpTaskParam = lpParam;
    LPTASK_CONTEXT lpCoreContext = lpTaskContext;
    LPARCH_CONTEXT lpArchContext = GetContextArchParameter(lpCoreContext);

    if (NULL == lpTaskParam)
    {
        return STATE_INVALID_PARAMETER;
    }
    
    if (IsCore != TRUE)
    {
        return STATE_SUCCESS;
    }
    
    if (0 == CORE_GetGlobalTaskCount())
    {
        lpHeader = CORE_GetBootStackBuffer();
        SetArchContextStackCapacity(lpArchContext, CORE_GetBootStackCapacity());
    }
    else if (TASK_PRIORITY_IDLE == GetContextThisPriority(lpCoreContext))
    {
        lpHeader = CORE_GetIdleStackBuffer(TASK_BOOTSTARTUP_CPUID);
        SetArchContextStackCapacity(lpArchContext, CORE_GetIdleStackCapacity());
    }
    else
    {
        if (CONFIG_DEFAULT_STACK_SIZE != lpTaskParam->StackSize)
        {
            CORE_ERROR(TRUE, "Invalid stack size %d.", lpTaskParam->StackSize);
            return STATE_INVALID_SIZE;
        }

        lpHeader = (LPVOID)CORE_MallocNoNameObject(STK_MAGIC, lpTaskParam);
        SetArchContextStackCapacity(lpArchContext, CONFIG_DEFAULT_STACK_SIZE);
    }
    
    SetArchContextStackBuffer(lpArchContext, lpHeader);

    if (NULL == lpHeader)
    {
        CORE_ERROR(TRUE, "Malloc stack for task '%s' failed.", GetContextTaskName(lpCoreContext));
        return CORE_GetError();
    }
    
    return STATE_SUCCESS;
}

PUBLIC E_STATUS CORE_FillTaskStack(LPVOID lpTaskContext, LPVOID lpParam, BOOL IsCore)
{
    LPTASK_CREATE_PARAM lpTaskParam = lpParam;
    LPTASK_CONTEXT lpCoreContext = lpTaskContext;
    LPARCH_CONTEXT lpArchContext = GetContextArchParameter(lpCoreContext);
    LPKSTACK_HEADER lpHeader = GetArchContextStackBuffer(lpArchContext);
    
    if (NULL == lpTaskParam)
    {
        return STATE_INVALID_PARAMETER;
    }
    
    if (IsCore != TRUE)
    {
        return STATE_SUCCESS;
    }
    
    if (0 != CORE_GetGlobalTaskCount())
    {
        DWORD StackCapacity = GetArchContextStackCapacity(lpArchContext);
        LPBYTE lpStackBuffer = (LPVOID) lpHeader;
        LPBYTE lpStackPosition = (lpStackBuffer + StackCapacity);
        
        lpStackPosition = CORE_FillStack(lpStackPosition, lpTaskParam->fnTaskMain,
                          lpTaskParam->lpArgument, GetContextHandle(lpCoreContext));

        SetArchContextStackHandle(lpArchContext, GetStackHandle(lpHeader));
        SetArchContextStackPosition(lpArchContext, lpStackPosition);
        SetStackMagic(lpHeader);
    }
    
    return STATE_SUCCESS;
}

PUBLIC E_STATUS CORE_StackFree(LPVOID lpTaskContext, BOOL IsCore)
{
    return STATE_SUCCESS;
}

PUBLIC E_STATUS initCoreSystemTaskStackManagerForBoard(VOID)
{
    E_STATUS State;
    /* ◊¢≤· stack ∂‘œÛ¿‡ */
    State = CORE_RegisterClass(&StackClass);

    CORE_ASSERT(STATE_SUCCESS == State, SYSTEM_CALL_OOPS(),
        "Register stack class failed, result = %d !", State);
    
    return STATE_SUCCESS;
}

