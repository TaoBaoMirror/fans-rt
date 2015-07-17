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
 
#ifndef __FA_TYPES_H
#define __FA_TYPES_H
#include "config.h"
#include <stddef.h>
#include <stdarg.h>
#include <fadefs.h>
#include <faerror.h>

typedef unsigned short WORD16;
typedef unsigned int WORD32;
typedef unsigned long long WORD64;
typedef unsigned int MMADDR;

typedef signed short S16;
typedef signed int S32;
typedef signed long long S64;

typedef short U16;
typedef unsigned long U32;
typedef unsigned long long U64;

#define     BYTE_WIDTH              (8) 
typedef unsigned char BYTE;
typedef unsigned char * PBYTE;
typedef unsigned char FAR * LPBYTE;

#define     WORD_WIDTH              (sizeof(WORD) * BYTE_WIDTH)
typedef unsigned short WORD;
typedef unsigned short * PWORD;
typedef unsigned short FAR * LPWORD;

#define     INVALID_VALUE           ((DWORD)(~(0UL)))
#define     DWORD_WIDTH             (sizeof(DWORD) * BYTE_WIDTH)
#define     GET_DWORD_BIT(x, b)     ((x) & (1<<(b)))
typedef unsigned long DWORD;
typedef unsigned long * DWORD_PTR;

#include <limits.h>
#ifdef ULLONG_MAX
#if (ULLONG_MAX>=0x7fffffffffffffffLL)
#define QWORD_WIDTH     (sizeof(QWORD) * BYTE_WIDTH)
#define  SYSTEM_HAVE_QWORD
typedef unsigned long long QWORD;
typedef unsigned long long * PQWORD;
typedef unsigned long long FAR * LPQWORD;
#endif
#endif

typedef char CHAR;
typedef char * PSTR;
typedef char * LPSTR;
typedef const char * LPCSTR;

#define WCHAR   wchar_t
typedef WCHAR * PWSTR;
typedef WCHAR FAR * LPWSTR;
typedef const WCHAR FAR * LPCWSTR;

#define _WTEXT(text)    (void *)(L ##text)

#ifdef _UNICODE
#define TCHAR WCHAR
#define PTSTR PWSTR
#define LPTSTR LPWSTR
#define LPCTSTR LPCWSTR
#define _TEXT(text)  _WTEXT(text)
#else
#define TCHAR CHAR
#define PTSTR PSTR
#define LPTSTR LPSTR
#define LPCTSTR LPCSTR
#define _TEXT(text) text
#endif

typedef short SHORT;
typedef short * PSHORT;
typedef short FAR * LPSHORT;

typedef int INT;
typedef int * PINT;
typedef int FAR * LPINT;

typedef unsigned int UINT;
typedef unsigned int * PUINT;
typedef unsigned int FAR * LPUINT;


#ifdef LLONG_MAX
#define INVALID_TICK        (-1LL)
#define TICK_INFINITE       (-1LL)
#define WAIT_INFINITE       (-1L)
#define SYSTEM_HAVE_TICK64
typedef long long TICK;
typedef long long * PTICK;
typedef long long FAR * LPTICK;
#else
#define SYSTEM_HAVE_TICK32
#define INVALID_TICK        (-1L)
#define TICK_INFINITE       (-1L)
#define WAIT_INFINITE       (-1L)
typedef long TICK;
typedef long * PTICK;
typedef long FAR * LPTICK;
#endif

typedef long LONG;
typedef long * PLONG;
typedef long FAR * LPLONG;

typedef unsigned long ULONG;
typedef unsigned long * PULONG;
typedef unsigned long FAR * LPULONG;


#define MAX_SIZE_T              2147483647L
#define INVALID_SIZE            ((SIZE_T)~(0ULL))
#define KB                      (1024)
#define MB                      (KB * 1024)
#define KBYTE(x)                (KB *(x))
#define MBYTE(x)                (1024 * KBYTE(x))

typedef unsigned long SIZE_T;
typedef unsigned long * PSIZE_T;
typedef unsigned long FAR * LPSIZE_T;

#ifdef ULLONG_MAX
#define SYSTEM_HAVE_SIZE64
typedef unsigned long long SIZE_L;
typedef unsigned long long * PSIZE_L;
typedef unsigned long long FAR * LPSIZE_L;
#endif

#define     INVALID_HANDLE_VALUE                ((HANDLE)(-1))
#define     TASK_SELF_HANDLE                    ((HANDLE)(-2))

typedef unsigned long HANDLE;
typedef unsigned long * PHANDLE;
typedef unsigned long FAR * LPHANDLE;

typedef float FLOAT;
typedef float * PFLOAT;
typedef float FAR * LPFLOAT;

#define VOID void
typedef void * PVOID;
typedef void FAR * LPVOID;

typedef void * VA_LIST;
#define VA_LIST va_list
#define VA_START(ap, p)     va_start(ap, p)
#define VA_ARG(ap, type)    va_arg(ap, type)
#define VA_END(ap)          va_end(ap)

#define FALSE   0
#define TRUE    1

typedef unsigned char BOOL;
typedef unsigned char * PBOOL;
typedef unsigned char FAR * LPBOOL;

typedef long long OFF_T;
typedef long long * POFF_T;
typedef long long FAR * LPOFF_T;


/**
 * 文件访问方式
 */
#define     FFILE_ACCESS_READ       0x1                         /**< 只读 */
#define     FFILE_ACCESS_WRITE      0x2                         /**< 只写 */
#define     FFILE_ACCESS_SHARED     0x10000                     /**< 共享 */

/**
 * 文件打开方式定义
 */
typedef enum{
    FFILE_CREATE_FORCE              =       0x0,                /**< 新建文件，如果文件存在则清空 */
    FFILE_CREATE_NEW                =       0x1,                /**< 新建文件，如果文件存在则失败 */
    FFILE_OPEN_FORCE                =       0x2,                /**< 打开文件，如果文件不存在则创建 */
    FFILE_OPEN_EXIST                =       0x3,                /**< 打开文件，如果文件不存在则失败 */
}E_FILEOPEN;

#define     ATTR_IS_FOLDER(Attr)        (0 != ((Attr) & FFILE_ATTRIBUTE_FOLDER))
#define     ATTR_IS_VOLUME(Attr)        (0 != ((Attr) & FFILE_ATTRIBUTE_VOLUME))
#define     ATTR_IS_DEVICE(Attr)        (0 != ((Attr) & FFILE_ATTRIBUTE_DEVICE))

typedef enum{
    FFILE_ATTRIBUTE_NORMAL          =       0x0,                /**< 默认属性 */
    FFILE_ATTRIBUTE_ARCHIVE         =       0x1,                /**< 归档属性 */
    FFILE_ATTRIBUTE_HIDDEN          =       0x2,                /**< 隐藏属性 */
    FFILE_ATTRIBUTE_READONLY        =       0x4,                /**< 只读属性 */
    FFILE_ATTRIBUTE_SYSTEM          =       0x8,                /**< 系统属性 */
    FFILE_ATTRIBUTE_FOLDER          =       0x10,               /**< 文件夹属性 */
    FFILE_ATTRIBUTE_VOLUME          =       0x20,               /**< 卷属性 */
    FFILE_ATTRIBUTE_DEVICE          =       0x40,               /**< 设备属性 */
    FFILE_ATTRIBUTE_MASK            =       0x0f,               /**< 掩码 */
}E_FILEATTRIBUTE;

typedef enum{
    FFILE_SEEK_SET                  =       0x0,
    FFILE_SEEK_CUR                  =       0x1,
    FFILE_SEEK_END                  =       0x2,
}E_SEEKMODE;

#define TASK_PRIORITY_REAL                  0                                   /**< 实时任务 */
#define TASK_PRIORITY_IDLE                  (CONFIG_TASK_PRIORITY_MAX - 1)      /**< 空闲任务 */
#define TASK_PRIORITY_NORMAL                (TASK_PRIORITY_IDLE - 1)            /**< 普通任务 */
#define TASK_PRIORITY_INVALID               (255)                               /**< 无效任务优先级 */
#define TASK_SLICE_INFINITE                 ((TIME_SLICE_T)(-1))                /**< 任务时间片无限 - 只能 IDLE 使用 */
#define TASK_LSOTKEY_INVALID                (~0U)

typedef unsigned int LSOT_KEY_T;
typedef short TIME_SLICE_T;
typedef unsigned char TASK_PRIORITY;
typedef unsigned char * PTASK_PRIORITY;
typedef unsigned char FAR * LPTASK_PRIORITY;

typedef enum emTASK_STATUS{
    TASK_STATE_CREATE       =       0,                          /**< 任务创建状态 */
    TASK_STATE_SLEEP        =       1,                          /**< 任务休眠状态 */
    TASK_STATE_READY        =       2,                          /**< 任务准备状态 */
    TASK_STATE_WAITING      =       3,                          /**< 任务等待状态 */
    TASK_STATE_WORKING      =       4,                          /**< 任务工作状态 */
    TASK_STATE_DETACH       =       5,                          /**< 任务分离状态 */
    TASK_STATE_DEATH        =       6,                          /**< 任务死亡状态 */
    TASK_STATE_MAX,
}TASK_STATUS;

typedef enum emTASK_PERMISSION{
    TASK_PERMISSION_CORE    =       0,
    TASK_PERMISSION_USER    =       1,
    TASK_PERMISSION_MAX     =       2,
}E_TASK_PERMISSION;

typedef FANSAPI E_STATUS (FAR *FNTASKMAIN)(LPVOID lpArgment);

typedef struct tagTASK_CREATE_PARAM{
    FNTASKMAIN              fnTaskMain;                         /**< 任务入口主函数 */
    LPCTSTR                 lpTaskPath;                         /**< 任务运行路径 */
    LPVOID                  lpArgument;                         /**< 任务入口参数 */
    SIZE_T                  StackSize;                          /**< 堆栈大小 */
#if (defined(CONFIG_BUILD_LOCAL_STORE) && (TRUE == CONFIG_BUILD_LOCAL_STORE))
    DWORD                   LsotKeys;                           /**< 局部变量对象的 key 数量 */
#endif
    BOOL                    AutoStartup;                        /**< 创建自动启动 */
    TASK_PRIORITY           Priority;                           /**< 任务优先级 */
    TIME_SLICE_T            SliceLength;                        /**< 任务时间片长度 */
}TASK_CREATE_PARAM, * PTASK_CREATE_PARAM, FAR * LPTASK_CREATE_PARAM;

typedef struct tagTASK_INFOR{
    CHAR                    TaskName[OBJECT_NAME_MAX];          /**< 任务名 */
    SIZE_T                  TaskStackCapacity;                  /**< 堆栈容量 */
    SIZE_T                  TaskStackRemain;                    /**< 堆栈剩余长度 */
    BYTE                    ReservedByte;
    BYTE                    TaskCPUPercent;                     /**< CPU 占用率 */
    TASK_STATUS             TaskWorkStatus;                     /**< 任务工作状态 */
    TASK_PRIORITY           TaskPriority;                       /**< 任务优先级 */
    WORD                    ReservedWord;
    TIME_SLICE_T            TaskTimeSlice;                      /**< 任务时间片长度 */
}TASK_INFOR, * PTASK_INFOR, FAR * LPTASK_INFOR; 

#if (CONFIG_MEM_REGION_MAX != 0)
typedef struct tagMMS_INFOR{
    SIZE_T                  SelfSize;                           /**< 结构自身长度 */
    SIZE_T                  PageSize;                           /**< 页面长度 */
    DWORD                   FreePages;                          /**< 空闲页面数*/
    DWORD                   TotalPages;                         /**< 总页数 */ 
    WORD                    BuddyMask;                          /**< 伙伴表位图 */
    DWORD                   BuddyCount[CONFIG_MEM_BUDDY_MAX];
}MMS_INFOR, * PMMS_INFOR, FAR * LPMMS_INFOR;
#endif



#if 0
typedef volatile unsigned int REG32;
typedef volatile unsigned short REG16;
typedef volatile unsigned char REG08;

typedef volatile unsigned int * PREG32;
typedef volatile unsigned short * PREG16;
typedef volatile unsigned char * PREG08;

typedef volatile unsigned int * IOU32;
typedef volatile unsigned short * IOU16;
typedef volatile unsigned char * IOU08;
#endif

#endif
