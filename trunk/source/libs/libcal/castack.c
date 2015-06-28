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
 *    2015-06-27     JiangYong       new file
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
#include "kstack.h"
#include "request.h"
#include "cadebug.h"


EXPORT CODE_TEXT E_STATUS caStackMalloc(HANDLE hTask, LPVOID lpParam, E_TASK_PERMISSION Permission)
{
    LPC_REQUEST_PACKET Packet;

    memset(&Packet, 0, sizeof(LPC_REQUEST_PACKET));

    Packet.u0.hParam = hTask;
    Packet.u1.pParam = lpParam;
    Packet.u2.dParam = Permission;
    
    return caSystemCall(&Packet, STK_MAGIC, LPC_STK_STACK_MALLOC);
}

EXPORT E_STATUS caStackFill(HANDLE hTask, LPVOID lpParam, E_TASK_PERMISSION Permission)
{
    LPC_REQUEST_PACKET Packet;

    memset(&Packet, 0, sizeof(LPC_REQUEST_PACKET));

    Packet.u0.hParam = hTask;
    Packet.u1.pParam = lpParam;
    Packet.u2.dParam = Permission;
    
    return caSystemCall(&Packet, STK_MAGIC, LPC_STK_STACK_INIT);
}

EXPORT E_STATUS caStackFree(HANDLE hTask, E_TASK_PERMISSION Permission)
{
    LPC_REQUEST_PACKET Packet;

    memset(&Packet, 0, sizeof(LPC_REQUEST_PACKET));

    Packet.u0.hParam = hTask;
    Packet.u1.dParam = Permission;
    
    return caSystemCall(&Packet, STK_MAGIC, LPC_STK_STACK_FREE);
}
