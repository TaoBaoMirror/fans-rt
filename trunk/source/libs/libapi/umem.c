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
#include <fauser.h>
#include <fadefs.h>
#include <faerror.h>
#include <fatypes.h>
#include <fapi.h>

#include "libcal.h"
#include "cadebug.h"



#if (CONFIG_MEM_REGION_MAX != 0)
FANSAPI RO_CODE LPVOID PageMalloc(SIZE_T Length)
{
    return caPageMalloc(Length);
}
EXPORT_SYMBOL(PageMalloc);


FANSAPI RO_CODE E_STATUS PageFree(LPVOID lpPageAddress)
{
    return caPageFree(lpPageAddress);
}
EXPORT_SYMBOL(PageFree);

FANSAPI RO_CODE DWORD GetSystemFreePages(BYTE RegionID)
{
    return caGetSystemFreePages(RegionID);
}
EXPORT_SYMBOL(GetSystemFreePages);

FANSAPI RO_CODE E_STATUS GetMmsInformation(LPMMS_INFOR lpInfor)
{
    return caGetMmsInformation(lpInfor);
}
EXPORT_SYMBOL(GetMmsInformation);

FANSAPI RO_CODE E_STATUS ShowMmsInformation(VOID)
{
    return caShowMmsInformation();
}
EXPORT_SYMBOL(ShowMmsInformation);

FANSAPI RO_CODE E_STATUS ShowMmsRegionSection(BYTE RegionID, BYTE Buddy)
{
    return caShowMmsRegionSection(RegionID, Buddy);
}
EXPORT_SYMBOL(ShowMmsRegionSection);

FANSAPI RO_CODE E_STATUS CreateMMRegion(LPVOID lpAddress, SIZE_T Length)
{
    return caCreateMMRegion(lpAddress, Length);
}
EXPORT_SYMBOL(CreateMMRegion);

#endif

