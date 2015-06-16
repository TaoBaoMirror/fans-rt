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
#include <fadefs.h>
#include <faerror.h>
#include <fatypes.h>

#include "kmem.h"
#include "klpc.h"
#include "kcore.h"
#include "cadebug.h"
#include "request.h"
#include "libcal.h"

#if (CONFIG_MEM_REGION_MAX != 0)
/**
 * Call system to allocate some page of memory.
 * @param The size of memory want to allocation.
 * @return The result of the kernel returned.
 *
 * date           author          notes
 * 2015-01-18     JiangYong       first version
 */
EXPORT LPVOID caPageMalloc(SIZE_T Length)
{
    LPC_REQUEST_PACKET Packet;

    Packet.u1.dParam = (WORD)(Length / CONFIG_MEM_PAGE_SIZE + (!!(Length % CONFIG_MEM_PAGE_SIZE)));

    if (STATE_SUCCESS != caSystemCall(&Packet, SMM_MAGIC, LPC_MMS_PAGE_ALLOC))
    {
        return NULL;
    }
    
    return Packet.u0.pParam;
}
EXPORT_SYMBOL(caPageMalloc);


/**
 * Call system to free some page of memory.
 * @param Address of some pages want to free.
 * @return The result of the kernel returned.
 *
 * date           author          notes
 * 2015-01-18     JiangYong       first version
 */
EXPORT E_STATUS caPageFree(LPVOID lpPageAddress)
{
    LPC_REQUEST_PACKET Packet;
    
    Packet.u0.pParam = lpPageAddress;
    
    return caSystemCall(&Packet, SMM_MAGIC, LPC_MMS_PAGE_FREE);
}
EXPORT_SYMBOL(caPageFree);

/**
 * Query the number of free pages.
 * @param The ID of region to want query.
 * @return The result of the kernel returned.
 *
 * date           author          notes
 * 2015-01-18     JiangYong       first version
 */
EXPORT DWORD caGetSystemFreePages(BYTE RegionID)
{
    E_STATUS State;
    LPC_REQUEST_PACKET Packet;

    Packet.u0.dParam = (DWORD) RegionID;

    if (STATE_SUCCESS != (State = caSystemCall(&Packet, SMM_MAGIC, LPC_MMS_FREE_PAGES)))
    {
        LOG_ERROR(TRUE, "Call system service MMS function %d failed, return %d", LPC_MMS_FREE_PAGES, State);
        caSetError(State);
        return INVALID_VALUE;
    }

    return Packet.u0.dParam;
}
EXPORT_SYMBOL(caGetSystemFreePages);

/**
 * Query the information of memory manager service(only for debug).
 * @param The information buffer.
 * @return The result of the kernel returned.
 *
 * date           author          notes
 * 2015-01-18     JiangYong       first version
 */
EXPORT E_STATUS caGetMmsInformation(LPMMS_INFOR lpInfor)
{
#if (CONFIG_MEM_DEBUG_ENABLE == FALSE)
    return STATE_NOT_SUPPORT;
#else
    E_STATUS State;
    LPC_REQUEST_PACKET Packet;

    if (NULL == lpInfor)
    {
        LOG_ERROR(TRUE, "Invalid information buffer pointer.");
        return STATE_INVALID_PARAMETER;
    }

    Packet.u0.pParam = lpInfor;

    State = caSystemCall(&Packet, SMM_MAGIC, LPC_MMS_TAKE_INFOR);

    if (STATE_SUCCESS != State)
    {
        LOG_ERROR(TRUE, "Call system service MMS function %d failed, return %d", LPC_MMS_TAKE_INFOR, State);
    }

    return State;
#endif
}
EXPORT_SYMBOL(caGetMmsInformation);

/**
 * Show the information of memory manager service(only for debug).
 * @return The result of the kernel returned.
 *
 * date           author          notes
 * 2015-01-18     JiangYong       first version
 */
EXPORT E_STATUS caShowMmsInformation(VOID)
{
#if (CONFIG_MEM_DEBUG_ENABLE == FALSE)
    return STATE_NOT_SUPPORT;
#else
    E_STATUS State;
    LPC_REQUEST_PACKET Packet;
    
    State = caSystemCall(&Packet, SMM_MAGIC, LPC_MMS_SHOW_INFOR);

    if (STATE_SUCCESS != State)
    {
        LOG_ERROR(TRUE, "Call system service MMS function %d failed, return %d", LPC_MMS_SHOW_INFOR, State);
    }

    return State;
#endif
}
EXPORT_SYMBOL(caShowMmsInformation);

/**
 * Show the information of sections(only for debug).
 * @param The ID of region to want show.
 * @param The buddy to want show
 * @return The result of the kernel returned.
 *
 * date           author          notes
 * 2015-01-18     JiangYong       first version
 */
EXPORT E_STATUS caShowMmsRegionSection(BYTE RegionID, BYTE Buddy)
{
#if (CONFIG_MEM_DEBUG_ENABLE == FALSE)
    return STATE_NOT_SUPPORT;
#else
    E_STATUS State;
    LPC_REQUEST_PACKET Packet;
    
    Packet.u0.dParam = (RegionID << 8) + Buddy;

    State = caSystemCall(&Packet, SMM_MAGIC, LPC_MMS_SHOW_SECTION);

    if (STATE_SUCCESS != State)
    {
        LOG_ERROR(TRUE, "Call system service MMS function %d failed, return %d", LPC_MMS_SHOW_SECTION, State);
    }

    return State;
#endif
}
EXPORT_SYMBOL(caShowMmsRegionSection);

/**
 * Create new memory region.
 * @param The start address of region.
 * @param The size of region.
 * @return The result of the kernel returned.
 *
 * date           author          notes
 * 2015-01-18     JiangYong       first version
 */
EXPORT E_STATUS caCreateMMRegion(LPVOID lpAddress, SIZE_T Length)
{
    E_STATUS State;
    LPC_REQUEST_PACKET Packet;

    Packet.u0.pParam = lpAddress;
    Packet.u1.dParam = (DWORD) Length;

    State = caSystemCall(&Packet, SMM_MAGIC, LPC_MMS_NEW_REGION);

    if (STATE_SUCCESS != State)
    {
        LOG_ERROR(TRUE, "Call system service MMS function %d failed, return %d", LPC_MMS_NEW_REGION, State);
    }

    return State;
}
EXPORT_SYMBOL(caCreateMMRegion);
#endif

