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
 *    2015-01-18     JiangYong       new file
 */
#include <string.h>
#include <fauser.h>
#include <fadefs.h>
#include <faerror.h>
#include <fatypes.h>
#include <fapi.h>

#include "kirq.h"
#include "request.h"
#include "libfw.h"
#include "libcal.h"
#include "cadebug.h"

/**
 * Call operating system core.
 * @param The request packet.
 * @param The ID of serivce to want call.
 * @param The ID of founction to want call.
 * @return The result of the kernel returned.
 *
 * date           author          notes
 * 2015-01-18     JiangYong       first version
 */
PUBLIC RO_CODE E_STATUS caSystemCall(LPLPC_REQUEST_PACKET lpPacket, DWORD ServiceID, BYTE FunctionID)
{
    if (NULL == lpPacket)
    {
        return STATE_INVALID_PARAMETER;
    }

    lpPacket->StateCode = STATE_NOT_SUPPORT;
    lpPacket->un.ServiceID = ServiceID;
    lpPacket->FunctionID = FunctionID;
    FW_SystemCall(lpPacket, ServiceID, FunctionID);
    
    return lpPacket->StateCode;
}

/**
 * Register new LPC service.
 * @param The LPC service object.
 * @return The result of the kernel returned.
 *
 * date           author          notes
 * 2015-01-18     JiangYong       first version
 */
EXPORT RO_CODE E_STATUS caInstallLPC(CONST LPC_SERVICE * lpService)
{
    LPC_REQUEST_PACKET Packet;
    
    Packet.u0.pParam = (void *) lpService;

    return caSystemCall(&Packet, LPC_MAGIC, LPC_SERVICE_INSTALL);
}
EXPORT_SYMBOL(caInstallLPC);

/**
 * Unregister the specified LPC service.
 * @param The LPC service object.
 * @return The result of the kernel returned.
 *
 * date           author          notes
 * 2015-01-18     JiangYong       first version
 */
EXPORT RO_CODE E_STATUS caUnstallLPC(CONST LPC_SERVICE * lpService)
{
    LPC_REQUEST_PACKET Packet;
    
    Packet.u0.pParam = (void *) lpService;

    return caSystemCall(&Packet, LPC_MAGIC, LPC_SERVICE_UNSTALL);
}
EXPORT_SYMBOL(caUnstallLPC);


/**
 * Register IRQ handler object.
 * @param The IRQ handler object.
 * @return The result of the kernel returned.
 *
 * date           author          notes
 * 2015-01-18     JiangYong       first version
 */
EXPORT RO_CODE E_STATUS caInstallIRQ(LPIRQ_SERVICE lpHandler)
{
    LPC_REQUEST_PACKET Packet;
    
    Packet.u0.pParam = lpHandler;

    return caSystemCall(&Packet, IRQ_MAGIC, LPC_SERVICE_INSTALL);
}
EXPORT_SYMBOL(caInstallIRQ);

/**
 * Unregister the specified IRQ handler object.
 * @param The IRQ handler object.
 * @return The result of the kernel returned.
 *
 * date           author          notes
 * 2015-01-18     JiangYong       first version
 */
EXPORT RO_CODE E_STATUS caUnstallIRQ(LPIRQ_SERVICE lpHandler)
{
    LPC_REQUEST_PACKET Packet;
    
    Packet.u0.pParam = lpHandler;

    return caSystemCall(&Packet, IRQ_MAGIC, LPC_SERVICE_UNSTALL);
}
EXPORT_SYMBOL(caUnstallIRQ);
