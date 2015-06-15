#include <stdio.h>

#define     TEST_FOR_WINDOWS
#define     EXPORT
#define     STATIC
#define     INLINE
#undef      CONST
#define     CONST
#define     EXTERN      extern

#define     DWORD_WIDTH             (sizeof(DWORD) * BYTE_WIDTH)
#define     GET_DWORD_BIT(x, b)     ((x) & (1<<(b)))
#define     MAX(x, y)                       ((x) > (y) ? (x) : (y))
#define     MIN(x, y)                       ((x) < (y) ? (x) : (y))
#define     SET_BIT_VALUE(x, shift, boolean)                                        \
            do{                                                                     \
                (x) &= ~((1UL) << (shift));                                         \
                (x) |= ((boolean) << (shift));                                      \
            }while(0)

#define     EXPORT_SYMBOL(x)
#define     FW_GetTickCount()       0
#define     MAKE_DWORD(a, b, c)         (((((DWORD) (a)) << 0) + \
                                        (((DWORD) (b)) << 8) + \
                                        (((DWORD) (c)) << 16)) & 0xffffff)
#define     INVALID_MAGIC               -1                                  
#define     CONFIG_MMS_REGION_MAX   8
#define     CONFIG_MMS_BUDDY_MAX    16
#define     CONFIG_MMS_ONCHIP_PAGES    10
#define     CONFIG_MMS_PAGE_SIZE     4096
#define     CHIP_REGION_SIZE            (CONFIG_MMS_ONCHIP_PAGES * CONFIG_MMS_PAGE_SIZE)
#define     SMM_MAGIC                   MAKE_DWORD('M', 'M', 'S')
#define     SIZEOF_ARRAY(Array)         (sizeof(Array)/sizeof(Array[0]))
            
#define     MM_ASSERT(condition, code, ...)                                                 \
            do{                                                                             \
                if (!(condition))                                                           \
                {                                                                           \
                    printf("[%05d][INFOR][%s]", __LINE__, __FUNCTION__);                    \
                    printf(__VA_ARGS__);                                                    \
                    printf("\r\n");                                                         \
                    code                                                                    \
                }                                                                           \
            }while(0)

#define     CORE_INFOR(Enter,...)                                                            \
            do{                                                                             \
                printf("[%05d][INFOR][%s]", __LINE__, __FUNCTION__);                        \
                printf(__VA_ARGS__);                                                        \
                if (Enter) printf("\r\n");                                                  \
            }while(0)
                
#define     CORE_ERROR(Enter,...)                                                            \
            do{                                                                             \
                printf("[%05d][ERROR][%s]", __LINE__, __FUNCTION__);                        \
                printf(__VA_ARGS__);                                                        \
                if (Enter) printf("\r\n");                                                  \
            }while(0)
#define     CORE_FATAL(Enter, ...)                                                           \
            do{                                                                             \
                printf("[%05d][FATAL][%s]", __LINE__, __FUNCTION__);                        \
                printf(__VA_ARGS__);                                                        \
                if (Enter) printf("\r\n");                                                  \
                while(1);                                                                   \
            }while(0)
#if 0
#define     CORE_DEBUG(Enter,...)                                                            \
            do{                                                                             \
                printf("[%05d][DEBUG][%s]", __LINE__, __FUNCTION__);                        \
                printf(__VA_ARGS__);                                                        \
                if (Enter) printf("\r\n");                                                  \
            }while(0)
#endif
#define     CORE_DEBUG(Enter,...)           
typedef enum{
    LOG_LEVEL_FATAL     =           0,
    LOG_LEVEL_ERROR     =           1,
    LOG_LEVEL_WARN      =           2,
    LOG_LEVEL_BUG       =           3,
    LOG_LEVEL_FIX       =           4,
    LOG_LEVEL_INFOR     =           5,
    LOG_LEVEL_TRACE     =           6,
    LOG_LEVEL_DEBUG     =           7,
    LOG_LEVEL_MAX
}E_LOG_LEVEL;


typedef enum{
    STATE_SUCCESS                   =       0,
    /**/
    STATE_NOT_IMPLEMENTED           =       -1,
    STATE_NOT_SUPPORT               =       -2,
    STATE_NOT_READY                 =       -3,
    STATE_NOT_MATCH                 =       -4,
    STATE_NOT_FOUND                 =       -5,
    STATE_EXISTING                  =       -6,
    STATE_REMOVED                   =       -7,
    /**/
    STATE_UNKNOW_ERROR              =       -21,
    STATE_OVER_RANGE                =       -22,
    STATE_OUT_OF_MEMORY             =       -23,
    STATE_MEMORY_OVERFLOW           =       -24,
    STATE_HARDWARE_FAULT            =       -25,
    STATE_SYSTEM_FAULT              =       -26,
    STATE_SYSTEM_BUSY               =       -27,
    STATE_TIME_OUT                  =       -28,
    /**/
    STATE_INVALID_SIZE              =       -40,
    STATE_INVALID_NAME              =       -41,
    STATE_INVALID_FILE              =       -42,
    STATE_INVALID_PATH              =       -43,
    STATE_INVALID_VALUE             =       -44,
    STATE_INVALID_TASK              =       -45,
    STATE_INVALID_OBJECT            =       -46,
    STATE_INVALID_CLASS             =       -47,
    STATE_INVALID_SERVICE           =       -48,
    STATE_INVALID_METHOD            =       -49,
    STATE_INVALID_REQUEST           =       -50,
    STATE_INVALID_HANDLER           =       -51,
    STATE_INVALID_PAGE              =       -52,
    STATE_INVALID_REGION            =       -53,

    STATE_INVALID_INTCODE           =       -60,
    STATE_INVALID_INTMAGIC          =       -61,
    STATE_INVALID_INTMETHOD         =       -62,
    STATE_INVALID_INTHANDLER        =       -63,

    STATE_INVALID_IRQCODE           =       -70,
    STATE_INVALID_IRQMAGIC          =       -71,
    STATE_INVALID_IRQMETHOD         =       -72,
    STATE_INVALID_IRQHANDLER        =       -73,
    STATE_INVALID_IRQPACKET         =       -74,
    
    STATE_INVALID_REQCODE           =       -80,
    STATE_INVALID_REQMAGIC          =       -81,
    STATE_INVALID_REQMETHOD         =       -82,
    STATE_INVALID_REQHANDLER        =       -83,
    STATE_INVALID_REQPACKET         =       -83,
    
    STATE_INVALID_CONTEXT           =       -90,
    STATE_INVALID_PRIORITY          =       -91,
    
    STATE_OBJECT_BUSY               =       -100,
    
    
    
    STATE_INVALID_PARAMETER         =       -250,
    /**/
    //STATE_PIPE_ERROR              =       -80,
}E_STATUS;

#ifndef FAR
#define FAR
#endif

#define VOID void
typedef void * PVOID;
typedef void FAR * LPVOID;

typedef unsigned long SIZE_T;
typedef unsigned long * PSIZE_T;
typedef unsigned long FAR * LPSIZE_T;

#define     INVALID_VALUE           ((DWORD)~(0ULL))
#define     DWORD_WIDTH             (sizeof(DWORD) * BYTE_WIDTH)
typedef unsigned long DWORD;
typedef unsigned long * PDWORD;
typedef unsigned long FAR * LPDWORD;


#define     BYTE_WIDTH              (8) 
typedef unsigned char BYTE;
typedef unsigned char * PBYTE;
typedef unsigned char FAR * LPBYTE;

typedef char * LPSTR;

#define     WORD_WIDTH              (sizeof(WORD) * BYTE_WIDTH)
typedef unsigned short WORD;
typedef unsigned short * PWORD;
typedef unsigned short FAR * LPWORD;

#define     QWORD_WIDTH             (sizeof(QWORD) * BYTE_WIDTH)
typedef unsigned long long QWORD;
typedef unsigned long long * PQWORD;
typedef unsigned long long FAR * LPQWORD;


#define TASK_PRIORITY_REAL                  0
#define TASK_PRIORITY_NORMAL                (TASK_PRIORITY_IDLE - 1)
#define TASK_PRIORITY_IDLE                  255

typedef unsigned char TASK_PRIORITY;
typedef unsigned char * PTASK_PRIORITY;
typedef unsigned char FAR * LPTASK_PRIORITY;

#define FALSE   0
#define TRUE    1
typedef unsigned int BOOL;
typedef unsigned int * PBOOL;
typedef unsigned int FAR * LPBOOL;
typedef unsigned long MMADDR;
#define CORE_DisableIRQ()        0
#define CORE_RestoreIRQ(x)

BYTE CORE_GetRegionID(MMADDR Address);

//#define CONFIG_BUILD_LITE_MAP

#ifdef CONFIG_BUILD_LITE_MAP
CONST BYTE g_HowManyBitsTable[16] = 
/* The size of how many bits table is 16 bytes */
{
    0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4, /* 0x00 ~ 0x0F */
};

CONST BYTE g_HighestBitmapTable[16] =
/* The size of highest bit table is 16 bytes */
{
    8, 0, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, /* Mask 0x00 ~ 0x0F */
};

CONST BYTE g_LowestBitTable[16]   =
/* The size of lowest bitmap table is 256 bytes */
{
    8, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, /* 0x00 ~ 0x0F */
};

#else

CONST BYTE g_HowManyBitsTable[256] = 
/* The size of how many bits table is 256 bytes */
{
    0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4, /* 0x00 ~ 0x0F */
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, /* 0x10 ~ 0x1F */
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, /* 0x20 ~ 0x2F */
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, /* 0x30 ~ 0x3F */
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, /* 0x40 ~ 0x4F */
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, /* 0x50 ~ 0x5F */
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, /* 0x60 ~ 0x6F */
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, /* 0x70 ~ 0x7F */
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, /* 0x80 ~ 0x8F */
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, /* 0x90 ~ 0x9F */
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, /* 0xA0 ~ 0xAF */
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, /* 0xB0 ~ 0xBF */
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, /* 0xC0 ~ 0xCF */
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, /* 0xD0 ~ 0xDF */
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, /* 0xE0 ~ 0xEF */
    4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8, /* 0xF0 ~ 0xFF */
};

CONST BYTE g_HighestBitmapTable[256] =
/* The size of highest bit table is 256 bytes */
{
    8, 0, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, /* Mask 0x00 ~ 0x0F */
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, /* Mask 0x10 ~ 0x1F */
    5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, /* Mask 0x20 ~ 0x2F */
    5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, /* Mask 0x30 ~ 0x3F */
    6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, /* Mask 0x40 ~ 0x4F */
    6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, /* Mask 0x50 ~ 0x5F */
    6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, /* Mask 0x60 ~ 0x6F */
    6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, /* Mask 0x70 ~ 0x7F */
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, /* Mask 0x80 ~ 0x8F */
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, /* Mask 0x90 ~ 0x9F */
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, /* Mask 0xA0 ~ 0xAF */
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, /* Mask 0xB0 ~ 0xBF */
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, /* Mask 0xC0 ~ 0xCF */
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, /* Mask 0xD0 ~ 0xDF */
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, /* Mask 0xE0 ~ 0xEF */
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, /* Mask 0xF0 ~ 0xFF */
};

CONST BYTE g_LowestBitTable[256]   =
/* The size of lowest bitmap table is 256 bytes */
{
    8, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, /* 0x00 ~ 0x0F */
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, /* 0x10 ~ 0x1F */
    5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, /* 0x20 ~ 0x2F */
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, /* 0x30 ~ 0x3F */
    6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, /* 0x40 ~ 0x4F */
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, /* 0x50 ~ 0x5F */
    5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, /* 0x60 ~ 0x6F */
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, /* 0x70 ~ 0x7F */
    7, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, /* 0x80 ~ 0x8F */
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, /* 0x90 ~ 0x9F */
    5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, /* 0xA0 ~ 0xAF */
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, /* 0xB0 ~ 0xBF */
    6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, /* 0xC0 ~ 0xCF */
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, /* 0xD0 ~ 0xDF */
    5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, /* 0xE0 ~ 0xEF */
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, /* 0xF0 ~ 0xFF */
};

#endif

#define __attribute__(x)
#define     WINAPI      __stdcall
EXTERN DWORD WINAPI GetTickCount(VOID);

/********************************************************************************************
                           中间部分为内存管理器调试代码
********************************************************************************************/
#ifndef TEST_FOR_WINDOWS
#define     MM_ASSERT(condition, code, ...)
#endif
#define     REGION_ID_MASK              0x7
#define     REGION_ID_SHIFT             16
#define     MAKE_ALLOC_ID(RID, PID)     (((RID) << REGION_ID_SHIFT) | ((PID) & PAGE_ID_MASK))
#define     ALLOC_PAGE_ID(AID)          ((AID) & PAGE_ID_MASK)
#define     ALLOC_REGION_ID(AID)        (((AID) >> REGION_ID_SHIFT) & REGION_ID_MASK)

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
#define     PAGE_TAILPAGES_MASK         0x00003fff
#define     PAGE_TAILPAGES_SHIFT        0
#define     PAGE_SECTFLAGS_INVALID      0
#define     PAGE_SECTFLAGS_MASK         (PAGE_HEAD_BIT_MASK | PAGE_MANY_BIT_MASK)
#define     PAGE_SECTFLAGS_SHIFT        (PAGE_HEAD_BIT_SHIFT < PAGE_MANY_BIT_SHIFT ? PAGE_HEAD_BIT_SHIFT : PAGE_MANY_BIT_SHIFT)
#define     MM_DisableIRQ()             CORE_DisableIRQ()
#define     MM_RestoreIRQ(x)            CORE_RestoreIRQ(x)

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
/************************************************************************/

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



typedef struct tagMM_REGION{
    DWORD               Magic;
    BOOL                RegionOnChip;                                /**< 该区域为片上内存 */
    MMADDR              RegionStart;                                 /**< 该区域的起始地址 */
    WORD                RegionPages;                                 /**< 该区域的页数量*/
    WORD                RegionUsedCount;                             /**< 该区域非空闲节数量 */
    MM_PAGE_ENTRIES     RegionUsedTable;                             /**< 该区域非空闲节链表 */
    MM_PAGE_ENTRIES     RegionBuddyTable[CONFIG_MMS_BUDDY_MAX];   /**< 该区域伙伴链表 */
    WORD                RegionBuddyCount[CONFIG_MMS_BUDDY_MAX];  /**< 该区域伙伴表计数器 */
    LPMM_PAGE_ENTRIES   lpRegionPageTable;                           /**< 该区域页表首指针 */
}MM_REGION, * PMM_REGION, FAR * LPMM_REGION;

typedef struct tagMM_MANAGER{
    WORD                SystemRegions;                  /**< 已注册到系统的区域数 */
    WORD                SystemBuddyMap;                 /**< 系统伙伴位图 */
    BYTE                RegionBuddyMap[CONFIG_MMS_BUDDY_MAX+1]; /**< 区域伙伴位图 */
    MM_REGION           RegionTable[CONFIG_MMS_REGION_MAX];     /**< 区域表 */
}MM_MANAGER, * PMM_MANAGER, FAR * LPMM_MANAGER;

STATIC  MM_MANAGER          g_SystemMemoryManager                           =   {0};
STATIC  MM_PAGE_ENTRIES     g_ChipRegionPageTable[CONFIG_MMS_ONCHIP_PAGES] =   {0};
STATIC  WORD                g_SectionBuddyMaskTable[] =
/* The size of g_SectionBuddyMaskTable is 32 bytes */
{
    0xffff, 0xfffe, 0xfffc, 0xfff8, 0xfff0, 0xffe0, 0xffc0, 0xff80, /* Buddy 0x00 ~ 0x07 */
    0xff00, 0xfe00, 0xfc00, 0xf800, 0xf000, 0xe000, 0xc000, 0x8000, /* Buddy 0x08 ~ 0x0f */
    0x0000,                                                         /* Invalid buddy */
};

#if 0
CONST
STATIC  DWORD               g_SectionCapacityTable[]      =   {
    0x00000001, 0x00000002, 0x00000004, 0x00000008,
    0x00000010, 0x00000020, 0x00000040, 0x00000080,
    0x00000100, 0x00000200, 0x00000400, 0x00000800,
    0x00001000, 0x00002000, 0x00004000, 0x00008000,
};
#endif
STATIC INLINE LPSTR GetBooleanString(BOOL Boolean)
{
    LPSTR lpString[] = { "N", "Y" };

    return lpString[Boolean & 1];
}
STATIC INLINE WORD GetBuddyMask(BYTE Buddy)
{
    return g_SectionBuddyMaskTable[Buddy & 0xf];
}

#ifdef CONFIG_BUILD_LITE_MAP
STATIC INLINE BYTE GetByteHowManyBits(BYTE Value)
{
    return g_HowManyBitsTable[(Value >> 0) & 0xf] + g_HowManyBitsTable[(Value >> 4) & 0xf];
}

STATIC INLINE BYTE GetWordHighestBit(WORD Mask)
{
    BYTE Llb = g_HighestBitmapTable[(Mask >> 0) & 0xf];
    BYTE Lhb = g_HighestBitmapTable[(Mask >> 4) & 0xf];
    BYTE Hlb = g_HighestBitmapTable[(Mask >> 8) & 0xf];
    BYTE Hhb = g_HighestBitmapTable[(Mask >> 12) & 0xf];
   
    CORE_DEBUG(TRUE, "Llb:%d,  Lhb:%d,  Hlb:%d,  Hhb:%d", Llb, Lhb, Hlb, Hhb);
    if (Hhb < 8) return Hhb + 12;
    else if (Hlb < 8) return Hlb + 8;
    else if (Lhb < 8) return Lhb + 4;
    else if (Llb < 8) return Llb;

    return 16;
}

STATIC INLINE BYTE GetWordLowestBit(WORD Mask)
{
    BYTE Llb = g_LowestBitTable[(Mask >> 0) & 0xf];
    BYTE Lhb = g_LowestBitTable[(Mask >> 4) & 0xf];
    BYTE Hlb = g_LowestBitTable[(Mask >> 8) & 0xf];
    BYTE Hhb = g_LowestBitTable[(Mask >> 12) & 0xf];

    if (Llb < 8) return Llb;
    if (Lhb < 8) return Lhb + 4;
    if (Hlb < 8) return Hlb + 8;
    if (Hhb < 8) return Hhb + 12;

    return 16;
}

#else

STATIC INLINE BYTE GetByteHowManyBits(BYTE Value)
{
    return g_HowManyBitsTable[Value];
}

STATIC INLINE BYTE GetWordHighestBit(WORD Value)
{
    BYTE Lb = g_HighestBitmapTable[(Value >> 0) & 0xff];
    BYTE Hb = g_HighestBitmapTable[(Value >> 8) & 0xff];
    
    if (Hb < 8) return Hb + 8;
    if (Lb < 8) return Lb;

    return 16;
}

STATIC INLINE BYTE GetWordLowestBit(WORD Mask)
{
    BYTE Lb = g_LowestBitTable[(Mask >> 0) & 0xff];
    BYTE Hb = g_LowestBitTable[(Mask >> 8) & 0xff];

    if (Lb < 8) return Lb;
    if (Hb < 8) return Hb + 8;
    
    return 16;
}
#endif

#ifdef TEST_FOR_WINDOWS
STATIC INLINE BYTE GetDwordLowestBit(DWORD Value)
{
    BYTE Lb = GetWordLowestBit( (Value >> 0) & 0xffff);
    BYTE Hb = GetWordLowestBit( (Value >> 16) & 0xffff);

    if (Lb < 16) return Lb;
    if (Hb < 16) return Hb + 16;
    
    return 32;
}
#endif

STATIC INLINE BYTE GetRequestBuddy(WORD Pages)
{
    BYTE Result = GetWordHighestBit(Pages);
    return Result + (!!(Pages ^ (1 << Result)));
}


STATIC INLINE BYTE GetTableBuddy(PAGE_ID_T FirstPage, WORD RemainPages)
{
    BYTE FirstBuddy = GetWordLowestBit(FirstPage);
    BYTE PagesBuddy = GetWordHighestBit(RemainPages);
    BYTE TableBuddy = MIN(FirstBuddy, PagesBuddy);

    CORE_DEBUG(TRUE, "FirstBuddy = %d,  PagesBuddy = %d", FirstBuddy, PagesBuddy);
    return MIN(TableBuddy, CONFIG_MMS_BUDDY_MAX - 1);
}

STATIC INLINE VOID SetPrevPage(LPMM_HEAD_PAGE lpHead, PAGE_ID_T Page)
{
    lpHead->PrveSection = Page;
}

STATIC INLINE VOID SetNextPage(LPMM_TAIL_PAGE lpTail, PAGE_ID_T Page)
{
    lpTail->NextSection = Page;
}

STATIC INLINE VOID HeadPageInit(LPMM_HEAD_PAGE lpHead, PAGE_ID_T PrvePage, WORD Pages, BOOL Used)
{
    lpHead->UsedBoolean  = !!Used;
    lpHead->ManyBoolean  = TRUE;
    lpHead->PrveSection  = PrvePage;
    lpHead->ReservedBit  = 0;
    lpHead->HeadBoolean  = TRUE;
    lpHead->SectionPages = Pages - 1;
}

STATIC INLINE VOID TailPageInit(LPMM_TAIL_PAGE lpTail, PAGE_ID_T NextPage, WORD Pages, BOOL Used)
{
    lpTail->UsedBoolean  = !!Used;
    lpTail->ManyBoolean  = TRUE;
    lpTail->NextSection  = NextPage;
    lpTail->ReservedBit  = 0;
    lpTail->HeadBoolean  = FALSE;
    lpTail->SectionPages = Pages - 1;
}


STATIC INLINE VOID SelfPageInit(LPMM_SELF_PAGE lpSelf, PAGE_ID_T PrvePage, PAGE_ID_T NextPage, BOOL Used)
{
    lpSelf->UsedBoolean  = !!Used;
    lpSelf->ManyBoolean  = FALSE;
    lpSelf->PrveSection  = PrvePage;
    lpSelf->ReservedBit  = 0;
    lpSelf->HeadBoolean  = TRUE;
    lpSelf->NextSection  = NextPage;
}



STATIC PAGE_ID_T FinalPage2StartPage(LPMM_REGION lpRegion, PAGE_ID_T PageID)
{
    register LPMM_PAGE_ENTRIES lpEntries = &lpRegion->lpRegionPageTable[PageID];

    if (!lpEntries->SelfPage.ManyBoolean)
    {
        MM_ASSERT(lpEntries->SelfPage.HeadBoolean, while(1);, "Page %u is not a final page !", PageID);
        CORE_DEBUG(TRUE, "It's self page head, final %d to start %d.", PageID, PageID);
        return PageID;
    }

    MM_ASSERT(!lpEntries->SelfPage.HeadBoolean, while(1);, "Page %u is not a final page !", PageID);
    CORE_DEBUG(TRUE, "It's many page tail, final %d to start %d.", PageID, PageID - lpEntries->TailPage.SectionPages);

    return (PageID - lpEntries->TailPage.SectionPages);
}

STATIC PAGE_ID_T StartPage2FinalPage(LPMM_REGION lpRegion, PAGE_ID_T PageID)
{
    register LPMM_PAGE_ENTRIES lpEntries = &lpRegion->lpRegionPageTable[PageID];

    MM_ASSERT(lpEntries->SelfPage.HeadBoolean, while(1);, "Page %u is not a start page !", PageID);

    if (!lpEntries->SelfPage.ManyBoolean)
    {
        CORE_DEBUG(TRUE, "It's self page head, start %d to final %d.", PageID, PageID);
        return PageID;
    }

    CORE_DEBUG(TRUE, "It's many page head, start %d to final %d.", PageID, PageID + lpEntries->TailPage.SectionPages);

    return (PageID + lpEntries->HeadPage.SectionPages);
}

STATIC PAGE_ID_T GetNextSectionStartID(LPMM_REGION lpRegion, PAGE_ID_T PageID)
{
    register LPMM_PAGE_ENTRIES lpEntries = &lpRegion->lpRegionPageTable[PageID];
    
    if (!lpEntries->SelfPage.ManyBoolean)
    {
        if (!lpEntries->SelfPage.HeadBoolean)
        {
            CORE_DEBUG(TRUE, "Invalid page object !");
            return INVALID_PAGE_ID;
        }

        return lpEntries->SelfPage.NextSection;
    }

    if (lpEntries->SelfPage.HeadBoolean)
    {
        CORE_DEBUG(TRUE, "It's head page, next section start page is %d",
            lpRegion->lpRegionPageTable[PageID + lpEntries->HeadPage.SectionPages].TailPage.NextSection);
        return lpRegion->lpRegionPageTable[PageID + lpEntries->HeadPage.SectionPages].TailPage.NextSection;
    }

    CORE_DEBUG(TRUE, "It's tail page, next section start page is %d", lpEntries->TailPage.NextSection);

    return lpEntries->TailPage.NextSection;
}

STATIC PAGE_ID_T GetPrveSectionFinalID(LPMM_REGION lpRegion, PAGE_ID_T PageID)
{
    register LPMM_PAGE_ENTRIES lpEntries = &lpRegion->lpRegionPageTable[PageID];

    if (!lpEntries->SelfPage.ManyBoolean)
    {
        if (!lpEntries->SelfPage.HeadBoolean)
        {
            CORE_DEBUG(TRUE, "Invalid page object !");
            return INVALID_PAGE_ID;
        }

        return lpEntries->SelfPage.PrveSection;
    }

    if (lpEntries->SelfPage.HeadBoolean)
    {
        CORE_DEBUG(TRUE, "It's head page, prve section final page is %d", lpEntries->HeadPage.PrveSection);
        return lpEntries->HeadPage.PrveSection;
    }

    CORE_DEBUG(TRUE, "It's tail page, next section start page is %d", 
        lpRegion->lpRegionPageTable[PageID - lpEntries->TailPage.SectionPages].HeadPage.PrveSection);

    return lpRegion->lpRegionPageTable[PageID - lpEntries->TailPage.SectionPages].HeadPage.PrveSection;
}

STATIC WORD GetSectionHeadPages(LPMM_REGION lpRegion, PAGE_ID_T PageID)
{
    register LPMM_PAGE_ENTRIES lpEntries = &lpRegion->lpRegionPageTable[PageID];

    if (!lpEntries->SelfPage.ManyBoolean)
    {
        if (!lpEntries->SelfPage.HeadBoolean)
        {
            CORE_DEBUG(TRUE, "Invalid page object !");

            return MAX_PAGES;
        }

        CORE_DEBUG(TRUE, "It's self page, section pages is %d", 1);

        return 1;
    }
    
    if (lpEntries->SelfPage.HeadBoolean)
    {
        CORE_DEBUG(TRUE, "It's head page, section pages is %d", lpEntries->HeadPage.SectionPages + 1);
        return lpEntries->HeadPage.SectionPages + 1;
    }

    CORE_DEBUG(TRUE, "It's tail page, section pages is %d", lpEntries->TailPage.SectionPages + 1);
    
    return lpEntries->TailPage.SectionPages + 1;
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

STATIC INLINE DWORD FreePageEntriesData(LPMM_REGION lpRegion, PAGE_ID_T PageID)
{
    return lpRegion->lpRegionPageTable[PageID].Entries = 0;
}

#if 0
STATIC INLINE BOOL NotPageAddress(MMADDR Address)
{
    return (0 != ((Address) & (CONFIG_MMS_PAGE_SIZE - 1)))
}

STATIC INLINE LPMM_PAGE_ENTRIES GetSystemPageTable(VOID)
{
    return (g_ChipRegionPageTable);
}

STATIC INLINE LPMM_REGION GetRegionObject(LPMM_MANAGER lpManager, BYTE RegionID)
{
    return &lpManager->RegionTable[RegionID];
}

STATIC INLINE LPVOID GetPageAddress(LPMM_MANAGER lpManager, BYTE RegionID, PAGE_ID_T PageID)
{
    return (LPVOID)(lpManager->RegionTable[RegionID].RegionStart + PageID * CONFIG_MMS_PAGE_SIZE);
}

STATIC INLINE BYTE GetGlobalRegions(LPMM_MANAGER lpManager)
{
    return lpManager->SystemRegions;
}

STATIC INLINE BYTE AddGlobalRegions(LPMM_MANAGER lpManager)
{
    return lpManager->SystemRegions ++;
}

STATIC INLINE VOID SetSystemBuddyBit(LPMM_MANAGER lpManager, BYTE Buddy, BOOL Value)
{
    SET_BIT_VALUE(lpManager->SystemBuddyMap, Buddy, Value);
}

STATIC INLINE VOID SetRegionBuddyBit(LPMM_MANAGER lpManager, BYTE RegionID, BYTE Buddy, BOOL Value)
{
    SET_BIT_VALUE(lpManager->RegionBuddyMap[Buddy], RegionID, Value);
}

STATIC INLINE BYTE GetRegionBuddyMapValue(LPMM_MANAGER lpManager, BYTE Buddy)
{
    return lpManager->RegionBuddyMap[Buddy];
}

STATIC INLINE WORD GetSystemBuddyMapVlue(LPMM_MANAGER lpManager)
{
    return lpManager->SystemBuddyMap;
}

STATIC INLINE SetRegionPageTable(LPMM_REGION lpRegion, LPMM_PAGE_ENTRIES lpTable)
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
    return ((PageAddress - (lpRegion)->RegionStart) / CONFIG_MMS_PAGE_SIZE)
}

STATIC INLINE VOID RegionUsedSectionInit(LPMM_REGION lpRegion)
{
    SelfPageInit(&lpRegion->RegionUsedTable.SelfPage, INVALID_PAGE_ID, INVALID_PAGE_ID);
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
    return lpRegion->RegionUsedTable.SelfPage.NextSection;
}

STATIC INLINE VOID SetRegionUsedFinalID(LPMM_REGION lpRegion, PAGE_ID_T FinalPage)
{
    SetPrevPage(&lpRegion->RegionUsedTable.HeadPage, FinalPage);
}

STATIC INLINE PAGE_ID_T GetRegionUsedFinalID(LPMM_REGION lpRegion)
{
    return lpRegion->RegionUsedTable.SelfPage.PrveSection;
}

STATIC INLINE VOID SetRegionUsedNextID(LPMM_REGION lpRegion, PAGE_ID_T StartPage)
{
    SetNextPage(&lpRegion->lpRegionPageTable[lpRegion->RegionUsedTable.SelfPage.PrveSection].TailPage, StartPage);
}

STATIC INLINE VOID SetRegionUsedPrveID(LPMM_REGION lpRegion, PAGE_ID_T FinalPage)
{
    SetPrevPage(&lpRegion->lpRegionPageTable[lpRegion->RegionUsedTable.SelfPage.NextSection].HeadPage, FinalPage);
}


STATIC INLINE VOID BuddyTableInit(LPMM_REGION lpRegion, BYTE Buddy)
{
    SelfPageInit(&lpRegion->RegionBuddyTable[Buddy].SelfPage, INVALID_PAGE_ID, INVALID_PAGE_ID);
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
    return lpRegion->RegionBuddyTable[Buddy].SelfPage.NextSection;
}

STATIC INLINE VOID SetBuddyFinalID(LPMM_REGION lpRegion, BYTE Buddy, PAGE_ID_T FinalPage)
{
    SetPrevPage(&lpRegion->RegionBuddyTable[Buddy].HeadPage, FinalPage);
}

STATIC INLINE PAGE_ID_T GetBuddyFinalID(LPMM_REGION lpRegion, BYTE Buddy)
{
    return lpRegion->RegionBuddyTable[Buddy].SelfPage.PrveSection;
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

STATIC INLINE LPMM_SELF_PAGE GetBuddyEntries(LPMM_REGION lpRegion, BYTE Buddy)
{
    return &lpRegion->RegionBuddyTable[Buddy].SelfPage;
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

STATIC VOID BuddySplitSection(LPMM_MANAGER lpManager, BYTE RegionID, PAGE_ID_T StartPageID, WORD RemainPages)
{
    while(RemainPages)
    {
        BYTE Buddy = GetTableBuddy(StartPageID, RemainPages);

        BuddyAddSection(lpManager, RegionID, StartPageID, Buddy);
        
        CORE_DEBUG(TRUE, "Start %d, remain %d, Buddy %d, pages %d "
                        "system map = 0x%04x,  region map = 0x%02x",
                        StartPageID, RemainPages, Buddy, 1<<Buddy,
                        GetSystemBuddyMapVlue(lpManager),
                        GetRegionBuddyMapValue(lpManager, Buddy));
    
        (StartPageID)  += (1 << Buddy);
        (RemainPages) -= (1 << Buddy);  
    }
}

#else
#define     NotPageAddress(Address)                 (0 != ((Address) & (CONFIG_MMS_PAGE_SIZE - 1)))
#define     GetSystemPageTable()                    (g_ChipRegionPageTable)
#define     GetRegionObject(lpM, ID)                (&(lpM)->RegionTable[ID])
#define     GetPageAddress(lpM, RID, PID)           (LPVOID)(lpM->RegionTable[RID].RegionStart + (PID) * CONFIG_MMS_PAGE_SIZE)
#define     GetGlobalRegions(lpM)                   ((lpM)->SystemRegions)
#define     AddGlobalRegions(lpM)                   ((lpM)->SystemRegions ++)
#define     SetSystemBuddyBit(lpM, B, V)            SET_BIT_VALUE((lpM)->SystemBuddyMap, B, V)
#define     SetRegionBuddyBit(lpM, ID, B, V)        SET_BIT_VALUE((lpM)->RegionBuddyMap[B], ID, V)
#define     GetRegionBuddyMapValue(lpM, B)          ((lpM)->RegionBuddyMap[B])
#define     GetSystemBuddyMapVlue(lpM)              ((lpM)->SystemBuddyMap)
/*some region operation macro*/
#define     RegionUsedSectionInit(lpR)              SelfPageInit(&(lpR)->RegionUsedTable.SelfPage, INVALID_PAGE_ID, INVALID_PAGE_ID, FALSE)
#define     SubRegionUsedCount(lpR)                 (-- (lpR)->RegionUsedCount)
#define     AddRegionUsedCount(lpR)                 ((lpR)->RegionUsedCount ++)
#define     GetRegionUsedCount(lpR)                 ((lpR)->RegionUsedCount)
#define     SetRegionUsedStartID(lpR, P)            SetNextPage(&(lpR)->RegionUsedTable.TailPage, P)
#define     GetRegionUsedStartID(lpR)               ((lpR)->RegionUsedTable.SelfPage.NextSection)
#define     SetRegionUsedFinalID(lpR, P)            SetPrevPage(&(lpR)->RegionUsedTable.HeadPage, P);
#define     GetRegionUsedFinalID(lpR)               ((lpR)->RegionUsedTable.SelfPage.PrveSection)
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
#define     GetRegionPageID(lpR, A)                 (((A) - (lpRegion)->RegionStart) / CONFIG_MMS_PAGE_SIZE)
#define     GetRegionStart(lpR)                     ((lpR)->RegionStart)
#define     SetRegionStart(lpR, Start)              do { (lpR)->RegionStart = (Start); } while(0)
/* some buddy operation macro */
#define     BuddyTableInit(lpR, B)                  SelfPageInit(&(lpR)->RegionBuddyTable[B].SelfPage, INVALID_PAGE_ID, INVALID_PAGE_ID, FALSE)
#define     AddBuddyCount(lpR, B)                   ((lpR)->RegionBuddyCount[B] ++)
#define     SubBuddyCount(lpR, B)                   (-- (lpR)->RegionBuddyCount[B])
#define     SetBuddyStartID(lpR, B, P)              SetNextPage(&(lpR)->RegionBuddyTable[B].TailPage, P)
#define     GetBuddyStartID(lpR, B)                 ((lpR)->RegionBuddyTable[B].SelfPage.NextSection)
#define     SetBuddyFinalID(lpR, B, P)              SetPrevPage(&(lpR)->RegionBuddyTable[B].HeadPage, P)
#define     GetBuddyFinalID(lpR, B)                 ((lpR)->RegionBuddyTable[B].SelfPage.PrveSection)
#define     SetBuddyNextID(lpR, B, P)               SetNextPage(&(lpR)->lpRegionPageTable[(lpR)->RegionBuddyTable[B].SelfPage.PrveSection].TailPage, P)
#define     SetBuddyPrveID(lpR, B, P)               SetPrevPage(&(lpR)->lpRegionPageTable[(lpR)->RegionBuddyTable[B].SelfPage.NextSection].HeadPage, P)

#define     GetBuddyEntries(lpR, B)                 (&(lpR)->RegionBuddyTable[B].SelfPage)
#define     GetPageEntries(lpR, P)                  (&(lpR)->lpRegionPageTable[P])
#define     GetHeadPageEntries(lpR, P)              (&(lpR)->lpRegionPageTable[P].HeadPage)
#define     GetTailPageEntries(lpR, P)              (&(lpR)->lpRegionPageTable[P].TailPage)
#define     GetSelfPageEntries(lpR, P)              (&(lpR)->lpRegionPageTable[P].SelfPage)
#define     GetRegionUsedEntries(lpR)               (&(lpR)->RegionUsedTable.SelfPage)

#define     BuddySplitSection(lpManager, ID, Start, Remain)                         \
            do{                                                                     \
                while (Remain)                                                      \
                {                                                                   \
                    BYTE Buddy = GetTableBuddy(Start, Remain);                      \
                    BuddyAddSection(lpManager, ID, Start, Buddy);                   \
                    CORE_DEBUG(TRUE, "Start %d, remain %d, Buddy %d, pages %d "     \
                                    "system map = 0x%04x,  region map = 0x%02x",    \
                                    Start, Remain, Buddy, 1<<Buddy,                 \
                                    GetSystemBuddyMapVlue(lpManager),               \
                                    GetRegionBuddyMapValue(lpManager, Buddy));      \
                    (Start)  += (1 << Buddy);                                       \
                    (Remain) -= (1 << Buddy);                                       \
                }                                                                   \
            }while(0)
#endif


STATIC E_STATUS RegionEnumerateUsedSection(LPMM_MANAGER lpManager, BYTE RegionID)
{
    DWORD Count = 0;
    PAGE_ID_T StartPage = 0;
    PAGE_ID_T FinalPage = 0;
    PAGE_ID_T SectionPages = 0;
    PAGE_ID_T NextSection = 0;
    LPMM_REGION lpRegion = NULL;
    
    if (RegionID >= CONFIG_MMS_REGION_MAX)
    {
        CORE_INFOR(TRUE, "Invalid region id %u.", RegionID);
        return STATE_INVALID_REGION;
    }
    
    lpRegion = GetRegionObject(lpManager, RegionID);
    
    if (SMM_MAGIC != GetRegionMagic(lpRegion))
    {
        CORE_INFOR(TRUE, "Invalid region object, id %u.", RegionID);
        return STATE_INVALID_REGION;
    }

    if (GetRegionUsedCount(lpRegion) == 0)
    {
        CORE_INFOR(TRUE, "This region %d is free.", RegionID);
        return STATE_INVALID_PAGE;
    }

    StartPage = GetRegionUsedStartID(lpRegion);
    FinalPage = StartPage2FinalPage(lpRegion, StartPage);
    SectionPages = GetSectionHeadPages(lpRegion, StartPage);
    NextSection = GetNextSectionStartID(lpRegion, StartPage);

    
    CORE_INFOR(TRUE, "Many | Head | Used | Start |   Value   | Many | Head | Used | Final | Pages | Next | Value");

    for (Count = 0; Count < GetRegionUsedCount(lpRegion); Count ++)
    {
        CORE_INFOR(TRUE, "  %s      %s      %s    0x%04X   0x%08x   %s      %s      %s    0x%04X  0x%04X 0x%04X  0x%08x",
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
        SectionPages = GetSectionHeadPages(lpRegion, StartPage);
        NextSection = GetNextSectionStartID(lpRegion, StartPage);
    }

    return STATE_SUCCESS;
}


STATIC VOID BuddyAddSection(LPMM_MANAGER lpManager, BYTE RegionID, PAGE_ID_T StartPage, BYTE Buddy)
{
    WORD Pages = 1 << Buddy;
    LPMM_REGION lpRegion = GetRegionObject(lpManager, RegionID);
    PAGE_ID_T FinalPage = StartPage + Pages - 1;
    LPMM_SELF_PAGE lpSelf = GetSelfPageEntries(lpRegion, StartPage);
    LPMM_HEAD_PAGE lpHead = GetHeadPageEntries(lpRegion, StartPage);
    LPMM_TAIL_PAGE lpTail = GetTailPageEntries(lpRegion, FinalPage);

    if (AddBuddyCount(lpRegion, Buddy))
        /* 如果 BUDDY 不为空链表 */
    {
        if (FinalPage == StartPage)
        {
            SelfPageInit(lpSelf,
                GetBuddyFinalID(lpRegion, Buddy),           /* 左边页 */
                GetBuddyStartID(lpRegion, Buddy),           /* 右边页 */
                FALSE);
        }
        else
        {
            HeadPageInit(lpHead, GetBuddyFinalID(lpRegion, Buddy), Pages, FALSE);
            TailPageInit(lpTail, GetBuddyStartID(lpRegion, Buddy), Pages, FALSE);
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
            SelfPageInit(lpSelf, FinalPage, StartPage, FALSE);
        }
        else
        {
            HeadPageInit(lpHead, FinalPage, Pages, FALSE);
            TailPageInit(lpTail, StartPage, Pages, FALSE);
        }

        SetBuddyStartID(lpRegion, Buddy, StartPage);
        SetBuddyFinalID(lpRegion, Buddy, FinalPage);
    }


    CORE_DEBUG(TRUE, "Buddy add section to region %d, StartPage %d, buddy %d, Buddy map value 0x%04x", RegionID, StartPage, Buddy, GetSystemBuddyMapVlue(lpManager));
    SetSystemBuddyBit(lpManager, Buddy, TRUE);
    SetRegionBuddyBit(lpManager, RegionID, Buddy, TRUE);
}

STATIC PAGE_ID_T BuddyDelSection(LPMM_MANAGER lpManager, BYTE RegionID, BYTE Buddy)
{
    LPMM_REGION lpRegion = GetRegionObject(lpManager, RegionID);
    PAGE_ID_T StartPage = GetBuddyStartID(lpRegion, Buddy);
    PAGE_ID_T FinalPage = StartPage + (1 << Buddy) - 1;
    PAGE_ID_T NextStart = GetNextSectionStartID(lpRegion, FinalPage);
    PAGE_ID_T PrveFinal = GetPrveSectionFinalID(lpRegion, StartPage);

    CORE_DEBUG(TRUE, "Alloc From Region %d, Buddy %d, PageID: %u", RegionID, Buddy, StartPage);
    /* 如果BUDDY组的还有SECTION，需要处理剩余的 SECTION */
    if (SubBuddyCount(lpRegion, Buddy))
    {
        /* 设置链表头 */
        SetBuddyStartID(lpRegion, Buddy, NextStart);
        /* 设置右边节点首页上的左边页(链表尾部处理) */
        SetBuddyPrveID(lpRegion, Buddy, PrveFinal);
        /* 设置左边节点尾页上的右边页(链表头部处理) */
        SetBuddyNextID(lpRegion, Buddy, NextStart);
    }
    else
    /* 如果BUDDY组的没有SECTION，则将链表头的置空，并处理相应的BUDDY位图标志 */
    {
        /* 设置 BUDDY 组的伙伴位图 */
        SetRegionBuddyBit(lpManager, RegionID, Buddy, FALSE);
        /* 设置全局伙伴位图 */
        SetSystemBuddyBit(lpManager, Buddy, !! GetRegionBuddyMapValue(lpManager, Buddy));
        /* 清空链表 */
        BuddyTableInit(lpRegion, Buddy);
    }

    return StartPage;
}

STATIC VOID RegionAddSection(LPMM_MANAGER lpManager, BYTE RegionID, PAGE_ID_T StartPage, WORD Pages)
{
    PAGE_ID_T FinalPage = StartPage + Pages - 1;
    LPMM_REGION lpRegion = GetRegionObject(lpManager, RegionID);
    LPMM_SELF_PAGE lpSelf = GetSelfPageEntries(lpRegion, StartPage);
    LPMM_HEAD_PAGE lpHead = GetHeadPageEntries(lpRegion, StartPage);
    LPMM_TAIL_PAGE lpTail = GetTailPageEntries(lpRegion, FinalPage);
    
    if (AddRegionUsedCount(lpRegion))
        /* 当链表非空时 */
    {
        if (FinalPage == StartPage)
        {
            SelfPageInit(lpSelf,
                GetRegionUsedFinalID(lpRegion),         /* 左边页 */
                GetRegionUsedStartID(lpRegion),         /* 右边页 */
                TRUE);
        }
        else
        {
            HeadPageInit(lpHead, GetRegionUsedFinalID(lpRegion), Pages, TRUE);
            TailPageInit(lpTail, GetRegionUsedStartID(lpRegion), Pages, TRUE);
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
            SelfPageInit(lpSelf, FinalPage, StartPage, TRUE);
        }
        else
        {
            HeadPageInit(lpHead, FinalPage, Pages, TRUE);
            TailPageInit(lpTail, StartPage, Pages, TRUE);
        }

        SetRegionUsedStartID(lpRegion, StartPage);
        SetRegionUsedFinalID(lpRegion, FinalPage);
    }
}

STATIC VOID RegionDelSection(LPMM_MANAGER lpManager, BYTE RegionID, PAGE_ID_T StartPage, PAGE_ID_T FinalPage)
{
    LPMM_REGION lpRegion = GetRegionObject(lpManager, RegionID);
    PAGE_ID_T NextStart = GetNextSectionStartID(lpRegion, FinalPage);
    PAGE_ID_T PrveFinal = GetPrveSectionFinalID(lpRegion, StartPage);
//  LPMM_SELF_PAGE lpHome = GetRegionUsedEntries(lpRegion);
//  LPMM_PAGE_ENTRIES lpHead = GetPageEntries(lpRegion, StartPage);
//  LPMM_PAGE_ENTRIES lpTail = GetPageEntries(lpRegion, FinalPage);
    LPMM_PAGE_ENTRIES lpPrve = GetPageEntries(lpRegion, PrveFinal);
    LPMM_PAGE_ENTRIES lpNext = GetPageEntries(lpRegion, NextStart);

    if (SubRegionUsedCount(lpRegion))
    /* 如果链表非空 */
    {
        SetNextPage(&lpPrve->TailPage, NextStart);
        SetPrevPage(&lpNext->HeadPage, PrveFinal);
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


STATIC E_STATUS PageFreeAddress(LPMM_MANAGER lpManager, LPVOID PageAddress)
{
    E_STATUS State;
    BYTE RegionID = 0;
    WORD SectionPages = 0;
    PAGE_ID_T StartPage = 0;
    PAGE_ID_T FinalPage = 0;
    LPMM_REGION lpRegion = NULL;
    
    /* 检查是否为页对齐地址 */
    if (NotPageAddress((MMADDR) PageAddress))
    {
        CORE_INFOR(TRUE, "%p is not page address.", PageAddress);
        return STATE_INVALID_PAGE;
    }

    RegionID = CORE_GetRegionID((MMADDR) PageAddress);

    /* 检查 REGION ID 是否有效 */   
    if (RegionID >= CONFIG_MMS_REGION_MAX)
    {
        CORE_ERROR(TRUE, "%p is not in any region.", PageAddress);
        return STATE_INVALID_REGION;
    }
    
    lpRegion = GetRegionObject(lpManager, RegionID);

    /* 检查 REGION 是否已经初始化 */    
    if (SMM_MAGIC != GetRegionMagic(lpRegion))
    {
        CORE_ERROR(TRUE, "%p is not in any region.", PageAddress);
        return STATE_INVALID_REGION;
    }
    
    StartPage = GetRegionPageID(lpRegion, (MMADDR) PageAddress);

    /* 检查首页号是否有效 */    
    if (StartPage >= GetRegionPages(lpRegion))
    {
        CORE_ERROR(TRUE, "Free page %p id %d from region %d out of range.",
            PageAddress, StartPage, RegionID);
        return STATE_OVER_RANGE;
    }

    /* 如果页面无使用标志则失败 */  
    if (TRUE != GetPageUsedBit(lpRegion, StartPage))
    {
        CORE_ERROR(TRUE, "Free page %p id %d from region %d is not free.",
            PageAddress, StartPage, RegionID);
        return STATE_INVALID_PAGE;
    }

    /* 如果页面不是首页则失败 */
    if (TRUE != GetPageHeadBit(lpRegion, StartPage))
    {
        CORE_ERROR(TRUE, "Free page %p id %d from region %d is not valid head/tail page.",
            PageAddress, StartPage, RegionID);
        return STATE_INVALID_PAGE;
    }

    SectionPages = GetSectionHeadPages(lpRegion, StartPage);

    /* 如果SECTION页数量大于REGION总页数，则失败 */
    if (SectionPages > GetRegionPages(lpRegion))
    {
        CORE_ERROR(TRUE, "Free page %p id %d section pages %d from region %d out of range.",
            PageAddress, StartPage, SectionPages, RegionID);
        return STATE_OVER_RANGE;
    }

    FinalPage = StartPage2FinalPage(lpRegion, StartPage);

    CORE_DEBUG(TRUE, "Free page %p start %d final %d from region %d.", PageAddress, StartPage, FinalPage, RegionID);

    RegionDelSection(lpManager, RegionID, StartPage, FinalPage);
    BuddyAddSection(lpManager, RegionID, StartPage, GetRequestBuddy(SectionPages));
    
    return STATE_INVALID_PAGE;
}


STATIC DWORD PageAllocID(LPMM_MANAGER lpManager, WORD Pages)
{
    PAGE_ID_T StartPage = 0;
//  PAGE_ID_T FinalPage = 0;
    PAGE_ID_T StartNext = 0;
    WORD RemainPages = 0;
    BYTE RequestBuddy = GetRequestBuddy(Pages);
    BYTE RegionBuddy = GetWordLowestBit(GetBuddyMask(RequestBuddy) & GetSystemBuddyMapVlue(lpManager));
    BYTE RegionID = GetWordLowestBit(GetRegionBuddyMapValue(lpManager, RegionBuddy));

    CORE_DEBUG(TRUE, "Alloc pages: %d, RequestBuddy: %d, RegionBuddy: %d, BuddyMask: 0x%04x",
        Pages, RequestBuddy, RegionBuddy, GetBuddyMask(RequestBuddy))

    CORE_DEBUG(TRUE, "SysBuddyMap: 0x%04x, RegBuddyMap: 0x%04x, RegionID: %d", 
        GetSystemBuddyMapVlue(lpManager), GetRegionBuddyMapValue(lpManager, RegionBuddy), RegionID);

    if (RegionID >= CONFIG_MMS_REGION_MAX)
    {
        return INVALID_VALUE;
    }
    
    StartPage = BuddyDelSection(lpManager, RegionID, RegionBuddy);
    StartNext = StartPage + Pages;
//  FinalPage = StartNext - 1;
    RemainPages = (1<<RegionBuddy) - Pages;

    CORE_DEBUG(TRUE, "Alloc pages: %d,  region: %d   RequestBuddy: %d, RegionBuddy: %d  StartPage: %d  RemainPages: %d",
                    Pages, RegionID, RequestBuddy, RegionBuddy, StartPage, RemainPages);
    
    RegionAddSection(lpManager, RegionID, StartPage, Pages);
    BuddySplitSection(lpManager, RegionID, StartNext, RemainPages);

    return MAKE_ALLOC_ID(RegionID, StartPage);
}


STATIC LPVOID PageAllocAddress(LPMM_MANAGER lpManager, WORD Pages)
{
    
    DWORD AllocID = PageAllocID(lpManager, Pages);
    
    if (INVALID_VALUE == AllocID)
    {
        CORE_ERROR(TRUE, "Alloc page %d failed.", Pages);
        return NULL;
    }

    CORE_DEBUG(TRUE, "Alloc %d pages, region id %d, page id %d, region start address %p, page address %p",
        Pages, ALLOC_REGION_ID(AllocID), ALLOC_PAGE_ID(AllocID),
        lpManager->RegionTable[ALLOC_REGION_ID(AllocID)].RegionStart,
        GetPageAddress(lpManager, ALLOC_REGION_ID(AllocID), ALLOC_PAGE_ID(AllocID)));

    return GetPageAddress(lpManager, ALLOC_REGION_ID(AllocID), ALLOC_PAGE_ID(AllocID));
}


STATIC VOID CreatePageTable(LPMM_MANAGER lpManager, BYTE RegionID)
{
    PAGE_ID_T StartPage = 0;
    LPMM_REGION lpRegion = GetRegionObject(lpManager, RegionID);
    WORD RemainPages = GetRegionPages(lpRegion);
    WORD TablePages = (RemainPages * sizeof(MM_PAGE_ENTRIES) / CONFIG_MMS_PAGE_SIZE)
                    + (!!(RemainPages * sizeof(MM_PAGE_ENTRIES) % CONFIG_MMS_PAGE_SIZE));
    LPMM_PAGE_ENTRIES lpTable = GetRegionOnchip(lpRegion)
                              ? GetSystemPageTable()
                              : (LPVOID) PageAllocAddress(lpManager, TablePages);

    if (NULL == lpTable)
    {
        MMADDR RegionStart = GetRegionStart(lpRegion);
        lpTable = (LPVOID) RegionStart;
        RegionStart += CONFIG_MMS_PAGE_SIZE * TablePages;
        CORE_INFOR(TRUE, "Change region start from %p to %p.", (LPVOID) GetRegionStart(lpRegion), (LPVOID) RegionStart);
        SetRegionStart(lpRegion, RegionStart);
        SetRegionPages(lpRegion, GetRegionPages(lpRegion) - TablePages);
        RemainPages = GetRegionPages(lpRegion);
    }

    CORE_INFOR(TRUE, "Create region id %d address %p pages %d.", RegionID, (LPVOID) GetRegionStart(lpRegion), RemainPages);
    
    memset(lpTable, 0, sizeof(MM_PAGE_ENTRIES) * RemainPages);
    SetRegionPageTable(lpRegion, lpTable);

    BuddySplitSection(lpManager, RegionID, StartPage, RemainPages);
    SetRegionMagic(lpRegion, SMM_MAGIC);
    
    return;
}


EXPORT E_STATUS CORE_CreateRegion(MMADDR Address, SIZE_T Length, BOOL OnChip)
{
    int i;
    DWORD RegionPages = 0;
    SIZE_T RegionLength = 0;
    MMADDR RegionAddress = 0;
    LPMM_MANAGER lpManager = &g_SystemMemoryManager;
    BYTE RegionID = CORE_GetRegionID(Address);
    LPMM_REGION lpRegion = GetRegionObject(lpManager, RegionID);
    SIZE_T Prefix = (Address % CONFIG_MMS_PAGE_SIZE)
                  ? CONFIG_MMS_PAGE_SIZE - (Address % CONFIG_MMS_PAGE_SIZE)
                  : (Address % CONFIG_MMS_PAGE_SIZE);
    SIZE_T Suffix = ((Address + Length) % CONFIG_MMS_PAGE_SIZE);
    
    if (SMM_MAGIC == GetRegionMagic(lpRegion) || CONFIG_MMS_REGION_MAX <= GetGlobalRegions(lpManager))
    {
        CORE_ERROR(TRUE, "Create too many regions.");
        return STATE_OVER_RANGE;
    }
    
    RegionAddress  =  (Address + Prefix);
    RegionLength   =  (Length - Prefix - Suffix);
    RegionPages    =  (RegionLength / CONFIG_MMS_PAGE_SIZE);
    
    CORE_INFOR(TRUE, "Create region id %d address %p size %ld bytes, pages %d.", RegionID, (LPVOID) RegionAddress, RegionLength, RegionPages);

    if (RegionPages > MAX_PAGES)
    {
        CORE_ERROR(TRUE, "The region have %d byte to large, I can't use it.", RegionLength);
        return STATE_OVER_RANGE;
    }
    
    if (TRUE == OnChip)
    {
        if (0 != GetGlobalRegions(lpManager))
        {
            CORE_ERROR(TRUE, "Create on chip region not support.");
            return STATE_NOT_SUPPORT;
        }
        
        if (CONFIG_MMS_ONCHIP_PAGES != RegionPages)
        {
            CORE_ERROR(TRUE, "On chip region pages '%d' not mutch define", RegionPages);
            return STATE_NOT_MATCH;
        }
    }

    AddGlobalRegions(lpManager);
    SetRegionOnchip(lpRegion, OnChip);
    SetRegionPages(lpRegion, RegionPages);
    SetRegionStart(lpRegion, RegionAddress);
    RegionUsedSectionInit(lpRegion);
    
    for (i = 0; i < CONFIG_MMS_BUDDY_MAX; i ++)
    {
        BuddyTableInit(lpRegion, i);
    }
    
    CreatePageTable(lpManager, RegionID);

    return STATE_SUCCESS;
}
EXPORT_SYMBOL(CORE_CreateRegion)
/********************************************************************************************
                           中间部分为内存管理器调试代码
********************************************************************************************/
EXPORT VOID kDebugShowData(E_LOG_LEVEL emLevel, LPVOID lpBuffer, SIZE_T Length)
{
    DWORD Scale = 0;
    LPBYTE lpData = lpBuffer;

    for (Scale = 0; Scale < Length; Scale ++)
    {
        if (0 ==(Scale & 0xf))
        {
            if (Scale)
            {
                printf("\r\n");
            }
            
            printf("%08X: ", Scale);
        }
        else if (0 == (Scale & 0x7))
        {
            printf("-");
        }
        else
        {
            printf(" ");
        }
        
        printf("%02X", lpData[Scale] & 0xff);
    }

    printf("\r\n");
}


STATIC VOID ShowByte2C(LPSTR lpName, LPVOID lpBuffer, SIZE_T Length)
{
    DWORD Scale = 0;
    LPBYTE lpData = lpBuffer;

    printf("/* The size of %s is %d bytes */\r\n{\r\n", lpName, Length);
    for (Scale = 0; Scale < Length; Scale ++)
    {
        if (0x0 ==((Scale & 0xf)))
        {
            printf("    ");
        }
        printf("%d, ", lpData[Scale] & 0xff);
        
        if (0xf ==((Scale & 0xf)))
        {
            printf("/* 0x%02X ~ 0x%02X */\r\n", Scale - 0xf, Scale);
        }
    }

    printf("};\r\n");
}

STATIC VOID ShowWord2C(LPSTR lpName, LPVOID lpBuffer, DWORD Count)
{
    DWORD Scale = 0;
    LPWORD lpData = lpBuffer;
    
    printf("STATIC\t\t%-16s%s[%d] =\r\n", "WORD", lpName, Count);
    printf("/* The size of %s is %d bytes */\r\n{\r\n", lpName, Count * sizeof(WORD));
    
    for (Scale = 0; Scale < Count; Scale ++)
    {
        if (0x0 ==((Scale & 0x7)))
        {
            printf("    ");
        }
        printf("0x%04x, ", lpData[Scale] & 0xffff);
        
        if (0x7 ==((Scale & 0x7)))
        {
            printf("/* Buddy 0x%02x ~ 0x%02x */\r\n", Scale - 0x7, Scale);
        }
    }
    
    printf("};\r\n");
}


STATIC DWORD CalcLowestBit(BYTE Value)
{

    DWORD Bits = 8;
    
    if (Value & (1 << 7)) Bits = 7;
    if (Value & (1 << 6)) Bits = 6;
    if (Value & (1 << 5)) Bits = 5;
    if (Value & (1 << 4)) Bits = 4;
    if (Value & (1 << 3)) Bits = 3;
    if (Value & (1 << 2)) Bits = 2;
    if (Value & (1 << 1)) Bits = 1;
    if (Value & (1 << 0)) Bits = 0;

    return Bits;

}

STATIC DWORD CalcHighestBit(BYTE Value)
{
    DWORD Bits = 8;
    
    if (Value & (1 << 0)) Bits = 0;
    if (Value & (1 << 1)) Bits = 1;
    if (Value & (1 << 2)) Bits = 2;
    if (Value & (1 << 3)) Bits = 3;
    if (Value & (1 << 4)) Bits = 4;
    if (Value & (1 << 5)) Bits = 5;
    if (Value & (1 << 6)) Bits = 6;
    if (Value & (1 << 7)) Bits = 7;

    return Bits;
}

STATIC WORD CalcBuddyMask(BYTE Value)
{
    int i = 0;
    WORD Mask = 0;
    
    for (i = 15; i >=Value; i--)
    {
        Mask |= (1<<i);
    }
    
    return Mask;
}

STATIC BYTE CalcHowManyBits(BYTE Value)
{
    int i = 0;

    for (; Value; Value >>= 1)
    {
        if (Value &1) i ++;
    }
    
    return i;
}

STATIC      BYTE MemoryOnChip[CONFIG_MMS_PAGE_SIZE * 32767];
STATIC      BYTE MemoryOffChip0[CONFIG_MMS_PAGE_SIZE * 32767];
STATIC      BYTE MemoryOffChip1[CONFIG_MMS_PAGE_SIZE * 32767];
STATIC      BYTE MemoryOffChip2[CONFIG_MMS_PAGE_SIZE * 32767];
STATIC      BYTE MemoryOffChip3[CONFIG_MMS_PAGE_SIZE * 32767];
STATIC      BYTE MemoryOffChip4[CONFIG_MMS_PAGE_SIZE * 32767];
STATIC      BYTE MemoryOffChip5[CONFIG_MMS_PAGE_SIZE * 32767];
STATIC      BYTE MemoryOffChip6[CONFIG_MMS_PAGE_SIZE * 32767];

BYTE CORE_GetRegionID(MMADDR PageAddress)
{
    BYTE ID = 0;
    MMADDR RegionAddress[][2] = {
        {(MMADDR) &MemoryOnChip,  ((MMADDR)&MemoryOnChip) + sizeof(MemoryOnChip)},
        {(MMADDR) &MemoryOffChip0,  ((MMADDR)&MemoryOffChip0) + sizeof(MemoryOffChip0)},
        {(MMADDR) &MemoryOffChip1,  ((MMADDR)&MemoryOffChip1) + sizeof(MemoryOffChip1)},
        {(MMADDR) &MemoryOffChip2,  ((MMADDR)&MemoryOffChip2) + sizeof(MemoryOffChip2)},
        {(MMADDR) &MemoryOffChip3,  ((MMADDR)&MemoryOffChip3) + sizeof(MemoryOffChip3)},
        {(MMADDR) &MemoryOffChip4,  ((MMADDR)&MemoryOffChip4) + sizeof(MemoryOffChip4)},
        {(MMADDR) &MemoryOffChip5,  ((MMADDR)&MemoryOffChip5) + sizeof(MemoryOffChip5)},
        {(MMADDR) &MemoryOffChip6,  ((MMADDR)&MemoryOffChip6) + sizeof(MemoryOffChip6)},
    };

    for (ID = 0; ID < CONFIG_MMS_REGION_MAX; ID ++)
    {
        if (PageAddress >= RegionAddress[ID][0] && PageAddress < RegionAddress[ID][1])
            break;
    }
    
    return ID;
}


int main(int argc, char * argv[])
{
    int i = 0;
    LPVOID Address[10] = {NULL};
    DWORD Tick0, Tick1;
#if 0
    for (i = 0; i < SIZEOF_ARRAY(g_LowestBitTable); i ++)
    {
        g_LowestBitTable[i] = CalcLowestBit(i);
    }
    
    for (i = 0; i < SIZEOF_ARRAY(g_HighestBitmapTable); i ++)
    {
        g_HighestBitmapTable[i] = CalcHighestBit(i);
    }
    
    for (i = 0; i < SIZEOF_ARRAY(g_SectionBuddyMaskTable); i++)
    {
        g_SectionBuddyMaskTable[i] = CalcBuddyMask(i);
    }
    
    for (i = 0; i < SIZEOF_ARRAY(g_HowManyBitsTable); i ++)
    {
        g_HowManyBitsTable[i] = CalcHowManyBits(i);
    }
#endif
    ShowByte2C("lowest bitmap table", g_LowestBitTable, sizeof(g_LowestBitTable));
    ShowByte2C("highest bitmap table", g_HighestBitmapTable, sizeof(g_HighestBitmapTable));
    ShowByte2C("how many bits table", g_HowManyBitsTable, sizeof(g_HowManyBitsTable));
    ShowWord2C("g_SectionBuddyMaskTable", g_SectionBuddyMaskTable, SIZEOF_ARRAY(g_SectionBuddyMaskTable));
    
    CORE_CreateRegion((MMADDR) MemoryOnChip, CHIP_REGION_SIZE + CONFIG_MMS_PAGE_SIZE, TRUE);
    //CORE_CreateRegion((MMADDR) MemoryOnChip, sizeof(MemoryOnChip), FALSE);
 //   CORE_CreateRegion((MMADDR) MemoryOffChip0, sizeof(MemoryOffChip0), FALSE);
  //  CORE_CreateRegion((MMADDR) MemoryOffChip1, sizeof(MemoryOffChip1), FALSE);
//  CORE_CreateRegion((MMADDR) MemoryOffChip2, sizeof(MemoryOffChip2), FALSE);
//  CORE_CreateRegion((MMADDR) MemoryOffChip3, sizeof(MemoryOffChip3), FALSE);
//  CORE_CreateRegion((MMADDR) MemoryOffChip4, sizeof(MemoryOffChip4), FALSE);
//  CORE_CreateRegion((MMADDR) MemoryOffChip5, sizeof(MemoryOffChip5), FALSE);
//  CORE_CreateRegion((MMADDR) MemoryOffChip6, sizeof(MemoryOffChip6), FALSE);

    
//  Address = PageAllocAddress(&g_SystemMemoryManager, 2);
//  Address = PageAllocAddress(&g_SystemMemoryManager, 2);
//  Address = PageAllocAddress(&g_SystemMemoryManager, 1);
//  Address = PageAllocAddress(&g_SystemMemoryManager, 1);

    for (i = 0; i < 2; i ++)
    {
        Address[i] = PageAllocAddress(&g_SystemMemoryManager, 4);
    }
    
    CORE_INFOR(TRUE, "Show 4 pages section.");

    RegionEnumerateUsedSection(&g_SystemMemoryManager, 0);

    for (i = 0; i < 2; i ++)
    {
        PageFreeAddress(&g_SystemMemoryManager, Address[i]);
    }

    for (i = 0; i < 5; i ++)
    {
        Address[i] = PageAllocAddress(&g_SystemMemoryManager, 2);
    }

    CORE_INFOR(TRUE, "Show 2 pages section.");

    RegionEnumerateUsedSection(&g_SystemMemoryManager, 0);

    for (i = 0; i < 5; i ++)
    {
        PageFreeAddress(&g_SystemMemoryManager, Address[i]);
    }

    for (i = 0; i < 10; i ++)
    {
        Address[i] = PageAllocAddress(&g_SystemMemoryManager, 1);
    }

    CORE_INFOR(TRUE, "Show 1 pages section.");

    RegionEnumerateUsedSection(&g_SystemMemoryManager, 0);

    for (i = 0; i < 10; i ++)
    {
        PageFreeAddress(&g_SystemMemoryManager, Address[i]);
    }
//  PageFreeAddress(&g_SystemMemoryManager, (LPVOID)(((MMADDR) Address) + CONFIG_MMS_PAGE_SIZE));
    
    RegionEnumerateUsedSection(&g_SystemMemoryManager, 0);

    Tick0 = GetTickCount();
    for (i = 0; i < 10000000; i ++)
    {
        if (i && 0 == (i%10))
        {
            int j = 0;
            for (j = 0; j < 10; j ++)
            {
                PageFreeAddress(&g_SystemMemoryManager, Address[j]);
            }
        }

        Address[i%10] = PageAllocAddress(&g_SystemMemoryManager, 1);

        if (NULL == Address[i%10])
        {
            CORE_ERROR(TRUE, "Alloc page count %d failed", i);
            break;
        }
    }
    Tick1 = GetTickCount();
    printf("Tick0 = %lu,  Tick1 = %lu, Time = %lu\n", Tick0, Tick1, Tick1 - Tick0);
    printf("sizeof(MM_PAGE_ENTRIES) = %d\n", sizeof(MM_PAGE_ENTRIES));
    {
        MM_PAGE_ENTRIES TestEntries = {0};

        TestEntries.SelfPage.HeadBoolean = TRUE;

        printf("#define     PAGE_HEAD_BIT_MASK          0x%08lx\n", TestEntries.Entries);
        printf("#define     PAGE_HEAD_BIT_SHIFT         %u\n", GetDwordLowestBit(TestEntries.Entries));

        TestEntries.Entries = 0;
        TestEntries.SelfPage.ManyBoolean = TRUE;
        printf("#define     PAGE_MANY_BIT_MASK          0x%08lx\n", TestEntries.Entries);
        printf("#define     PAGE_MANY_BIT_SHIFT         %u\n", GetDwordLowestBit(TestEntries.Entries));

        TestEntries.Entries = 0;
        TestEntries.SelfPage.UsedBoolean = TRUE;
        printf("#define     PAGE_USED_BIT_MASK          0x%08lx\n", TestEntries.Entries);
        printf("#define     PAGE_USED_BIT_SHIFT         %u\n", GetDwordLowestBit(TestEntries.Entries));

        TestEntries.Entries = 0;
        TestEntries.SelfPage.PrveSection = PAGE_ID_MASK;
        printf("#define     PAGE_PRVEPAGE_MASK          0x%08lx\n", TestEntries.Entries);
        printf("#define     PAGE_PRVEPAGE_SHIFT         %u\n", GetDwordLowestBit(TestEntries.Entries));

        TestEntries.Entries = 0;
        TestEntries.SelfPage.NextSection = PAGE_ID_MASK;
        printf("#define     PAGE_NEXTPAGE_MASK          0x%08lx\n", TestEntries.Entries);
        printf("#define     PAGE_NEXTPAGE_SHIFT         %u\n", GetDwordLowestBit(TestEntries.Entries));

        TestEntries.Entries = 0;
        TestEntries.HeadPage.SectionPages = PAGE_ID_MASK;
        printf("#define     PAGE_HEADPAGES_MASK         0x%08lx\n", TestEntries.Entries);
        printf("#define     PAGE_HEADPAGES_SHIFT        %u\n", GetDwordLowestBit(TestEntries.Entries)); 

        TestEntries.Entries = 0;
        TestEntries.TailPage.SectionPages = PAGE_ID_MASK;
        printf("#define     PAGE_TAILPAGES_MASK         0x%08lx\n", TestEntries.Entries);
        printf("#define     PAGE_TAILPAGES_SHIFT        %u\n", GetDwordLowestBit(TestEntries.Entries)); 

    }

    getchar();

    return 0;
}