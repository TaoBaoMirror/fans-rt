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
 * �ļ����ʷ�ʽ
 */
#define     FFILE_ACCESS_READ       0x1                         /**< ֻ�� */
#define     FFILE_ACCESS_WRITE      0x2                         /**< ֻд */
#define     FFILE_ACCESS_SHARED     0x10000                     /**< ���� */

/**
 * �ļ��򿪷�ʽ����
 */
typedef enum{
    FFILE_CREATE_FORCE              =       0x0,                /**< �½��ļ�������ļ���������� */
    FFILE_CREATE_NEW                =       0x1,                /**< �½��ļ�������ļ�������ʧ�� */
    FFILE_OPEN_FORCE                =       0x2,                /**< ���ļ�������ļ��������򴴽� */
    FFILE_OPEN_EXIST                =       0x3,                /**< ���ļ�������ļ���������ʧ�� */
}E_FILEOPEN;

#define     ATTR_IS_FOLDER(Attr)        (0 != ((Attr) & FFILE_ATTRIBUTE_FOLDER))
#define     ATTR_IS_VOLUME(Attr)        (0 != ((Attr) & FFILE_ATTRIBUTE_VOLUME))
#define     ATTR_IS_DEVICE(Attr)        (0 != ((Attr) & FFILE_ATTRIBUTE_DEVICE))

typedef enum{
    FFILE_ATTRIBUTE_NORMAL          =       0x0,                /**< Ĭ������ */
    FFILE_ATTRIBUTE_ARCHIVE         =       0x1,                /**< �鵵���� */
    FFILE_ATTRIBUTE_HIDDEN          =       0x2,                /**< �������� */
    FFILE_ATTRIBUTE_READONLY        =       0x4,                /**< ֻ������ */
    FFILE_ATTRIBUTE_SYSTEM          =       0x8,                /**< ϵͳ���� */
    FFILE_ATTRIBUTE_FOLDER          =       0x10,               /**< �ļ������� */
    FFILE_ATTRIBUTE_VOLUME          =       0x20,               /**< ������ */
    FFILE_ATTRIBUTE_DEVICE          =       0x40,               /**< �豸���� */
    FFILE_ATTRIBUTE_MASK            =       0x0f,               /**< ���� */
}E_FILEATTRIBUTE;

typedef enum{
    FFILE_SEEK_SET                  =       0x0,
    FFILE_SEEK_CUR                  =       0x1,
    FFILE_SEEK_END                  =       0x2,
}E_SEEKMODE;

#define TASK_PRIORITY_REAL                  0                                   /**< ʵʱ���� */
#define TASK_PRIORITY_IDLE                  (CONFIG_TASK_PRIORITY_MAX - 1)      /**< �������� */
#define TASK_PRIORITY_NORMAL                (TASK_PRIORITY_IDLE - 1)            /**< ��ͨ���� */
#define TASK_PRIORITY_INVALID               (255)                               /**< ��Ч�������ȼ� */
#define TASK_SLICE_INFINITE                 ((TIME_SLICE_T)(-1))                /**< ����ʱ��Ƭ���� - ֻ�� IDLE ʹ�� */
#define TASK_LSOTKEY_INVALID                (~0U)

typedef unsigned int LSOT_KEY_T;
typedef short TIME_SLICE_T;
typedef unsigned char TASK_PRIORITY;
typedef unsigned char * PTASK_PRIORITY;
typedef unsigned char FAR * LPTASK_PRIORITY;

typedef enum emTASK_STATUS{
    TASK_STATE_CREATE       =       0,                          /**< ���񴴽�״̬ */
    TASK_STATE_SLEEP        =       1,                          /**< ��������״̬ */
    TASK_STATE_READY        =       2,                          /**< ����׼��״̬ */
    TASK_STATE_WAITING      =       3,                          /**< ����ȴ�״̬ */
    TASK_STATE_WORKING      =       4,                          /**< ������״̬ */
    TASK_STATE_DETACH       =       5,                          /**< �������״̬ */
    TASK_STATE_DEATH        =       6,                          /**< ��������״̬ */
    TASK_STATE_MAX,
}TASK_STATUS;

typedef enum emTASK_PERMISSION{
    TASK_PERMISSION_CORE    =       0,
    TASK_PERMISSION_USER    =       1,
    TASK_PERMISSION_MAX     =       2,
}E_TASK_PERMISSION;

typedef FANSAPI E_STATUS (FAR *FNTASKMAIN)(LPVOID lpArgment);

typedef struct tagTASK_CREATE_PARAM{
    FNTASKMAIN              fnTaskMain;                         /**< ������������� */
    LPCTSTR                 lpTaskPath;                         /**< ��������·�� */
    LPVOID                  lpArgument;                         /**< ������ڲ��� */
    SIZE_T                  StackSize;                          /**< ��ջ��С */
#if (defined(CONFIG_BUILD_LOCAL_STORE) && (TRUE == CONFIG_BUILD_LOCAL_STORE))
    DWORD                   LsotKeys;                           /**< �ֲ���������� key ���� */
#endif
    BOOL                    AutoStartup;                        /**< �����Զ����� */
    TASK_PRIORITY           Priority;                           /**< �������ȼ� */
    TIME_SLICE_T            SliceLength;                        /**< ����ʱ��Ƭ���� */
}TASK_CREATE_PARAM, * PTASK_CREATE_PARAM, FAR * LPTASK_CREATE_PARAM;

typedef struct tagTASK_INFOR{
    CHAR                    TaskName[OBJECT_NAME_MAX];          /**< ������ */
    SIZE_T                  TaskStackCapacity;                  /**< ��ջ���� */
    SIZE_T                  TaskStackRemain;                    /**< ��ջʣ�೤�� */
    BYTE                    ReservedByte;
    BYTE                    TaskCPUPercent;                     /**< CPU ռ���� */
    TASK_STATUS             TaskWorkStatus;                     /**< ������״̬ */
    TASK_PRIORITY           TaskPriority;                       /**< �������ȼ� */
    WORD                    ReservedWord;
    TIME_SLICE_T            TaskTimeSlice;                      /**< ����ʱ��Ƭ���� */
}TASK_INFOR, * PTASK_INFOR, FAR * LPTASK_INFOR; 

#if (CONFIG_MEM_REGION_MAX != 0)
typedef struct tagMMS_INFOR{
    SIZE_T                  SelfSize;                           /**< �ṹ������ */
    SIZE_T                  PageSize;                           /**< ҳ�泤�� */
    DWORD                   FreePages;                          /**< ����ҳ����*/
    DWORD                   TotalPages;                         /**< ��ҳ�� */ 
    WORD                    BuddyMask;                          /**< ����λͼ */
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
