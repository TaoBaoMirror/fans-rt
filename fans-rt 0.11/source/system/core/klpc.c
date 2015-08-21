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
#include <fadefs.h>
#include <faerror.h>
#include <fatypes.h>

#include "klpc.h"
#include "khash.h"
#include "klist.h"
#include "kcore.h"
#include "ktask.h"
#include "kdebug.h"
#include "libfw.h"

STATIC RW_CORE_DATA LPC_SERVICE * g_ServiceHashTable[CONFIG_LPC_SERVICE_MAX] = {NULL};

#ifdef __DEBUG__
STATIC DWORD ServiceHashKey(DWORD Magic)
{
    return Magic2RequestIDKey(Magic);
}

STATIC DWORD LoadMagic(LPVOID lpPrivate, DWORD Id)
{
    if (g_ServiceHashTable[Id])
    {
        return g_ServiceHashTable[Id]->u.ServiceID;
    }
    
    return 0;
}
#endif

EXPORT E_STATUS CORE_InstallLPC(CONST LPC_SERVICE * lpService)
{
    BYTE RequestID;

    if (NULL == lpService)
    {
        return STATE_INVALID_PARAMETER;
    }
    
    if (NULL == lpService->fnHandlers ||
        LPC_MAGIC != lpService->Magic)
    {
        return STATE_INVALID_OBJECT;
    }
    
    RequestID = Magic2RequestID(lpService->u.ServiceID);
    
    if (NULL != g_ServiceHashTable[RequestID])
    {
#ifdef __DEBUG__
        DWORD Count = 0;
        DWORD MagicArray[CONFIG_LPC_SERVICE_MAX];

        Count = CORE_HashSetArray(LoadMagic, g_ServiceHashTable,
                MagicArray, lpService->u.ServiceID, CONFIG_LPC_SERVICE_MAX);

        CORE_HashDebug(ServiceHashKey, MagicArray, Count, 8, 24,
                "CONFIG_LPC_SERVICE_MAX", "Magic2RequestID");
#endif
        CORE_ERROR(TRUE, "Install service '%s' to %d has been registered !",
            lpService->u.SvcName, RequestID);

        return STATE_EXISTING;
    }

    g_ServiceHashTable[RequestID] = (LPVOID) lpService;
    
    CORE_INFOR(TRUE, "Install service '%s' to %d successfully.", lpService->u.SvcName, RequestID);
    
    return STATE_SUCCESS;
}


STATIC E_STATUS CORE_UnstallLPC(LPLPC_SERVICE lpService)
{
    BYTE RequestID;

    if (NULL == lpService)
    {
        return STATE_INVALID_PARAMETER;
    }
    
    if (NULL == lpService->fnHandlers ||
        LPC_MAGIC != lpService->Magic)
    {
        return STATE_INVALID_OBJECT;
    }
    
    RequestID = Magic2RequestID(lpService->u.ServiceID);
    
    if (lpService != g_ServiceHashTable[RequestID])
    {
        return STATE_NOT_FOUND;
    }
    
    g_ServiceHashTable[RequestID] = NULL;
    
    CORE_DEBUG(TRUE, "Unstall service %d name '%s' successfully.",
        RequestID, lpService->u.SvcName);
    
    return STATE_SUCCESS;
}

EXPORT VOID CORE_HandlerLPC(LPLPC_REQUEST_PACKET lpPacket, DWORD ServiceID, BYTE FunctionID)
{
    if (NULL != lpPacket)
    {
        REQUEST_HANDLER fnHandle = NULL;
        BYTE RequestID = Magic2RequestID(ServiceID);
        CONST LPC_SERVICE * lpService = g_ServiceHashTable[RequestID];

        if (NULL == lpService ||
            ServiceID != lpService->u.ServiceID ||
            FunctionID >= lpService->MaxFuncs )
        {
            lpPacket->StateCode = STATE_INVALID_SERVICE;
            CORE_ERROR(TRUE, "Call service 0x%08x, function %d failed, packet %p.",
                ServiceID, FunctionID, lpPacket);
            return;
        }

        CORE_SetCurrentTaskRequestPacket(lpPacket);
        
        fnHandle = lpService->fnHandlers[FunctionID];

        CORE_DEBUG(TRUE, "Call service 0x%08x, function %d ... now", ServiceID, FunctionID);
        
        lpPacket->StateCode = NULL == fnHandle
                            ? STATE_INVALID_HANDLER
                            : fnHandle(lpService->lpPrivate, lpPacket);
        
        if (STATE_SUCCESS != lpPacket->StateCode)
        {
            CORE_SetError(lpPacket->StateCode);
        }
        
        CORE_DEBUG(TRUE, "Call service 0x%08x, function %d, result %d.",
            ServiceID, FunctionID, lpPacket->StateCode);
    }
    
    return;
}
EXPORT_SYMBOL(CORE_HandlerLPC);

STATIC E_STATUS SVC_InstallLPC(LPVOID lpPrivate, LPVOID lpPacket)
{
    return CORE_InstallLPC(((LPLPC_REQUEST_PACKET)lpPacket)->u0.pParam);
}

STATIC E_STATUS SVC_UnstallLPC(LPVOID lpPrivate, LPVOID lpPacket)
{
    return CORE_UnstallLPC(((LPLPC_REQUEST_PACKET)lpPacket)->u0.pParam);
}

STATIC CONST REQUEST_HANDLER fnHandlers[] = {
    SVC_InstallLPC,                 /* LPC_SERVICE_INSTALL */
    SVC_UnstallLPC,                 /* LPC_SERVICE_UNSTALL */
};

DEFINE_LPC_SERVICE(LPCService, LPC_MAGIC, SIZEOF_ARRAY(fnHandlers), NULL, fnHandlers);

PUBLIC E_STATUS initCoreLocalProcedureCallManager(VOID)
{
    if (STATE_SUCCESS != CORE_InstallLPC(&LPCService))
    {
        CORE_ERROR(TRUE, "System call(%s) service starting ... %s\n", LPCService.u.SvcName, "[FAILED]");
        SYSTEM_CALL_OOPS();
    }
    
    CORE_INFOR(TRUE, "System call(%s) service starting ... %s", LPCService.u.SvcName, "[OK]");
    
    return STATE_SUCCESS;
}

