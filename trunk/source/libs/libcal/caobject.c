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
#include <string.h>
#include <stdio.h>
#include <fadefs.h>
#include <faerror.h>
#include <fatypes.h>

#include "kipc.h"
#include "kcore.h"
#include "kobject.h"

#include "cadebug.h"
#include "request.h"
#include "libcal.h"


EXPORT LPSTR ntChooseName(__IN __OUT LPSTR lpName, LPSTR lpType)
{
    STATIC UINT Count = 0;
    
    snprintf(lpName, OBJECT_NAME_MAX-1, "%s%04X", lpType, (Count ++) & 0xffff);
    
    return lpName;
}

EXPORT HANDLE ntMallocObject(LPCSTR lpName, DWORD Magic, LPVOID lpParam)
{
    LPC_REQUEST_PACKET Packet;

    Packet.u0.dParam = Magic;
    Packet.u1.pParam = (LPVOID) lpName;
    Packet.u2.pParam = lpParam;
    
    if (STATE_SUCCESS != ntSystemCall(&Packet, SOM_MAGIC, LPC_SOM_OBJECT_MALLOC))
    {
        return INVALID_HANDLE_VALUE;
    }
    
    return Packet.u1.hParam; 
}

EXPORT E_STATUS ntActiveObject(HANDLE handle, LPVOID lpParam)
{
    LPC_REQUEST_PACKET Packet;

    Packet.u0.hParam = handle;
    Packet.u1.pParam = lpParam;

    return ntSystemCall(&Packet, SOM_MAGIC, LPC_SOM_OBJECT_ACTIVE);
}

EXPORT HANDLE ntTakeObject(LPCSTR lpName, LPVOID lpParam)
{
    LPC_REQUEST_PACKET Packet;

    Packet.u0.pParam = (LPVOID) lpName;
    Packet.u1.pParam = lpParam;
    
    if (STATE_SUCCESS != ntSystemCall(&Packet, SOM_MAGIC, LPC_SOM_OBJECT_TAKE))
    {
        return INVALID_HANDLE_VALUE;
    }
    
    return Packet.u1.hParam;    
}

EXPORT E_STATUS ntWaitObject(HANDLE handle, LONG WaitTime)
{
    LPC_REQUEST_PACKET Packet;

    Packet.u0.hParam = handle;
    Packet.u1.dParam = WaitTime;

    return ntSystemCall(&Packet, SOM_MAGIC, LPC_SOM_OBJECT_WAIT);
}

EXPORT E_STATUS ntPostObject(HANDLE handle, LPVOID lpParam)
{
    LPC_REQUEST_PACKET Packet;

    Packet.u0.hParam = handle;
    Packet.u1.pParam = lpParam;

    return ntSystemCall(&Packet, SOM_MAGIC, LPC_SOM_OBJECT_POST);
}

EXPORT E_STATUS ntResetObject(HANDLE handle, LPVOID lpParam)
{
    LPC_REQUEST_PACKET Packet;

    Packet.u0.hParam = handle;
    Packet.u1.pParam = lpParam;

    return ntSystemCall(&Packet, SOM_MAGIC, LPC_SOM_OBJECT_RESET);
}

EXPORT E_STATUS ntFreeObject(HANDLE handle)
{
    LPC_REQUEST_PACKET Packet;

    Packet.u0.hParam = handle;
    
    return ntSystemCall(&Packet, SOM_MAGIC, LPC_SOM_OBJECT_FREE);
}
