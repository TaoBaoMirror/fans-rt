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


EXPORT RO_USER_CODE LPSTR caChooseName(__IN __OUT LPSTR lpName, LPSTR lpType)
{
    STATIC RW_USER_DATA WORD Count = 0;
    
    snprintf(lpName, OBJECT_NAME_MAX-1, "%s%04X", lpType, (Count ++) & 0xffff);
    
    return lpName;
}

EXPORT RO_USER_CODE HANDLE caMallocObject(LPCSTR lpName, DWORD Magic, LPVOID lpParam)
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

EXPORT RO_USER_CODE HANDLE caMallocNoNameObject(DWORD Magic, LPVOID lpParam)
{
    LPC_REQUEST_PACKET Packet;

    memset(&Packet, 0, sizeof(Packet));

    Packet.u0.dParam = Magic;
    Packet.u1.pParam = lpParam;

    if (STATE_SUCCESS != caSystemCall(&Packet, SOM_MAGIC, LPC_SOM_OBJECT_MALLOCNM))
    {
        return INVALID_HANDLE_VALUE;
    }
    
    return Packet.u2.hParam; 
}


EXPORT RO_USER_CODE E_STATUS caActiveObject(HANDLE handle, LPVOID lpParam)
{
    LPC_REQUEST_PACKET Packet;

    memset(&Packet, 0, sizeof(Packet));

    Packet.u0.hParam = handle;
    Packet.u1.pParam = lpParam;

    return caSystemCall(&Packet, SOM_MAGIC, LPC_SOM_OBJECT_ACTIVE);
}

EXPORT RO_USER_CODE HANDLE caTakeObject(LPCSTR lpName, LPVOID lpParam)
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

EXPORT RO_USER_CODE E_STATUS caFreeObject(HANDLE handle)
{
    LPC_REQUEST_PACKET Packet;

    memset(&Packet, 0, sizeof(Packet));

    Packet.u0.hParam = handle;
    
    return caSystemCall(&Packet, SOM_MAGIC, LPC_SOM_OBJECT_FREE);
}

EXPORT RO_USER_CODE E_STATUS caGetObjectName(HANDLE handle, CHAR Name[OBJECT_NAME_MAX], SIZE_T Length)
{
    LPC_REQUEST_PACKET Packet;

    memset(&Packet, 0, sizeof(Packet));

    Packet.u0.hParam = handle;
    Packet.u1.pParam = Name;
    Packet.u2.dParam = Length;
    
    return caSystemCall(&Packet, SOM_MAGIC, LPC_SOM_OBJECT_GETNAME);
}

EXPORT RO_USER_CODE E_STATUS caRequestMethod(HANDLE handle, LPVOID lpParam, DWORD Method)
{
    LPC_REQUEST_PACKET Packet;

    memset(&Packet, 0, sizeof(Packet));

    Packet.u0.hParam = handle;
    Packet.u1.pParam = lpParam;
    Packet.u2.dParam = Method;

    return caSystemCall(&Packet, SOM_MAGIC, LPC_SOM_OBJECT_METHOD);
}

EXPORT RO_USER_CODE SHORT caWaitObject(HANDLE handle, LONG WaitTime)
{
    E_STATUS Result;
    KIPC_WAIT_PARAM Param;
    
    SetSignalID2Param(&Param, WAIT_SIGNAL_ID_0);
    SetWaitTime2Param(&Param, WaitTime);
    
    if (STATE_SUCCESS != (Result = caRequestMethod(handle, &Param, KIPC_METHOD_WAIT)))
    {
        caSetError(Result);
        return WAIT_SIGNAL_INVALID;
    }
    
    return Param.SignalID;
}

