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

#include <fauser.h>
#include <fadefs.h>
#include <fatypes.h>

#ifdef __cplusplus
extern "C" {
#endif
    EXPORT TICK caGetSystemTick(VOID);
#if (CONFIG_MEM_REGION_MAX != 0)
    EXPORT LPVOID caPageMalloc(SIZE_T Length);
    EXPORT E_STATUS caPageFree(LPVOID lpPageAddress);
    EXPORT DWORD caGetSystemFreePages(BYTE RegionID);
    EXPORT E_STATUS caGetMmsInformation(LPMMS_INFOR lpInfor);
    EXPORT E_STATUS caShowMmsInformation(VOID);
    EXPORT E_STATUS caShowMmsRegionSection(BYTE RegionID, BYTE Buddy);
    EXPORT E_STATUS caCreateMMRegion(LPVOID lpAddress, SIZE_T Length);
#endif
    EXPORT E_STATUS caGetError(VOID);
    EXPORT E_STATUS caSetError(E_STATUS emCode);
    EXPORT HANDLE caGetCurrentTask(VOID);
    EXPORT TICK caGetTaskStartTick(HANDLE hTask);

    EXPORT E_STATUS caScheduleTimeout(LONG Timeout);
    EXPORT E_STATUS caTaskWakeup(HANDLE hTask);
    EXPORT BOOL caTestCancel(VOID);
    EXPORT E_STATUS caPostCancel(HANDLE hTask);
    EXPORT TASK_PRIORITY caGetPriority(HANDLE hTask);
    EXPORT E_STATUS caSetPriority(HANDLE hTask, TASK_PRIORITY Priority);
    EXPORT E_STATUS caCloseTask(HANDLE hTask);
    
    EXPORT E_STATUS caIdleEntry(LPVOID lpParam);
    EXPORT VOID caTaskEntry(FNTASKMAIN fnMain, LPVOID lpArgument, HANDLE hTask);

    EXPORT SMLT_KEY_T caGetSmltKey(VOID);
    EXPORT E_STATUS caPutSmltKey(SMLT_KEY_T SmltKey);
    EXPORT E_STATUS caGetSmltValue(SMLT_KEY_T SmltKey, LPDWORD lpValue);
    EXPORT E_STATUS caSetSmltValue(SMLT_KEY_T SmltKey, DWORD Value);
    
    EXPORT TASK_STATUS caGetTaskState(HANDLE hTask);
    EXPORT E_STATUS ntGetTaskInfor(HANDLE hTask, LPTASK_INFOR lpTaskInfor);
    EXPORT HANDLE ntEnumNextTask(HANDLE hTask);
    
#if (CONFIG_PROFILER_CYCLE != 0)
    EXPORT E_STATUS ntSystemProfiler(VOID);
#endif
    EXPORT CODE_TEXT E_STATUS caGetObjectName(HANDLE handle,
                                CHAR Name[OBJECT_NAME_MAX], SIZE_T Length);
    EXPORT HANDLE caMallocObject(LPCSTR lpName, DWORD Magic, LPVOID lpParam);
    EXPORT E_STATUS caActiveObject(HANDLE handle, LPVOID lpParam);
    EXPORT HANDLE caTakeObject(LPCSTR lpName, LPVOID lpParam);
    EXPORT E_STATUS caPostObject(HANDLE handle, LPVOID lpParam);
    EXPORT E_STATUS caWaitObject(HANDLE handle, LONG WaitTime);
    EXPORT E_STATUS caResetObject(HANDLE handle, LPVOID lpParam);
    EXPORT E_STATUS caFreeObject(HANDLE handle);
    EXPORT E_STATUS caStackMalloc(HANDLE hTask, LPVOID lpParam, E_TASK_PERMISSION Permission);
    EXPORT E_STATUS caStackFill(HANDLE hTask, E_TASK_PERMISSION Permission);
    EXPORT E_STATUS caStackFree(HANDLE hTask, E_TASK_PERMISSION Permission);

    EXPORT LPSTR caChooseName(__IN __OUT LPSTR lpName, LPSTR lpType);

#ifdef __cplusplus
}
#endif
#endif
