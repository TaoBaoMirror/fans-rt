#ifndef BUILD_TESTCODE_THREAD
#include "stdio.h"
#include <fadefs.h>
#include <faklist.h>
#include <faerror.h>
#include <fatypes.h>
#include <fanscore.h>
#include <kdebug.h>
#include <fapi.h>
#include <fapp.h>

#include "libfw.h"
#include "kobject.h"
#include "libpool.h"
#include "taskcore.h"

PUBLIC DWORD GetAfterScheduleHWTick(VOID);
PUBLIC DWORD GetBeforScheduleHWTick(VOID);

STATIC E_STATUS TestTask(LPVOID lpArgment)
{
    CHAR Name[OBJECT_NAME_MAX];
    //Sleep(10);
    GetTaskName(INVALID_HANDLE_VALUE, Name, sizeof(Name));
    CORE_INFOR(TRUE, "Task '%s' starting", Name);
    
    return STATE_SUCCESS;
}


E_STATUS FansMain(VOID)
{
    HANDLE hTask;
    unsigned int i = 0;
    unsigned int j = 0;
    DWORD Tick0, Tick1, Tick3 = 0;
    CHAR Name[OBJECT_NAME_MAX] = {"test object"};
    KOBJECT_HEADER Header;
    
    for (i=0; i < 5000; i ++)
    {
        Tick0 = FW_GetTickCount();
        ntSystemProfiler();
        Tick1 = FW_GetTickCount();
        
        if (Tick0 > Tick1)
        {
            Tick3 = Tick3 < (Tick0 - Tick1) ? (Tick0 - Tick1) : Tick3;
        }
    }
    
    CORE_INFOR(TRUE, "The system profiler time is %d ticks.", Tick3);
    
    //GetTaskName(INVALID_HANDLE_VALUE, Name, sizeof(Name));
    //CORE_INFOR(TRUE, "Current task '%s' ...", Name);
    //Sleep(100);
#if 0
    //ObjectInit(lpHeader, mg, Pid, Eid, Sid, lpsName)
    ObjectInit(&Header, STM_MAGIC, 0, 0, 0, ((LPDWORD) Name));

    CORE_INFOR(TRUE, "Handle 0x%08x    cid: %x", GetObjectHandle(&Header), GetObjectClassID(&Header));
    
    ObjectInit(&Header, STM_MAGIC, 0x1f, 0, 0, ((LPDWORD) Name));
    CORE_INFOR(TRUE, "Handle 0x%08x    pid: %x", GetObjectHandle(&Header), GetObjectPid(&Header));
    
    ObjectInit(&Header, STM_MAGIC, 0, 0x1f, 0, ((LPDWORD) Name));
    CORE_INFOR(TRUE, "Handle 0x%08x    eid: %x", GetObjectHandle(&Header), GetObjectEid(&Header));
    
    ObjectInit(&Header, STM_MAGIC, 0, 0, 0x3fff, ((LPDWORD) Name));
    CORE_INFOR(TRUE, "Handle 0x%08x    eid: %x", GetObjectHandle(&Header), GetObjectSingleID(&Header));
#endif

#if 1   
    for (i = 0; i < 1; i ++)
    {
        for (j = 0; j < 100; j ++)
        {
            snprintf(Name, OBJECT_NAME_MAX, "TSK%08X", i * 32 + j);
            hTask = CreateTask(Name, TestTask, NULL);
            //if (INVALID_HANDLE_VALUE == hTask)
            //{
            //  CORE_INFOR(TRUE, "********Create task '%s' handle is 0x%08x, failed !!!!!!!!", Name, hTask);
                Sleep(1000);
            //}
            
            CORE_INFOR(TRUE, "Create task '%s' handle is 0x%08x.", Name, hTask);
        }
        Sleep(5000);
    }
#endif
//  CORE_INFOR(TRUE, "Create task '%s' handle is 0x%08x.", Name, hTask);
    return STATE_SUCCESS;
}
#endif
