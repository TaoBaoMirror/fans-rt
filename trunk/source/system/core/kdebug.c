/*
 *    Copyright(C) 2013-2015, Fans-rt development team.
 *
 *    All rights reserved.
 *
 *    This is open source software.
 *    Learning and research can be unrestricted to  modification, use and dissemination.
 *    If you need for commercial purposes, you should get the author's permission.
 *
 *    date           author          notes
 *    2014-09-07     JiangYong       new file
 */
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <fadefs.h>
#include <faerror.h>
#include <fatypes.h>

#include "kirq.h"
#include "klpc.h"
#include "ktask.h"
#include "ktick.h"
#include "kcore.h"
#include "kboard.h"
#include "kdebug.h"
#include "libcmini.h"

STATIC RW_CORE_DATA     BOOL DebugEnable = TRUE;
STATIC RW_CORE_DATA     BYTE DebugLevelMask = DEBUG_LOG_MASK;
EXTERN CONST CHAR *     CONST g_DebugLevelStringTable[LOG_LEVEL_MAX + 1];

EXPORT LPSTR kDebugLevel2String(E_LOG_LEVEL emLevel)
{
    if (emLevel > LOG_LEVEL_MAX)
    {
        emLevel = LOG_LEVEL_MAX;
    }
    
    return (LPTSTR) g_DebugLevelStringTable[emLevel];
}
EXPORT_SYMBOL(kDebugLevel2String);

DWORD kDebugGetMask(VOID)
{
    return DebugLevelMask;
}
EXPORT_SYMBOL(kDebugGetMask);

EXPORT VOID kDebugEnableLevel(E_LOG_LEVEL emLevel)
{
    DebugLevelMask |= (1 << emLevel);
}
EXPORT_SYMBOL(kDebugEnableLevel);

EXPORT VOID kDebugDisableLevel(E_LOG_LEVEL emLevel)
{
    DebugLevelMask &= (~(1 << emLevel));
}
EXPORT_SYMBOL(kDebugDisableLevel);

EXPORT BOOL kDebugGetState(VOID)
{
    return DebugEnable;
}
EXPORT_SYMBOL(kDebugGetState);

EXPORT INT kDebugLog(BOOL Enter, INT Line, CONST CHAR * Function, E_LOG_LEVEL emLevel, CONST CHAR * Format, ...)
{
    INT Length;
    va_list vargs;

    Length = kprintf("[%016llu][%04d][%s][%s][%s] ", 
            CORE_GetSystemTick(), Line, kDebugLevel2String(emLevel), CORE_GetCurrentTaskName(), Function);
    
    va_start (vargs, Format);
    Length += kvprintf(Format, vargs);
    if (Enter) Length += kprintf("\n");
    va_end(vargs);
    
    if (LOG_LEVEL_FATAL == emLevel) SYSTEM_CALL_OOPS();
    
    return Length;
}
EXPORT_SYMBOL(kDebugLog);


EXPORT VOID kDebugShowData(E_LOG_LEVEL emLevel, LPVOID lpBuffer, SIZE_T Length)
{
    DWORD Scale = 0;
    BYTE_PTR lpData = lpBuffer;

    if (!DEBUG_LOG_CONDITION(emLevel))
    {
        return;
    }

    kprintf("Show buffer(%p) is:", lpBuffer);
    
    for (Scale = 0; Scale < Length; Scale ++)
    {
        CHAR Separator = ' ';
        
        if (0 ==(Scale & 0xf))
        {
            kprintf("\n%08X:", Scale);
        }
        else if (0 == (Scale & 0x7))
        {
            Separator = '-';
        }
        else
        {
            Separator = ' ';
        }
        
        kprintf("%c%02X", Separator, lpData[Scale] & 0xff);
    }

    kprintf("\n");
}

STATIC E_STATUS SVC_WriteByte(LPVOID lpPrivate, LPVOID lpParam)
{
    LPLPC_REQUEST_PACKET lpPacket = lpParam;
    
    CORE_DebugWriteByte(lpPacket->u0.dParam);

    return STATE_SUCCESS;;
}

STATIC E_STATUS SVC_WriteData(LPVOID lpPrivate, LPVOID lpParam)
{
    kprintf("%s %s() %d not support.", __FILE__, __FUNCTION__, __LINE__);
    SYSTEM_CALL_OOPS();

    return STATE_SUCCESS;
}

STATIC E_STATUS SVC_DebugMask(LPVOID lpPrivate, LPVOID lpParam)
{
    LPLPC_REQUEST_PACKET lpPacket = lpParam;
    
    lpPacket->u0.dParam = kDebugGetMask();

    return STATE_SUCCESS;;
}

STATIC CONST REQUEST_HANDLER fnHandlers[] = {
    SVC_WriteByte,                  /* LPC_SDM_WRITE_BYTE */
    SVC_WriteData,                  /* LPC_SDM_WRITE_DATA */
    SVC_DebugMask,                  /* LPC_SDM_DEBUG_MASK */
};


DEFINE_LPC_SERVICE(SdmService, SDM_MAGIC, SIZEOF_ARRAY(fnHandlers), NULL, fnHandlers);


PUBLIC E_STATUS initCoreSystemDebugManager(VOID)
{
    LPC_INSTALL(&SdmService, "System debug manager(SDM) starting");
    return STATE_SUCCESS;
}

