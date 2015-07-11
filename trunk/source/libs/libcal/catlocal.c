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

#include "libfw.h"
#include "libcal.h"
#include "ktlocal.h"
#include "request.h"
#include "cadebug.h"


EXPORT RO_CODE E_STATUS caCreateLsotObject(HANDLE hTask, DWORD Total)
{
    LPC_REQUEST_PACKET Packet;

    memset(&Packet, 0, sizeof(LPC_REQUEST_PACKET));
    
    Packet.u0.hParam = hTask;
    Packet.u1.dParam = Total;

    return caSystemCall(&Packet, LST_MAGIC, LPC_KLSOT_CREATE);
}

EXPORT RO_CODE E_STATUS caRemoveLsotObject(HANDLE hTask)
{
    LPC_REQUEST_PACKET Packet;

    memset(&Packet, 0, sizeof(LPC_REQUEST_PACKET));
    
    Packet.u0.hParam = hTask;

    return caSystemCall(&Packet, LST_MAGIC, LPC_KLSOT_DELETE);
}


EXPORT RO_CODE SMLT_KEY_T caGetLsotKey(VOID)
{
    LPC_REQUEST_PACKET Packet;

    memset(&Packet, 0, sizeof(LPC_REQUEST_PACKET));

    if (STATE_SUCCESS != caSystemCall(&Packet, LST_MAGIC, LPC_KLSOT_GET_KEY))
    {
        return TASK_SMLTKEY_INVALID;
    }
    
    return (SMLT_KEY_T) Packet.u0.dParam;
}

EXPORT RO_CODE E_STATUS caPutLsotKey(SMLT_KEY_T SmltKey)
{
    LPC_REQUEST_PACKET Packet;

    memset(&Packet, 0, sizeof(LPC_REQUEST_PACKET));
    
    Packet.u0.dParam = SmltKey;

    return caSystemCall(&Packet, LST_MAGIC, LPC_KLSOT_PUT_KEY);
}

EXPORT RO_CODE E_STATUS caGetLsotValue(SMLT_KEY_T SmltKey, LPDWORD lpValue)
{
    E_STATUS State;
    LPC_REQUEST_PACKET Packet;
    
    if (NULL == lpValue)
    {
        return STATE_INVALID_PARAMETER;
    }

    memset(&Packet, 0, sizeof(LPC_REQUEST_PACKET));
    
    Packet.u0.dParam = SmltKey;
    
    if (STATE_SUCCESS == (State = caSystemCall(&Packet, LST_MAGIC, LPC_KLSOT_GET_VALUE)))
    {
        *lpValue = Packet.u1.dParam;
    }

    return State;
}

EXPORT RO_CODE E_STATUS caSetLsotValue(SMLT_KEY_T SmltKey, DWORD Value)
{
    LPC_REQUEST_PACKET Packet;

    memset(&Packet, 0, sizeof(LPC_REQUEST_PACKET));
    
    Packet.u0.dParam = SmltKey;
    Packet.u1.dParam = Value;

    return caSystemCall(&Packet, LST_MAGIC, LPC_KLSOT_SET_VALUE);
}
