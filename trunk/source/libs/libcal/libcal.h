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

#if (defined(CONFIG_BUILD_LOCAL_STORE) && (TRUE == CONFIG_BUILD_LOCAL_STORE))
    EXPORT HANDLE caGetLsotHandle(HANDLE hTask);
    EXPORT HANDLE caMallocLsotObject(DWORD Total);
    EXPORT E_STATUS caActiveLsotObject(HANDLE hLsot, HANDLE hTask);
#define     caCloseLsotObject(hLsot)       caFreeObject(handle)
    EXPORT LSOT_KEY_T caGetLsotKey(HANDLE hLsot);
    EXPORT E_STATUS caPutLsotKey(HANDLE hLsot, LSOT_KEY_T LsotKey);
    EXPORT E_STATUS caGetLsotValue(HANDLE hLsot, LSOT_KEY_T LsotKey, DWORD_PTR lpValue);
    EXPORT E_STATUS caSetLsotValue(HANDLE hLsot, LSOT_KEY_T LsotKey, DWORD Value);
#endif

    EXPORT TASK_STATUS caGetTaskState(HANDLE hTask);
    EXPORT E_STATUS ntGetTaskInfor(HANDLE hTask, LPTASK_INFOR lpTaskInfor);
    EXPORT HANDLE ntEnumNextTask(HANDLE hTask);
    
#if (CONFIG_PROFILER_CYCLE != 0)
    EXPORT E_STATUS ntSystemProfiler(VOID);
#endif
    EXPORT E_STATUS caGetObjectName(HANDLE handle,
                                CHAR Name[OBJECT_NAME_MAX], SIZE_T Length);
    EXPORT HANDLE caMallocObject(LPCSTR lpName, DWORD Magic, LPVOID lpParam);
    EXPORT HANDLE caMallocNoNameObject(DWORD Magic, LPVOID lpParam);
    EXPORT E_STATUS caActiveObject(HANDLE handle, LPVOID lpParam);
    EXPORT HANDLE caTakeObject(LPCSTR lpName, LPVOID lpParam);
    EXPORT E_STATUS caFreeObject(HANDLE handle);
    EXPORT E_STATUS caRequestMethod(HANDLE handle, LPVOID lpParam, DWORD Method);
    EXPORT SHORT caWaitObject(HANDLE handle, LONG WaitTime);
    EXPORT HANDLE caCreateObject(LPCTSTR lpCTName, DWORD ClassMagic, LPVOID Attribute);

    EXPORT E_STATUS caStackMalloc(HANDLE hTask, LPVOID lpParam, E_TASK_PERMISSION Permission);
    EXPORT E_STATUS caStackFill(HANDLE hTask, LPVOID lpParam, E_TASK_PERMISSION Permission);
    EXPORT E_STATUS caStackFree(HANDLE hTask, E_TASK_PERMISSION Permission);
    
    EXPORT VOID USER_TaskEntry(FNTASKMAIN fnMain, LPVOID lpArgument, HANDLE hTask);
    EXPORT VOID USER_TaskLeave(VOID);

    EXPORT LPSTR caChooseName(__IN __OUT LPSTR lpName, LPSTR lpType);

#ifdef __cplusplus
}
#endif
#endif
