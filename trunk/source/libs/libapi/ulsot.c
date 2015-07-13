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
 *    2015-07-12     JiangYong       new file
 */
#include <string.h>
#include <stdio.h>
#include <fauser.h>
#include <fadefs.h>
#include <faerror.h>
#include <fatypes.h>

#include "libcal.h"


PUBLIC RO_CODE E_STATUS uCreateLsot(HANDLE hTask, DWORD Total)
{
    HANDLE handle;
    E_STATUS Result;
    
    if (INVALID_HANDLE_VALUE == hTask)
    {
        return STATE_INVALID_TASK;
    }

    handle = caMallocLsotObject(Total);
    
    if (INVALID_HANDLE_VALUE == handle)
    {
        return STATE_INVALID_OBJECT;
    }
    
    Result = caActiveLsotObject(handle, hTask);
    
    if (STATE_SUCCESS != Result)
    {
        caCloseLsotObject(handle);
    }
    
    return Result;
}

EXPORT RO_CODE E_STATUS uCloseLsotObject(HANDLE hTask)
{
    HANDLE handle = caGetLsotHandle(hTask);

    if (INVALID_HANDLE_VALUE == handle)
    {
        return STATE_INVALID_OBJECT;
    }

    return caCloseLsotObject(handle);
}

FANSAPI RO_CODE LSOT_KEY_T GetLsotKey(VOID)
{
    HANDLE handle = caGetLsotHandle(TASK_SELF_HANDLE);
    
    if (INVALID_HANDLE_VALUE == handle)
    {
        if (STATE_SUCCESS != uCreateLsot(TASK_SELF_HANDLE, CONFIG_DEFAULT_SLOT_KEYS))
        {
            return TASK_LSOTKEY_INVALID;
        }
        
        handle = caGetLsotHandle(TASK_SELF_HANDLE);
        
        if (INVALID_HANDLE_VALUE == handle)
        {
            return TASK_LSOTKEY_INVALID;
        }
    }

    return caGetLsotKey(handle);
}


FANSAPI RO_CODE E_STATUS PutLsotKey(LSOT_KEY_T LsotKey)
{
    HANDLE handle = caGetLsotHandle(TASK_SELF_HANDLE);
    
    if (INVALID_HANDLE_VALUE == handle)
    {
        return STATE_INVALID_OBJECT;
    }

    return caPutLsotKey(handle, LsotKey);
}

FANSAPI RO_CODE E_STATUS GetLsotValue(LSOT_KEY_T LsotKey, DWORD_PTR lpValue)
{
    HANDLE handle = caGetLsotHandle(TASK_SELF_HANDLE);

    if (INVALID_HANDLE_VALUE == handle)
    {
        return STATE_INVALID_OBJECT;
    }
    
    return caGetLsotValue(handle, LsotKey, lpValue);
}


FANSAPI RO_CODE E_STATUS SetLsotValue(LSOT_KEY_T LsotKey, DWORD Value)
{
    HANDLE handle = caGetLsotHandle(TASK_SELF_HANDLE);

    if (INVALID_HANDLE_VALUE == handle)
    {
        return STATE_INVALID_OBJECT;
    }
    
    return caSetLsotValue(handle, LsotKey, Value);
}

