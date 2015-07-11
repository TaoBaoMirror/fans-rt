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
#include "kcore.h"
#include "kdebug.h"
#include "kobject.h"

#define     GetLsotTotal(lpHeader)                      ((lpHeader)->un.s.Total)
#define     SetLsotTotal(lpHeader, Value)               do { ((lpHeader)->un.s.Total) = (Value) } while(0)
#define     GetLsotKeyValue(lpHeader, Id)               ((lpHeader)->un.s.dwArray[Id])
#define     SetLsotKeyValue(lpHeader, Id)               do { ((lpHeader)->un.s.dwArray[Id]) = (Value) } while(0)
#define     GetLsotMarkBits(lpHeader)                   ((lpHeader)->un.Marks)
#define     SetLsotMarkBit(lpHeader, Shift)             ((lpHeader)->un.Marks | (1UL<<(Shift)))

typedef struct tagKLSOT_OBJECT KLSOT_OBJECT;
typedef struct tagKLSOT_OBJECT * PKLSOT_OBJECT;
typedef struct tagKLSOT_OBJECT * LPKLSOT_OBJECT;

struct tagKLSOT_OBJECT{
    KOBJECT_HEADER              Base;
    DWORD                       Array[1]; 
};


STATIC E_STATUS KLSOT_DummyOperation(LPKOBJECT_HEADER lpHeader, LPVOID lpParam)
{
    return STATE_SUCCESS;
}

STATIC SIZE_T KLSOT_SizeofObject(LPKCLASS_DESCRIPTOR lpClass, LPVOID lpParam)
{
    CORE_INFOR(TRUE, "This object size is %u bytes.", sizeof(KOBJECT_HEADER) - sizeof(DWORD) + ((DWORD) lpParam) * sizeof(DWORD));
    return sizeof(KOBJECT_HEADER) - sizeof(DWORD) + ((DWORD) lpParam) * sizeof(DWORD);
}

STATIC E_STATUS KLSOT_MallocObject(LPKOBJECT_HEADER lpHeader, LPVOID lpParam)
{
    return STATE_NOT_SUPPORT;
}

STATIC E_STATUS KLSOT_WaitObject(LPKOBJECT_HEADER lpHeader, LONG WaitTime)
{
    return STATE_NOT_SUPPORT;
}

STATIC E_STATUS KLSOT_FreeObject(LPKOBJECT_HEADER lpHeader)
{
    return STATE_NOT_SUPPORT;
}

#define     KLSOT_TakeObject        KLSOT_DummyOperation
#define     KLSOT_PostObject        KLSOT_DummyOperation
#define     KLSOT_ActiveObject      KLSOT_DummyOperation
#define     KLSOT_DetachObject      KLSOT_DummyOperation
#define     KLSOT_ResetObject       KLSOT_DummyOperation

DEFINE_CLASS(LST_MAGIC, KlsotClass,
            KLSOT_SizeofObject,
            KLSOT_MallocObject,
            KLSOT_ActiveObject,
            KLSOT_TakeObject,
            KLSOT_WaitObject,
            KLSOT_PostObject,
            KLSOT_ResetObject,
            KLSOT_DetachObject,
            KLSOT_FreeObject);



#if 0
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
#endif

STATIC E_STATUS SVC_Create(LPVOID lpPrivate, LPVOID lpParam)
{
    LPKOBJECT_HEADER lpHeader;
    LPLPC_REQUEST_PACKET lpPacket = lpParam;
    LPTASK_CONTEXT lpTaskContext = Handle2TaskContext(lpPacket->u0.hParam);
    
    if (NULL == lpTaskContext)
    {
        return STATE_INVALID_OBJECT;
    }
    
    if (NULL != GetContextLsotObject(lpTaskContext))
    {
        return STATE_EXISTING;
    }
    
    lpHeader = CORE_MallocNoNameObject(LST_MAGIC, (LPVOID) REQdParam(lpPacket,u1));
    
    if (NULL == lpHeader)
    {
        return CORE_GetError();
    }
    
    SetContextLsotObject(lpTaskContext, lpHeader);
    
    return STATE_SUCCESS;
}


STATIC E_STATUS SVC_Delete(LPVOID lpPrivate, LPVOID lpParam)
{
    return STATE_SUCCESS;
}

STATIC E_STATUS SVC_GetKey(LPVOID lpPrivate, LPVOID lpParam)
{
    return STATE_SUCCESS;
}

STATIC E_STATUS SVC_PutKey(LPVOID lpPrivate, LPVOID lpParam)
{
    return STATE_SUCCESS;
}

STATIC E_STATUS SVC_GetValue(LPVOID lpPrivate, LPVOID lpParam)
{
    return STATE_SUCCESS;
}

STATIC E_STATUS SVC_SetValue(LPVOID lpPrivate, LPVOID lpParam)
{
    return STATE_SUCCESS;
}


STATIC CONST REQUEST_HANDLER fnHandlers[] = {
    SVC_Create,
    SVC_Delete,
    SVC_GetKey,
    SVC_PutKey,
    SVC_GetValue,
    SVC_SetValue,
};

DEFINE_LPC_SERVICE(LPCService, LST_MAGIC, SIZEOF_ARRAY(fnHandlers), NULL, fnHandlers);

PUBLIC E_STATUS initCoreLocalStorageOfTaskManager(VOID)
{
    E_STATUS State;
    
    State = CORE_RegisterClass(&KlsotClass);

    CORE_ASSERT(STATE_SUCCESS == State, SYSTEM_CALL_OOPS(),
        "Register task local storage of task class failed, result %d !", State);
    
    LPC_INSTALL(&LPCService, "Local storage of task(LSOT) service starting");
    
    return STATE_SUCCESS;
}

