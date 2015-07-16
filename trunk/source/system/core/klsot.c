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
                
typedef struct tagKLSOT_CLASS_DESCRIPTOR KLSOT_CLASS_DESCRIPTOR;
typedef struct tagKLSOT_CLASS_DESCRIPTOR * PKLSOT_CLASS_DESCRIPTOR;
typedef struct tagKLSOT_CLASS_DESCRIPTOR FAR * LPKLSOT_CLASS_DESCRIPTOR;


struct tagKLSOT_CLASS_DESCRIPTOR{
    KCLASS_HEADER               Header;
    FNCLASSMETHOD               fnClassMethods[KLSOT_CLASS_METHODS];
};


typedef struct tagKLSOT_OBJECT KLSOT_OBJECT;
typedef struct tagKLSOT_OBJECT * PKLSOT_OBJECT;
typedef struct tagKLSOT_OBJECT * LPKLSOT_OBJECT;

struct tagKLSOT_OBJECT{
    KOBJECT_HEADER              Header;
    DWORD                       Array[CONFIG_DEFAULT_SLOT_KEYS]; 
};

#define     GetTask4mLsotObject(lpHeader)                   ((LPTASK_CONTEXT)((lpHeader)->un.lsot.lpTask))
#define     SetTask2LsotObject(lpHeader, Task)              do { ((lpHeader)->un.lsot.lpTask) = (Task); } while(0)
#define     GetTotalKeys4mLsotObject(lpHeader)              ((lpHeader)->un.lsot.Total)
#define     SetTotalKeys2LsotObject(lpHeader, Value)        do { ((lpHeader)->un.lsot.Total) = (Value); } while(0)
#define     GetKeyValue4mLsotObject(lpHeader, Id)           (((LPKLSOT_OBJECT)(lpHeader))->Array[Id])
#define     SetKeyValue2LsotObject(lpHeader, Id, Value)     do { (((LPKLSOT_OBJECT)(lpHeader))->Array[Id]) = (Value); } while(0)
#define     GetFreeBitmap4mLsotObject(lpHeader)             ((lpHeader)->un.lsot.Marks)
#define     CheckKeyIsFree(lpHeader, ID)                    (0 != (((lpHeader)->un.lsot.Marks) & (1UL << (ID))))
#define     SetFreeBitmap2LsotObject(lpHeader, V)           do { ((lpHeader)->un.lsot.Marks) = (V); } while(0);
#define     PutKey2LsotObject(lpHeader, Shift)              ((lpHeader)->un.lsot.Marks |= (1UL<<(Shift)))
#define     GetKey4mLsotObject(lpHeader, Shift)             ((lpHeader)->un.lsot.Marks ^= (1UL<<(Shift)))

STATIC SIZE_T KLSOT_SizeofObject(LPKCLASS_DESCRIPTOR lpClass, LPVOID lpParam)
{
    if (NULL == lpParam || SIZE_OF_KLSOT_FOR_PARAM(lpParam) > 128)
    {
        CORE_INFOR(TRUE, "This object size invalid.");
        return INVALID_SIZE;
    }

    return SIZE_OF_KLSOT_FOR_PARAM(lpParam);
}

STATIC E_STATUS KLSOT_MallocObject(LPKOBJECT_HEADER lpHeader, LPVOID lpParam)
{
    SetTotalKeys2LsotObject(lpHeader, GetKLPTotal(lpParam));
    SetFreeBitmap2LsotObject(lpHeader, GetBitsMaskValue(GetKLPTotal(lpParam)-1))
 
    CORE_INFOR(TRUE, "The mark value is 0x%08X, total is %d.",
            GetFreeBitmap4mLsotObject(lpHeader), GetTotalKeys4mLsotObject(lpHeader));

    return STATE_SUCCESS;
}

STATIC E_STATUS KLSOT_ActiveObject(LPKOBJECT_HEADER lpHeader, LPVOID lpParam)
{
    LPTASK_CONTEXT lpTaskContext;
    
    if (NULL == lpParam)
    {
        return STATE_INVALID_PARAMETER;
    }

    lpTaskContext = TASK_SELF_HANDLE == GetKLPhTask(lpParam)
                  ? CORE_GetCurrentTask()
                  : CORE_Handle2TaskContextCheck(GetKLPhTask(lpParam), TRUE);
    
    if (NULL == lpTaskContext)
    {
        return STATE_INVALID_TASK;
    }
    
    if (NULL != GetContextLsotObject(lpTaskContext))
    {
        return STATE_EXISTING;
    }

    SetTask2LsotObject(lpHeader, lpTaskContext);
    SetContextLsotObject(lpTaskContext, lpHeader);
    
    return STATE_SUCCESS;
}

STATIC E_STATUS KLSOT_TakeObject(LPKOBJECT_HEADER lpHeader, LPVOID lpParam)
{
    return STATE_SUCCESS;
}

STATIC E_STATUS KLSOT_FreeObject(LPKOBJECT_HEADER lpHeader)
{
    if (NULL == GetTask4mLsotObject(lpHeader))
    {
        return STATE_INVALID_OBJECT;
    }
    
    SetContextLsotObject(GetTask4mLsotObject(lpHeader), NULL);
    SetTask2LsotObject(lpHeader, NULL);
    
    return STATE_SUCCESS;
}

STATIC E_STATUS KLSOT_GetKey(LPKOBJECT_HEADER lpHeader, LPVOID lpParam)
{
    LSOT_KEY_T LsotKey;
    
    if (NULL == lpParam)
    {
        return STATE_INVALID_PARAMETER;
    }
    
    LsotKey = GetDwordLowestBit(GetFreeBitmap4mLsotObject(lpHeader));

    if (LsotKey >= GetTotalKeys4mLsotObject(lpHeader))
    {
        SetKLPKeyID(lpParam, TASK_LSOTKEY_INVALID);
        return STATE_OVER_RANGE;
    }

    GetKey4mLsotObject(lpHeader, LsotKey);
    SetKLPKeyID(lpParam, LsotKey);
    
    CORE_INFOR(TRUE, "The mark bits value is: 0x%X", GetFreeBitmap4mLsotObject(lpHeader));
    
    return STATE_SUCCESS;
}

STATIC E_STATUS KLSOT_PutKey(LPKOBJECT_HEADER lpHeader, LPVOID lpParam)
{
    if (NULL == lpParam)
    {
        return STATE_INVALID_PARAMETER;
    }
    
    if (GetKLPKeyID(lpParam) >= GetTotalKeys4mLsotObject(lpHeader))
    {
        return STATE_OVER_RANGE;
    }

    PutKey2LsotObject(lpHeader, GetKLPKeyID(lpParam));

    return STATE_SUCCESS;
}

STATIC E_STATUS KLSOT_GetValue(LPKOBJECT_HEADER lpHeader, LPVOID lpParam)
{
    if (NULL != lpParam)
    {
        DWORD KeyID = GetKLPKeyID(lpParam);

        if (KeyID >= GetTotalKeys4mLsotObject(lpHeader))
        {
            return STATE_OVER_RANGE;
        }
        
        if (CheckKeyIsFree(lpHeader, KeyID))
        {
            return STATE_NOT_EXIST;
        }
        
        SetKLPValue(lpParam, GetKeyValue4mLsotObject(lpHeader, KeyID));
        
        return STATE_SUCCESS;;
    }
    
    return STATE_INVALID_PARAMETER;
}


STATIC E_STATUS KLSOT_SetValue(LPKOBJECT_HEADER lpHeader, LPVOID lpParam)
{
    if (NULL != lpParam)
    {
        DWORD KeyID = GetKLPKeyID(lpParam);
        
        if (KeyID >= GetTotalKeys4mLsotObject(lpHeader))
        {
            return STATE_OVER_RANGE;
        }

        if (CheckKeyIsFree(lpHeader, KeyID))
        {
            return STATE_NOT_EXIST;
        }
        
        SetKeyValue2LsotObject(lpHeader, KeyID, GetKLPValue(lpParam));

        return STATE_SUCCESS;;
    }
    
    return STATE_INVALID_PARAMETER;
}

DEFINE_KCLASS(KLSOT_CLASS_DESCRIPTOR,
            KlsotClass,
            LST_MAGIC,
            KLSOT_CLASS_METHODS,
            KLSOT_SizeofObject,
            KLSOT_MallocObject,
            KLSOT_ActiveObject,
            KLSOT_TakeObject,
            KLSOT_FreeObject,
            KLSOT_GetKey,
            KLSOT_PutKey,
            KLSOT_GetValue,
            KLSOT_SetValue);

PUBLIC E_STATUS initCoreLocalStorageOfTaskManager(VOID)
{
    E_STATUS State;
    
    State = REGISTER_KCLASS(KlsotClass);

    CORE_ASSERT(STATE_SUCCESS == State, SYSTEM_CALL_OOPS(),
        "Register task local storage of task class failed, result %d !", State);
    
    return STATE_SUCCESS;
}

