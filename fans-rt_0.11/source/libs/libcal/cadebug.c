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
#include "request.h"
#include "libcal.h"
#include "libcmini.h"
#include "cadebug.h"

CONST RO_USER_DATA CHAR * CONST g_DebugLevelStringTable[LOG_LEVEL_MAX + 1] = {
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

STATIC RW_USER_DATA HANDLE g_caDebugMutexHandle = INVALID_HANDLE_VALUE;

STATIC RO_CODE E_STATUS REQ_WriteDebug(BYTE cbData)
{
    LPC_REQUEST_PACKET Packet;

    memset(&Packet, 0, sizeof(LPC_REQUEST_PACKET));

    Packet.u0.dParam = (DWORD) cbData;

    caSystemCall(&Packet, SDM_MAGIC, LPC_SDM_WRITE_BYTE);
    
    return Packet.StateCode;
}

EXPORT RO_CODE LPSTR caDebugLevel2String(E_LOG_LEVEL emLevel)
{
    if (emLevel > LOG_LEVEL_MAX)
    {
        emLevel = LOG_LEVEL_MAX;
    }
    
    return (LPSTR) g_DebugLevelStringTable[emLevel];
}
EXPORT_SYMBOL(caDebugLevel2String);

EXPORT RO_CODE DWORD caDebugGetMask(VOID)
{
    E_STATUS State;
    LPC_REQUEST_PACKET Packet;

    memset(&Packet, 0, sizeof(LPC_REQUEST_PACKET));

    if (STATE_SUCCESS != (State = caSystemCall(&Packet, SDM_MAGIC, LPC_SDM_DEBUG_MASK)))
    {
        caSetError(State);
        return 0;
    }
    
    return Packet.u0.dParam;
}
EXPORT_SYMBOL(caDebugGetMask);

STATIC RO_CODE int put_debug_char(INT Ch, LPVOID lpPrivate)
{
    if ('\n' == Ch)
    {
        REQ_WriteDebug('\r');
    }

    REQ_WriteDebug(Ch);

    return Ch;
}

EXPORT RO_CODE int caDebugVPrintf(BOOL Enter, CONST CHAR * Format, va_list Vargs)
{
    INT Length;

    Length = fa_vxnprintf(put_debug_char, NULL, ~0, Format, Vargs);

    if (TRUE == Enter)
    {
        Length += fa_xnprintf(put_debug_char, NULL, ~0, "\n");
    }
    
    return Length;
}
EXPORT_SYMBOL(caDebugVPrintf);

EXPORT RO_CODE int caDebugPrintf(BOOL Enter, CONST CHAR * Format,...)
{
    int Length;
    VA_LIST Args;

    VA_START(Args, Format);
    Length = caDebugVPrintf(Enter, Format, Args);
    VA_END(Args);

    return Length;
}
EXPORT_SYMBOL(caDebugPrintf);

EXPORT RO_CODE int caDebugLog(BOOL Enter, INT Line, CONST CHAR * Function, E_LOG_LEVEL emLevel, CONST CHAR * Format,...)
{
    SIZE_T Length = 0;
    VA_LIST Args;

    if (!DEBUG_LOG_CONDITION(emLevel))
    {
        return STATE_SUCCESS;
    }
    
    caWaitObject(g_caDebugMutexHandle, WAIT_INFINITE);

    Length += caDebugPrintf(FALSE, "[%016lld][Line: %04d][%s][%s] ",
            caGetSystemTick(), Line, caDebugLevel2String(emLevel), Function);
    
    VA_START (Args, Format);
    
    Length += caDebugVPrintf(Enter, Format, Args);
    
    caRequestMethod(g_caDebugMutexHandle, NULL, KIPC_METHOD_POST);

    VA_END(Args);

    return Length;
}
EXPORT_SYMBOL(caDebugLog);


EXPORT RO_CODE VOID caDebugShowData(E_LOG_LEVEL emLevel, LPVOID lpBuffer, SIZE_T Length)
{
    DWORD Scale = 0;
    BYTE_PTR lpData = lpBuffer;

    if (!DEBUG_LOG_CONDITION(emLevel))
    {
        return;
    }
    
    caWaitObject(g_caDebugMutexHandle, WAIT_INFINITE);

    caDebugPrintf(FALSE, "Show buffer(%p) is:", lpBuffer);
    
    for (Scale = 0; Scale < Length; Scale ++)
    {
        CHAR Separator = ' ';
        
        if (0 ==(Scale & 0xf))
        {
            caDebugPrintf(FALSE, "\n%08X:", Scale);
        }
        else if (0 == (Scale & 0x7))
        {
            Separator = '-';
        }
        else
        {
            Separator = ' ';
        }
        
        caDebugPrintf(FALSE, "%c%02X", Separator, lpData[Scale] & 0xff);
    }

    caDebugPrintf(FALSE, "\n");
    
    caRequestMethod(g_caDebugMutexHandle, NULL, KIPC_METHOD_POST);
}


PUBLIC E_STATUS initSystemApplicationDebug(VOID)
{
    MUTEX_ATTRIBUTE Attribute;
    
    Attribute.Bits.MutexValue = 1;

    g_caDebugMutexHandle = caCreateObject(NULL, MTX_MAGIC, &Attribute);
    
    if (INVALID_HANDLE_VALUE == g_caDebugMutexHandle)
    {
        caDebugPrintf(TRUE, "Create debug mutex failed !\r\n");
        return caGetError();
    }
    
    return STATE_SUCCESS;
}

