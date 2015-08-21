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

typedef WORD PAGE_ID_T;

STATIC INLINE PAGE_ID_T GetMergePageID(PAGE_ID_T FirstPage, WORD Pages)
{
    return (FirstPage & (~Pages)) == FirstPage ? FirstPage + Pages : FirstPage - 1;
}

WORD TestData[][3] = 
{
    { 0, 1, 1},
    { 1, 1, 0},
    { 0, 2, 2},
    { 2, 2, 0},
    { 7, 1, 6},
    { 6, 1, 7},
    { 10, 2, 8},
    { 8, 2, 10},
    { 512, 512, 0},
    { 768, 256, 512},
};


int main(int argc, char * argv[])
{
    int i = 0;
    
    printf(" StartPage | Pages | Merge | Result\n");

    for (i=0; i<sizeof(TestData)/sizeof(TestData[0]); i ++)
    {
        printf("0x%04x   0x%04x   0x%04x   0x%04x\n", TestData[i][0], TestData[i][1], TestData[i][2],
            GetMergePageID(TestData[i][0], TestData[i][1]));
    }
    
    return 0;
}