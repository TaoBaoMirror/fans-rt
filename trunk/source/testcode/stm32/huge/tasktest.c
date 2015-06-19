#ifndef BUILD_TESTCODE_THREAD
#include "stdio.h"
#include "string.h"
#include "limits.h"
#include <fadefs.h>
#include <faerror.h>
#include <fatypes.h>
#include <fapi.h>
#include <fapp.h>

#include "karch.h"
#include "libfw.h"
#include "ntdebug.h"

#define     TEST_CASE_CALL(Func, Execute)                                                   \
            do{                                                                             \
                if (STATE_SUCCESS != Func())                                                \
                {                                                                           \
                    LOG_ERROR(TRUE, "%s ... [FAILED]", #Func);                              \
                    Execute;                                                                \
                }                                                                           \
                else                                                                        \
                {                                                                           \
                    LOG_INFOR(TRUE, "%s ... [SUCCESS]", #Func);                             \
                }                                                                           \
            }while(0)

#define     TEST_CASE_ASSERT(Condition, Execute, ...)                                       \
            do{                                                                             \
                if (!(Condition))                                                           \
                {                                                                           \
                    LOG_ERROR(TRUE, __VA_ARGS__);                                           \
                    Execute;                                                                \
                }                                                                           \
            }while(0)

STATIC VOLATILE DWORD g_TaskCount = 0;
STATIC VOLATILE DWORD g_Priority = 0;
            
#define TEST02_TASK_NAME    "Task02"

STATIC E_STATUS TestCase00(VOID)
{
    
    signed int int_min = INT_MIN;
    signed int int_max = INT_MAX;
    signed short short_min = SHRT_MIN;
    signed short short_max = SHRT_MAX;
    
    TEST_CASE_ASSERT(int_max == INT_MAX, return STATE_NOT_MATCH,
        "The int_max %d not equal %d", int_max, INT_MAX);
    TEST_CASE_ASSERT(int_min == INT_MIN, return STATE_NOT_MATCH,
        "The int_min %d not equal %d", int_min, INT_MIN);
    TEST_CASE_ASSERT(short_min == SHRT_MIN, return STATE_NOT_MATCH,
        "The short_min %d not equal %d", short_min, SHRT_MIN);
    TEST_CASE_ASSERT(short_max == SHRT_MAX, return STATE_NOT_MATCH,
        "The short_max %d not equal %d", short_max, SHRT_MAX);
    
    return STATE_SUCCESS;
}


STATIC E_STATUS TestCase01(VOID)
{
    SetError(STATE_INVALID_PRIORITY);
    TEST_CASE_ASSERT(STATE_INVALID_PRIORITY == GetError(), return STATE_NOT_MATCH,
        "The error code(%d) not expect(%d).", GetError(), STATE_INVALID_PRIORITY);
    return STATE_SUCCESS;
}

STATIC E_STATUS TestTask02(LPVOID lpArgment)
{
    Sleep(10);
    g_TaskCount ++;
    return STATE_SUCCESS;
}

STATIC E_STATUS TestCase02(VOID)
{
    HANDLE hTask;
    E_STATUS State;
    CHAR Name[OBJECT_NAME_MAX] = {TEST02_TASK_NAME};

    g_TaskCount = 0;

    hTask = CreateTask(Name, TestTask02, NULL);
    
    TEST_CASE_ASSERT(INVALID_HANDLE_VALUE != hTask, return STATE_NOT_MATCH, "Create task %s failed !", Name);

    memset(Name, 0, sizeof(Name));

    State = GetTaskName(hTask, Name, sizeof(Name));

    TEST_CASE_ASSERT(STATE_SUCCESS == State, return State, "Get task name failed.", Name);

    TEST_CASE_ASSERT (0 == strcmp(Name, TEST02_TASK_NAME), return STATE_NOT_MATCH, "Task name '%s' not expect '%s'", Name, TEST02_TASK_NAME);

    while(0==g_TaskCount);

    Sleep(100);
    
    return STATE_SUCCESS;
}

STATIC E_STATUS TestTask03(LPVOID lpArgment)
{
    static int count = 0;
    CHAR Name[OBJECT_NAME_MAX];
    DWORD dwFlags = CORE_DisableIRQ();
    g_TaskCount ++;
    CORE_RestoreIRQ(dwFlags);

    if (STATE_SUCCESS != GetTaskSelfName(Name, sizeof(Name)))
    {
        LOG_INFOR(TRUE, "Get current task name failed !");
        return STATE_UNKNOW_ERROR;
    }
    
    LOG_INFOR(TRUE, "'%s' starting...", Name);

    do{
        count++;
        if(count >=100000){
            count = 0;
            LOG_INFOR(TRUE, "'%s' , stack %p, %ld, start test...", Name, FW_StackPosition(), (DWORD)GetSystemTick());
        }
    }while(STATE_SUCCESS == TestCancel());
    LOG_INFOR(TRUE, "'%s' exit...", Name);
    dwFlags = CORE_DisableIRQ();
    g_TaskCount ++;
    CORE_RestoreIRQ(dwFlags);

    return STATE_SUCCESS;
}

STATIC E_STATUS TestCase03(VOID)
{
    HANDLE hTask;
    E_STATUS State;
    CHAR Name[OBJECT_NAME_MAX] = {"Test03"};
    
    g_TaskCount = 0;

    hTask = CreateTask(Name, TestTask03, NULL);
    
    TEST_CASE_ASSERT(INVALID_HANDLE_VALUE != hTask, return STATE_NOT_MATCH, "Create task %s failed !", Name);
    
    Sleep(1440);

    TEST_CASE_ASSERT(1 == g_TaskCount, return STATE_NOT_MATCH, "The counter1(%d) not expect(%d).", g_TaskCount, 1);

    State = CancelTask(hTask);

    TEST_CASE_ASSERT(STATE_SUCCESS == State, return State, "Cancel task %s failed.", Name);

    Sleep(200);

    TEST_CASE_ASSERT(2 == g_TaskCount, return STATE_NOT_MATCH, "The counter2(%d) not expect(%d).", g_TaskCount, 2);
    
    return STATE_SUCCESS;
}

STATIC E_STATUS TestTask04(LPVOID lpArgment)
{
    DWORD dwFlags;
    do{
        dwFlags = CORE_DisableIRQ();
        g_TaskCount ++;
        CORE_RestoreIRQ(dwFlags);
    }while(STATE_SUCCESS == TestCancel());

    return STATE_SUCCESS;
}

STATIC E_STATUS TestCase04(VOID)
{
    DWORD OldValue;
    DWORD Count = 0;
    HANDLE hTask;
    E_STATUS State;
    CHAR Name[OBJECT_NAME_MAX] = {"Test04"};
    
    g_TaskCount = 0;
    OldValue = g_TaskCount;

    hTask = CreateTask(Name, TestTask04, NULL);
    
    TEST_CASE_ASSERT(INVALID_HANDLE_VALUE != hTask, return STATE_NOT_MATCH, "Create task %s failed !", Name);
    
    for (Count = 0 ; Count < 10; Count ++)
    {
        Sleep(1);
        TEST_CASE_ASSERT(g_TaskCount != OldValue, return STATE_NOT_MATCH, "The test task not started !");
        OldValue = g_TaskCount;
    }
    
    State = KillTask(hTask);
    
    TEST_CASE_ASSERT(STATE_SUCCESS == State, return State, "kill task %s failed.", Name);

    Sleep(50);
    
    for (Count = 0 ; Count < 10; Count ++)
    {
        Sleep(1);
        TEST_CASE_ASSERT(g_TaskCount == OldValue, return STATE_NOT_MATCH, "The test task not stoped !");
    }

    Sleep(50);

    return STATE_SUCCESS;
}


STATIC E_STATUS TestTask05(LPVOID lpArgment)
{
    DWORD dwFlags = CORE_DisableIRQ();
    g_TaskCount ++;
    CORE_RestoreIRQ(dwFlags);

    TaskExit();

    CORE_DisableIRQ();
    g_TaskCount ++;
    CORE_RestoreIRQ(dwFlags);
    
    LOG_INFOR(TRUE, "Why to here ?");
    
    return STATE_SUCCESS;
}

STATIC E_STATUS TestCase05(VOID)
{
    HANDLE hTask;
    DWORD OldValue;
    DWORD Count = 0;
    CHAR Name[OBJECT_NAME_MAX] = {"Test05"};
    
    g_TaskCount = 0;

    OldValue = g_TaskCount;

    for (Count = 0 ; Count < 10; Count ++)
    {
        hTask = CreateTask(Name, TestTask05, NULL);
        TEST_CASE_ASSERT(INVALID_HANDLE_VALUE != hTask, return STATE_NOT_MATCH, "Create task %s failed !", Name);
        Sleep(10);
        TEST_CASE_ASSERT(g_TaskCount == OldValue + 1, return STATE_NOT_MATCH, "The test task not started, count = %d!", g_TaskCount);
        OldValue = g_TaskCount;
    }

    Sleep(50);

    return STATE_SUCCESS;
}


STATIC E_STATUS TestTask06(LPVOID lpArgment)
{
    DWORD dwFlags = CORE_DisableIRQ();
    g_TaskCount ++;
    CORE_RestoreIRQ(dwFlags);

    return STATE_SUCCESS;
}

STATIC E_STATUS TestCase06(VOID)
{
    HANDLE hTask;
    DWORD Count;
    CHAR Name[OBJECT_NAME_MAX] = {"Test06"};
    TASK_CREATE_PARAM Param = {0};
    
    g_TaskCount = 0;
    Param.fnTaskEntry   =   TestTask06;
    Param.lpTaskPath    =   NULL;
    Param.lpArgument    =   NULL;
    Param.StackSize     =   0;
    Param.AutoStartup   =   FALSE;
    Param.Priority      =   0;
    Param.SliceLength   =   0;

    hTask = CreateTaskEx(Name, &Param);
    TEST_CASE_ASSERT(INVALID_HANDLE_VALUE != hTask, return STATE_NOT_MATCH, "Create task %s failed !", Name);

    for (Count = 0 ; Count < 10; Count ++)
    {
        Sleep(10);
        TEST_CASE_ASSERT(0 == g_TaskCount, return STATE_NOT_MATCH, "The count value %d failed", g_TaskCount);
    }

    TaskStartup(hTask);
    Sleep(50);
    TEST_CASE_ASSERT(1 == g_TaskCount, return STATE_NOT_MATCH, "The count value %d failed", g_TaskCount);

    return STATE_SUCCESS;
}

STATIC E_STATUS TestTask07(LPVOID lpArgment)
{
    SetTaskSelfPriority(g_Priority);
    Sleep(5);
    return STATE_SUCCESS;
}

STATIC E_STATUS TestCase07(VOID)
{
    HANDLE hTask;
    CHAR Name[OBJECT_NAME_MAX] = {"Test07"};
    TASK_CREATE_PARAM Param = {0};
    
    g_Priority          =   GetTaskSelfPriority();
    Param.fnTaskEntry   =   TestTask07;
    Param.lpTaskPath    =   NULL;
    Param.lpArgument    =   NULL;
    Param.StackSize     =   0;
    Param.AutoStartup   =   FALSE;
    Param.Priority      =   0;
    Param.SliceLength   =   0;

    hTask = CreateTaskEx(Name, &Param);
    TEST_CASE_ASSERT(INVALID_HANDLE_VALUE != hTask, return STATE_NOT_MATCH, "Create task %s failed !", Name);
    TEST_CASE_ASSERT(0 == GetTaskPriority(hTask), return STATE_NOT_MATCH, "Task priority %u failed", GetTaskPriority(hTask));
    TaskStartup(hTask);
    TEST_CASE_ASSERT(GetTaskSelfPriority() == GetTaskPriority(hTask), return STATE_NOT_MATCH, "Task priority %u failed", GetTaskPriority(hTask));
    SetTaskPriority(hTask, 0);
    TEST_CASE_ASSERT(0 == GetTaskPriority(hTask), return STATE_NOT_MATCH, "Task priority %u failed", GetTaskPriority(hTask));
    Sleep(10);

    return STATE_SUCCESS;
}



STATIC E_STATUS TestCase17(VOID)
{
    HANDLE hTask = INVALID_HANDLE_VALUE;
    
    LOG_INFOR(TRUE, " task name  | CPU |  Priority |  Stack");
    while(INVALID_HANDLE_VALUE != (hTask = EnumerationNextTask(hTask)))
    {
        TASK_INFOR TaskInfor;

        memset(&TaskInfor, 0, sizeof(TASK_INFOR));
        GetTaskInformation(hTask, &TaskInfor);

        LOG_INFOR(TRUE, "%-12s   %3d       %d       %4u Bytes.",
            TaskInfor.TaskName, (DWORD)TaskInfor.TaskCPUPercent,
            (DWORD)TaskInfor.TaskPriority, TaskInfor.TaskStackRemain);
    }
    
    return STATE_SUCCESS;
}

//static CHAR g_Buffer[4096];

E_STATUS FansMain(VOID)
{
    TEST_CASE_CALL(TestCase00, return STATE_SYSTEM_FAULT);
    TEST_CASE_CALL(TestCase01, return STATE_SYSTEM_FAULT);
    TEST_CASE_CALL(TestCase02, return STATE_SYSTEM_FAULT);
    TEST_CASE_CALL(TestCase03, return STATE_SYSTEM_FAULT);
    TEST_CASE_CALL(TestCase04, return STATE_SYSTEM_FAULT);
    TEST_CASE_CALL(TestCase05, return STATE_SYSTEM_FAULT);
    TEST_CASE_CALL(TestCase06, return STATE_SYSTEM_FAULT);
    TEST_CASE_CALL(TestCase07, return STATE_SYSTEM_FAULT);
    //memset(g_Buffer, 0, sizeof(g_Buffer));
    //LOG_INFOR(TRUE, "Buffer address %p ... [SUCCESS]", g_Buffer);
    do{
        TEST_CASE_CALL(TestCase17, return STATE_SYSTEM_FAULT);
        Sleep(5000);
    }while(STATE_SUCCESS == TestCancel());
    return STATE_SUCCESS;
}
#endif

