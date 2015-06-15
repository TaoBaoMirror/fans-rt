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
#include <stdio.h>
#include <string.h>
#include <fadefs.h>
#include <faerror.h>
#include <fatypes.h>

#include "klpc.h"
#include "kdebug.h"
#include "kcore.h"
#include "ktable.h"
#include "kboard.h"

#if (CONFIG_MEM_REGION_MAX != 0)
//#define MMS_STATIC_REGION_ADD_SECTION
//#define MMS_STATIC_BUDDY_SPLIT_SECTION
//#define MMS_STATIC_BUDDY_MERGE_SECTION


#if (CONFIG_MEM_DEBUG_ENABLE == TRUE)
#define     MM_DEBUG(Enter, ...)//                          CORE_DEBUG(Enter, __VA_ARGS__)
#define     MM_INFOR(Enter, ...)                            CORE_INFOR(Enter, __VA_ARGS__)
#define     MM_ERROR(Enter, ...)                            CORE_ERROR(Enter, __VA_ARGS__)
#ifdef _MSC_VER
#define     MM_ASSERT(condition, code, ...)                 FANS_ASSERT(condition, code, __VA_ARGS__)
#else
#define     MM_ASSERT(condition, code, ...)
#endif
#else
#define     MM_DEBUG(...)
#define     MM_INFOR(...)
#define     MM_ERROR(...)
#define     MM_ASSERT(condition, code, ...)
#endif

#define     MAX_PAGES                   0x4000
#define     INVALID_PAGE_ID             0x3fff
#define     PAGE_ID_MASK                0x3fff

#define     PAGE_HEAD_BIT_MASK          0x00008000
#define     PAGE_HEAD_BIT_SHIFT         15
#define     PAGE_MANY_BIT_MASK          0x00004000
#define     PAGE_MANY_BIT_SHIFT         14
#define     PAGE_USED_BIT_MASK          0x40000000
#define     PAGE_USED_BIT_SHIFT         30
#define     PAGE_PRVEPAGE_MASK          0x00003fff
#define     PAGE_PRVEPAGE_SHIFT         0
#define     PAGE_NEXTPAGE_MASK          0x3fff0000
#define     PAGE_NEXTPAGE_SHIFT         16
#define     PAGE_HEADPAGES_MASK         0x3fff0000
#define     PAGE_HEADPAGES_SHIFT        16
#define     PAGE_HEADPAGES_VALUE(x)     (((x) & PAGE_HEADPAGES_MASK) >> PAGE_HEADPAGES_SHIFT)
#define     PAGE_TAILPAGES_MASK         0x00003fff
#define     PAGE_TAILPAGES_SHIFT        0

#define     PAGE_SECTFLAGS_INVALID      0
#define     PAGE_HEAD_MANY_MASK         (PAGE_HEAD_BIT_MASK | PAGE_MANY_BIT_MASK)
#define     PAGE_HEAD_MANY_VALUE(x)     (((x) & PAGE_HEAD_MANY_MASK))
#define     PAGE_IS_VALID(x)            (0 != PAGE_HEAD_MANY_VALUE(x))
#define     PAGE_IS_FINAL(x)            (PAGE_HEAD_MANY_MASK != PAGE_HEAD_MANY_VALUE(x))
#define     PAGE_IS_START(x)            ((x) & PAGE_HEAD_BIT_MASK)
#define     PAGE_IS_SELF(x)             (!((x) & PAGE_MANY_BIT_MASK))
#define     PAGE_IS_USE_HEAD(x)         ((PAGE_HEAD_BIT_MASK | PAGE_USED_BIT_MASK) == ((x) & (PAGE_HEAD_BIT_MASK | PAGE_USED_BIT_MASK)))

#define     PAGE_SECTFLAGS_MASK         (PAGE_HEAD_BIT_MASK | PAGE_MANY_BIT_MASK)
#define     PAGE_SECTFLAGS_SHIFT        (PAGE_HEAD_BIT_SHIFT < PAGE_MANY_BIT_SHIFT ? PAGE_HEAD_BIT_SHIFT : PAGE_MANY_BIT_SHIFT)

typedef     WORD                    PAGE_ID_T;
typedef     WORD *                  PPAGE_ID_T;
typedef     WORD FAR *              LPPAGE_ID_T;


typedef union tagMM_PAGE_ENTRIES MM_PAGE_ENTRIES;
typedef union tagMM_PAGE_ENTRIES * PMM_PAGE_ENTRIES;
typedef union tagMM_PAGE_ENTRIES FAR * LPMM_PAGE_ENTRIES;

#ifdef _MSC_VER
#pragma  pack (push,1) 
#endif
/************************************************************************
  Many 和 Head 组合方式
  Many = 0 Head = 0                 无效首尾页  Flags = 0
  Many = 0 Head = 1                 单页SECTION  Flags = 1
  Many = 1 Head = 0                 SECTION尾页  Flags = 2
  Many = 1 Head = 1                 SECTION首页  Flags = 3
************************************************************************/

typedef struct tagMM_HEAD_PAGE{
    DWORD               PrveSection:14;                 /**< 上一个节 */
    DWORD               ManyBoolean:1;                  /**< 多页标志 */
    DWORD               HeadBoolean:1;                  /**< 首页标志 */
    DWORD               SectionPages:14;                /**< 页数量 */
    DWORD               UsedBoolean:1;                  /**< 使用标志 */
    DWORD               ReservedBit:1;                  /**< 保留 */
}__attribute__ ((packed)) MM_HEAD_PAGE, * PMM_HEAD_PAGE, FAR * LPMM_HEAD_PAGE;

typedef struct tagMM_TAIL_PAGE{
    DWORD               SectionPages:14;                /**< 页数量 */
    DWORD               ManyBoolean:1;                  /**< 多页标志 */
    DWORD               HeadBoolean:1;                  /**< 首页标志 */
    DWORD               NextSection:14;                 /**< 下一个节 */
    DWORD               UsedBoolean:1;                  /**< 使用标志 */
    DWORD               ReservedBit:1;                  /**< 保留 */
}__attribute__ ((packed)) MM_TAIL_PAGE, * PMM_TAIL_PAGE, FAR * LPMM_TAIL_PAGE;

typedef struct tagMM_SELF_PAGE{
    DWORD               PrveSection:14;                 /**< 上一个节 */
    DWORD               ManyBoolean:1;                  /**< 多页标志 */
    DWORD               HeadBoolean:1;                  /**< 首页标志 */
    DWORD               NextSection:14;                 /**< 下一个节 */
    DWORD               UsedBoolean:1;                  /**< 使用标志 */
    DWORD               ReservedBit:1;                  /**< 链表首尾标志 */
}__attribute__ ((packed)) MM_SELF_PAGE, * PMM_SELF_PAGE, FAR * LPMM_SELF_PAGE;

union tagMM_PAGE_ENTRIES{
    MM_HEAD_PAGE        HeadPage;
    MM_TAIL_PAGE        TailPage;
    MM_SELF_PAGE        SelfPage;
    DWORD               Entries;
}__attribute__ ((packed));

#ifdef _MSC_VER
#pragma  pack (pop) 
#endif

#if (CONFIG_MEM_REGION_MAX > 16)
typedef     DWORD   BUDDY_MAP_T;
#elif (CONFIG_MEM_REGION_MAX > 8)
typedef     WORD    BUDDY_MAP_T;
#else
typedef     BYTE    BUDDY_MAP_T;
#endif

typedef struct tagMM_REGION{
    DWORD               Magic;
    MMADDR              RegionStart;                                 /**< 该区域的起始地址 */
    BOOL                RegionOnChip;                                /**< 该区域为片上内存 */
    WORD                RegionPages;                                 /**< 该区域的页数量*/
    WORD                RegionFrees;                                 /**< 该区域空闲页数*/
    WORD                RegionUsedCount;                             /**< 该区域非空闲节数量 */
    MM_PAGE_ENTRIES     RegionUsedTable;                             /**< 该区域非空闲节链表 */
    MM_PAGE_ENTRIES     RegionBuddyTable[CONFIG_MEM_BUDDY_MAX];      /**< 该区域伙伴链表 */
    WORD                RegionBuddyCount[CONFIG_MEM_BUDDY_MAX];      /**< 该区域伙伴表计数器 */
    LPMM_PAGE_ENTRIES   lpRegionPageTable;                           /**< 该区域页表首指针 */
}MM_REGION, * PMM_REGION, FAR * LPMM_REGION;

typedef struct tagMM_MANAGER{
    DWORD               ManagerPages;                                /**< MM 管理的总页数 */
    BYTE                ReservedByte;                                /**< 保留 */
    BYTE                RegisterRegions;                             /**< 已注册到系统的区域数 */
    WORD                ManagerBuddyMap;                             /**< 系统伙伴位图 */
#if (CONFIG_MEM_REGION_MAX > 1)
    BUDDY_MAP_T         RegionBuddyMap[CONFIG_MEM_BUDDY_MAX+1];      /**< 区域伙伴位图 */
#endif
    MM_REGION           RegionTable[CONFIG_MEM_REGION_MAX];          /**< 区域表 */
}MM_MANAGER, * PMM_MANAGER, FAR * LPMM_MANAGER;

STATIC  MM_MANAGER              g_SystemMemoryManager       =   {0};
STATIC  CONST WORD              g_SectionBuddyMaskTable[]   =
/* The size of g_SectionBuddyMaskTable is 32 bytes */
{
    0xffff, 0xfffe, 0xfffc, 0xfff8, 0xfff0, 0xffe0, 0xffc0, 0xff80, /* Buddy 0x00 ~ 0x07 */
    0xff00, 0xfe00, 0xfc00, 0xf800, 0xf000, 0xe000, 0xc000, 0x8000, /* Buddy 0x08 ~ 0x0f */
    0x0000,                                                         /* Invalid buddy */
};

#if (CONFIG_MEM_STATIC_PAGETABLE == TRUE)
STATIC  MM_PAGE_ENTRIES         g_OnchipPageTable[CONFIG_MEM_ONCHIP_PAGES];
#endif

STATIC INLINE WORD GetBuddyMask(BYTE Buddy)
{
    return g_SectionBuddyMaskTable[Buddy & 0xf];
}


/*******************************************************************************************
 *  函 数 名：GetPagesBuddy
 *  功    能：根据 PAGE 数量计算 Buddy
 *  参    数：WORD         Pages        页数量
 *  返 回 值：BYTE                      返回Buddy
 *  修改历史：
 *      1.修改时间：2014-10-26
 *        作    者: 姜勇
 *        修改内容: 创建函数
 *  实现说明：
 *        无论 Pages 为任何值，一定可以将Pages划分为若干个 2 的整数(记为N)次幂之和。
 *        如果 Pages 刚好为 2 的 N 次幂则 Buddy = N
 *        如果 Pages 大于 2 的 N 次幂则 Buddy = N + 1
 *        例如
 *              当 Pages 等于 8  时  8 = 1^3  所以 Buddy = 3
 *              当 Pages 等于 9  时  9 = 2^3 + 2^0  所以 Buddy = 3 + 1
 *              当 Pages 等于 15 时 15 = 2^3 + 2^2 + 2^1 + 2^0  所以 Buddy = 3 + 1
 *        即：去掉 Pages 的最高位之后，其值不为 0 时，Buddy = N + 1
 *******************************************************************************************/
STATIC INLINE BYTE GetPagesBuddy(WORD Pages)
{
    BYTE Buddy = GetWordHighestBit(Pages);
    return (BYTE) (Buddy + (TRUE && (Pages ^ (1 << Buddy))));
}

/*******************************************************************************************
 *  函 数 名：GetSplitBuddy
 *  功    能：根据起始页面号和页面数计算出当前SECTION应当挂到哪个BUDDY TABLE上
 *  参    数：PAGE_ID_T    FirstPage    页面号
 *             WORD          Pages       页面数
 *  返 回 值：BYTE                      返回Buddy
 *  修改历史：
 *      1.修改时间：2014-10-26
 *        作    者: 姜勇
 *        修改内容: 创建函数
 *  实现说明：
 *        分别计算起始页面号和剩余页面数量的 BUDDY 值，然后返回最小值
 *******************************************************************************************/
STATIC INLINE BYTE GetSplitBuddy(PAGE_ID_T FirstPage, WORD Pages)
{
    BYTE FirstBuddy = GetWordLowestBit(FirstPage);
    BYTE PagesBuddy = GetWordHighestBit(Pages);
    BYTE TableBuddy = MIN(FirstBuddy, PagesBuddy);

    return MIN(TableBuddy, CONFIG_MEM_BUDDY_MAX - 1);
}

/*******************************************************************************************
 *  函 数 名：GetMergePageID
 *  功    能：根据起始页面号和页面数计算出计算出需要合并的块的首页或者尾页地址
 *  参    数：PAGE_ID_T    FirstPage    页面号
 *             WORD          Pages       页面数
 *  返 回 值：BYTE                      返回Buddy
 *  修改历史：
 *      1.修改时间：2014-10-26
 *        作    者: 姜勇
 *        修改内容: 创建函数
 *******************************************************************************************/
STATIC INLINE PAGE_ID_T GetMergePageID(PAGE_ID_T FirstPage, WORD Pages)
{
    return (FirstPage & (~Pages)) == FirstPage ? FirstPage + Pages : FirstPage - 1;
}

#if ((CONFIG_MEM_STATIC_FUNCTION == TRUE) && (CONFIG_MEM_DEBUG_ENABLE == TRUE))
STATIC INLINE VOID SetPrevPage(LPMM_HEAD_PAGE lpHead, PAGE_ID_T Page)
{
    lpHead->PrveSection = Page;
}

STATIC INLINE VOID SetNextPage(LPMM_TAIL_PAGE lpTail, PAGE_ID_T Page)
{
    lpTail->NextSection = Page;
}

STATIC INLINE VOID SelfPageInit(LPMM_PAGE_ENTRIES lpEntries, PAGE_ID_T PrvePage, PAGE_ID_T NextPage, BOOL Used)
{
    lpEntries->Entries = (((DWORD)(Used)) << PAGE_USED_BIT_SHIFT)
        + (((NextPage << PAGE_NEXTPAGE_SHIFT) + (PrvePage << PAGE_PRVEPAGE_SHIFT))
        & (PAGE_NEXTPAGE_MASK | PAGE_PRVEPAGE_MASK))
        + (PAGE_HEAD_BIT_MASK);
}


STATIC INLINE VOID HeadPageInit(LPMM_PAGE_ENTRIES lpEntries, PAGE_ID_T PrvePage, WORD Pages, BOOL Used)
{
    lpEntries->Entries = (((DWORD)(Used)) << PAGE_USED_BIT_SHIFT)
        + (((PrvePage << PAGE_PRVEPAGE_SHIFT) + ((Pages - 1) << PAGE_HEADPAGES_SHIFT))
        & (PAGE_PRVEPAGE_MASK | PAGE_HEADPAGES_MASK))
        + (PAGE_MANY_BIT_MASK + PAGE_HEAD_BIT_MASK);
}

STATIC INLINE VOID TailPageInit(LPMM_PAGE_ENTRIES lpEntries, PAGE_ID_T NextPage, WORD Pages, BOOL Used)
{
    lpEntries->Entries = (((DWORD)(Used)) << PAGE_USED_BIT_SHIFT)
        + (((NextPage << PAGE_NEXTPAGE_SHIFT) + ((Pages - 1) << PAGE_TAILPAGES_SHIFT))
        & (PAGE_NEXTPAGE_MASK | PAGE_TAILPAGES_MASK))
        + (PAGE_MANY_BIT_MASK);
}

STATIC PAGE_ID_T StartPage2FinalPage(LPMM_REGION lpRegion, PAGE_ID_T PageID)
{
    MM_ASSERT(PAGE_IS_START(lpRegion->lpRegionPageTable[PageID].Entries), SYSTEM_CALL_OOPS();, "Page %u is not a start page !", PageID);

    return ((PAGE_ID_T) (PAGE_IS_SELF(lpRegion->lpRegionPageTable[PageID].Entries)
        ? (PageID) : ((PageID) + lpRegion->lpRegionPageTable[PageID].HeadPage.SectionPages)));
}

STATIC PAGE_ID_T FinalPage2StartPage(LPMM_REGION lpRegion, PAGE_ID_T PageID)
{
    MM_ASSERT(PAGE_IS_FINAL(lpRegion->lpRegionPageTable[PageID].Entries), SYSTEM_CALL_OOPS();, "Page %u is not a final page !", PageID);

    return ((PAGE_ID_T) (PAGE_IS_SELF(lpRegion->lpRegionPageTable[PageID].Entries)
        ? (PageID) : (PageID - lpRegion->lpRegionPageTable[PageID].TailPage.SectionPages)));
}



STATIC PAGE_ID_T GetNextSectionStartID(LPMM_REGION lpRegion, PAGE_ID_T PageID)
{
    MM_ASSERT(PAGE_IS_VALID(lpRegion->lpRegionPageTable[PageID].Entries), SYSTEM_CALL_OOPS();,
        "Page %u is not a valid page, entries is 0x%08x!", PageID, lpRegion->lpRegionPageTable[PageID].Entries);
    MM_ASSERT(PAGE_IS_FINAL(lpRegion->lpRegionPageTable[PageID].Entries), SYSTEM_CALL_OOPS();,
        "Page %u is not a final page, entries is 0x%08x!", PageID, lpRegion->lpRegionPageTable[PageID].Entries);

    return ((PAGE_ID_T) (lpRegion->lpRegionPageTable[PageID].SelfPage.NextSection));
}




STATIC PAGE_ID_T GetPrveSectionFinalID(LPMM_REGION lpRegion, PAGE_ID_T PageID)
{
    MM_ASSERT(PAGE_IS_START(lpRegion->lpRegionPageTable[PageID].Entries), SYSTEM_CALL_OOPS();,
        "Page %u is not a start page, entries is 0x%08x !", PageID, lpRegion->lpRegionPageTable[PageID].Entries);

    return ((PAGE_ID_T) lpRegion->lpRegionPageTable[PageID].SelfPage.PrveSection);
}

STATIC WORD GetStartEntriesPages(LPMM_REGION lpRegion, PAGE_ID_T PageID)
{
    return ((PAGE_ID_T) (PAGE_IS_SELF(lpRegion->lpRegionPageTable[PageID].Entries)
        ? 1 : lpRegion->lpRegionPageTable[PageID].HeadPage.SectionPages + 1));
}

STATIC WORD GetFinalEntriesPages(LPMM_REGION lpRegion, PAGE_ID_T PageID)
{
    return (PAGE_ID_T) (PAGE_IS_SELF(lpRegion->lpRegionPageTable[PageID].Entries)
        ? 1 : lpRegion->lpRegionPageTable[PageID].TailPage.SectionPages + 1);
}

STATIC INLINE BOOL NotPageAddress(MMADDR Address)
{
    return (0 != ((Address) & (CONFIG_MEM_PAGE_SIZE - 1)));
}

STATIC INLINE LPMM_REGION GetRegionObject(LPMM_MANAGER lpManager, BYTE RegionID)
{
    return &lpManager->RegionTable[RegionID];
}

STATIC INLINE LPVOID GetPageAddress(LPMM_MANAGER lpManager, BYTE RegionID, PAGE_ID_T PageID)
{
    return (LPVOID)(lpManager->RegionTable[RegionID].RegionStart + PageID * CONFIG_MEM_PAGE_SIZE);
}

STATIC INLINE BYTE GetRegisterRegions(LPMM_MANAGER lpManager)
{
    return lpManager->RegisterRegions;
}

STATIC INLINE BYTE AddRegisterRegions(LPMM_MANAGER lpManager)
{
    return lpManager->RegisterRegions ++;
}

STATIC INLINE VOID SetManagerBuddyBitmap(LPMM_MANAGER lpManager, BYTE Buddy, BOOL Value)
{
    SET_BIT_VALUE(lpManager->ManagerBuddyMap, Buddy, Value);
}

STATIC INLINE VOID AddManagerBuddyBitmap(LPMM_MANAGER lpManager, WORD Mask)
{
    ((lpManager)->ManagerBuddyMap) |= (Mask);
}

STATIC INLINE VOID SubManagerBuddyBitmap(LPMM_MANAGER lpManager, WORD Mask)
{
    ((lpManager)->ManagerBuddyMap) ^= (Mask);
}

#if (CONFIG_MEM_REGION_MAX > 1)
STATIC INLINE VOID SetRegionBuddyBitmap(LPMM_MANAGER lpManager, BYTE RegionID, BYTE Buddy, BOOL Value)
{
    SET_BIT_VALUE(lpManager->RegionBuddyMap[Buddy], RegionID, Value);
}

STATIC INLINE VOID AddRegionBuddyBitmap(LPMM_MANAGER lpManager, BYTE Buddy, BUDDY_MAP_T Mask)
{
    ((lpManager)->RegionBuddyMap[Buddy]) |= (Mask);
}

STATIC INLINE VOID SubRegionBuddyBitmap(LPMM_MANAGER lpManager, BYTE Buddy, BUDDY_MAP_T Mask)
{
    ((lpManager)->RegionBuddyMap[Buddy]) ^= (Mask);
}

STATIC INLINE BUDDY_MAP_T GetRegionBuddyMapValue(LPMM_MANAGER lpManager, BYTE Buddy)
{
    return lpManager->RegionBuddyMap[Buddy];
}
#else
#define SetRegionBuddyBitmap(lpManager, RegionID, Buddy, Value)
#define AddRegionBuddyBitmap(lpManager, Buddy, Mask)
#define SubRegionBuddyBitmap(lpManager, Buddy, Mask)
#define GetRegionBuddyMapValue(lpManager, Buddy)        (TRUE && GetBuddyCount(GetRegionObject(lpManager, 0), Buddy))
#endif


STATIC INLINE WORD GetManagerBuddyMapVlue(LPMM_MANAGER lpManager)
{
    return lpManager->ManagerBuddyMap;
}

STATIC INLINE VOID SetRegionPageTable(LPMM_REGION lpRegion, LPMM_PAGE_ENTRIES lpTable)
{
    lpRegion->lpRegionPageTable = lpTable;
}

STATIC INLINE LPMM_PAGE_ENTRIES GetRegionPageTable(LPMM_REGION lpRegion)
{
    return lpRegion->lpRegionPageTable;
}

STATIC INLINE DWORD GetRegionMagic(LPMM_REGION lpRegion)
{
    return lpRegion->Magic;
}

STATIC INLINE DWORD SetRegionMagic(LPMM_REGION lpRegion, DWORD Magic)
{
    return lpRegion->Magic = Magic;
}

STATIC INLINE BOOL GetRegionOnchip(LPMM_REGION lpRegion)
{
    return lpRegion->RegionOnChip;
}

STATIC INLINE VOID SetRegionOnchip(LPMM_REGION lpRegion, BOOL Onchip)
{
    lpRegion->RegionOnChip = Onchip;
}

STATIC INLINE WORD GetRegionPages(LPMM_REGION lpRegion)
{
    return lpRegion->RegionPages;
}

STATIC INLINE VOID SetRegionPages(LPMM_REGION lpRegion, WORD Pages)
{
    lpRegion->RegionPages = Pages;
}

STATIC INLINE VOID SetRegionFrees(LPMM_REGION lpRegion, WORD Pages)
{
    lpRegion->RegionFrees = (Pages);
}

STATIC INLINE VOID AddRegionFrees(LPMM_REGION lpRegion, WORD Pages)
{
    lpRegion->RegionFrees += (Pages);
}

STATIC INLINE VOID SubRegionFrees(LPMM_REGION lpRegion, WORD Pages)
{
    lpRegion->RegionFrees -= (Pages);
}

STATIC INLINE VOID SetRegionStart(LPMM_REGION lpRegion, MMADDR Start)
{
    lpRegion->RegionStart = Start;
}

STATIC INLINE MMADDR GetRegionStart(LPMM_REGION lpRegion)
{
    return lpRegion->RegionStart;
}

STATIC INLINE PAGE_ID_T GetRegionPageID(LPMM_REGION lpRegion, MMADDR PageAddress)
{
    return (PAGE_ID_T)((PageAddress - (lpRegion)->RegionStart) / CONFIG_MEM_PAGE_SIZE);
}

STATIC INLINE VOID RegionUsedSectionInit(LPMM_REGION lpRegion)
{
    SelfPageInit(&lpRegion->RegionUsedTable, INVALID_PAGE_ID, INVALID_PAGE_ID, FALSE);
}

STATIC INLINE DWORD SubRegionUsedCount(LPMM_REGION lpRegion)
{
    return -- lpRegion->RegionUsedCount;
}

STATIC INLINE DWORD AddRegionUsedCount(LPMM_REGION lpRegion)
{
    return lpRegion->RegionUsedCount ++;
}

STATIC INLINE DWORD GetRegionUsedCount(LPMM_REGION lpRegion)
{
    return lpRegion->RegionUsedCount;
}

STATIC INLINE VOID SetRegionUsedStartID(LPMM_REGION lpRegion, PAGE_ID_T StartPage)
{
    SetNextPage(&lpRegion->RegionUsedTable.TailPage, StartPage);
}

STATIC INLINE PAGE_ID_T GetRegionUsedStartID(LPMM_REGION lpRegion)
{
    return (PAGE_ID_T)(lpRegion->RegionUsedTable.SelfPage.NextSection);
}

STATIC INLINE VOID SetRegionUsedFinalID(LPMM_REGION lpRegion, PAGE_ID_T FinalPage)
{
    SetPrevPage(&lpRegion->RegionUsedTable.HeadPage, FinalPage);
}

STATIC INLINE PAGE_ID_T GetRegionUsedFinalID(LPMM_REGION lpRegion)
{
    return (PAGE_ID_T)(lpRegion->RegionUsedTable.SelfPage.PrveSection);
}

STATIC INLINE VOID SetRegionUsedNextID(LPMM_REGION lpRegion, PAGE_ID_T StartPage)
{
    SetNextPage(&lpRegion->lpRegionPageTable[lpRegion->RegionUsedTable.SelfPage.PrveSection].TailPage, StartPage);
}

STATIC INLINE VOID SetRegionUsedPrveID(LPMM_REGION lpRegion, PAGE_ID_T FinalPage)
{
    SetPrevPage(&lpRegion->lpRegionPageTable[lpRegion->RegionUsedTable.SelfPage.NextSection].HeadPage, FinalPage);
}


STATIC INLINE BOOL GetPageManyBit(LPMM_REGION lpRegion, PAGE_ID_T PageID)
{
    return lpRegion->lpRegionPageTable[PageID].SelfPage.ManyBoolean;
}

STATIC INLINE BOOL GetPageHeadBit(LPMM_REGION lpRegion, PAGE_ID_T PageID)
{
    return lpRegion->lpRegionPageTable[PageID].SelfPage.HeadBoolean;
}

STATIC INLINE BOOL GetPageUsedBit(LPMM_REGION lpRegion, PAGE_ID_T PageID)
{
    return lpRegion->lpRegionPageTable[PageID].SelfPage.UsedBoolean;
}

STATIC INLINE DWORD GetPageFlagsValue(LPMM_REGION lpRegion, PAGE_ID_T PageID)
{
    return ((lpRegion->lpRegionPageTable[PageID].Entries & PAGE_SECTFLAGS_MASK) >> PAGE_SECTFLAGS_SHIFT);
}

STATIC INLINE DWORD GetPageEntriesData(LPMM_REGION lpRegion, PAGE_ID_T PageID)
{
    return lpRegion->lpRegionPageTable[PageID].Entries;
}

STATIC INLINE VOID FreePageEntriesData(LPMM_REGION lpRegion, PAGE_ID_T PageID)
{
    lpRegion->lpRegionPageTable[PageID].Entries = 0;
}


STATIC INLINE VOID BuddyTableInit(LPMM_REGION lpRegion, BYTE Buddy)
{
    SelfPageInit(&lpRegion->RegionBuddyTable[Buddy], INVALID_PAGE_ID, INVALID_PAGE_ID, FALSE);
}

STATIC INLINE DWORD AddBuddyCount(LPMM_REGION lpRegion, BYTE Buddy)
{
    return lpRegion->RegionBuddyCount[Buddy] ++;
}

STATIC INLINE DWORD SubBuddyCount(LPMM_REGION lpRegion, BYTE Buddy)
{
    return -- lpRegion->RegionBuddyCount[Buddy];
}

STATIC INLINE VOID SetBuddyStartID(LPMM_REGION lpRegion, BYTE Buddy, PAGE_ID_T StartPage)
{
    SetNextPage(&lpRegion->RegionBuddyTable[Buddy].TailPage, StartPage);
}

STATIC INLINE PAGE_ID_T GetBuddyStartID(LPMM_REGION lpRegion, BYTE Buddy)
{
    return (PAGE_ID_T)(lpRegion->RegionBuddyTable[Buddy].SelfPage.NextSection);
}

STATIC INLINE VOID SetBuddyFinalID(LPMM_REGION lpRegion, BYTE Buddy, PAGE_ID_T FinalPage)
{
    SetPrevPage(&lpRegion->RegionBuddyTable[Buddy].HeadPage, FinalPage);
}

STATIC INLINE PAGE_ID_T GetBuddyFinalID(LPMM_REGION lpRegion, BYTE Buddy)
{
    return (PAGE_ID_T)(lpRegion->RegionBuddyTable[Buddy].SelfPage.PrveSection);
}

STATIC INLINE VOID SetBuddyNextID(LPMM_REGION lpRegion, BYTE Buddy, PAGE_ID_T StartPage)
{
    SetNextPage(&lpRegion->lpRegionPageTable[lpRegion->RegionBuddyTable[Buddy].SelfPage.PrveSection].TailPage, StartPage);
}

STATIC INLINE VOID SetBuddyPrveID(LPMM_REGION lpRegion, BYTE Buddy, PAGE_ID_T FinalPage)
{
    SetPrevPage(&lpRegion->lpRegionPageTable[lpRegion->RegionBuddyTable[Buddy].SelfPage.NextSection].HeadPage, FinalPage);
}

STATIC INLINE LPMM_SELF_PAGE GetRegionUsedEntries(LPMM_REGION lpRegion)
{
    return &lpRegion->RegionUsedTable.SelfPage;
}

STATIC INLINE LPMM_PAGE_ENTRIES GetBuddyEntries(LPMM_REGION lpRegion, BYTE Buddy)
{
    return &lpRegion->RegionBuddyTable[Buddy];
}

STATIC INLINE LPMM_HEAD_PAGE GetHeadPageEntries(LPMM_REGION lpRegion, PAGE_ID_T StartPage)
{
    return &lpRegion->lpRegionPageTable[StartPage].HeadPage;
}

STATIC INLINE LPMM_TAIL_PAGE GetTailPageEntries(LPMM_REGION lpRegion, PAGE_ID_T FinalPage)
{
    return &lpRegion->lpRegionPageTable[FinalPage].TailPage;
}

STATIC INLINE LPMM_SELF_PAGE GetSelfPageEntries(LPMM_REGION lpRegion, PAGE_ID_T SelfPage)
{
    return &lpRegion->lpRegionPageTable[SelfPage].SelfPage;
}

STATIC INLINE LPMM_PAGE_ENTRIES GetPageEntries(LPMM_REGION lpRegion, PAGE_ID_T PageID)
{
    return &lpRegion->lpRegionPageTable[PageID];
}

STATIC INLINE BOOL CheckPageIsUsedHead(LPMM_REGION lpRegion, PAGE_ID_T PageID)
{
    return (PAGE_IS_USE_HEAD((lpRegion)->lpRegionPageTable[PageID].Entries));
}

STATIC INLINE WORD GetRegionFrees(LPMM_REGION lpRegion)
{
    return (lpRegion->RegionFrees);
}

STATIC INLINE VOID AddSystemTotalPages(LPMM_MANAGER lpManager, WORD Pages)
{
    ((lpManager)->ManagerPages += (Pages));
}

STATIC INLINE DWORD GetSystemTotalPages(LPMM_MANAGER lpManager)
{
    return ((lpManager)->ManagerPages);
}

STATIC INLINE WORD GetBuddyCount(LPMM_REGION lpRegion, BYTE Buddy)
{
    return ((lpRegion)->RegionBuddyCount[Buddy]);
}

#else

#define     SelfPageInit(lpEntries, PrvePage, NextPage, Used)                                           \
            do{                                                                                         \
                (lpEntries)->Entries = (((DWORD)(Used)) << PAGE_USED_BIT_SHIFT)                         \
                    + ((((NextPage) << PAGE_NEXTPAGE_SHIFT) + ((PrvePage) << PAGE_PRVEPAGE_SHIFT))      \
                    & (PAGE_NEXTPAGE_MASK | PAGE_PRVEPAGE_MASK))                                        \
                    + (PAGE_HEAD_BIT_MASK);                                                             \
            }while(0)

#define     HeadPageInit(lpEntries, PrvePage, Pages, Used)                                              \
            do{                                                                                         \
                (lpEntries)->Entries = (((DWORD)(Used)) << PAGE_USED_BIT_SHIFT)                         \
                    + ((((PrvePage) << PAGE_PRVEPAGE_SHIFT) + (((Pages) - 1) << PAGE_HEADPAGES_SHIFT))  \
                    & (PAGE_PRVEPAGE_MASK | PAGE_HEADPAGES_MASK))                                       \
                    + (PAGE_MANY_BIT_MASK + PAGE_HEAD_BIT_MASK);                                        \
            }while(0)

#define     TailPageInit(lpEntries, NextPage, Pages, Used)                                              \
            do{                                                                                         \
                (lpEntries)->Entries = (((DWORD)(Used)) << PAGE_USED_BIT_SHIFT)                         \
                    + ((((NextPage) << PAGE_NEXTPAGE_SHIFT) + (((Pages) - 1) << PAGE_TAILPAGES_SHIFT))  \
                    & (PAGE_NEXTPAGE_MASK | PAGE_TAILPAGES_MASK))                                       \
                    + (PAGE_MANY_BIT_MASK);                                                             \
            }while(0)

#define     SetNextPage(lpTail, Page)               do { (lpTail)->NextSection = (Page); } while(0)
#define     SetPrevPage(lpHead, Page)               do { (lpHead)->PrveSection = (Page); } while(0)
#define     StartPage2FinalPage(lpR, P)             ((PAGE_ID_T) (PAGE_IS_SELF((lpR)->lpRegionPageTable[P].Entries) ? (P) : ((P) + (lpR)->lpRegionPageTable[P].HeadPage.SectionPages)))
#define     FinalPage2StartPage(lpR, P)             ((PAGE_ID_T) (PAGE_IS_SELF((lpR)->lpRegionPageTable[P].Entries) ? (P) : ((P) - (lpR)->lpRegionPageTable[P].TailPage.SectionPages)))
#define     GetNextSectionStartID(lpR, P)           ((PAGE_ID_T) ((lpR)->lpRegionPageTable[P].SelfPage.NextSection))
#define     GetPrveSectionFinalID(lpR, P)           ((PAGE_ID_T) ((lpR)->lpRegionPageTable[P].SelfPage.PrveSection))
#define     GetStartEntriesPages(lpR, P)            ((PAGE_ID_T) (PAGE_IS_SELF((lpR)->lpRegionPageTable[P].Entries) ? 1 : (lpR)->lpRegionPageTable[P].HeadPage.SectionPages + 1))
#define     GetFinalEntriesPages(lpR, P)            ((PAGE_ID_T) (PAGE_IS_SELF((lpR)->lpRegionPageTable[P].Entries) ? 1 : (lpR)->lpRegionPageTable[P].TailPage.SectionPages + 1))
#define     NotPageAddress(Address)                 (0 != ((Address) & (CONFIG_MEM_PAGE_SIZE - 1)))
#define     GetRegionObject(lpM, ID)                (&(lpM)->RegionTable[ID])
#define     GetPageAddress(lpM, RID, PID)           (LPVOID)((lpM)->RegionTable[RID].RegionStart + (PID) * CONFIG_MEM_PAGE_SIZE)
#define     GetRegisterRegions(lpM)                 ((lpM)->RegisterRegions)
#define     AddRegisterRegions(lpM)                 ((lpM)->RegisterRegions ++)
#define     GetSystemTotalPages(lpM)                ((lpM)->ManagerPages)
#define     AddSystemTotalPages(lpM, Pages)         do { ((lpM)->ManagerPages += (Pages)); } while(0)

#define     SetManagerBuddyBitmap(lpM, B, V)        SET_BIT_VALUE((lpM)->ManagerBuddyMap, B, V)
#define     AddManagerBuddyBitmap(lpM, Mask)        ((lpM)->ManagerBuddyMap) |= (Mask)
#define     SubManagerBuddyBitmap(lpM, Mask)        ((lpM)->ManagerBuddyMap) ^= (Mask)
#if (CONFIG_MEM_REGION_MAX > 1)
#define     SetRegionBuddyBitmap(lpM, ID, B, V)     SET_BIT_VALUE((lpM)->RegionBuddyMap[B], ID, V)
#define     AddRegionBuddyBitmap(lpM, B, Mask)      ((lpM)->RegionBuddyMap[B]) |= (Mask)
#define     SubRegionBuddyBitmap(lpM, B, Mask)      ((lpM)->RegionBuddyMap[B]) ^= (Mask)
#define     GetRegionBuddyMapValue(lpM, B)          ((lpM)->RegionBuddyMap[B])
#else
#define     SetRegionBuddyBitmap(lpM, ID, B, V)
#define     AddRegionBuddyBitmap(lpM, B, Mask)
#define     SubRegionBuddyBitmap(lpM, B, Mask)
#define     GetRegionBuddyMapValue(lpM, B)          (TRUE && GetBuddyCount(GetRegionObject(lpM, 0), B))
#endif

#define     GetManagerBuddyMapVlue(lpM)             ((lpM)->ManagerBuddyMap)
/*some region operation macro*/
#define     RegionUsedSectionInit(lpR)              SelfPageInit(&(lpR)->RegionUsedTable, INVALID_PAGE_ID, INVALID_PAGE_ID, FALSE)
#define     SubRegionUsedCount(lpR)                 (-- (lpR)->RegionUsedCount)
#define     AddRegionUsedCount(lpR)                 ((lpR)->RegionUsedCount ++)
#define     GetRegionUsedCount(lpR)                 ((lpR)->RegionUsedCount)
#define     SetRegionUsedStartID(lpR, P)            SetNextPage(&(lpR)->RegionUsedTable.TailPage, P)
#define     GetRegionUsedStartID(lpR)               ((PAGE_ID_T)((lpR)->RegionUsedTable.SelfPage.NextSection))
#define     SetRegionUsedFinalID(lpR, P)            SetPrevPage(&(lpR)->RegionUsedTable.HeadPage, P);
#define     GetRegionUsedFinalID(lpR)               ((PAGE_ID_T)((lpR)->RegionUsedTable.SelfPage.PrveSection))
#define     SetRegionUsedNextID(lpR, P)             SetNextPage(&(lpR)->lpRegionPageTable[(lpR)->RegionUsedTable.SelfPage.PrveSection].TailPage, P);
#define     SetRegionUsedPrveID(lpR, P)             SetPrevPage(&(lpR)->lpRegionPageTable[(lpR)->RegionUsedTable.SelfPage.NextSection].HeadPage, P);

#define     GetRegionMagic(lpR)                     ((lpR)->Magic)
#define     SetRegionMagic(lpR, M)                  do {(lpR)->Magic = (M); } while(0)
#define     GetRegionOnchip(lpR)                    ((lpR)->RegionOnChip)
#define     SetRegionOnchip(lpR, Onchip)            do{ (lpR)->RegionOnChip = (Onchip); }while(0)
#define     SetRegionPageTable(lpR, lpT)            do{ (lpR)->lpRegionPageTable = lpT; } while(0)
#define     GetRegionPageTable(lpR)                 ((lpR)->lpRegionPageTable)
#define     GetRegionPages(lpR)                     ((lpR)->RegionPages)



#define     SetRegionPages(lpR, Pages)              do { (lpR)->RegionPages = (Pages); } while(0)
#define     GetRegionFrees(lpR)                     ((lpR)->RegionFrees)
#define     SetRegionFrees(lpM, Pages)              do { ((lpM)->RegionFrees = (Pages)); }while(0)
#define     AddRegionFrees(lpR, Pages)              do { ((lpR)->RegionFrees += (Pages)); }while(0)
#define     SubRegionFrees(lpR, Pages)              do { ((lpR)->RegionFrees -= (Pages)); }while(0)

#define     GetRegionPageID(lpR, A)                 ((PAGE_ID_T) (((A) - (lpRegion)->RegionStart) / CONFIG_MEM_PAGE_SIZE))
#define     GetRegionStart(lpR)                     ((lpR)->RegionStart)
#define     SetRegionStart(lpR, Start)              do { (lpR)->RegionStart = (Start); } while(0)

#define     GetPageManyBit(lpR, P)                  ((BOOL) ((lpR)->lpRegionPageTable[P].SelfPage.ManyBoolean))
#define     GetPageHeadBit(lpR, P)                  ((BOOL) ((lpR)->lpRegionPageTable[P].SelfPage.HeadBoolean))
#define     GetPageUsedBit(lpR, P)                  ((BOOL) ((lpR)->lpRegionPageTable[P].SelfPage.UsedBoolean))
#define     CheckPageIsUsedHead(lpR, P)             (PAGE_IS_USE_HEAD((lpR)->lpRegionPageTable[P].Entries))
#define     FreePageEntriesData(lpR, P)             do {(lpR)->lpRegionPageTable[P].Entries = 0; } while(0)
#define     GetPageEntriesData(lpR, P)              (lpR)->lpRegionPageTable[P].Entries
#define     GetPageFlagsValue(lpR, P)               (((lpR)->lpRegionPageTable[P].Entries & PAGE_SECTFLAGS_MASK) >> PAGE_SECTFLAGS_SHIFT)

/* some buddy operation macro */
#define     BuddyTableInit(lpR, B)                  SelfPageInit(&(lpR)->RegionBuddyTable[B], INVALID_PAGE_ID, INVALID_PAGE_ID, FALSE)
#define     AddBuddyCount(lpR, B)                   ((lpR)->RegionBuddyCount[B] ++)
#define     SubBuddyCount(lpR, B)                   (-- (lpR)->RegionBuddyCount[B])
#define     GetBuddyCount(lpR, B)                   ((lpR)->RegionBuddyCount[B])
#define     SetBuddyStartID(lpR, B, P)              SetNextPage(&(lpR)->RegionBuddyTable[B].TailPage, P)
#define     GetBuddyStartID(lpR, B)                 ((PAGE_ID_T)((lpR)->RegionBuddyTable[B].SelfPage.NextSection))
#define     SetBuddyFinalID(lpR, B, P)              SetPrevPage(&(lpR)->RegionBuddyTable[B].HeadPage, P)
#define     GetBuddyFinalID(lpR, B)                 ((PAGE_ID_T)((lpR)->RegionBuddyTable[B].SelfPage.PrveSection))

#define     SetBuddyNextID(lpR, B, P)               SetNextPage(&(lpR)->lpRegionPageTable[(lpR)->RegionBuddyTable[B].SelfPage.PrveSection].TailPage, P)
#define     SetBuddyPrveID(lpR, B, P)               SetPrevPage(&(lpR)->lpRegionPageTable[(lpR)->RegionBuddyTable[B].SelfPage.NextSection].HeadPage, P)

#define     GetBuddyEntries(lpR, B)                 (&(lpR)->RegionBuddyTable[B])
#define     GetPageEntries(lpR, P)                  (&(lpR)->lpRegionPageTable[P])
#define     GetHeadPageEntries(lpR, P)              (&(lpR)->lpRegionPageTable[P].HeadPage)
#define     GetTailPageEntries(lpR, P)              (&(lpR)->lpRegionPageTable[P].TailPage)
#define     GetSelfPageEntries(lpR, P)              (&(lpR)->lpRegionPageTable[P].SelfPage)
#define     GetRegionUsedEntries(lpR)               (&(lpR)->RegionUsedTable.SelfPage)
#endif

/*******************************************************************************************
 *  函 数 名：BuddyAddSection
 *  功    能：添加一个 SECTION 到 BUDDY 组
 *  参    数：
 *              LPMM_MANAGER    lpManager   内存管理器指针
 *              LPMM_REGION     lpRegion    SECTION 所在的 REGION 指针
 *                                  注意：该参数和 REGION 号必须对应
 *              BYTE            RegionID    SECTION 所在的 REGION 号
 *                                  注意：该参数和 REGION 指针必须对应
 *              PAGE_ID_T       StartPage   SECTION 首页页面号
 *              WORD            Pages       SECTION 的页面数
 *                                  注意：Pages = (1<<Buddy)
 *              BYTE            Buddy       SECTION 所在的 BUDDY
 *                                  注意：Pages = (1<<Buddy)
 *  返 回 值：NONE
 *  修改历史：
 *      1.修改时间：2014-10-26
 *        作    者: 姜勇
 *        修改内容: 创建函数
 *******************************************************************************************/

#ifdef MMS_STATIC_BUDDY_ADD_SECTION
STATIC VOID BuddyAddSection(LPMM_MANAGER lpManager, LPMM_REGION lpRegion,
                            BYTE RegionID, PAGE_ID_T StartPage, WORD Pages, BYTE Buddy)
{
    PAGE_ID_T FinalPage = StartPage + Pages - 1;

    if (AddBuddyCount(lpRegion, Buddy))
        /* 如果 BUDDY 不为空链表 */
    {
        if (FinalPage == StartPage)
        {
            SelfPageInit(GetPageEntries(lpRegion, StartPage),
                GetBuddyFinalID(lpRegion, Buddy),           /* 左边页 */
                GetBuddyStartID(lpRegion, Buddy),           /* 右边页 */
                FALSE);
        }
        else
        {
            HeadPageInit(GetPageEntries(lpRegion, StartPage),
                GetBuddyFinalID(lpRegion, Buddy), Pages, FALSE);
            TailPageInit(GetPageEntries(lpRegion, FinalPage),
                GetBuddyStartID(lpRegion, Buddy), Pages, FALSE);
        }
        /* 设置右边节点首页上的左边页(链表尾部页处理) */
        SetBuddyPrveID(lpRegion, Buddy, FinalPage);
        /* 设置左边节点尾页上的右边页(链表头部页处理) */
        SetBuddyNextID(lpRegion, Buddy, StartPage);
        /* 设置链表头的首页 */
        SetBuddyStartID(lpRegion, Buddy, StartPage);
    }
    else
    /* 如果 BUDDY 为空链表 */
    {
        if (FinalPage == StartPage)
        {
            SelfPageInit(GetPageEntries(lpRegion, StartPage),
                FinalPage, StartPage, FALSE);
        }
        else
        {
            HeadPageInit(GetPageEntries(lpRegion, StartPage),
                FinalPage, Pages, FALSE);
            TailPageInit(GetPageEntries(lpRegion, FinalPage),
                StartPage, Pages, FALSE);
        }

        SelfPageInit(GetBuddyEntries(lpRegion, Buddy),
            FinalPage, StartPage, FALSE);
    }


    MM_DEBUG(TRUE, "Buddy add section to region %d, StartPage %d, buddy %d, Buddy map value 0x%04x",
        RegionID, StartPage, Buddy, GetManagerBuddyMapVlue(lpManager));

    AddManagerBuddyBitmap(lpManager, Pages);
    AddRegionBuddyBitmap(lpManager, Buddy, 1 << (RegionID));
}

#else /************************************* MMS_STATIC_BUDDY_ADD_SECTION **************************************/
#define     BuddyAddSection(lpManager, lpRegion, RegionID, StartPage, Pages, Buddy)                             \
            do{                                                                                                 \
                PAGE_ID_T FinalPage = StartPage + Pages - 1;                                                    \
                                                                                                                \
                if (AddBuddyCount(lpRegion, Buddy))                                                             \
                    /* 如果 BUDDY 不为空链表 */                                                             \
                {                                                                                               \
                    if (FinalPage == StartPage)                                                                 \
                    {                                                                                           \
                        SelfPageInit(GetPageEntries(lpRegion, StartPage),                                       \
                            GetBuddyFinalID(lpRegion, Buddy),           /* 左边页 */                            \
                            GetBuddyStartID(lpRegion, Buddy),           /* 右边页 */                            \
                            FALSE);                                                                             \
                    }                                                                                           \
                    else                                                                                        \
                    {                                                                                           \
                        HeadPageInit(GetPageEntries(lpRegion, StartPage),                                       \
                            GetBuddyFinalID(lpRegion, Buddy), Pages, FALSE);                                    \
                        TailPageInit(GetPageEntries(lpRegion, FinalPage),                                       \
                            GetBuddyStartID(lpRegion, Buddy), Pages, FALSE);                                    \
                    }                                                                                           \
                    /* 设置右边节点首页上的左边页(链表尾部页处理) */                                        \
                    SetBuddyPrveID(lpRegion, Buddy, FinalPage);                                                 \
                    /* 设置左边节点尾页上的右边页(链表头部页处理) */                                        \
                    SetBuddyNextID(lpRegion, Buddy, StartPage);                                                 \
                    /* 设置链表头的首页 */                                                                  \
                    SetBuddyStartID(lpRegion, Buddy, StartPage);                                                \
                }                                                                                               \
                else                                                                                            \
                    /* 如果 BUDDY 为空链表 */                                                                   \
                {                                                                                               \
                    if (FinalPage == StartPage)                                                                 \
                    {                                                                                           \
                        SelfPageInit(GetPageEntries(lpRegion, StartPage),                                       \
                            FinalPage, StartPage, FALSE);                                                       \
                    }                                                                                           \
                    else                                                                                        \
                    {                                                                                           \
                        HeadPageInit(GetPageEntries(lpRegion, StartPage),                                       \
                            FinalPage, Pages, FALSE);                                                           \
                        TailPageInit(GetPageEntries(lpRegion, FinalPage),                                       \
                            StartPage, Pages, FALSE);                                                           \
                    }                                                                                           \
                                                                                                                \
                    SelfPageInit(GetBuddyEntries(lpRegion, Buddy),                                              \
                        FinalPage, StartPage, FALSE);                                                           \
                }                                                                                               \
                                                                                                                \
                MM_DEBUG(TRUE, "Buddy add section to region %d,"                                                \
                    " StartPage %d, buddy %d, Buddy map value 0x%04x",                                          \
                    RegionID, StartPage, Buddy, GetManagerBuddyMapVlue(lpManager));                             \
                                                                                                                \
                AddManagerBuddyBitmap(lpManager, Pages);                                                        \
                AddRegionBuddyBitmap(lpManager, Buddy, 1 << (RegionID));                                        \
            }while(0)
#endif /************************************* MMS_STATIC_BUDDY_ADD_SECTION **************************************/

#ifdef MMS_STATIC_BUDDY_DEL_SECTION
STATIC VOID BuddyDelSection(LPMM_MANAGER lpManager, LPMM_REGION lpRegion,
                            BYTE RegionID, PAGE_ID_T StartPage, PAGE_ID_T FinalPage, BYTE Buddy)
{
    MM_DEBUG(TRUE, "Alloc From Region %d, Buddy %d, PageID: %u", RegionID, Buddy, StartPage);
    /* 如果BUDDY组的还有SECTION，需要处理剩余的 SECTION */
    if (SubBuddyCount(lpRegion, Buddy))
    {
        PAGE_ID_T NextStart = GetNextSectionStartID(lpRegion, FinalPage);
        PAGE_ID_T PrveFinal = GetPrveSectionFinalID(lpRegion, StartPage);
        /* 设置链表头 */
        if (GetBuddyStartID(lpRegion, Buddy) == StartPage)
        {
            SetBuddyStartID(lpRegion, Buddy, NextStart);
        }
        else
        {
            SetBuddyFinalID(lpRegion, Buddy, PrveFinal);
        }
        /* 设置右边节点首页上的左边页(链表尾部处理) */
        SetBuddyPrveID(lpRegion, Buddy, PrveFinal);
        /* 设置左边节点尾页上的右边页(链表头部处理) */
        SetBuddyNextID(lpRegion, Buddy, NextStart);
    }
    else
        /* 如果BUDDY组的没有SECTION，则将链表头的置空，并处理相应的BUDDY位图标志 */
    {
        /* 设置 BUDDY 组的伙伴位图 */
        SubRegionBuddyBitmap(lpManager, Buddy, 1 << (RegionID));
        /* 设置全局伙伴位图 */
        SetManagerBuddyBitmap(lpManager, Buddy, !! GetRegionBuddyMapValue(lpManager, Buddy));
        /* 清空链表 */
        BuddyTableInit(lpRegion, Buddy);
    }

    FreePageEntriesData(lpRegion, StartPage);
    FreePageEntriesData(lpRegion, FinalPage);
}
#else  /************************************* MMS_STATIC_BUDDY_DEL_SECTION *************************************/
#define     BuddyDelSection(lpManager, lpRegion, RegionID, StartPage, FinalPage, Buddy)                         \
            do{                                                                                                 \
                MM_DEBUG(TRUE, "Alloc From Region %d, Buddy %d, PageID: %u",                                    \
                    RegionID, Buddy, StartPage);                                                                \
                /* 如果BUDDY组的还有SECTION，需要处理剩余的 SECTION */                                      \
                if (SubBuddyCount(lpRegion, Buddy))                                                             \
                {                                                                                               \
                    PAGE_ID_T NextStart = GetNextSectionStartID(lpRegion, FinalPage);                           \
                    PAGE_ID_T PrveFinal = GetPrveSectionFinalID(lpRegion, StartPage);                           \
                    /* 设置链表头 */                                                                            \
                    if (GetBuddyStartID(lpRegion, Buddy) == StartPage)                                          \
                    {                                                                                           \
                        SetBuddyStartID(lpRegion, Buddy, NextStart);                                            \
                    }                                                                                           \
                    else                                                                                        \
                    {                                                                                           \
                        SetBuddyFinalID(lpRegion, Buddy, PrveFinal);                                            \
                    }                                                                                           \
                    /* 设置右边节点首页上的左边页(链表尾部处理) */                                          \
                    SetBuddyPrveID(lpRegion, Buddy, PrveFinal);                                                 \
                    /* 设置左边节点尾页上的右边页(链表头部处理) */                                          \
                    SetBuddyNextID(lpRegion, Buddy, NextStart);                                                 \
                }                                                                                               \
                else                                                                                            \
                    /* 如果BUDDY组的没有SECTION，则将链表头的置空，并处理相应的BUDDY位图标志 */         \
                {                                                                                               \
                    /* 设置 BUDDY 组的伙伴位图 */                                                               \
                    SubRegionBuddyBitmap(lpManager, Buddy, 1 << (RegionID));                                    \
                    /* 设置全局伙伴位图 */                                                                  \
                    SetManagerBuddyBitmap(lpManager, Buddy, !! GetRegionBuddyMapValue(lpManager, Buddy));       \
                    /* 清空链表 */                                                                              \
                    BuddyTableInit(lpRegion, Buddy);                                                            \
                }                                                                                               \
                                                                                                                \
                FreePageEntriesData(lpRegion, StartPage);                                                       \
                FreePageEntriesData(lpRegion, FinalPage);                                                       \
            }while(0)
#endif /************************************* MMS_STATIC_BUDDY_DEL_SECTION *************************************/

#ifdef MMS_STATIC_REGION_ADD_SECTION
STATIC VOID RegionAddSection(LPMM_REGION lpRegion, PAGE_ID_T StartPage, PAGE_ID_T FinalPage, WORD Pages)
{
    if (AddRegionUsedCount(lpRegion))
        /* 当链表非空时 */
    {
        if (FinalPage == StartPage)
        {
            SelfPageInit(GetPageEntries(lpRegion, StartPage),
                GetRegionUsedFinalID(lpRegion),         /* 左边页 */
                GetRegionUsedStartID(lpRegion),         /* 右边页 */
                TRUE);
        }
        else
        {
            HeadPageInit(GetPageEntries(lpRegion, StartPage),
                    GetRegionUsedFinalID(lpRegion), Pages, TRUE);
            TailPageInit(GetPageEntries(lpRegion, FinalPage),
                    GetRegionUsedStartID(lpRegion), Pages, TRUE);
        }
        /* 设置右边节点首页上的左边页(链表尾部页处理) */
        SetRegionUsedPrveID(lpRegion, FinalPage);
        /* 设置左边节点尾页上的右边页(链表头部页处理) */
        SetRegionUsedNextID(lpRegion, StartPage);
        /* 设置新的链表头部页 */
        SetRegionUsedStartID(lpRegion, StartPage);
    }
    else
        /* 当链表为空时 */
    {
        if (FinalPage == StartPage)
        {
            SelfPageInit(GetPageEntries(lpRegion, StartPage),
                    FinalPage, StartPage, TRUE);
        }
        else
        {
            HeadPageInit(GetPageEntries(lpRegion, StartPage),
                    FinalPage, Pages, TRUE);
            TailPageInit(GetPageEntries(lpRegion, FinalPage),
                    StartPage, Pages, TRUE);
        }

        SetRegionUsedStartID(lpRegion, StartPage);
        SetRegionUsedFinalID(lpRegion, FinalPage);
    }
}
#else  /************************************ MMS_STATIC_REGION_ADD_SECTION *************************************/
#define     RegionAddSection(lpRegion, StartPage, FinalPage, Pages)                                             \
            do{                                                                                                 \
                                                                                                                \
                if (AddRegionUsedCount(lpRegion))                                                               \
                    /* 当链表非空时 */                                                                          \
                {                                                                                               \
                    if (FinalPage == StartPage)                                                                 \
                    {                                                                                           \
                        SelfPageInit(GetPageEntries(lpRegion, StartPage),                                       \
                            GetRegionUsedFinalID(lpRegion),         /* 左边页 */                                \
                            GetRegionUsedStartID(lpRegion),         /* 右边页 */                                \
                            TRUE);                                                                              \
                    }                                                                                           \
                    else                                                                                        \
                    {                                                                                           \
                        HeadPageInit(GetPageEntries(lpRegion, StartPage),                                       \
                                GetRegionUsedFinalID(lpRegion), Pages, TRUE);                                   \
                        TailPageInit(GetPageEntries(lpRegion, FinalPage),                                       \
                                GetRegionUsedStartID(lpRegion), Pages, TRUE);                                   \
                    }                                                                                           \
                    /* 设置右边节点首页上的左边页(链表尾部页处理) */                                        \
                    SetRegionUsedPrveID(lpRegion, FinalPage);                                                   \
                    /* 设置左边节点尾页上的右边页(链表头部页处理) */                                        \
                    SetRegionUsedNextID(lpRegion, StartPage);                                                   \
                    /* 设置新的链表头部页 */                                                                    \
                    SetRegionUsedStartID(lpRegion, StartPage);                                                  \
                }                                                                                               \
                else                                                                                            \
                    /* 当链表为空时 */                                                                          \
                {                                                                                               \
                    if (FinalPage == StartPage)                                                                 \
                    {                                                                                           \
                        SelfPageInit(GetPageEntries(lpRegion, StartPage),                                       \
                                FinalPage, StartPage, TRUE);                                                    \
                    }                                                                                           \
                    else                                                                                        \
                    {                                                                                           \
                        HeadPageInit(GetPageEntries(lpRegion, StartPage),                                       \
                                FinalPage, Pages, TRUE);                                                        \
                        TailPageInit(GetPageEntries(lpRegion, FinalPage),                                       \
                                StartPage, Pages, TRUE);                                                        \
                    }                                                                                           \
                                                                                                                \
                    SetRegionUsedStartID(lpRegion, StartPage);                                                  \
                    SetRegionUsedFinalID(lpRegion, FinalPage);                                                  \
                }                                                                                               \
            }while(0)
#endif /************************************ MMS_STATIC_REGION_ADD_SECTION *************************************/

#ifndef MMS_STATIC_REGION_DEL_SECTION
STATIC VOID RegionDelSection(LPMM_REGION lpRegion, PAGE_ID_T StartPage, PAGE_ID_T FinalPage)
{
    if (SubRegionUsedCount(lpRegion))
        /* 如果链表非空 */
    {
        PAGE_ID_T NextStart = GetNextSectionStartID(lpRegion, FinalPage);
        PAGE_ID_T PrveFinal = GetPrveSectionFinalID(lpRegion, StartPage);
        SetNextPage(GetTailPageEntries(lpRegion, PrveFinal), NextStart);
        SetPrevPage(GetHeadPageEntries(lpRegion, NextStart), PrveFinal);
        SetRegionUsedStartID(lpRegion, NextStart);
        SetRegionUsedFinalID(lpRegion, PrveFinal);
    }
    else
    {
        RegionUsedSectionInit(lpRegion);
    }

    FreePageEntriesData(lpRegion, StartPage);
    FreePageEntriesData(lpRegion, FinalPage);
}
#else  /************************************ MMS_STATIC_REGION_DEL_SECTION *************************************/
#define     RegionDelSection(lpRegion, StartPage, FinalPage)                                                    \
            do{                                                                                                 \
                                                                                                                \
                if (SubRegionUsedCount(lpRegion))                                                               \
                    /* 如果链表非空 */                                                                          \
                {                                                                                               \
                    PAGE_ID_T NextStart = GetNextSectionStartID(lpRegion, FinalPage);                           \
                    PAGE_ID_T PrveFinal = GetPrveSectionFinalID(lpRegion, StartPage);                           \
                    SetNextPage(GetTailPageEntries(lpRegion, PrveFinal), NextStart);                            \
                    SetPrevPage(GetHeadPageEntries(lpRegion, NextStart), PrveFinal);                            \
                    SetRegionUsedStartID(lpRegion, NextStart);                                                  \
                    SetRegionUsedFinalID(lpRegion, PrveFinal);                                                  \
                }                                                                                               \
                else                                                                                            \
                {                                                                                               \
                    RegionUsedSectionInit(lpRegion);                                                            \
                }                                                                                               \
                                                                                                                \
                FreePageEntriesData(lpRegion, StartPage);                                                       \
                FreePageEntriesData(lpRegion, FinalPage);                                                       \
            }while(0)
#endif /************************************ MMS_STATIC_REGION_DEL_SECTION *************************************/

#ifndef MMS_STATIC_BUDDY_MERGE_SECTION
STATIC VOID BuddyMergeSection(LPMM_MANAGER lpManager, LPMM_REGION lpRegion,
                              BYTE RegionID, PAGE_ID_T StartPage, WORD Pages)
{
    BYTE Buddy;
    WORD MergePages = (Pages);
    WORD RegionPages = GetRegionPages(lpRegion);

    while(MergePages < (1<<(CONFIG_MEM_BUDDY_MAX-1)))
    {
        PAGE_ID_T MergePage = GetMergePageID(StartPage, MergePages);
        PAGE_ID_T MergeHead = MergePage < (StartPage)
                            ? (StartPage) - (MergePages)
                            : (StartPage) + (MergePages);
        PAGE_ID_T MergeTail = MergeHead + MergePages - 1;

        MM_DEBUG(TRUE, "StartPage: %d, Pages: %d,  MergeHead: %d,"
                        " MergePage: %d, SectionPages: %5d  RegionPages: %5d  %s   %s", 
                        StartPage, MergePages, MergeHead, MergePage,
                        GetStartEntriesPages(lpRegion, MergeHead),
                        RegionPages,
                        GetPageUsedBit(lpRegion, MergePage) ? "USED" : "FREE",
                        MergePages == GetStartEntriesPages(lpRegion, MergeHead) ? "Y" : "N");

        if (((WORD) MergeTail) >= RegionPages) break;                       /*超出范围 */
        if (GetPageUsedBit(lpRegion, MergePage)) break;                     /* 非空闲页 */
        if (GetStartEntriesPages(lpRegion, MergeHead) != (MergePages)) break;   /* 非伙伴页 */

        Buddy = GetPagesBuddy(MergePages);
        BuddyDelSection(lpManager, lpRegion, RegionID, MergeHead, MergeTail, Buddy);
        (StartPage) = MIN(StartPage, MergeHead);
        (MergePages) <<= 1;
    }

    Buddy = GetPagesBuddy(MergePages);
    MM_DEBUG(TRUE, "Add section to buddy %d.", Buddy);
    BuddyAddSection(lpManager, lpRegion, RegionID, StartPage, MergePages, Buddy);
}
#else  /*********************************** MMS_STATIC_BUDDY_MERGE_SECTION *************************************/
#define     BuddyMergeSection(lpManager, lpRegion, RegionID, StartPage, Pages)                                  \
            do{                                                                                                 \
                BYTE Buddy;                                                                                     \
                WORD MergePages = (Pages);                                                                      \
                WORD RegionPages = GetRegionPages(lpRegion);                                                    \
                                                                                                                \
                while(MergePages < (1<<(CONFIG_MEM_BUDDY_MAX-1)))                                           \
                {                                                                                               \
                    PAGE_ID_T MergePage = GetMergePageID(StartPage, MergePages);                                \
                    PAGE_ID_T MergeHead = MergePage < (StartPage)                                               \
                                        ? (StartPage) - (MergePages)                                            \
                                        : (StartPage) + (MergePages);                                           \
                    PAGE_ID_T MergeTail = MergeHead + MergePages - 1;                                           \
                                                                                                                \
                    MM_DEBUG(TRUE, "StartPage: %d, Pages: %d,  MergeHead: %d,"                                  \
                                    " MergePage: %d, SectionPages: %5d  RegionPages: %5d  %s   %s",             \
                                    StartPage, MergePages, MergeHead, MergePage,                                \
                                    GetStartEntriesPages(lpRegion, MergeHead),                                  \
                                    RegionPages,                                                                \
                                    GetPageUsedBit(lpRegion, MergePage) ? "USED" : "FREE",                      \
                                    MergePages == GetStartEntriesPages(lpRegion, MergeHead) ? "Y" : "N");       \
                                                                                                                \
                    if (((WORD) MergeTail) >= RegionPages) break;                       /*超出范围 */           \
                    if (GetPageUsedBit(lpRegion, MergePage)) break;                     /* 非空闲页 */          \
                    if (GetStartEntriesPages(lpRegion, MergeHead) != (MergePages)) break;   /* 非伙伴页 */      \
                                                                                                                \
                    Buddy = GetPagesBuddy(MergePages);                                                          \
                    BuddyDelSection(lpManager, lpRegion, RegionID, MergeHead,                                   \
                        MergeTail, Buddy);                                                                      \
                    (StartPage) = MIN(StartPage, MergeHead);                                                    \
                    (MergePages) <<= 1;                                                                         \
                }                                                                                               \
                                                                                                                \
                Buddy = GetPagesBuddy(MergePages);                                                              \
                MM_DEBUG(TRUE, "Add section to buddy %d.", Buddy);                                              \
                BuddyAddSection(lpManager, lpRegion, RegionID, StartPage, MergePages, Buddy);                   \
            }while(0)
#endif /*********************************** MMS_STATIC_BUDDY_MERGE_SECTION *************************************/                                                                                           \

/****************************************** MMS_BUDDY_SPLIT_SECTION ********************************************/
#define     BUDDY_SPLIT_SECTION(lpManager, lpRegion, RegionID, StartPage, FreePages, condition)                 \
            do{                                                                                                 \
                while (condition)                                                                               \
                /* 页数量不是 2 的整数次幂 */                                                               \
                {                                                                                               \
                    BYTE Buddy = GetSplitBuddy(StartPage, FreePages);                                           \
                    WORD Pages = (1 << Buddy);                                                                  \
                                                                                                                \
                    MM_DEBUG(TRUE, "Free start page %d, pages %d.", StartPage, FreePages);                      \
                                                                                                                \
                    BuddyAddSection(lpManager, lpRegion, RegionID, StartPage, Pages, Buddy);                    \
                                                                                                                \
                    MM_DEBUG(TRUE, "Start %d, remain %d, Buddy %d, pages %d "                                   \
                                    "system map = 0x%04x,  region map = 0x%08x",                                \
                                    StartPage, FreePages, Buddy, Pages,                                         \
                                    GetManagerBuddyMapVlue(lpManager),                                          \
                                    GetRegionBuddyMapValue(lpManager, Buddy));                                  \
                    (StartPage)  += Pages;                                                                      \
                    (FreePages)  -= Pages;                                                                      \
                                                                                                                \
                    MM_DEBUG(TRUE, "Merge pages start page %d, pages %d.", StartPage, FreePages);               \
                }                                                                                               \
            }while(0)
/****************************************** MMS_BUDDY_SPLIT_SECTION ********************************************/

STATIC E_STATUS PageFreeAddress(LPMM_MANAGER lpManager, LPVOID PageAddress)
{
    BYTE RegionID = 0;
    WORD FreePages = 0;
    PAGE_ID_T StartPage = 0;
    PAGE_ID_T FinalPage = 0;
    LPMM_REGION lpRegion = NULL;
    
    /* 检查是否为页对齐地址 */
    if (NotPageAddress((MMADDR) PageAddress))
    {
        MM_ERROR(TRUE, "%p is not page address.", PageAddress);
        return STATE_INVALID_PAGE;
    }

    RegionID = BSP_GetRegionID((MMADDR) PageAddress);

    /* 检查 REGION ID 是否有效 */   
    if (RegionID >= CONFIG_MEM_REGION_MAX)
    {
        MM_ERROR(TRUE, "%p is not in any region.", PageAddress);
        return STATE_INVALID_REGION;
    }
    
    lpRegion = GetRegionObject(lpManager, RegionID);

    /* 检查 REGION 是否已经初始化 */    
    if (SMM_MAGIC != GetRegionMagic(lpRegion))
    {
        MM_ERROR(TRUE, "%p is not in any region.", PageAddress);
        return STATE_INVALID_REGION;
    }
    
    StartPage = GetRegionPageID(lpRegion, (MMADDR) PageAddress);

    /* 检查首页号是否有效 */    
    if (StartPage >= GetRegionPages(lpRegion))
    {
        MM_ERROR(TRUE, "Free page %p id %d from region %d out of range.",
            PageAddress, StartPage, RegionID);
        return STATE_OVER_RANGE;
    }

    if (!CheckPageIsUsedHead(lpRegion, StartPage))
    {
        MM_ERROR(TRUE, "Free page %p id %d from region %d is not used.",
            PageAddress, StartPage, RegionID);
        return STATE_INVALID_PAGE;
    }
    
    MM_DEBUG(TRUE, "Free address %p, StartPage %d, pages %d",
        PageAddress, StartPage, GetStartEntriesPages(lpRegion, StartPage));

    FreePages = GetStartEntriesPages(lpRegion, StartPage);
    FinalPage = StartPage + FreePages - 1;

    RegionDelSection(lpRegion, StartPage, FinalPage);
    AddRegionFrees(lpRegion, FreePages);
    BUDDY_SPLIT_SECTION(lpManager, lpRegion, RegionID, StartPage, FreePages, FreePages & (FreePages - 1));
    BuddyMergeSection(lpManager, lpRegion, RegionID, StartPage, FreePages);

    return STATE_SUCCESS;
}

#if (CONFIG_MEM_REGION_MAX > 16)
#define     FindRegion4Buddy(lpM, Buddy)                GetDwordLowestBit(GetRegionBuddyMapValue(lpM, Buddy))
#elif (CONFIG_MEM_REGION_MAX > 8)
#define     FindRegion4Buddy(lpM, Buddy)                GetWordLowestBit(GetRegionBuddyMapValue(lpM, Buddy))
#elif (CONFIG_MEM_REGION_MAX > 1)
#define     FindRegion4Buddy(lpM, Buddy)                GetByteLowestBit(GetRegionBuddyMapValue(lpM, Buddy))
#else
#define     FindRegion4Buddy(lpM, Buddy)                (0)
#endif

#define     FindBuddy4Pages(lpM, Pages)                 GetWordLowestBit(GetBuddyMask(GetPagesBuddy(Pages)) & GetManagerBuddyMapVlue(lpM))


STATIC LPVOID PageAllocAddress(LPMM_MANAGER lpManager, WORD Pages)
{
    WORD RemainPages = 0;
    PAGE_ID_T StartPage = 0;
    PAGE_ID_T FinalPage = 0;
    PAGE_ID_T StartNext = 0;
    LPMM_REGION lpRegion = NULL;
    BYTE Buddy = FindBuddy4Pages(lpManager, Pages); 
    BYTE RegionID = FindRegion4Buddy(lpManager, Buddy);

    if (RegionID >= CONFIG_MEM_REGION_MAX || Buddy >= CONFIG_MEM_BUDDY_MAX)
    {
        MM_ERROR(TRUE, "Malloc %d pages failed, Region %d, Buddy %d.", Pages, RegionID, Buddy);
        return NULL;
    }

    lpRegion = GetRegionObject(lpManager, RegionID);
    StartPage = GetBuddyStartID(lpRegion, Buddy);
    FinalPage = StartPage + ((1<<Buddy) - 1);
    BuddyDelSection(lpManager, lpRegion, RegionID, StartPage, FinalPage, Buddy);

    StartNext = StartPage + Pages;
    FinalPage = StartNext - 1;
    RemainPages = (1<<Buddy) - Pages;
    SubRegionFrees(lpRegion, Pages);
    RegionAddSection(lpRegion, StartPage, FinalPage, Pages);

    MM_DEBUG(TRUE, "Alloc pages: %d, Buddy: %d, StartPage: %d, StartNext: %d  RemainPages: %d",
        Pages, Buddy, StartPage, StartNext, RemainPages);

    BUDDY_SPLIT_SECTION(lpManager, lpRegion, RegionID, StartNext, RemainPages, 0 != RemainPages);

    MM_INFOR(TRUE, "Alloc address %p, StartPage %d, pages %d.",
        GetPageAddress(lpManager, RegionID, StartPage), StartPage, Pages);

    return GetPageAddress(lpManager, RegionID, StartPage);
}


STATIC DWORD FreePagesQuery(LPMM_MANAGER lpManager, BYTE RegionID)
{
    DWORD Pages = 0;

    if (RegionID < CONFIG_MEM_REGION_MAX)
    {
        return GetRegionFrees(GetRegionObject(lpManager, RegionID));
    }

    for (RegionID = 0; RegionID < CONFIG_MEM_REGION_MAX; RegionID ++)
    {
        Pages += GetRegionFrees(GetRegionObject(lpManager, RegionID));
    }

    return Pages;
}

STATIC VOID CreatePageTable(LPMM_MANAGER lpManager, BYTE RegionID)
{
    PAGE_ID_T StartPage = 0;
    LPMM_REGION lpRegion = GetRegionObject(lpManager, RegionID);
    WORD RemainPages = GetRegionPages(lpRegion);
    WORD TablePages = (PAGE_ID_T) ((RemainPages * sizeof(MM_PAGE_ENTRIES) / CONFIG_MEM_PAGE_SIZE)
                    + (!!(RemainPages * sizeof(MM_PAGE_ENTRIES) % CONFIG_MEM_PAGE_SIZE)));
    LPMM_PAGE_ENTRIES lpTable =  NULL;
    
#if (CONFIG_MEM_STATIC_PAGETABLE == TRUE)
    if (0 == GetRegisterRegions(lpManager))
    {
        if (CONFIG_MEM_ONCHIP_PAGES == TablePages)
        {
            lpTable = g_OnchipPageTable;
        }
    }
    else
#endif

    lpTable = PageAllocAddress(lpManager, TablePages);

    if (NULL == lpTable)
    {
        MMADDR RegionStart = GetRegionStart(lpRegion);
        lpTable = (LPVOID) RegionStart;
        RegionStart += CONFIG_MEM_PAGE_SIZE * TablePages;
        MM_DEBUG(TRUE, "Change region start from %p to %p.",
            (LPVOID) GetRegionStart(lpRegion), (LPVOID) RegionStart);
        SetRegionStart(lpRegion, RegionStart);
        SetRegionPages(lpRegion, GetRegionPages(lpRegion) - TablePages);
        SetRegionFrees(lpRegion, GetRegionPages(lpRegion));
        RemainPages = GetRegionPages(lpRegion);
    }

    MM_DEBUG(TRUE, "Create region id %d address %p pages %d.",
        RegionID, (LPVOID) GetRegionStart(lpRegion), RemainPages);
    
    memset(lpTable, 0, sizeof(MM_PAGE_ENTRIES) * RemainPages);
    SetRegionPageTable(lpRegion, lpTable);
    AddSystemTotalPages(lpManager, RemainPages);

    BUDDY_SPLIT_SECTION(lpManager, lpRegion, RegionID, StartPage, RemainPages, 0 != RemainPages);
    
    return;
}


EXPORT E_STATUS CORE_CreateRegion(MMADDR Address, SIZE_T Length)
{
    BYTE Buddy;
    BYTE RegionID = 0;
    DWORD RegionPages = 0;
    SIZE_T RegionLength = 0;
    MMADDR RegionAddress = 0;
    LPMM_REGION lpRegion = NULL;
    LPMM_MANAGER lpManager = &g_SystemMemoryManager;
    SIZE_T Prefix = (Address % CONFIG_MEM_PAGE_SIZE)
                  ? CONFIG_MEM_PAGE_SIZE - (Address % CONFIG_MEM_PAGE_SIZE)
                  : (Address % CONFIG_MEM_PAGE_SIZE);
    SIZE_T Suffix = ((Address + Length) % CONFIG_MEM_PAGE_SIZE);
    
    RegionID = BSP_GetRegionID(Address);

    /* 检查 REGION ID 是否有效 */   
    if (RegionID >= CONFIG_MEM_REGION_MAX)
    {
        MM_ERROR(TRUE, "%p is not in any region.", Address);
        return STATE_INVALID_REGION;
    }

    lpRegion = GetRegionObject(lpManager, RegionID);;

    if (SMM_MAGIC == GetRegionMagic(lpRegion) || 
        GetRegisterRegions(lpManager) >= CONFIG_MEM_REGION_MAX)
    {
        MM_ERROR(TRUE, "Create too many regions %d, id is %d, magic is %x. max is %d, pointer is %p",
            GetRegisterRegions(lpManager), RegionID, GetRegionMagic(lpRegion), CONFIG_MEM_REGION_MAX, lpRegion);
        return STATE_OVER_RANGE;
    }

    RegionAddress  =  (Address + Prefix);
    RegionLength   =  (Length - Prefix - Suffix);
    RegionPages    =  (RegionLength / CONFIG_MEM_PAGE_SIZE);
    
    MM_DEBUG(TRUE, "Create region id %d address %p size %ld bytes, pages %d, regions %d, region object is %p.",
        RegionID, (LPVOID) RegionAddress, RegionLength, RegionPages, GetRegisterRegions(lpManager), lpRegion);

    if (RegionPages > MAX_PAGES)
    {
        MM_ERROR(TRUE, "The region have %d byte to large, I can't use it.", RegionLength);
        return STATE_OVER_RANGE;
    }

    SetRegionPages(lpRegion, (WORD) RegionPages);
    SetRegionFrees(lpRegion, (WORD) RegionPages);
    SetRegionStart(lpRegion, RegionAddress);
    SetRegionMagic(lpRegion, SMM_MAGIC);
    RegionUsedSectionInit(lpRegion);

    for (Buddy = 0; Buddy < CONFIG_MEM_BUDDY_MAX; Buddy ++)
    {
        BuddyTableInit(lpRegion, Buddy);
    }

    CreatePageTable(lpManager, RegionID);
    AddRegisterRegions(lpManager);

    CORE_INFOR(TRUE, "Create region %d address %p size %ld bytes, pages %d....[OK]",
        RegionID, (LPVOID) RegionAddress, RegionLength, RegionPages);
    
    return STATE_SUCCESS;
}


#if (CONFIG_MEM_DEBUG_ENABLE == TRUE)

STATIC DWORD GetBuddySectionCount(LPMM_MANAGER lpManager, BYTE Buddy)
{
    BYTE Id = 0;
    DWORD Count = 0;

    for (; Id < CONFIG_MEM_REGION_MAX; Id ++)
    {
        Count += GetBuddyCount(GetRegionObject(lpManager, Id), Buddy);
    }

    return Count;
}

STATIC E_STATUS TakeManagerInfor(LPMM_MANAGER lpManager, LPMMS_INFOR lpInfor)
{
    BYTE i = 0;
    if (NULL == lpInfor)
    {
        MM_ERROR(TRUE, "Invalid parameter to call this operation.");
        return STATE_INVALID_VALUE;
    }

    if (sizeof(MMS_INFOR) != lpInfor->SelfSize)
    {
        MM_ERROR(TRUE, "Self size %u out of range %u.", lpInfor->SelfSize, sizeof(MMS_INFOR));
        return STATE_INVALID_OBJECT;
    }

    lpInfor->PageSize = CONFIG_MEM_PAGE_SIZE;
    lpInfor->FreePages = FreePagesQuery(lpManager, CONFIG_MEM_REGION_MAX);
    lpInfor->TotalPages = GetSystemTotalPages(lpManager);
    lpInfor->BuddyMask = GetManagerBuddyMapVlue(lpManager);

    for (i = 0; i < CONFIG_MEM_BUDDY_MAX; i ++)
    {
        lpInfor->BuddyCount[i] = GetBuddySectionCount(lpManager, i);
    }

    return STATE_SUCCESS;
}


STATIC INLINE LPTSTR GetBooleanString(BOOL Boolean)
{
    LPTSTR lpString[] = { _TEXT("N"), _TEXT("Y") };

    return lpString[Boolean & 1];
}


STATIC E_STATUS EnumerateSections(LPMM_REGION lpRegion, PAGE_ID_T StartPage, DWORD Total)
{
    DWORD Count = 0;
    PAGE_ID_T FinalPage = 0;
    PAGE_ID_T SectionPages = 0;
    PAGE_ID_T NextSection = 0;

    FinalPage = StartPage2FinalPage(lpRegion, StartPage);
    SectionPages = GetStartEntriesPages(lpRegion, StartPage);
    NextSection = GetNextSectionStartID(lpRegion, FinalPage);
    
    CORE_INFOR(TRUE, "Many | Head | Used | Start |   Value"
        "   | Many | Head | Used | Final | Pages | Next | Value");

    for (Count = 0; Count < Total; Count ++)
    {
        CORE_INFOR(TRUE, "  %s      %s      %s    0x%04X   0x%08x   %s      %s"
            "      %s    0x%04X  0x%04X 0x%04X  0x%08x",
            GetBooleanString(GetPageManyBit(lpRegion, StartPage)),
            GetBooleanString(GetPageHeadBit(lpRegion, StartPage)),
            GetBooleanString(GetPageUsedBit(lpRegion, StartPage)),
            StartPage,
            GetPageEntriesData(lpRegion, StartPage),
            GetBooleanString(GetPageManyBit(lpRegion, FinalPage)),
            GetBooleanString(GetPageHeadBit(lpRegion, FinalPage)),
            GetBooleanString(GetPageUsedBit(lpRegion, FinalPage)),
            FinalPage, SectionPages, NextSection,
            GetPageEntriesData(lpRegion, FinalPage));

        StartPage = NextSection;
        FinalPage = StartPage2FinalPage(lpRegion, StartPage);
        SectionPages = GetStartEntriesPages(lpRegion, StartPage);
        NextSection = GetNextSectionStartID(lpRegion, FinalPage);
    }

    return STATE_SUCCESS;
}

STATIC E_STATUS RegionEnumerateSections(LPMM_MANAGER lpManager, BYTE RegionID, BYTE Buddy)
{
    DWORD Total = 0;
    PAGE_ID_T StartPage = 0;
    LPMM_REGION lpRegion = NULL;

    if (RegionID >= CONFIG_MEM_REGION_MAX)
    {
        MM_ERROR(TRUE, "Invalid region id %u.", RegionID);
        return STATE_INVALID_REGION;
    }

    lpRegion = GetRegionObject(lpManager, RegionID);

    if (SMM_MAGIC != GetRegionMagic(lpRegion))
    {
        MM_ERROR(TRUE, "Invalid region object, id %u.", RegionID);
        return STATE_INVALID_REGION;
    }

    if (Buddy < CONFIG_MEM_BUDDY_MAX)
    {
        Total = GetBuddyCount(lpRegion, Buddy);
        StartPage = GetBuddyStartID(lpRegion, Buddy);   
        CORE_INFOR(TRUE, "BUDDY *** Show region %d buddy %d start page %d total %d sections:", RegionID, Buddy, StartPage, Total);
    }
    else
    {
        Total = GetRegionUsedCount(lpRegion);
        StartPage = GetRegionUsedStartID(lpRegion);
        CORE_INFOR(TRUE, "USED *** Show region %d start page %d total %d used sections:", RegionID, StartPage, Total);
    }

    if (0 == Total)
    {
        MM_ERROR(TRUE, "This region %d is free.", RegionID);
        return STATE_INVALID_PAGE;
    }

    return EnumerateSections(lpRegion, StartPage, Total);
}

STATIC VOID ShowManager(LPMM_MANAGER lpManager)
{
    BYTE Buddy = 0;
    BYTE RegionID = 0;
    CORE_INFOR(TRUE, "***** Show manager information *****");
    CORE_INFOR(TRUE, "ManagerPages       :    %lu", lpManager->ManagerPages);
    CORE_INFOR(TRUE, "RegisterRegions    :    %u", lpManager->RegisterRegions);
    CORE_INFOR(TRUE, "ManagerBuddyMap    :    0x%04x", lpManager->ManagerBuddyMap);

    CORE_INFOR(FALSE, "Buddy information  :    ");

#if (CONFIG_MEM_REGION_MAX > 1)
    kDebugShowData(LOG_LEVEL_INFOR, lpManager->RegionBuddyMap, CONFIG_MEM_BUDDY_MAX);
#endif

    for(; RegionID < CONFIG_MEM_REGION_MAX; RegionID ++)
    {
        LPMM_REGION lpRegion = &lpManager->RegionTable[RegionID];
        if (SMM_MAGIC == lpRegion->Magic)
        {
            CORE_INFOR(TRUE, "***** Show Region %u information *****", RegionID);
            CORE_INFOR(TRUE, "RegionOnChip       :    %d", lpRegion->RegionOnChip);
            CORE_INFOR(TRUE, "RegionStart        :    0x%p", (LPVOID) lpRegion->RegionStart);
            CORE_INFOR(TRUE, "RegionPages        :    %u", lpRegion->RegionPages);
            CORE_INFOR(TRUE, "RegionFrees        :    %u", lpRegion->RegionFrees);
            CORE_INFOR(TRUE, "RegionUsedCount    :    %u", lpRegion->RegionUsedCount);
            CORE_INFOR(TRUE, "***** Show Region %u UsedTable *****", RegionID);
            CORE_INFOR(TRUE, "PrveSection        :    0x%LX", lpRegion->RegionUsedTable.SelfPage.PrveSection);
            CORE_INFOR(TRUE, "ManyBoolean        :    %lu", lpRegion->RegionUsedTable.SelfPage.ManyBoolean);
            CORE_INFOR(TRUE, "HeadBoolean        :    %lu", lpRegion->RegionUsedTable.SelfPage.HeadBoolean);
            CORE_INFOR(TRUE, "NextSection        :    0x%LX", lpRegion->RegionUsedTable.SelfPage.NextSection);
            CORE_INFOR(TRUE, "UsedBoolean        :    %lu", lpRegion->RegionUsedTable.SelfPage.UsedBoolean);
            CORE_INFOR(TRUE, "ReservedBit        :    %lu", lpRegion->RegionUsedTable.SelfPage.ReservedBit);
            for (Buddy = 0; Buddy < CONFIG_MEM_BUDDY_MAX; Buddy ++)
            {
                CORE_INFOR(TRUE, "***** Show Region %u buddy table %u *****", RegionID, Buddy);
                CORE_INFOR(TRUE, "PrveSection        :    0x%LX", lpRegion->RegionBuddyTable[Buddy].SelfPage.PrveSection);
                CORE_INFOR(TRUE, "ManyBoolean        :    %lu", lpRegion->RegionBuddyTable[Buddy].SelfPage.ManyBoolean);
                CORE_INFOR(TRUE, "HeadBoolean        :    %lu", lpRegion->RegionBuddyTable[Buddy].SelfPage.HeadBoolean);
                CORE_INFOR(TRUE, "NextSection        :    0x%lX", lpRegion->RegionBuddyTable[Buddy].SelfPage.NextSection);
                CORE_INFOR(TRUE, "UsedBoolean        :    %lu", lpRegion->RegionBuddyTable[Buddy].SelfPage.UsedBoolean);
                CORE_INFOR(TRUE, "ReservedBit        :    %lu", lpRegion->RegionBuddyTable[Buddy].SelfPage.ReservedBit);
                CORE_INFOR(TRUE, "RegionBuddyCount   :    %u", lpRegion->RegionBuddyCount[Buddy]);
            }
        }
    }
}

#endif

STATIC E_STATUS SVC_PageAlloc(LPVOID lpPrivate, LPVOID lpParam)
{
    LPLPC_REQUEST_PACKET lpPacket = lpParam;
    
    if (NULL == lpParam)
    {
        return STATE_INVALID_PARAMETER;
    }
    
    lpPacket->u0.pParam = PageAllocAddress(lpPrivate, lpPacket->u1.dParam);

    return lpPacket->u0.pParam ? STATE_SUCCESS : STATE_OUT_OF_MEMORY;
}

STATIC E_STATUS SVC_PageFree(LPVOID lpPrivate, LPVOID lpParam)
{
    LPLPC_REQUEST_PACKET lpPacket = lpParam;
    
    if (NULL == lpParam)
    {
        return STATE_INVALID_PARAMETER;
    }
    
    return PageFreeAddress(lpPrivate, lpPacket->u0.pParam);
}


STATIC E_STATUS SVC_FreePages(LPVOID lpPrivate, LPVOID lpParam)
{
    LPLPC_REQUEST_PACKET lpPacket = lpParam;

    if (NULL == lpParam)
    {
        return STATE_INVALID_PARAMETER;
    }
    
    lpPacket->u0.dParam = FreePagesQuery(lpPrivate, (BYTE) lpPacket->u0.dParam);
    return STATE_SUCCESS;
}

STATIC E_STATUS SVC_NewRegion(LPVOID lpPrivate, LPVOID lpParam)
{
    LPLPC_REQUEST_PACKET lpPacket = lpParam;

    if (NULL == lpParam)
    {
        return STATE_INVALID_PARAMETER;
    }

    return CORE_CreateRegion((MMADDR)lpPacket->u0.pParam, lpPacket->u1.dParam);
}

#if (CONFIG_MEM_DEBUG_ENABLE == TRUE)

STATIC E_STATUS SVC_TakeInfor(LPVOID lpPrivate, LPVOID lpParam)
{
    return TakeManagerInfor(lpPrivate, ((LPLPC_REQUEST_PACKET) lpParam)->u0.pParam);
}

STATIC E_STATUS SVC_ShowInfor(LPVOID lpPrivate, LPVOID lpParam)
{
    ShowManager(lpPrivate);
    return STATE_SUCCESS;
}

STATIC E_STATUS SVC_ShowSection(LPVOID lpPrivate, LPVOID lpParam)
{
    LPLPC_REQUEST_PACKET lpPacket = lpParam;

    return RegionEnumerateSections(lpPrivate, (BYTE) (lpPacket->u0.dParam >> 8), (BYTE) (lpPacket->u0.dParam & 0xff));
}
#endif

STATIC CONST REQUEST_HANDLER fnHandlers[] = {
    SVC_PageAlloc,                  /* LPC_MMS_PAGE_ALLOC */
    SVC_PageFree,                   /* LPC_MMS_PAGE_FREE */
    SVC_NewRegion,                  /* LPC_MMS_NEW_REGION */
    SVC_FreePages,                  /* LPC_MMS_FREE_PAGES */
#if (CONFIG_MEM_DEBUG_ENABLE == TRUE)
    SVC_TakeInfor,                  /* LPC_MMS_TAKE_INFOR */
    SVC_ShowInfor,                  /* LPC_MMS_SHOW_INFOR */
    SVC_ShowSection,                /* LPC_MMS_SHOW_SECTION */
#endif
};


DEFINE_LPC_SERVICE(MemService, SMM_MAGIC, SIZEOF_ARRAY(fnHandlers), &g_SystemMemoryManager, fnHandlers);

EXPORT LPVOID CORE_PageAlloc(SIZE_T Length)
{
    WORD Pages = (WORD)(Length / CONFIG_MEM_PAGE_SIZE + (!!(Length % CONFIG_MEM_PAGE_SIZE)));
    return PageAllocAddress(&g_SystemMemoryManager, Pages);
}
EXPORT_SYMBOL(CORE_PageAlloc);

EXPORT E_STATUS CORE_PageFree(LPVOID lpAddress)
{
    return PageFreeAddress(&g_SystemMemoryManager, lpAddress);
}
EXPORT_SYMBOL(CORE_PageFree);


PUBLIC E_STATUS initCoreSystemGlobalMemoryManager(VOID)
{
    CORE_INFOR(TRUE, "Max region is %d, max buddy is %d, page size is %d.",
                    CONFIG_MEM_REGION_MAX, CONFIG_MEM_BUDDY_MAX, CONFIG_MEM_PAGE_SIZE);
    CORE_INFOR(TRUE, "Show the size of mms type for debug:");
    CORE_INFOR(TRUE, "MMREGION: %d,  MM_REGION: %d,  MM_PAGE_ENTRIES: %d,  ",
            sizeof(MM_MANAGER), sizeof(MM_REGION), sizeof(MM_PAGE_ENTRIES));

    LPC_INSTALL(&MemService, "System global Memory Manager(SMM) starting");
    
    BSP_ScanMemoryRegion();
    
    return STATE_SUCCESS;
}

#else

PUBLIC E_STATUS initCoreSystemGlobalMemoryManager(VOID)
{
    CORE_INFOR(TRUE, "system global memory manager(SMM) not install.");
    return STATE_SUCCESS;
}

#endif

