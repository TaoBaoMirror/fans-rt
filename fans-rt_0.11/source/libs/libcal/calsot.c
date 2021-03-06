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
 *    2015-07-11     JiangYong       new file
 */
#include <stdio.h>
#include <string.h>
#include <fauser.h>
#include <fadefs.h>
#include <faerror.h>
#include <fatypes.h>
#include <fapi.h>

#include "klsot.h"

#include "libfw.h"
#include "libcal.h"
#include "request.h"
#include "cadebug.h"

#if (defined(CONFIG_BUILD_LOCAL_STORE) && (TRUE == CONFIG_BUILD_LOCAL_STORE))
/**
 * Alloc a lsot object from object management service.
 * @param The total keys of lsot object.
 * @return The handle of this lsot object.
 *
 * date           author          notes
 * 2015-07-16     JiangYong       new function
 */
EXPORT RO_CODE HANDLE caMallocLsotObject(DWORD Total)
{
    KLSOT_REQUEST_PACKET Packet;

    SetKLPTotal(&Packet, Total);

    return caMallocNoNameObject(SLT_MAGIC, &Packet);
}

/**
 * active specified lsot object.
 * @param The handle of lsot object.
 * @param The handle of the task.
 * @return The state of current operating result.
 *
 * date           author          notes
 * 2015-07-16     JiangYong       new function
 */
EXPORT RO_CODE E_STATUS caActiveLsotObject(HANDLE hLsot, HANDLE hTask)
{
    KLSOT_REQUEST_PACKET Packet;

    SetKLPhTask(&Packet, hTask);

    return caActiveObject(hLsot, &Packet);
}

/**
 * Get the key ID from specified lsot object.
 * @param The handle of lsot object.
 * @return The free key ID of this lsot object.
 *
 * date           author          notes
 * 2015-07-16     JiangYong       new function
 */
EXPORT RO_CODE LSOT_KEY_T caGetLsotKey(HANDLE hLsot)
{
    KLSOT_REQUEST_PACKET Packet;

    SetKLPKeyID(&Packet, TASK_LSOTKEY_INVALID);
    
    if (STATE_SUCCESS != caRequestMethod(hLsot, &Packet, KLSOT_METHOD_GET_KEY))
    {
        return TASK_LSOTKEY_INVALID;
    }

    return GetKLPKeyID(&Packet);
}

/**
 * Pet the key ID to specified lsot object.
 * @param The handle of lsot object.
 * @param The key id want to put.
 * @return The free key ID of this lsot object.
 *
 * date           author          notes
 * 2015-07-16     JiangYong       new function
 */
EXPORT RO_CODE E_STATUS caPutLsotKey(HANDLE hLsot, LSOT_KEY_T LsotKey)
{
    KLSOT_REQUEST_PACKET Packet;

    SetKLPKeyID(&Packet, LsotKey);
    
    return caRequestMethod(hLsot, &Packet, KLSOT_METHOD_PUT_KEY);
}

/**
 * Get the value of specified key from a lsot object.
 * @param The handle of lsot object.
 * @param The key ID want to get.
 * @param The pointer to store result value.
 * @return The state of current operating result.
 *
 * date           author          notes
 * 2015-07-16     JiangYong       new function
 */
EXPORT RO_CODE E_STATUS caGetLsotValue(HANDLE hLsot, LSOT_KEY_T LsotKey, DWORD_PTR lpValue)
{
    E_STATUS Result;
    KLSOT_REQUEST_PACKET Packet;

    if (NULL == lpValue)
    {
        return STATE_INVALID_PARAMETER;
    }

    SetKLPKeyID(&Packet, LsotKey);
    SetKLPValue(&Packet, 0);

    Result = caRequestMethod(hLsot, &Packet, KLSOT_METHOD_GET_VALUE);

    if (STATE_SUCCESS == Result)
    {
        *lpValue = GetKLPValue(&Packet);
    }

    return Result;
}

/**
 * Set value to specified lsot object key.
 * @param The handle of lsot object.
 * @param The key ID want to set.
 * @param The value want to set.
 * @return The state of current operating result.
 *
 * date           author          notes
 * 2015-07-16     JiangYong       new function
 */
EXPORT RO_CODE E_STATUS caSetLsotValue(HANDLE hLsot, LSOT_KEY_T LsotKey, DWORD Value)
{
    KLSOT_REQUEST_PACKET Packet;

    SetKLPKeyID(&Packet, LsotKey);
    SetKLPValue(&Packet, Value);

    return caRequestMethod(hLsot, &Packet, KLSOT_METHOD_SET_VALUE);
}
#endif

