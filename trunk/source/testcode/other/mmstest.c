#include <fadefs.h>
#include <faklist.h>
#include <faerror.h>
#include <fatypes.h>
#include <fanscore.h>
#include <kdebug.h>
#include <fapi.h>
#include <fapp.h>

#ifdef _MSC_VER
E_STATUS RegionEnumerateUsedSection(BYTE RegionID);
#else
#define RegionEnumerateUsedSection(ID)              do{}while(0)
#endif

E_STATUS FansMain(VOID)
{
    DWORD Tick0, Tick1, i;
    LPVOID lpAddress = NULL;
    LPVOID lpMergeTest[10];

    Tick0 = FW_GetTickCount();
    lpAddress = PageMalloc(CONFIG_MMS_PAGE_SIZE);
    Tick1 = FW_GetTickCount();
    CORE_INFOR(TRUE, "Allocate page %p tick %d to %d, long is %d", lpAddress, Tick0, Tick1, Tick0 - Tick1);
    
    Tick0 = FW_GetTickCount();
    PageFree(lpAddress);
    Tick1 = FW_GetTickCount();
    CORE_INFOR(TRUE, "Free page tick %d to %d, long is %d", Tick0, Tick1, Tick0 - Tick1);

    for (i = 0; i < sizeof(lpMergeTest)/ sizeof(lpMergeTest[0]); i ++)
    {
        lpMergeTest[i] = PageMalloc(CONFIG_MMS_PAGE_SIZE);
    }

    for (i = 0; i < sizeof(lpMergeTest)/ sizeof(lpMergeTest[0]); i ++)
    {
        PageFree(lpMergeTest[i]);
    }

    lpAddress = PageMalloc(CONFIG_MMS_PAGE_SIZE * 128);

    CORE_INFOR(TRUE, "Allocate 4 pages is %p.", lpAddress);

    return STATE_SUCCESS;
}

