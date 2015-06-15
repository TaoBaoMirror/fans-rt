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
#include <fapi.h>

#include "libcal.h"
#include "cadebug.h"



#if (CONFIG_MEM_REGION_MAX != 0)
FANSAPI LPVOID PageMalloc(SIZE_T Length)
{
    return ntPageMalloc(Length);
}
EXPORT_SYMBOL(PageMalloc);


FANSAPI E_STATUS PageFree(LPVOID lpPageAddress)
{
    return ntPageFree(lpPageAddress);
}
EXPORT_SYMBOL(PageFree);

FANSAPI DWORD GetSystemFreePages(BYTE RegionID)
{
    return ntGetSystemFreePages(RegionID);
}
EXPORT_SYMBOL(GetSystemFreePages);

FANSAPI E_STATUS GetMmsInformation(LPMMS_INFOR lpInfor)
{
    return ntGetMmsInformation(lpInfor);
}
EXPORT_SYMBOL(GetMmsInformation);

FANSAPI E_STATUS ShowMmsInformation(VOID)
{
    return ntShowMmsInformation();
}
EXPORT_SYMBOL(ShowMmsInformation);

FANSAPI E_STATUS ShowMmsRegionSection(BYTE RegionID, BYTE Buddy)
{
    return ntShowMmsRegionSection(RegionID, Buddy);
}
EXPORT_SYMBOL(ShowMmsRegionSection);

FANSAPI E_STATUS CreateMMRegion(LPVOID lpAddress, SIZE_T Length)
{
    return ntCreateMMRegion(lpAddress, Length);
}
EXPORT_SYMBOL(CreateMMRegion);

#endif

