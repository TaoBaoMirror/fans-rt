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

/**
 * Create local store of task object.
 * @param The handle of the task.
 * @param The total keys of this object.
 * @return The state of current operating result.
 *
 * date           author          notes
 * 2015-07-16     JiangYong       new function
 */
PUBLIC RO_USER_CODE E_STATUS uCreateLsot(HANDLE hTask, DWORD Total)
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

/**
 * Close local store of task.
 * @param The handle of the task.
 * @return The state of current operating result.
 *
 * date           author          notes
 * 2015-07-16     JiangYong       new function
 */
EXPORT RO_USER_CODE E_STATUS uCloseLsotObject(HANDLE hTask)
{
    HANDLE handle = caGetLsotHandle(hTask);

    if (INVALID_HANDLE_VALUE == handle)
    {
        return STATE_INVALID_OBJECT;
    }

    return caCloseLsotObject(handle);
}

/**
 * Get the key ID from current task lsot.
 * @return The free key ID of this lsot object.
 *
 * date           author          notes
 * 2015-07-16     JiangYong       new function
 */
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

/**
 * Put specified key to current task lsot.
 * @param The key ID want to put.
 * @return The state of current operating result.
 *
 * date           author          notes
 * 2015-07-16     JiangYong       new function
 */
FANSAPI RO_CODE E_STATUS PutLsotKey(LSOT_KEY_T LsotKey)
{
    HANDLE handle = caGetLsotHandle(TASK_SELF_HANDLE);
    
    if (INVALID_HANDLE_VALUE == handle)
    {
        return STATE_NOT_EXIST;
    }

    return caPutLsotKey(handle, LsotKey);
}

/**
 * Get the value of specified key.
 * @param The key ID want to get.
 * @param The pointer to store result value.
 * @return The state of current operating result.
 *
 * date           author          notes
 * 2015-07-16     JiangYong       new function
 */
FANSAPI RO_CODE E_STATUS GetLsotValue(LSOT_KEY_T LsotKey, DWORD_PTR lpValue)
{
    HANDLE handle = caGetLsotHandle(TASK_SELF_HANDLE);

    if (INVALID_HANDLE_VALUE == handle)
    {
        return STATE_NOT_EXIST;
    }
    
    return caGetLsotValue(handle, LsotKey, lpValue);
}

/**
 * Set value to specified key.
 * @param The key ID want to set.
 * @param The value want to set.
 * @return The state of current operating result.
 *
 * date           author          notes
 * 2015-07-16     JiangYong       new function
 */
FANSAPI RO_CODE E_STATUS SetLsotValue(LSOT_KEY_T LsotKey, DWORD Value)
{
    HANDLE handle = caGetLsotHandle(TASK_SELF_HANDLE);

    if (INVALID_HANDLE_VALUE == handle)
    {
        return STATE_NOT_EXIST;
    }
    
    return caSetLsotValue(handle, LsotKey, Value);
}

