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
#include <stdio.h>
#include <string.h>
#include <fadefs.h>
#include <faerror.h>
#include <fatypes.h>

#include "ktick.h"
#include "request.h"
#include "libcal.h"
#include "cadebug.h"

EXPORT TICK ntGetSystemTick(VOID)
{
    TICK Tick;
    E_STATUS State;
    LPC_REQUEST_PACKET Packet;
    
    memset(&Packet, 0, sizeof(LPC_REQUEST_PACKET));

    if (STATE_SUCCESS != (State = ntSystemCall(&Packet, SCM_MAGIC, LPC_SCM_GET_SYSTICK)))
    {
        ntSetError(State);
        return INVALID_TICK;
    }

    Tick = (((TICK)(Packet.u0.dParam)) << 0);
    Tick |= (((TICK)(Packet.u1.dParam)) << 32);

    return Tick;
}
EXPORT_SYMBOL(ntGetSystemTick);

