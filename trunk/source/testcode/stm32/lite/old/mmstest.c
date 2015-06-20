#ifdef BUILD_TESTCODE_MMSTEST
#include <string.h>
#include <stdlib.h>
#include <fadefs.h>
#include <faerror.h>
#include <fatypes.h>
#include <fapi.h>
#include <fapp.h>

#include "ktable.h"
#include "libfw.h"




#define     TEST_CASE_CALL(Func, Execute)                                                   \
            do{                                                                             \
                if (STATE_SUCCESS != Func())                                                \
                {                                                                           \
                    LOG_ERROR(TRUE, "%s ... [FAILED]", #Func);                              \
                    Execute;                                                                \
                }                                                                           \
                else                                                                        \
                {                                                                           \
                    LOG_INFOR(TRUE, "%s ... [SUCCESS]", #Func);                             \
                }                                                                           \
            }while(0)

#define     TEST_CASE_ASSERT(Condition, Execute, ...)                                       \
            do{                                                                             \
                if (!(Condition))                                                           \
                {                                                                           \
                    LOG_ERROR(TRUE, __VA_ARGS__);                                           \
                    Execute;                                                                \
                }                                                                           \
            }while(0)



STATIC E_STATUS TestResult(LPMMS_INFOR lpSource, LPMMS_INFOR lpDestination)
{
    BYTE i = 0;
    TEST_CASE_ASSERT (lpSource->FreePages == lpDestination->FreePages, 
        ShowMmsInformation(); return STATE_SYSTEM_FAULT,
        "Test free pages %d not expect %d.", lpDestination->FreePages, lpSource->FreePages);

    TEST_CASE_ASSERT (lpSource->BuddyMask == lpDestination->BuddyMask, 
        ShowMmsInformation(); return STATE_SYSTEM_FAULT,
        "Test buddy mask 0x%x not expect 0x%x.", lpDestination->BuddyMask, lpSource->BuddyMask);
    
    for (i = 0; i < CONFIG_MEM_BUDDY_MAX; i ++)
    {
        TEST_CASE_ASSERT(lpSource->BuddyCount[i] == lpDestination->BuddyCount[i],
            ShowMmsInformation(); return STATE_SYSTEM_FAULT,
            "Test buddy %d count %d not expect %d.", i, lpDestination->BuddyCount[i], lpSource->BuddyCount[i]);
    }

    return STATE_SUCCESS;
}

STATIC E_STATUS TestCase01(VOID)
{
    E_STATUS State;
    LPVOID lpAddress = NULL;
    MMS_INFOR MmsInfor = {0};
    MMS_INFOR TestInfor = {0};

    MmsInfor.SelfSize = sizeof(MMS_INFOR);
    TestInfor.SelfSize = sizeof(MMS_INFOR);
    State = GetMmsInformation(&MmsInfor);
    TEST_CASE_ASSERT(STATE_SUCCESS == State, ShowMmsInformation(); return State,
        "Get MMS information failed return %d.", State);

    LOG_DEBUG(TRUE, "MmsInfor data : ******");
    LOG_DEBUG(TRUE, "PageSize      :    %u", MmsInfor.PageSize);
    LOG_DEBUG(TRUE, "FreePages     :    %u", MmsInfor.FreePages);
    LOG_DEBUG(TRUE, "TotalPages    :    %u", MmsInfor.TotalPages);


    lpAddress = PageMalloc(CONFIG_MEM_PAGE_SIZE * 3);
    TEST_CASE_ASSERT(lpAddress, ShowMmsInformation(); return STATE_SYSTEM_FAULT, "Malloc %d bytes test array failed.", CONFIG_MEM_PAGE_SIZE * 3);
    PageFree(lpAddress);

    State = GetMmsInformation(&TestInfor);
    TEST_CASE_ASSERT(STATE_SUCCESS == State, ShowMmsInformation(); return State,
        "Get MMS information failed return %d.", State);

    LOG_DEBUG(TRUE, "Alloc Pages   : ******");
    LOG_DEBUG(TRUE, "PageSize      :    %u", TestInfor.PageSize);
    LOG_DEBUG(TRUE, "FreePages     :    %u", TestInfor.FreePages);
    LOG_DEBUG(TRUE, "TotalPages    :    %u", TestInfor.TotalPages);

    return TestResult(&MmsInfor, &TestInfor);
}

/* µ•“≥∑÷≈‰ Õ∑≈≤‚ ‘ */
STATIC E_STATUS TestCase02(VOID)
{
    DWORD i;
    DWORD FreePages = 0;
    LPVOID lpAddress = NULL;
    LPVOID * lpMergeTest = NULL;
    DWORD TotalPages = 0;
    MMS_INFOR MmsInfor = {0};
    MMS_INFOR TestInfor = {0};
    E_STATUS State;

    MmsInfor.SelfSize = sizeof(MMS_INFOR);
    TestInfor.SelfSize = sizeof(MMS_INFOR);
    State = GetMmsInformation(&MmsInfor);
    TEST_CASE_ASSERT(STATE_SUCCESS == State, ShowMmsInformation(); return State,
        "Get MMS information failed return %d.", State);

    LOG_DEBUG(TRUE, "MmsInfor data : ******");
    LOG_DEBUG(TRUE, "PageSize      :    %u", MmsInfor.PageSize);
    LOG_DEBUG(TRUE, "FreePages     :    %u", MmsInfor.FreePages);
    LOG_DEBUG(TRUE, "TotalPages    :    %u", MmsInfor.TotalPages);

    lpAddress = PageMalloc(CONFIG_MEM_PAGE_SIZE * 3);
    TEST_CASE_ASSERT(lpAddress, ShowMmsInformation(); return STATE_SYSTEM_FAULT, "Malloc %d bytes test array failed.", CONFIG_MEM_PAGE_SIZE * 3);
    LOG_DEBUG(TRUE, "System total free pages is %d.", TotalPages);
    lpMergeTest = PageMalloc(sizeof(LPVOID) * GetSystemFreePages(CONFIG_MEM_REGION_MAX));
    TotalPages = GetSystemFreePages(CONFIG_MEM_REGION_MAX);

    LOG_DEBUG(TRUE, "System total free pages is %d.", TotalPages);
    TEST_CASE_ASSERT(lpMergeTest, ShowMmsInformation(); return STATE_SYSTEM_FAULT, "Malloc %d bytes test array failed.", sizeof(LPVOID) * TotalPages);

    memset(lpMergeTest, 0, sizeof(LPVOID) * TotalPages);

    for (i = 0; i < TotalPages; i ++)
    {
        lpMergeTest[i] = PageMalloc(CONFIG_MEM_PAGE_SIZE);
        LOG_DEBUG(TRUE, "Malloc page address %p.", lpMergeTest[i]);
        TEST_CASE_ASSERT(lpMergeTest[i], PageFree(lpMergeTest); ShowMmsInformation(); return STATE_SYSTEM_FAULT, "Page malloc failed %d.", i);

        memset(lpMergeTest[i], 0xa5, CONFIG_MEM_PAGE_SIZE);

        FreePages = GetSystemFreePages(CONFIG_MEM_REGION_MAX);
        TEST_CASE_ASSERT(FreePages == TotalPages - i - 1, PageFree(lpMergeTest); ShowMmsInformation(); return STATE_SYSTEM_FAULT,
            "Free pages %d not expect %d.", FreePages, TotalPages - i);
    }

    TEST_CASE_ASSERT(NULL == PageMalloc(CONFIG_MEM_PAGE_SIZE), PageFree(lpMergeTest); ShowMmsInformation(); return STATE_SYSTEM_FAULT,
        "No space to malloc, but the service return 0x%p.", lpAddress);
    
    for (i = 0; i < TotalPages; i ++)
    {
        PageFree(lpMergeTest[i]);
        FreePages = GetSystemFreePages(CONFIG_MEM_REGION_MAX);
        TEST_CASE_ASSERT(i + 1 == FreePages, PageFree(lpMergeTest); ShowMmsInformation(); return STATE_SYSTEM_FAULT, "Free pages %d not expect %d, lpMergeTest = %p.", FreePages, i, lpMergeTest);
    }

    PageFree(lpMergeTest);
    PageFree(lpAddress);

    State = GetMmsInformation(&TestInfor);
    TEST_CASE_ASSERT(STATE_SUCCESS == State, ShowMmsInformation(); return State,
        "Get MMS information failed return %d.", State);

    LOG_DEBUG(TRUE, "Alloc Pages   : ******");
    LOG_DEBUG(TRUE, "PageSize      :    %u", TestInfor.PageSize);
    LOG_DEBUG(TRUE, "FreePages     :    %u", TestInfor.FreePages);
    LOG_DEBUG(TRUE, "TotalPages    :    %u", TestInfor.TotalPages);


    return TestResult(&MmsInfor, &TestInfor);
}

#if 0
STATIC DWORD GetBuddyCount(LPMMS_INFOR lpInfor, BYTE Buddy)
{
    return lpInfor->BuddyCount[Buddy];
}
#endif

STATIC DWORD GetMaxBuddyCount(LPMMS_INFOR lpInfor)
{
    int id = GetWordHighestBit(lpInfor->BuddyMask);
    return lpInfor->BuddyCount[id >= CONFIG_MEM_BUDDY_MAX ? 0 : id];
}

#if 0
STATIC DWORD GetLowBuddyCount(LPMMS_INFOR lpInfor)
{
    return lpInfor->BuddyCount[GetWordLowestBit(lpInfor->BuddyMask)];
}
#endif


STATIC WORD GetBuddyPages(BYTE Buddy)
{
    return 1 << Buddy;
}


STATIC WORD GetMaxBuddyPages(LPMMS_INFOR lpInfor)
{
    return GetBuddyPages(GetWordHighestBit(lpInfor->BuddyMask));
}

#if 0
STATIC WORD GetLowBuddyPages(LPMMS_INFOR lpInfor)
{
    return GetBuddyPages(GetWordLowestBit(lpInfor->BuddyMask));
}
#endif

STATIC E_STATUS TestCase03(VOID)
{
    E_STATUS State;

    DWORD i;
    MMS_INFOR MmsInfor = {0};
    MMS_INFOR TestInfor = {0};
    LPVOID * lpAddress = NULL;
    LPVOID * lpMergeTest = NULL;

    MmsInfor.SelfSize = sizeof(MMS_INFOR);
    TestInfor.SelfSize = sizeof(MMS_INFOR);
    State = GetMmsInformation(&MmsInfor);
    TEST_CASE_ASSERT(STATE_SUCCESS == State, ShowMmsInformation(); return State,
        "Get MMS information failed return %d.", State);

    LOG_DEBUG(TRUE, "MmsInfor data : ******");
    LOG_DEBUG(TRUE, "PageSize      :    %u", MmsInfor.PageSize);
    LOG_DEBUG(TRUE, "FreePages     :    %u", MmsInfor.FreePages);
    LOG_DEBUG(TRUE, "TotalPages    :    %u", MmsInfor.TotalPages);

    lpAddress = PageMalloc(sizeof(LPVOID) * GetMaxBuddyCount(&MmsInfor));


    TEST_CASE_ASSERT(lpAddress, ShowMmsInformation(); return STATE_SYSTEM_FAULT,
        "Malloc %d bytes test array failed.", sizeof(LPVOID) * GetMaxBuddyCount(&MmsInfor));

    memset(lpAddress, 0, sizeof(LPVOID) * GetMaxBuddyCount(&MmsInfor));

    State = GetMmsInformation(&MmsInfor);
    TEST_CASE_ASSERT(STATE_SUCCESS == State, ShowMmsInformation(); return State,
        "Get MMS information failed return %d.", State);

    for (i = 0; i <  GetMaxBuddyCount(&MmsInfor); i ++)
    {
        lpAddress[i] = PageMalloc(MmsInfor.PageSize * GetMaxBuddyPages(&MmsInfor));
        TEST_CASE_ASSERT(lpAddress[i], PageFree(lpAddress); ShowMmsInformation(); return STATE_SYSTEM_FAULT,
            "Malloc page %d bytes failed.", MmsInfor.PageSize * GetMaxBuddyPages(&MmsInfor));

        State = GetMmsInformation(&TestInfor);
        TEST_CASE_ASSERT(STATE_SUCCESS == State, PageFree(lpAddress); ShowMmsInformation(); return State,
            "Get MMS information failed return %d.", State);

        memset(lpAddress[i], 0, MmsInfor.PageSize * GetMaxBuddyPages(&MmsInfor));

        LOG_DEBUG(TRUE, "AllocMax buddy: ***[%d]***", i);
        LOG_DEBUG(TRUE, "PageSize      :    %u", TestInfor.PageSize);
        LOG_DEBUG(TRUE, "FreePages     :    %u", TestInfor.FreePages);
        LOG_DEBUG(TRUE, "TotalPages    :    %u", TestInfor.TotalPages);

        TEST_CASE_ASSERT (GetMaxBuddyPages(&MmsInfor) != GetMaxBuddyPages(&TestInfor) ||
                          GetMaxBuddyCount(&TestInfor) == GetMaxBuddyCount(&MmsInfor) - i - 1, 
                          PageFree(lpAddress); ShowMmsInformation(); return STATE_SYSTEM_FAULT,
                         "Test max buddy pages %d not expect %d.", GetMaxBuddyCount(&TestInfor), GetMaxBuddyCount(&MmsInfor) - i - 1);
    }

    State = GetMmsInformation(&TestInfor);
    TEST_CASE_ASSERT(STATE_SUCCESS == State, PageFree(lpAddress); ShowMmsInformation(); return State,
        "Get MMS information failed return %d.", State);

    LOG_DEBUG(TRUE, "AllocMax buddy: ******");
    LOG_DEBUG(TRUE, "PageSize      :    %u", TestInfor.PageSize);
    LOG_DEBUG(TRUE, "FreePages     :    %u", TestInfor.FreePages);
    LOG_DEBUG(TRUE, "TotalPages    :    %u", TestInfor.TotalPages);

    TEST_CASE_ASSERT (GetMaxBuddyPages(&TestInfor) != GetMaxBuddyPages(&MmsInfor), 
                      PageFree(lpAddress); ShowMmsInformation(); return STATE_SYSTEM_FAULT,
                      "Test max buddy pages %d not expect != %d.", GetMaxBuddyPages(&TestInfor), GetMaxBuddyPages(&MmsInfor));
    TEST_CASE_ASSERT (TestInfor.FreePages == MmsInfor.FreePages - GetMaxBuddyPages(&MmsInfor) * GetMaxBuddyCount(&MmsInfor),
                      PageFree(lpAddress); ShowMmsInformation(); return STATE_SYSTEM_FAULT,
                      "Test free pages %d not expect %d.", TestInfor.FreePages, MmsInfor.FreePages - GetMaxBuddyPages(&MmsInfor));

    for (i = 0; i < GetMaxBuddyCount(&MmsInfor); i ++)
    {
        State = PageFree(lpAddress[i]);

        TEST_CASE_ASSERT(STATE_SUCCESS == State, PageFree(lpAddress); ShowMmsInformation(); return State,
            "Free page 0x%p failed return %d.", lpAddress[i], State);
        State = GetMmsInformation(&TestInfor);
        TEST_CASE_ASSERT(STATE_SUCCESS == State, PageFree(lpAddress); ShowMmsInformation(); return State,
            "Get MMS information failed return %d.", State);

        LOG_DEBUG(TRUE, "Free max buddy: ***[%d]***", i);
        LOG_DEBUG(TRUE, "PageSize      :    %u", TestInfor.PageSize);
        LOG_DEBUG(TRUE, "FreePages     :    %u", TestInfor.FreePages);
        LOG_DEBUG(TRUE, "TotalPages    :    %u", TestInfor.TotalPages);

        TEST_CASE_ASSERT (GetMaxBuddyCount(&TestInfor) == i + 1, PageFree(lpAddress); ShowMmsInformation(); return STATE_SYSTEM_FAULT,
            "Test max buddy pages %d not expect %d.", GetMaxBuddyCount(&TestInfor), i + 1);
    }

    State = GetMmsInformation(&TestInfor);
    TEST_CASE_ASSERT(STATE_SUCCESS == State, return State,
        "Get MMS information failed return %d.", State);

    LOG_DEBUG(TRUE, "Free max buddy: ******");
    LOG_DEBUG(TRUE, "PageSize      :    %u", TestInfor.PageSize);
    LOG_DEBUG(TRUE, "FreePages     :    %u", TestInfor.FreePages);
    LOG_DEBUG(TRUE, "TotalPages    :    %u", TestInfor.TotalPages);

    TEST_CASE_ASSERT (GetMaxBuddyPages(&TestInfor) == GetMaxBuddyPages(&MmsInfor), ShowMmsInformation(); return STATE_SYSTEM_FAULT,
        "Test max buddy pages %d not expect %d.", GetMaxBuddyPages(&TestInfor), GetMaxBuddyPages(&MmsInfor));

    State = PageFree(lpAddress);
    TEST_CASE_ASSERT(STATE_SUCCESS == State, ShowMmsInformation(); return State,
        "Free Buddy array %p failed, state = %d", State);

    lpAddress = NULL;

    State = GetMmsInformation(&MmsInfor);
    TEST_CASE_ASSERT(STATE_SUCCESS == State, ShowMmsInformation(); return State,
        "Get MMS information failed return %d.", State);

    lpMergeTest = PageMalloc(sizeof(LPVOID) * MmsInfor.FreePages);

    TEST_CASE_ASSERT(lpMergeTest, ShowMmsInformation(); return STATE_SYSTEM_FAULT,
        "Malloc %d bytes test array failed.", sizeof(LPVOID) * MmsInfor.FreePages);
    memset(lpMergeTest, 0, sizeof(LPVOID) * MmsInfor.FreePages);

    State = GetMmsInformation(&MmsInfor);
    TEST_CASE_ASSERT(STATE_SUCCESS == State, ShowMmsInformation(); return State,
        "Get MMS information failed return %d.", State);

    LOG_DEBUG(TRUE, "MmsInfor buddy: ******");
    LOG_DEBUG(TRUE, "PageSize      :    %u", MmsInfor.PageSize);
    LOG_DEBUG(TRUE, "FreePages     :    %u", MmsInfor.FreePages);
    LOG_DEBUG(TRUE, "TotalPages    :    %u", MmsInfor.TotalPages);

    for (i = 0; i < MmsInfor.FreePages; i ++)
    {
        DWORD FreePages = GetSystemFreePages(CONFIG_MEM_REGION_MAX);
        lpMergeTest[i] = PageMalloc(CONFIG_MEM_PAGE_SIZE);
        TEST_CASE_ASSERT(lpMergeTest[i], PageFree(lpMergeTest);
            ShowMmsInformation(); return STATE_SYSTEM_FAULT, "Page malloc failed %d.", i);

        memset(lpMergeTest[i], 0xa5, CONFIG_MEM_PAGE_SIZE);

        LOG_DEBUG(TRUE, "Page malloc %p idx %d ... successfully.", lpMergeTest[i], i);

        TEST_CASE_ASSERT(GetSystemFreePages(CONFIG_MEM_REGION_MAX) == FreePages - 1, 
            PageFree(lpMergeTest); ShowMmsInformation(); return STATE_SYSTEM_FAULT,
            "Free pages %d not expect %d.", GetSystemFreePages(CONFIG_MEM_REGION_MAX), FreePages - 1);
    }


    State = GetMmsInformation(&TestInfor);
    TEST_CASE_ASSERT(STATE_SUCCESS == State, PageFree(lpMergeTest); ShowMmsInformation(); return State,
        "Get MMS information failed return %d.", State);

    LOG_DEBUG(TRUE, "Alloc Pages   : ******");
    LOG_DEBUG(TRUE, "PageSize      :    %u", TestInfor.PageSize);
    LOG_DEBUG(TRUE, "FreePages     :    %u", TestInfor.FreePages);
    LOG_DEBUG(TRUE, "TotalPages    :    %u", TestInfor.TotalPages);

    TEST_CASE_ASSERT (0 == GetMaxBuddyPages(&TestInfor),
        ShowMmsInformation(); PageFree(lpMergeTest); ShowMmsInformation(); return STATE_SYSTEM_FAULT,
        "Test max buddy pages %d not expect %d, free pages is %d.",
        GetMaxBuddyPages(&TestInfor), 0, GetSystemFreePages(CONFIG_MEM_REGION_MAX) );

    for (i = 0; i < MmsInfor.FreePages; i ++)
    {
        DWORD FreePages = GetSystemFreePages(CONFIG_MEM_REGION_MAX);
        State = PageFree(lpMergeTest[i]);
        TEST_CASE_ASSERT(STATE_SUCCESS == State,
            PageFree(lpMergeTest); ShowMmsInformation(); return State,
            "Free page 0x%p idx %d failed return %d.", lpMergeTest[i], i, State);
        TEST_CASE_ASSERT(GetSystemFreePages(CONFIG_MEM_REGION_MAX) == FreePages + 1,
            PageFree(lpMergeTest); ShowMmsInformation(); return STATE_SYSTEM_FAULT,
            "Free pages %d not expect %d.", GetSystemFreePages(CONFIG_MEM_REGION_MAX), FreePages + 1);
        LOG_DEBUG(TRUE, "Page Free %p idx %d ... successfully.", lpMergeTest[i], i);
    }

    State = GetMmsInformation(&TestInfor);
    TEST_CASE_ASSERT(STATE_SUCCESS == State, ShowMmsInformation(); return State,
        "Get MMS information failed return %d.", State);

    LOG_DEBUG(TRUE, "Alloc Pages   : ******");
    LOG_DEBUG(TRUE, "PageSize      :    %u", TestInfor.PageSize);
    LOG_DEBUG(TRUE, "FreePages     :    %u", TestInfor.FreePages);
    LOG_DEBUG(TRUE, "TotalPages    :    %u", TestInfor.TotalPages);

    return TestResult(&MmsInfor, &TestInfor);
}

STATIC E_STATUS TestCase04(VOID)
{

    E_STATUS State;

    DWORD i = 0, Tick0, Tick1;
    MMS_INFOR MmsInfor = {0};
    MMS_INFOR TestInfor = {0};
    LPVOID * lpAddress = NULL;
    LPVOID * lpArray = NULL;

    MmsInfor.SelfSize = sizeof(MMS_INFOR);
    TestInfor.SelfSize = sizeof(MMS_INFOR);
    State = GetMmsInformation(&MmsInfor);
    TEST_CASE_ASSERT(STATE_SUCCESS == State, ShowMmsInformation(); return State,
        "Get MMS information failed return %d.", State);


    if (0 == (MmsInfor.BuddyMask & 1))
    {
        lpArray = PageMalloc(MmsInfor.PageSize);
    }

    Tick0 = FW_GetTickCount();
    lpAddress = PageMalloc(MmsInfor.PageSize);
    Tick1 = FW_GetTickCount();

    LOG_INFOR(TRUE, "Allocate page %p tick %d to %d, time is %d, Total pages is %d.",
        lpAddress, Tick0, Tick1, Tick0 - Tick1, MmsInfor.TotalPages);

    Tick0 = FW_GetTickCount();
    PageFree(lpAddress);
    Tick1 = FW_GetTickCount();
    LOG_INFOR(TRUE, "Free page %p tick %d to %d, time is %d.",
        lpAddress, Tick0, Tick1, Tick0 - Tick1);

    if (lpArray)
    {
        PageFree(lpArray);
    }

    lpArray = PageMalloc(sizeof(LPVOID) * (MmsInfor.TotalPages + 1));

    TEST_CASE_ASSERT(lpArray, ShowMmsInformation(); return STATE_SYSTEM_FAULT,
        "Malloc %d bytes test array failed.", sizeof(LPVOID) * (MmsInfor.TotalPages + 1));

    memset(lpArray, 0, sizeof(LPVOID) * (MmsInfor.TotalPages + 1));

    for (i = 0; i < MmsInfor.TotalPages; i ++)
    {
        State = GetMmsInformation(&TestInfor);
        TEST_CASE_ASSERT(STATE_SUCCESS == State, ShowMmsInformation(); return State,
            "Get MMS information failed return %d.", State);

        LOG_DEBUG(TRUE, " MMS Buddy mask infor is 0x%04x, must be alloc %d pages.",
            TestInfor.BuddyMask, (1 << GetWordLowestBit(TestInfor.BuddyMask)));

        if (0 != (TestInfor.BuddyMask & (TestInfor.BuddyMask-1)))
        {
            
            lpArray[i] = PageMalloc((1 << GetWordLowestBit(TestInfor.BuddyMask)) * TestInfor.PageSize);
            
        }
        else break;
    }

    State = GetMmsInformation(&TestInfor);
    TEST_CASE_ASSERT(STATE_SUCCESS == State, ShowMmsInformation(); return State,
        "Get MMS information failed return %d.", State);

    Tick0 = FW_GetTickCount();
    lpArray[MmsInfor.TotalPages] = PageMalloc(MmsInfor.PageSize);
    Tick1 = FW_GetTickCount();

    LOG_INFOR(TRUE, "Allocate %d pages address %p tick %d to %d, time is %d.",
        1, lpArray[MmsInfor.TotalPages], Tick0, Tick1, Tick0 - Tick1);

    TEST_CASE_ASSERT(lpArray[MmsInfor.TotalPages], ShowMmsInformation(); return State,
        "Malloc %u bytes page failed.", MmsInfor.PageSize);

    Tick0 = FW_GetTickCount();
    State = PageFree(lpArray[MmsInfor.TotalPages]);
    Tick1 = FW_GetTickCount();

    LOG_INFOR(TRUE, "Free %d pages adfdress %p tick %d to %d, time is %d.",
        1, lpArray[MmsInfor.TotalPages], Tick0, Tick1, Tick0 - Tick1);

    TEST_CASE_ASSERT(STATE_SUCCESS == State, ShowMmsInformation(); return State,
        "Free page %p failed, result %d.", lpArray[MmsInfor.TotalPages], State);


    Tick0 = FW_GetTickCount();
    lpArray[MmsInfor.TotalPages] = PageMalloc((TestInfor.BuddyMask - 1) * MmsInfor.PageSize);
    Tick1 = FW_GetTickCount();

    LOG_INFOR(TRUE, "Allocate %d pages address %p tick %d to %d, time is %d.",
        (TestInfor.BuddyMask - 1), lpArray[MmsInfor.TotalPages], Tick0, Tick1, Tick0 - Tick1);

    TEST_CASE_ASSERT(lpArray[MmsInfor.TotalPages], ShowMmsInformation(); return State,
        "Malloc %u bytes page failed.", (TestInfor.BuddyMask - 1) * MmsInfor.PageSize);

    Tick0 = FW_GetTickCount();
    State = PageFree(lpArray[MmsInfor.TotalPages]);
    Tick1 = FW_GetTickCount();

    LOG_INFOR(TRUE, "Free %d pages adfdress %p tick %d to %d, time is %d.",
        (TestInfor.BuddyMask - 1) , lpArray[MmsInfor.TotalPages], Tick0, Tick1, Tick0 - Tick1);

    TEST_CASE_ASSERT(STATE_SUCCESS == State, ShowMmsInformation(); return State,
        "Free page %p failed, result %d.", lpArray[MmsInfor.TotalPages], State);

    for (i = 0; i < MmsInfor.TotalPages; i ++)
    {
        if (lpArray[i]) PageFree(lpArray[i]);
    }

    PageFree(lpArray);

    State = GetMmsInformation(&TestInfor);
    TEST_CASE_ASSERT(STATE_SUCCESS == State, ShowMmsInformation(); return State,
        "Get MMS information failed return %d.", State);

    return TestResult(&MmsInfor, &TestInfor);
}


E_STATUS FansMain(VOID)
{
    TEST_CASE_CALL(TestCase01, return STATE_SYSTEM_FAULT);
    TEST_CASE_CALL(TestCase02, return STATE_SYSTEM_FAULT);
    TEST_CASE_CALL(TestCase03, return STATE_SYSTEM_FAULT);
    TEST_CASE_CALL(TestCase04, return STATE_SYSTEM_FAULT);

    //while(1);
    return STATE_SUCCESS;
}

#endif
