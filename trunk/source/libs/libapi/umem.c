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
FANSAPI CODE_TEXT LPVOID PageMalloc(SIZE_T Length)
{
    return caPageMalloc(Length);
}
EXPORT_SYMBOL(PageMalloc);


FANSAPI CODE_TEXT E_STATUS PageFree(LPVOID lpPageAddress)
{
    return caPageFree(lpPageAddress);
}
EXPORT_SYMBOL(PageFree);

FANSAPI CODE_TEXT DWORD GetSystemFreePages(BYTE RegionID)
{
    return caGetSystemFreePages(RegionID);
}
EXPORT_SYMBOL(GetSystemFreePages);

FANSAPI CODE_TEXT E_STATUS GetMmsInformation(LPMMS_INFOR lpInfor)
{
    return caGetMmsInformation(lpInfor);
}
EXPORT_SYMBOL(GetMmsInformation);

FANSAPI CODE_TEXT E_STATUS ShowMmsInformation(VOID)
{
    return caShowMmsInformation();
}
EXPORT_SYMBOL(ShowMmsInformation);

FANSAPI CODE_TEXT E_STATUS ShowMmsRegionSection(BYTE RegionID, BYTE Buddy)
{
    return caShowMmsRegionSection(RegionID, Buddy);
}
EXPORT_SYMBOL(ShowMmsRegionSection);

FANSAPI CODE_TEXT E_STATUS CreateMMRegion(LPVOID lpAddress, SIZE_T Length)
{
    return caCreateMMRegion(lpAddress, Length);
}
EXPORT_SYMBOL(CreateMMRegion);

#endif

