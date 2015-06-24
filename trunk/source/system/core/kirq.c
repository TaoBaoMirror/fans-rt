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
#include <string.h>
#include <fadefs.h>
#include <faerror.h>
#include <fatypes.h>

#include "klpc.h"
#include "kirq.h"
#include "klist.h"
#include "kcore.h"
#include "karch.h"
#include "kdebug.h"

STATIC LIST_HEAD g_IRQServiceTable[E_HARDWARE_IRQ_MAX];


EXPORT VOID CORE_HandlerIRQ(LPVOID lpPacket, E_IRQ_DEFINE IRQNumber)
{
    LPLIST_HEAD lpList;

    if (IRQNumber < E_HARDWARE_IRQ_MAX)
    {
        LIST_FOR_EACH(lpList, &g_IRQServiceTable[IRQNumber])
        {
            LPIRQ_SERVICE lpService = LIST_ENTRY(lpList, IRQ_SERVICE, ISRList);
            if (STATE_NOT_SUPPORT != lpService->fnHandler(lpPacket, lpService->lpPrivate))
            {
                break;
            }
        }
    }

    return;
}
EXPORT_SYMBOL(CORE_HandlerIRQ);


STATIC LPIRQ_SERVICE CORE_FindService(LPLIST_HEAD lpHead, LPIRQ_SERVICE lpHandler)
{
    LPLIST_HEAD lpList;
    /* 1. 该部分代码在系统服务中执行 */
    /* 2. 非系统服务中断不会直接调用该函数 */
    /* 所以不需要加锁或关中断 */
    LIST_FOR_EACH(lpList, lpHead)
    {
        LPIRQ_SERVICE lpService = LIST_ENTRY(lpList, IRQ_SERVICE, ISRList);
        
        if (lpService->u.ServiceID == lpHandler->u.ServiceID)
        {
            return lpService;
        }
    }
    
    return NULL;
}


STATIC E_STATUS CORE_InstallISR(LPIRQ_SERVICE lpHandler)
{
    DWORD dwFlags;
    
    if (NULL == lpHandler)
    {
        return STATE_INVALID_PARAMETER;
    }

    if (IRQ_MAGIC != lpHandler->Magic ||
        NULL == lpHandler->fnHandler ||
        lpHandler->IRQNumber >= E_HARDWARE_IRQ_MAX)
    {
        return STATE_INVALID_OBJECT;
    }

    if (NULL != CORE_FindService(&g_IRQServiceTable[lpHandler->IRQNumber], lpHandler))
    {
        return STATE_EXISTING;
    }

    dwFlags = CORE_DisableIRQ();
    
    LIST_INSERT_TAIL(&g_IRQServiceTable[lpHandler->IRQNumber], &lpHandler->ISRList);
    
    CORE_RestoreIRQ(dwFlags);
    
    return STATE_SUCCESS;
}

STATIC E_STATUS CORE_UnstallISR(LPIRQ_SERVICE lpHandler)
{
    DWORD dwFlags;
    
    if (NULL == lpHandler)
    {
        return STATE_INVALID_PARAMETER;
    }

    if (IRQ_MAGIC != lpHandler->Magic)
    {
        return STATE_INVALID_OBJECT;
    }

    dwFlags = CORE_DisableIRQ();
    LIST_REMOVE_NODE(&lpHandler->ISRList);
    CORE_RestoreIRQ(dwFlags);
    
    return STATE_SUCCESS;
}

STATIC E_STATUS SVC_InstallISR(LPVOID lpPrivate, LPVOID lpPacket)
{
    return CORE_InstallISR(((LPLPC_REQUEST_PACKET)lpPacket)->u0.pParam);
}

STATIC E_STATUS SVC_UnstallISR(LPVOID lpPrivate, LPVOID lpPacket)
{
    return CORE_UnstallISR(((LPLPC_REQUEST_PACKET)lpPacket)->u0.pParam);
}

STATIC CONST REQUEST_HANDLER fnHandlers[] = {
    SVC_InstallISR,                 /* LPC_SERVICE_INSTALL */
    SVC_UnstallISR,                 /* LPC_SERVICE_UNSTALL */
};

DEFINE_LPC_SERVICE(LPCService, IRQ_MAGIC, SIZEOF_ARRAY(fnHandlers), NULL, fnHandlers);

PUBLIC E_STATUS initCoreInterruptServiceRoutinesManager(VOID)
{
    INT i = 0;
    
    for (i = 0; i < SIZEOF_ARRAY(g_IRQServiceTable); i ++)
    {
        LIST_HEAD_INIT(&g_IRQServiceTable[i]);
    }

    LPC_INSTALL(&LPCService, "Interrupt request(IRQ) manager starting");
    
    return STATE_SUCCESS;
}

