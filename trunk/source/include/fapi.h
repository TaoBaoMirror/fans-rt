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
    FANSAPI E_STATUS SetError(E_STATUS emCode);
    
    FANSAPI HANDLE GetCurrentTask(VOID);
    
    FANSAPI E_STATUS TestCancel(VOID);
    FANSAPI E_STATUS PostCancel(HANDLE hTask);
    
    FANSAPI E_STATUS TaskExit(VOID);
    FANSAPI E_STATUS KillTask(HANDLE hTask);
    
    FANSAPI E_STATUS Sleep(LONG Timeout);
    FANSAPI E_STATUS TaskStartup(HANDLE hTask);
    
    FANSAPI TASK_PRIORITY GetPriority(HANDLE hTask);
    FANSAPI E_STATUS SetPriority(HANDLE hTask, TASK_PRIORITY Priority);
        

    FANSAPI LPVOID GetTaskLocalData(VOID);
    FANSAPI E_STATUS SetTaskLocalData(LPVOID lpData);

    FANSAPI TASK_STATUS GetTaskState(HANDLE hTask);

#define     TaskExit()                          KillTask(INVALID_HANDLE_VALUE)
#define     GetTaskSelfState()                  GetTaskState(INVALID_HANDLE_VALUE)
#define     GetTaskSelfPriority()               GetPriority(INVALID_HANDLE_VALUE)
#define     SetTaskSelfPriority(p)              SetPriority(INVALID_HANDLE_VALUE, p)
#define     GetTaskSelfName(n, sz)              GetTaskName(INVALID_HANDLE_VALUE, n, sz)
#define     GetTaskSelfInformation(lpInfor)     GetTaskInformation(INVALID_HANDLE_VALUE, lpInfor)


    FANSAPI E_STATUS GetTaskInformation(HANDLE hTask, LPTASK_INFOR lpTaskInfor);
    FANSAPI HANDLE EnumerationNextTask(HANDLE hTask);
    
    FANSAPI E_STATUS GetTaskName(HANDLE hTask, LPTSTR lpName, SIZE_T SizeofBuffer);
    FANSAPI HANDLE CreateTaskEx(LPCTSTR lpTaskName, LPTASK_CREATE_PARAM lpParam);
    FANSAPI HANDLE CreateTask(LPCTSTR __IN lpTaskName, FNTASKMAIN fnMain, LPVOID lpArgument);
    FANSAPI HANDLE CreatePriorityTask(LPCSTR __IN lpTaskName, FNTASKMAIN fnMain, 
                                      LPVOID lpArgument, TASK_PRIORITY Priority);

    FANSAPI HANDLE TakeObject(LPCTSTR lpName);

    FANSAPI HANDLE CreateEvent(LPCTSTR lpCTName, BOOL Automatic, BOOL Signal);
    FANSAPI E_STATUS PostEvent(HANDLE handle);
    FANSAPI E_STATUS ResetEvent(HANDLE handle);

    FANSAPI HANDLE CreateMutex(LPCTSTR lpCTName, BOOL Owner);
    FANSAPI E_STATUS MutexLock(HANDLE hMutex);
    FANSAPI E_STATUS MutexUnlock(HANDLE hMutex);

    FANSAPI E_STATUS WaitObject(HANDLE handle, LONG WaitTime);
    FANSAPI E_STATUS CloseHandle(HANDLE handle);
#if 0
    FANSAPI E_STATUS PostEvent(LPVOID lpParam, E_EVENT_LISTS emEvent, E_EVENT_PRIORITY emPriority);
    FANSAPI E_STATUS SubscribeEvent(LPEVENT_HANDLER lpHandler, E_EVENT_LISTS emEvent);
    
    FANSAPI E_STATUS CreateFile(__IN CONST LPTSTR FileName, DWORD Attribute);
    FANSAPI E_STATUS FileIOCtrl(HANDLE Handle, DWORD dwCmd, ...);
    FANSAPI HANDLE OpenFile(__IN CONST LPTSTR FileName, DWORD AccessFlags, E_FILEOPEN OpenMode);
    FANSAPI SIZE_T ReadFile(HANDLE Handle, __IN __OUT LPVOID Buffer, SIZE_T Length);
    FANSAPI SIZE_T WriteFile(HANDLE Handle, __IN CONST LPVOID Buffer, SIZE_T Length);
    FANSAPI E_STATUS CloseHandle(HANDLE Handle);
#endif
#ifdef __cplusplus
}
#endif 

#endif
