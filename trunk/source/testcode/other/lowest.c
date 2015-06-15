#include <stdio.h>

#define EXPORT
#define STATIC
#define INLINE

#define SIZEOF_ARRAY(Array)                                                     \
        (sizeof(Array)/sizeof(Array[0]))

#define     CORE_INFOR(Enter,...)           kDebugLog(Enter, __LINE__, __FUNCTION__, LOG_LEVEL_INFOR, __VA_ARGS__)

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


STATIC  BYTE            g_PriorityReadyBitmap0  =       0;
STATIC  BYTE            g_PriorityReadyBitmap1[4];
STATIC  BYTE            g_PriorityReadyBitmap2[32];
STATIC  BYTE            g_LowestBitTable[256];

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


EXPORT VOID Show2C(LPVOID lpBuffer, SIZE_T Length)
{
    DWORD Scale = 0;
    LPBYTE lpData = lpBuffer;

    printf("/* The byte of lowest bit table is %d bytes */\r\n{\r\n", Length);
    for (Scale = 0; Scale < Length; Scale ++)
    {
        if (0x0 ==((Scale & 0xf)))
        {
            printf("    ");
        }
        printf("%d, ", lpData[Scale] & 0xff);
        
        if (0xf ==((Scale & 0xf)))
        {
            printf("/* Mask 0x%02X ~ 0x%02X */\r\n", Scale - 0xf, Scale);
        }
    }

    printf("};\r\n");
}


STATIC INLINE DWORD GetBitmapPriority(BYTE Bitmap)
{

    DWORD Priority = 0;
    
    if (Bitmap & (1 << 7)) Priority = 7;
    if (Bitmap & (1 << 6)) Priority = 6;
    if (Bitmap & (1 << 5)) Priority = 5;
    if (Bitmap & (1 << 4)) Priority = 4;
    if (Bitmap & (1 << 3)) Priority = 3;
    if (Bitmap & (1 << 2)) Priority = 2;
    if (Bitmap & (1 << 1)) Priority = 1;
    if (Bitmap & (1 << 0)) Priority = 0;

    return Priority;

}


STATIC INLINE VOID SetReadyBitmap(TASK_PRIORITY Priority)
{
    g_PriorityReadyBitmap0 |= (1 << (Priority / (BYTE_WIDTH * BYTE_WIDTH)));
    g_PriorityReadyBitmap1[Priority/(BYTE_WIDTH * BYTE_WIDTH)] |= (1 << ((Priority / BYTE_WIDTH) % BYTE_WIDTH));
    g_PriorityReadyBitmap2[Priority/BYTE_WIDTH] |= (1 << (Priority % BYTE_WIDTH));
}


STATIC INLINE VOID ClrReadyBitmap(TASK_PRIORITY Priority, BOOL Clear)
{
    printf("After Clean priority %d ready g_PriorityReadyBitmap0: 0x%x\r\n", Priority, g_PriorityReadyBitmap0);
    printf("After Clean priority %d ready g_PriorityReadyBitmap1: \r\n", Priority);
    kDebugShowData(LOG_LEVEL_INFOR, g_PriorityReadyBitmap1, sizeof(g_PriorityReadyBitmap1));
    printf("After Clean priority %d ready g_PriorityReadyBitmap2: \r\n", Priority);
    kDebugShowData(LOG_LEVEL_INFOR, g_PriorityReadyBitmap2, sizeof(g_PriorityReadyBitmap2));
    
    g_PriorityReadyBitmap2[Priority/BYTE_WIDTH] &= ~(Clear << (Priority % BYTE_WIDTH));
    Clear = (!g_PriorityReadyBitmap2[Priority/BYTE_WIDTH]);
    g_PriorityReadyBitmap1[Priority/(BYTE_WIDTH * 8)] &= ~(Clear << ((Priority / BYTE_WIDTH) % BYTE_WIDTH));
    Clear = (!g_PriorityReadyBitmap1[Priority/(BYTE_WIDTH * 8)]);
    g_PriorityReadyBitmap0 &= ~(Clear << (Priority / (BYTE_WIDTH * BYTE_WIDTH)));
    
    printf("Befor Clean priority %d ready g_PriorityReadyBitmap0: 0x%x\r\n", Priority, g_PriorityReadyBitmap0);
    printf("Befor Clean priority %d ready g_PriorityReadyBitmap1: \r\n", Priority);
    kDebugShowData(LOG_LEVEL_INFOR, g_PriorityReadyBitmap1, sizeof(g_PriorityReadyBitmap1));
    printf("Befor Clean priority %d ready g_PriorityReadyBitmap2: \r\n", Priority);
    kDebugShowData(LOG_LEVEL_INFOR, g_PriorityReadyBitmap2, sizeof(g_PriorityReadyBitmap2));
}

STATIC INLINE TASK_PRIORITY SearchHighestPriority(VOID)
{
    DWORD Offs0 = g_LowestBitTable[g_PriorityReadyBitmap0];
    DWORD Offs1 = g_LowestBitTable[g_PriorityReadyBitmap1[Offs0]];
    DWORD Offs2 = g_LowestBitTable[g_PriorityReadyBitmap2[(Offs0 * 8) + Offs1]];
    
    return (Offs0 * 64) + (Offs1 * 8) + Offs2;
}

STATIC BOOL TestPiroirty(TASK_PRIORITY Piroirty)
{
    TASK_PRIORITY Piro = 0;
    SetReadyBitmap(Piroirty);

    if (Piroirty != (Piro = SearchHighestPriority()))
    {
        printf("Test priority %d , result %d failed !\r\n", Piroirty, Piro);
        Show2C(g_LowestBitTable, sizeof(g_LowestBitTable));
        return FALSE;
    }
    
    ClrReadyBitmap(Piroirty, TRUE);
    
    return TRUE;
}


int main(int argc, char * argv[])
{
    int i = 0;
    
    for (i = 0; i < SIZEOF_ARRAY(g_LowestBitTable); i ++)
    {
        g_LowestBitTable[i] = GetBitmapPriority(i);
    }

    for (i = 0; i < 256; i ++)
    {
        printf("******************************\r\n");
        if (TRUE != TestPiroirty(i))
            break;
    }
    
    Show2C(g_LowestBitTable, sizeof(g_LowestBitTable));
    
    return 0;
}