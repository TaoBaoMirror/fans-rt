/*
 *    Copyright(C) 2013-2015, Fans-rt development team.
 *
 *    All rights reserved.
 *
 *    This is open source software.
 *    Learning and research can be unrestricted to modification, use and dissemination.
 *    If you need for commercial purposes, you should get the author's permission.
 *
 *    date           author          notes
 *    2014-09-07     JiangYong       new file
 */
#ifndef __LIB_CAL_H
#define __LIB_CAL_H

#include <fadefs.h>
#include <fatypes.h>

#ifdef __cplusplus
extern "C" {
#endif
    EXPORT TICK ntGetSystemTick(VOID);
#if (CONFIG_MEM_REGION_MAX != 0)
    EXPORT LPVOID ntPageMalloc(SIZE_T Length);
    EXPORT E_STATUS ntPageFree(LPVOID lpPageAddress);
    EXPORT DWORD ntGetSystemFreePages(BYTE RegionID);
    EXPORT E_STATUS ntGetMmsInformation(LPMMS_INFOR lpInfor);
    EXPORT E_STATUS ntShowMmsInformation(VOID);
    EXPORT E_STATUS ntShowMmsRegionSection(BYTE RegionID, BYTE Buddy);
    EXPORT E_STATUS ntCreateMMRegion(LPVOID lpAddress, SIZE_T Length);
#endif
    EXPORT E_STATUS ntGetError(VOID);
    EXPORT E_STATUS ntSetError(E_STATUS emCode);
    EXPORT TICK ntGetTaskStartTick(HANDLE hTask);

    EXPORT E_STATUS ntScheduleTimeout(LONG Timeout);
    EXPORT E_STATUS ntTaskWakeup(HANDLE hTask);
    EXPORT E_STATUS ntTestCancel(VOID);
    EXPORT E_STATUS ntPostCancel(HANDLE hTask);
    EXPORT TASK_PRIORITY ntGetPriority(HANDLE hTask);
    EXPORT E_STATUS ntSetPriority(HANDLE hTask, TASK_PRIORITY Priority);
    EXPORT E_STATUS ntCloseTask(HANDLE hTask);
    EXPORT E_STATUS caIdleTask_Main(LPVOID lpParam);
    
    EXPORT E_STATUS ntStackMalloc(HANDLE hTask, LPVOID lpParam);
    EXPORT E_STATUS ntStackFree(HANDLE hTask);

    EXPORT E_STATUS ntGetTaskName(HANDLE hTask, CHAR Name[OBJECT_NAME_MAX]);
    EXPORT HANDLE ntGetCurrentTask(VOID);
    
    EXPORT VOID ntTaskEntry(FNTASKMAIN fnMain, LPVOID lpArgument, HANDLE hTask);
#if 0
    EXPORT E_STATUS ntSchedulerRun(LPVOID StackDump);
    EXPORT E_STATUS ntScheduleTimeout(DWORD Timeout);
    
    
    EXPORT E_STATUS ntTaskCancel(HANDLE hTask);
    
    EXPORT TASK_PRIORITY ntGetPriority(HANDLE hTask);
    EXPORT E_STATUS ntSetPriority(HANDLE hTask, TASK_PRIORITY Priority);
    
    EXPORT LPVOID ntGetLocalData(BYTE LocalID);
    EXPORT E_STATUS ntSetLocalData(BYTE LocalID, LPVOID lpPrivate);
    
    EXPORT TASK_STATUS ntGetTaskState(HANDLE hTask);
    EXPORT E_STATUS ntGetTaskInfor(HANDLE hTask, LPTASK_INFOR lpTaskInfor);
    EXPORT HANDLE ntEnumNextTask(HANDLE hTask);
    
#if (CONFIG_PROFILER_CYCLE != 0)
    EXPORT E_STATUS ntSystemProfiler(VOID);
#endif

#endif                    
    EXPORT HANDLE ntMallocObject(LPCSTR lpName, DWORD Magic, LPVOID lpParam);
    EXPORT E_STATUS ntActiveObject(HANDLE handle, LPVOID lpParam);
    EXPORT HANDLE ntTakeObject(LPCSTR lpName, LPVOID lpParam);
    EXPORT E_STATUS ntCloseObject(HANDLE handle);
    EXPORT E_STATUS ntFreeObject(HANDLE handle);
    EXPORT E_STATUS ntPostObject(HANDLE handle, LPVOID lpParam);
    EXPORT E_STATUS ntWaitObject(HANDLE handle, LONG WaitTime);
    EXPORT E_STATUS ntResetObject(HANDLE handle, LPVOID lpParam);
    
    EXPORT LPSTR ntChooseName(__IN __OUT LPSTR lpName, LPSTR lpType);

#ifdef __cplusplus
}
#endif
#endif
