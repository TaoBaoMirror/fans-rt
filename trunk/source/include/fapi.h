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
#ifndef __FA_API_H
#define __FA_API_H
#include <fadefs.h>
#include <fatypes.h>
#include <faerror.h>


#ifdef __cplusplus
extern "C" {
#endif
    FANSAPI TICK GetSystemTick(VOID);
#if (CONFIG_MEM_REGION_MAX != 0)    
    FANSAPI LPVOID PageMalloc(SIZE_T Length);
    FANSAPI E_STATUS PageFree(LPVOID lpPageAddress);
    FANSAPI DWORD GetSystemFreePages(BYTE RegionID);
    FANSAPI E_STATUS GetMmsInformation(__IN __OUT LPMMS_INFOR lpInfor);
    FANSAPI E_STATUS ShowMmsRegionSection(BYTE RegionID, BYTE Buddy);
    FANSAPI E_STATUS ShowMmsInformation(VOID);
    FANSAPI E_STATUS CreateMMRegion(LPVOID lpAddress, SIZE_T Length);
#endif

    FANSAPI E_STATUS GetError(VOID);
    FANSAPI E_STATUS SetError(E_STATUS State);
    
    FANSAPI HANDLE GetCurrentTask(VOID);
    
    FANSAPI BOOL TestCancel(VOID);
    FANSAPI E_STATUS PostCancel(HANDLE hTask);

    FANSAPI E_STATUS TaskExit(VOID);
    FANSAPI E_STATUS KillTask(HANDLE hTask);
    
    FANSAPI E_STATUS Sleep(LONG Timeout);
    FANSAPI E_STATUS TaskStartup(HANDLE hTask);
    
    FANSAPI TASK_PRIORITY GetPriority(HANDLE hTask);
    FANSAPI E_STATUS SetPriority(HANDLE hTask, TASK_PRIORITY Priority);

    FANSAPI TASK_STATUS GetTaskState(HANDLE hTask);

#define     GetTaskSelfState()                  GetTaskState(TASK_SELF_HANDLE)
#define     GetTaskSelfPriority()               GetPriority(TASK_SELF_HANDLE)
#define     SetTaskSelfPriority(p)              SetPriority(TASK_SELF_HANDLE, p)
#define     GetTaskSelfName(n, sz)              GetObjectName(TASK_SELF_HANDLE, n, sz)
#define     GetTaskSelfInformation(lpInfor)     GetTaskInformation(TASK_SELF_HANDLE, lpInfor)


    FANSAPI E_STATUS GetTaskInformation(HANDLE hTask, LPTASK_INFOR lpTaskInfor);
    FANSAPI HANDLE EnumerationNextTask(HANDLE hTask);

    FANSAPI HANDLE CreateTaskEx(LPCTSTR lpCTName, LPTASK_CREATE_PARAM lpParam);
#define     CreateTask(lpTaskName, fnMain, lpArgument)                                          \
            CreatePriorityTask(lpTaskName, fnMain, lpArgument, TASK_PRIORITY_NORMAL)

    FANSAPI HANDLE CreatePriorityTask(LPCTSTR __IN lpCTName, FNTASKMAIN fnMain, 
                                      LPVOID lpArgument, TASK_PRIORITY Priority);

    FANSAPI HANDLE TakeObject(LPCTSTR lpCTName);

#if (defined(CONFIG_BUILD_LOCAL_STORE) && (TRUE == CONFIG_BUILD_LOCAL_STORE))
    FANSAPI LSOT_KEY_T GetLsotKey(VOID);
    FANSAPI E_STATUS PutLsotKey(LSOT_KEY_T LsotKey);
    FANSAPI E_STATUS GetLsotValue(LSOT_KEY_T LsotKey, DWORD_PTR lpValue);
    FANSAPI E_STATUS SetLsotValue(LSOT_KEY_T LsotKey, DWORD Value);
#endif

    FANSAPI HANDLE CreateEvent(LPCTSTR lpCTName, BOOL Automatic, BOOL Signal);
    FANSAPI E_STATUS PostEvent(HANDLE hEvent);
    FANSAPI E_STATUS ResetEvent(HANDLE hEvent);

    FANSAPI HANDLE CreateMutex(LPCTSTR lpCTName, BOOL Owner);
    FANSAPI E_STATUS MutexLock(HANDLE hMutex);
    FANSAPI E_STATUS MutexUnlock(HANDLE hMutex);
    
    FANSAPI HANDLE CreateSemaphore(LPCTSTR lpCTName, SHORT Lights, SHORT MaxLights);
    FANSAPI E_STATUS PostSemaphore(HANDLE hSemaphore, SHORT Lights);

    FANSAPI HANDLE CreateSemset(LPCTSTR lpCTName, DWORD Mask, BOOL WaitFull);
    FANSAPI E_STATUS PostSemset(HANDLE hSemset, BYTE SemID);


    FANSAPI E_STATUS GetObjectName(HANDLE hObject, LPTSTR lpName, SIZE_T SizeofBuffer);
    FANSAPI SHORT WaitObject(HANDLE handle, LONG WaitTime);
    FANSAPI E_STATUS CloseHandle(HANDLE handle);
#if 0
    FANSAPI E_STATUS CreateFile(__IN CONST LPTSTR FileName, DWORD Attribute);
    FANSAPI E_STATUS FileIOCtrl(HANDLE Handle, DWORD dwCmd, ...);
    FANSAPI HANDLE OpenFile(__IN CONST LPTSTR FileName, DWORD AccessFlags, E_FILEOPEN OpenMode);
    FANSAPI SIZE_T ReadFile(HANDLE Handle, __IN __OUT LPVOID Buffer, SIZE_T Length);
    FANSAPI SIZE_T WriteFile(HANDLE Handle, __IN CONST LPVOID Buffer, SIZE_T Length);
#endif
#ifdef __cplusplus
}
#endif 

#endif
