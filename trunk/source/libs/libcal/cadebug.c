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

#include "request.h"
#include "libcal.h"
#include "libcmini.h"
#include "cadebug.h"

PUBLIC CONST CHAR * CONST g_DebugLevelStringTable[LOG_LEVEL_MAX + 1] = {
    "FATAL",
    "ERROR",
    "WARN",
    "BUG",
    "FIX",
    "INFOR",
    "TRACE",
    "DEBUG",
    "UNKNOW",
};

STATIC E_STATUS REQ_WriteDebug(BYTE cbData)
{
    LPC_REQUEST_PACKET Packet;

    memset(&Packet, 0, sizeof(LPC_REQUEST_PACKET));

    Packet.u0.dParam = (DWORD) cbData;

    ntSystemCall(&Packet, SDM_MAGIC, LPC_SDM_WRITE_BYTE);
    
    return Packet.StateCode;
}

EXPORT LPSTR ntDebugLevel2String(E_LOG_LEVEL emLevel)
{
    if (emLevel > LOG_LEVEL_MAX)
    {
        emLevel = LOG_LEVEL_MAX;
    }
    
    return (LPSTR) g_DebugLevelStringTable[emLevel];
}
EXPORT_SYMBOL(ntDebugLevel2String);

DWORD ntDebugGetMask(VOID)
{
    E_STATUS State;
    LPC_REQUEST_PACKET Packet;

    memset(&Packet, 0, sizeof(LPC_REQUEST_PACKET));

    if (STATE_SUCCESS != (State = ntSystemCall(&Packet, SDM_MAGIC, LPC_SDM_DEBUG_MASK)))
    {
        ntSetError(State);
        return 0;
    }
    
    return Packet.u0.dParam;
}
EXPORT_SYMBOL(ntDebugGetMask);

static int put_debug_char(INT Ch, LPVOID lpPrivate)
{
    if ('\n' == Ch)
    {
        REQ_WriteDebug('\r');
    }

    REQ_WriteDebug(Ch);

    return Ch;
}

EXPORT int ntDebugVPrintf(BOOL Enter, CONST CHAR * Format, va_list Vargs)
{
    INT Length;

    Length = fa_vxnprintf(put_debug_char, NULL, ~0, Format, Vargs);

    if (TRUE == Enter)
    {
        Length += fa_xnprintf(put_debug_char, NULL, ~0, "\n");
    }
    
    return Length;
}
EXPORT_SYMBOL(ntDebugVPrintf);

EXPORT int ntDebugPrintf(BOOL Enter, CONST CHAR * Format,...)
{
    int Length;
    VA_LIST Args;

    VA_START(Args, Format);
    Length = ntDebugVPrintf(Enter, Format, Args);
    VA_END(Args);

    return Length;
}
EXPORT_SYMBOL(ntDebugPrintf);

EXPORT int ntDebugLog(BOOL Enter, INT Line, CONST CHAR * Function, E_LOG_LEVEL emLevel, CONST CHAR * Format,...)
{
    SIZE_T Length = 0;
    VA_LIST Args;

    if (!DEBUG_LOG_CONDITION(emLevel))
    {
        return STATE_SUCCESS;
    }

    Length += ntDebugPrintf(FALSE, "[%016lld][Line: %04d][%s][%s] ",
            ntGetSystemTick(), Line, ntDebugLevel2String(emLevel), Function);
    
    VA_START (Args, Format);
    
    Length += ntDebugVPrintf(Enter, Format, Args);

    VA_END(Args);

    return Length;
}
EXPORT_SYMBOL(ntDebugLog);


EXPORT VOID ntDebugShowData(E_LOG_LEVEL emLevel, LPVOID lpBuffer, SIZE_T Length)
{
    DWORD Scale = 0;
    LPBYTE lpData = lpBuffer;

    if (!DEBUG_LOG_CONDITION(emLevel))
    {
        return;
    }

    ntDebugPrintf(FALSE, "Show buffer(%p) is:", lpBuffer);
    
    for (Scale = 0; Scale < Length; Scale ++)
    {
        CHAR Separator = ' ';
        
        if (0 ==(Scale & 0xf))
        {
            ntDebugPrintf(FALSE, "\n%08X:", Scale);
        }
        else if (0 == (Scale & 0x7))
        {
            Separator = '-';
        }
        else
        {
            Separator = ' ';
        }
        
        ntDebugPrintf(FALSE, "%c%02X", Separator, lpData[Scale] & 0xff);
    }

    ntDebugPrintf(FALSE, "\n");
}

