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


typedef struct tagMONITOR_IRQ_TABLE
{
    VOLATILE    IRQ_REGISTER_STACK          Registers;
    VOLATILE    DWORD                       Layer;
    VOLATILE    TICK                        Tick;
    VOLATILE    DWORD                       Count;
    VOLATILE    LPTASK_CONTEXT              lpTask;
}MONITOR_IRQ_TABLE, * PMONITOR_IRQ_TABLE, FAR * LPMONITOR_IRQ_TABLE;

STATIC BOOL DebugEnable = TRUE;
STATIC DWORD DebugLevelMask = DEBUG_LOG_MASK;

EXTERN CONST CHAR * CONST g_DebugLevelStringTable[LOG_LEVEL_MAX + 1];

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

static int put_debug_char(INT Ch, LPVOID lpPrivate)
{
    if ('\n' == Ch)
    {
        CORE_DebugWriteByte('\r');
    }

    CORE_DebugWriteByte(Ch);

    return Ch;
}


EXPORT INT kDebugVPrintf(BOOL Enter, CONST CHAR * Format, va_list Vargs)
{
    INT Length;

    Length = fa_vxnprintf(put_debug_char, NULL, ~0, Format, Vargs);

    if (TRUE == Enter)
    {
        put_debug_char('\n', NULL);
        Length ++;
    }
    
    return Length;
}
EXPORT_SYMBOL(kDebugVPrintf);


EXPORT INT kDebugPrintf(BOOL Enter, CONST CHAR * Format,...)
{
    INT Length;
    VA_LIST Vargs;

    va_start (Vargs, Format);
    Length = kDebugVPrintf(Enter, Format, Vargs);
    va_end(Vargs);

    return Length;
}
EXPORT_SYMBOL(kDebugPrintf);


EXPORT INT kDebugLog(BOOL Enter, INT Line, CONST CHAR * Function, E_LOG_LEVEL emLevel, CONST CHAR * Format, ...)
{
    INT Length;
    va_list vargs;
    
    
    Length = kDebugPrintf(FALSE, "[%016llu][%04d][%s][%s] ", 
            CORE_GetSystemTick(), Line, kDebugLevel2String(emLevel), Function);
    
    va_start (vargs, Format);
    Length += kDebugVPrintf(Enter, Format, vargs);
    va_end(vargs);
    
    return Length;
}
EXPORT_SYMBOL(kDebugLog);


EXPORT VOID kDebugShowData(E_LOG_LEVEL emLevel, LPVOID lpBuffer, SIZE_T Length)
{
    DWORD Scale = 0;
    LPBYTE lpData = lpBuffer;

    if (!DEBUG_LOG_CONDITION(emLevel))
    {
        return;
    }

    kDebugPrintf(FALSE, "Show buffer(%p) is:", lpBuffer);
    
    for (Scale = 0; Scale < Length; Scale ++)
    {
        CHAR Separator = ' ';
        
        if (0 ==(Scale & 0xf))
        {
            kDebugPrintf(FALSE, "\n%08X:", Scale);
        }
        else if (0 == (Scale & 0x7))
        {
            Separator = '-';
        }
        else
        {
            Separator = ' ';
        }
        
        kDebugPrintf(FALSE, "%c%02X", Separator, lpData[Scale] & 0xff);
    }

    kDebugPrintf(FALSE, "\n");
}

#ifdef DEBUG_IRQ_MONITOR

STATIC MONITOR_IRQ_TABLE g_MonitorTable[E_SYSTEM_IRQ_MAX];

PUBLIC VOID CORE_SaveIRQ(LPDWORD lpStackPosition, DWORD Id)
{
    STATIC DWORD Count = 0;
    LPMONITOR_IRQ_TABLE lpTable = &g_MonitorTable[Id];

    memcpy((VOID *)&lpTable->Registers, lpStackPosition, sizeof(IRQ_REGISTER_STACK) - sizeof(LPVOID));
    lpTable->Registers.SP = lpStackPosition;
    lpTable->Layer =  CORE_GetInterruptLayer();
    lpTable->Tick  =  CORE_GetSystemTick();
    lpTable->lpTask=  CORE_GetCurrentTask();
    lpTable->Count =  Count++;
}

STATIC VOID CORE_DebugShowMonitorTable(E_IRQ_DEFINE Id)
{
    LPMONITOR_IRQ_TABLE lpTable = &g_MonitorTable[Id];
    
    kDebugPrintf(TRUE, LOG_LEVEL_FATAL, "******%s [%s] dump infor: Layer = %lu  Tick = %u ******",
            CORE_GetIRQNameString(Id), 
            GetContextTaskName(lpTable->lpTask),
            lpTable->Layer,
            (DWORD)lpTable->Tick);
    kDebugShowStack((LPVOID) &lpTable->Registers, FALSE);
    kDebugPrintf(TRUE, LOG_LEVEL_FATAL, "****Count = %d****", lpTable->Count);
}

PUBLIC VOID kDebugMonitorIRQ(VOID)
{
    CORE_DebugShowMonitorTable(E_CORE_SYSTICK_IRQ_ID);
    CORE_DebugShowMonitorTable(E_CORE_SYSCALL_IRQ_ID);

}
#endif

STATIC E_STATUS SVC_WriteByte(LPVOID lpPrivate, LPVOID lpParam)
{
    LPLPC_REQUEST_PACKET lpPacket = lpParam;
    
    CORE_DebugWriteByte(lpPacket->u0.dParam);

    return STATE_SUCCESS;;
}

STATIC E_STATUS SVC_WriteData(LPVOID lpPrivate, LPVOID lpParam)
{
    kDebugPrintf(FALSE, "%s %s() %d not support.", __FILE__, __FUNCTION__, __LINE__);
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

