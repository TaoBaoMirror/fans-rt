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
#include <fauser.h>
#include <fadefs.h>
#include <faerror.h>
#include <fatypes.h>

#include "kipc.h"
#include "kcore.h"
#include "kobject.h"

#include "cadebug.h"
#include "request.h"
#include "libcal.h"


EXPORT CODE_TEXT LPSTR caChooseName(__IN __OUT LPSTR lpName, LPSTR lpType)
{
    STATIC UINT Count = 0;
    
    snprintf(lpName, OBJECT_NAME_MAX-1, "%s%04X", lpType, (Count ++) & 0xffff);
    
    return lpName;
}

EXPORT CODE_TEXT HANDLE caMallocObject(LPCSTR lpName, DWORD Magic, LPVOID lpParam)
{
    LPC_REQUEST_PACKET Packet;

    memset(&Packet, 0, sizeof(Packet));

    Packet.u0.dParam = Magic;
    Packet.u1.pParam = (LPVOID) lpName;
    Packet.u2.pParam = lpParam;
    
    if (STATE_SUCCESS != caSystemCall(&Packet, SOM_MAGIC, LPC_SOM_OBJECT_MALLOC))
    {
        return INVALID_HANDLE_VALUE;
    }
    
    return Packet.u1.hParam; 
}

EXPORT CODE_TEXT E_STATUS caActiveObject(HANDLE handle, LPVOID lpParam)
{
    LPC_REQUEST_PACKET Packet;

    memset(&Packet, 0, sizeof(Packet));

    Packet.u0.hParam = handle;
    Packet.u1.pParam = lpParam;

    return caSystemCall(&Packet, SOM_MAGIC, LPC_SOM_OBJECT_ACTIVE);
}

EXPORT CODE_TEXT HANDLE caTakeObject(LPCSTR lpName, LPVOID lpParam)
{
    LPC_REQUEST_PACKET Packet;

    memset(&Packet, 0, sizeof(Packet));

    Packet.u0.pParam = (LPVOID) lpName;
    Packet.u1.pParam = lpParam;
    
    if (STATE_SUCCESS != caSystemCall(&Packet, SOM_MAGIC, LPC_SOM_OBJECT_TAKE))
    {
        return INVALID_HANDLE_VALUE;
    }
    
    return Packet.u1.hParam;    
}

EXPORT CODE_TEXT E_STATUS caWaitObject(HANDLE handle, LONG WaitTime)
{
    LPC_REQUEST_PACKET Packet;

    memset(&Packet, 0, sizeof(Packet));

    Packet.u0.hParam = handle;
    Packet.u1.dParam = WaitTime;

    return caSystemCall(&Packet, SOM_MAGIC, LPC_SOM_OBJECT_WAIT);
}

EXPORT CODE_TEXT E_STATUS caPostObject(HANDLE handle, LPVOID lpParam)
{
    LPC_REQUEST_PACKET Packet;

    memset(&Packet, 0, sizeof(Packet));

    Packet.u0.hParam = handle;
    Packet.u1.pParam = lpParam;

    return caSystemCall(&Packet, SOM_MAGIC, LPC_SOM_OBJECT_POST);
}

EXPORT CODE_TEXT E_STATUS caResetObject(HANDLE handle, LPVOID lpParam)
{
    LPC_REQUEST_PACKET Packet;

    memset(&Packet, 0, sizeof(Packet));

    Packet.u0.hParam = handle;
    Packet.u1.pParam = lpParam;

    return caSystemCall(&Packet, SOM_MAGIC, LPC_SOM_OBJECT_RESET);
}

EXPORT CODE_TEXT E_STATUS caFreeObject(HANDLE handle)
{
    LPC_REQUEST_PACKET Packet;

    memset(&Packet, 0, sizeof(Packet));

    Packet.u0.hParam = handle;
    
    return caSystemCall(&Packet, SOM_MAGIC, LPC_SOM_OBJECT_FREE);
}

EXPORT CODE_TEXT E_STATUS caGetObjectName(HANDLE handle, CHAR Name[OBJECT_NAME_MAX], SIZE_T Length)
{
    LPC_REQUEST_PACKET Packet;

    memset(&Packet, 0, sizeof(Packet));

    Packet.u0.hParam = handle;
    Packet.u1.pParam = Name;
    Packet.u2.dParam = Length;
    
    return caSystemCall(&Packet, SOM_MAGIC, LPC_SOM_OBJECT_GETNAME);
}
