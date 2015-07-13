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
#include "klsot.h"
#include "kdebug.h"
#include "kobject.h"


#define     LSOT_OBJECT_TOTAL_KEYS              ((3) + (32/4) + (64/4))

#define     SIZE_OF_KLSOT_FOR_PARAM(lpParam)                                                                          \
                (sizeof(KOBJECT_HEADER) + GetKLPTotal(lpParam) * sizeof(DWORD))

typedef struct tagKLSOT_OBJECT KLSOT_OBJECT;
typedef struct tagKLSOT_OBJECT * PKLSOT_OBJECT;
typedef struct tagKLSOT_OBJECT * LPKLSOT_OBJECT;

struct tagKLSOT_OBJECT{
    KOBJECT_HEADER              Header;
    DWORD                       Array[CONFIG_DEFAULT_SLOT_KEYS]; 
};

#define     GetLsotTask(lpHeader)                       ((LPTASK_CONTEXT)((lpHeader)->un.lsot.lpTask))
#define     SetLsotTask(lpHeader, Task)                  do { ((lpHeader)->un.lsot.lpTask) = (Task); } while(0)
#define     GetLsotTotal(lpHeader)                      ((lpHeader)->un.lsot.Total)
#define     SetLsotTotal(lpHeader, Value)               do { ((lpHeader)->un.lsot.Total) = (Value); } while(0)
#define     GetLsotKeyValue(lpHeader, Id)               (((LPKLSOT_OBJECT)(lpHeader))->Array[Id])
#define     SetLsotKeyValue(lpHeader, Id, Value)        do { (((LPKLSOT_OBJECT)(lpHeader))->Array[Id]) = (Value); } while(0)
#define     GetLsotMarkBits(lpHeader)                   ((lpHeader)->un.lsot.Marks)
#define     SetLsotMarkBit(lpHeader, Shift)             ((lpHeader)->un.lsot.Marks | (1UL<<(Shift)))

STATIC SIZE_T KLSOT_SizeofObject(LPKCLASS_DESCRIPTOR lpClass, LPVOID lpParam)
{
    CORE_INFOR(TRUE, "This object size is %u bytes.", SIZE_OF_KLSOT_FOR_PARAM(lpParam));

    if (SIZE_OF_KLSOT_FOR_PARAM(lpParam) > 128)
    {
        return INVALID_SIZE;
    }

    return SIZE_OF_KLSOT_FOR_PARAM(lpParam);
}

STATIC E_STATUS KLSOT_MallocObject(LPKOBJECT_HEADER lpHeader, LPVOID lpParam)
{
    SetLsotTotal(lpHeader, GetKLPTotal(lpParam));

    return STATE_SUCCESS;
}

STATIC E_STATUS KLSOT_ActiveObject(LPKOBJECT_HEADER lpHeader, LPVOID lpParam)
{
    LPTASK_CONTEXT lpTaskContext;

    lpTaskContext = TASK_SELF_HANDLE == GetKLPhTask(lpParam)
                  ? CORE_GetCurrentTask()
                  : CORE_Handle2TaskContextCheck(GetKLPhTask(lpParam), TRUE);
    
    if (NULL == lpTaskContext)
    {
        return STATE_INVALID_TASK;
    }

    SetLsotTask(lpHeader, lpTaskContext);
    SetContextLsotObject(lpTaskContext, lpHeader);
    
    return STATE_SUCCESS;
}

STATIC E_STATUS KLSOT_TakeObject(LPKOBJECT_HEADER lpHeader, LPVOID lpParam)
{
    return STATE_SUCCESS;
}

STATIC E_STATUS KLSOT_FreeObject(LPKOBJECT_HEADER lpHeader)
{
    SetContextLsotObject(GetLsotTask(lpHeader), NULL);
    SetLsotTask(lpHeader, NULL);
    
    return STATE_SUCCESS;
}

typedef struct tagKLSOT_CLASS_DESCRIPTOR KLSOT_CLASS_DESCRIPTOR;
typedef struct tagKLSOT_CLASS_DESCRIPTOR * PKLSOT_CLASS_DESCRIPTOR;
typedef struct tagKLSOT_CLASS_DESCRIPTOR FAR * LPKLSOT_CLASS_DESCRIPTOR;


struct tagKLSOT_CLASS_DESCRIPTOR{
    KCLASS_HEADER               Header;
    FNCLASSMETHOD               fnClassMethods[KLSOT_CLASS_METHODS];
};

DEFINE_KCLASS(KLSOT_CLASS_DESCRIPTOR,
            KlsotClass,
            LST_MAGIC,
            KLSOT_CLASS_METHODS,
            KLSOT_SizeofObject,
            KLSOT_MallocObject,
            KLSOT_ActiveObject,
            KLSOT_TakeObject,
            KLSOT_FreeObject,
            NULL);


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
    
    return GetSmltKeyValue(lpCurrentTask, (SMLT_KEY_T)lpPacket->u0.dParam, (DWORD_PTR)&lpPacket->u1.dParam);
}

STATIC E_STATUS SVC_SetSmltValue(LPVOID lpPrivate, LPVOID lpParam)
{
    LPLPC_REQUEST_PACKET lpPacket = lpParam;
    LPTASK_CONTEXT lpCurrentTask = GetCurrentTaskContext();
    
    CORE_ASSERT(lpCurrentTask, SYSTEM_CALL_OOPS();, "BUG: Invalid current task context.");
    
    return SetSmltKeyValue(lpCurrentTask, (SMLT_KEY_T)lpPacket->u0.dParam, lpPacket->u1.dParam);
}
#endif

PUBLIC E_STATUS initCoreLocalStorageOfTaskManager(VOID)
{
    E_STATUS State;
    
    State = REGISTER_KCLASS(KlsotClass);

    CORE_ASSERT(STATE_SUCCESS == State, SYSTEM_CALL_OOPS(),
        "Register task local storage of task class failed, result %d !", State);
    
    return STATE_SUCCESS;
}

