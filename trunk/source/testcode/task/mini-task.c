#ifndef BUILD_TESTCODE_THREAD
#include "stdio.h"
#include "string.h"
#include <fadefs.h>
#include <faklist.h>
#include <faerror.h>
#include <fatypes.h>
#include <fanscore.h>
#include <kdebug.h>
#include <fapi.h>
#include <fapp.h>

#include "libfw.h"


#define     TEST_CASE_CALL(Func, Execute)                                                   \
            do{                                                                             \
                if (STATE_SUCCESS != Func())                                                \
                {                                                                           \
                    CORE_ERROR(TRUE, "%s ... [FAILED]", #Func);                             \
                    Execute;                                                                \
                }                                                                           \
                else                                                                        \
                {                                                                           \
                    CORE_INFOR(TRUE, "%s ... [SUCCESS]", #Func);                                \
                }                                                                           \
            }while(0)

#define     TEST_CASE_ASSERT(Condition, Execute, ...)                                       \
            do{                                                                             \
                if (!(Condition))                                                           \
                {                                                                           \
                    CORE_ERROR(TRUE, __VA_ARGS__);                                          \
                    Execute;                                                                \
                }                                                                           \
            }while(0)

STATIC DWORD DummyCount = 0;

STATIC E_STATUS DummyTask(LPVOID lpArgment)
{
    DummyCount ++;

    return STATE_SUCCESS;
}

STATIC E_STATUS TastCase01(VOID)
{
    HANDLE hTask;
    unsigned int i = 0;
    CHAR Name[OBJECT_NAME_MAX];
    
    for (i = 0; i < 1000; i ++)
    {
        if (0 == (i % (CONFIG_TSS_ENTRIES_MAX-2)))
        {
            Sleep(1);
        }

        snprintf(Name, OBJECT_NAME_MAX, "TSK%08X", i);
        hTask = CreateTask(Name, DummyTask, NULL);
        
        TEST_CASE_ASSERT(hTask != INVALID_HANDLE_VALUE, 
            return STATE_SYSTEM_FAULT, 
            "Create task '%s' handle is 0x%08x failed !", Name, hTask);
    }
    
    Sleep(1000);

    TEST_CASE_ASSERT(DummyCount == i, 
        return STATE_SYSTEM_FAULT, 
        "Test task recovery failed , DummyCount(%d) not expect %d.", DummyCount, i);
    
    return STATE_SUCCESS;
}




STATIC E_STATUS TestTask02(LPVOID lpArgment)
{
    E_STATUS State;
    int i = 0;
    
    do{
        CHAR String00[20];
        
        strcpy(String00, "TestMessage1");
        
        {
            CHAR String01[30];
            strcpy(String01, String00);
            
            if (0 == (i % 33))
            {
                Sleep(1);
            }
            {
                CHAR String01[40];
                strcpy(String01, String00);
                
                if (0 == (i % 55))
                {
                    Sleep(1);
                }
                {
                    HANDLE hCurrentTask = GetCurrentTask();
                    
                    GetTaskName(hCurrentTask, String00, sizeof(String00));
                    
                    if (0 == (i % 77))
                    {
                        Sleep(1);
                    }
                }
            }
        }
        
        if (0 == (i % 100))
        {
            Sleep(10);
        }
        
        i++;
    }while(STATE_SUCCESS == (State = TestCancel()));

    return State;
}

STATIC E_STATUS TastCase02(VOID)
{
    CreateTask("TestTask01", TestTask02, NULL);
    CreateTask("TestTask02", TestTask02, NULL);
    CreateTask("TestTask03", TestTask02, NULL);
    CreateTask("TestTask04", TestTask02, NULL);

    return STATE_SUCCESS;
}



STATIC E_STATUS TestTask03(LPVOID lpArgment)
{
    E_STATUS State;
    int i = 0;
    
    do{
        CHAR String00[20];
        
        strcpy(String00, "TestMessage1");
        
        {
            CHAR String01[30];
            strcpy(String01, String00);
            
            if (0 == (i % 111))
            {
                Sleep(1);
            }
            {
                CHAR String01[40];
                strcpy(String01, String00);
                
                if (0 == (i % 222))
                {
                    Sleep(1);
                }
                {
                    HANDLE hCurrentTask = GetCurrentTask();
                    
                    GetTaskName(hCurrentTask, String00, sizeof(String00));
                    
                    if (0 == (i % 333))
                    {
                        Sleep(1);
                    }
                }
            }
        }
        
        if (0 == (i % 444))
        {
            Sleep(10);
        }
        
        i++;
    }while(STATE_SUCCESS == (State = TestCancel()));

    return State;
}


STATIC E_STATUS TastCase03(VOID)
{
    TASK_CREATE_PARAM TaskParam;
    
    memset(&TaskParam, 0, sizeof(TASK_CREATE_PARAM));

    //TaskParam.lpTaskPath  =   NULL;                       //no need
    TaskParam.AutoStartup   =   TRUE;
    TaskParam.fnTaskEntry   =   TestTask03;
    //TaskParam.lpArgument  =   NULL;                       //no need
    TaskParam.Priority      =   TASK_PRIORITY_REAL;
    TaskParam.SliceLength   =   CONFIG_TIME_SLICE_NORMAL;
    
    if (INVALID_HANDLE_VALUE == CreateTaskEx("TestTask05", &TaskParam))
    {
        CORE_FATAL(TRUE, "The idle task create failed !");
    }
    
    return STATE_SUCCESS;
}

STATIC E_STATUS TestCase17(VOID)
{
    HANDLE hTask = INVALID_HANDLE_VALUE;
    
    CORE_INFOR(TRUE, " task name  | CPU |  Priority |  Stack");
    while(INVALID_HANDLE_VALUE != (hTask = EnumerationNextTask(hTask)))
    {
        TASK_INFOR TaskInfor = {0};
        
        GetTaskInformation(hTask, &TaskInfor);

        CORE_INFOR(TRUE, "%-12s   %02d       %d       %4u Bytes.",
            TaskInfor.TaskName, (DWORD)TaskInfor.TaskCPUPercent,
            (DWORD)TaskInfor.TaskPriority, TaskInfor.TaskStackRemain);
    }
    
    return STATE_SUCCESS;
}

E_STATUS FansMain(VOID)
{
    TEST_CASE_CALL(TastCase01, return STATE_SYSTEM_FAULT);
    TEST_CASE_CALL(TastCase02, return STATE_SYSTEM_FAULT);
    TEST_CASE_CALL(TastCase03, return STATE_SYSTEM_FAULT);
    while(1)
    {
        TEST_CASE_CALL(TestCase17, return STATE_SYSTEM_FAULT);
        Sleep(1000);
    }
    return STATE_SUCCESS;
}
#endif
