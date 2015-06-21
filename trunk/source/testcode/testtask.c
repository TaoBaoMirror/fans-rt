#include "stdio.h"
#include "string.h"
#include <fauser.h>
#include <fadefs.h>
#include <faerror.h>
#include <fatypes.h>
#include <fapi.h>

#include "testcode.h"

#define TEST_MUTEX_ID   0
#define TEST_EVENT_ID   1
STATIC RW_DATA VOLATILE DWORD g_TaskCount = 0;
STATIC RW_DATA HANDLE g_TestHandles[2] = {INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE};

STATIC CODE_TEXT E_STATUS TestSetCount(DWORD Value)
{
    E_STATUS State;
    
    TEST_CASE_ASSERT((STATE_SUCCESS == (State = MutexLock(g_TestHandles[TEST_MUTEX_ID]))),
        return State, "Lock mutex failed result %d !", State);

    g_TaskCount = Value;

    TEST_CASE_ASSERT((STATE_SUCCESS == (State = MutexUnlock(g_TestHandles[TEST_MUTEX_ID]))),
        return State, "Unlock mutex failed result %d !", State);
    
    return State;
}

STATIC E_STATUS TASK_TEST_CASE00(VOID)
{
    E_STATUS State;
    TICK TickStart = (TICK) 0;
    TICK TickFinish = (TICK) 0;
    TASK_STATUS TaskState;
    TCHAR TaskName[OBJECT_NAME_MAX];
    HANDLE hCurrent = INVALID_HANDLE_VALUE;
    TASK_PRIORITY Priority = TASK_PRIORITY_INVALID;

    SetError(STATE_INVALID_PRIORITY);
    TEST_CASE_ASSERT(STATE_INVALID_PRIORITY == GetError(), return STATE_NOT_MATCH,
        "The error code(%d) not expect(%d).", GetError(), STATE_INVALID_PRIORITY);

    hCurrent = GetCurrentTask();

    TEST_CASE_ASSERT(INVALID_HANDLE_VALUE != hCurrent, return STATE_INVALID_NAME,
        "The task handle value invalid.");

    State = GetTaskName(hCurrent, TaskName, sizeof(TaskName));

    TEST_CASE_ASSERT(STATE_SUCCESS == State, return State,
        "Get task name failed, result %d.", TaskName, BOOT_TASK_NAME);

    TEST_CASE_ASSERT(0 == strcmp(TaskName, BOOT_TASK_NAME), return STATE_INVALID_NAME,
        "Task name '%s' not expect '%s'", TaskName, BOOT_TASK_NAME);

    Priority = GetPriority(hCurrent);

    TEST_CASE_ASSERT(TASK_PRIORITY_NORMAL == Priority, return STATE_INVALID_STATE,
        "Task name '%s' not expect '%s'", TaskName, BOOT_TASK_NAME);

    TaskState = GetTaskState(hCurrent);

    TEST_CASE_ASSERT(TASK_STATE_WORKING == TaskState, return STATE_INVALID_STATE,
        "Task name '%s' state %d invalid.", TaskName, TaskState);

    TickStart = GetSystemTick();

    Sleep(1001);

    TickFinish = GetSystemTick();

    TEST_CASE_ASSERT(TickStart + MILLI_SECOND_TO_TICK(1000) <= TickFinish,
        return STATE_INVALID_STATE, "Task name '%s' befor sleep tick is %lld"
        ", after tick is %lld.", TaskName, TickStart, TickFinish);

    return STATE_SUCCESS;
}

STATIC E_STATUS TASK_TEST_THREAD01(LPVOID lpArgment)
{
    DWORD i;
    E_STATUS State;

    Sleep(10);
    
    for (i = 0; i < 100; i ++)
    {
        if (STATE_SUCCESS != (State = TestSetCount(i)))
        {
            LOG_ERROR(TRUE, "Set count failed, result %d.", State);
            return State;
        }
    }

    return STATE_SUCCESS;
}

#define TASK_NAME_THREAD01    "Task01"

STATIC E_STATUS TASK_TEST_CASE01(VOID)
{
    HANDLE hTask;
    E_STATUS State;
    DWORD i = 0x10000000;
    TICK StartTick = GetSystemTick();
    CHAR Name[OBJECT_NAME_MAX] = {TASK_NAME_THREAD01};

    TEST_CASE_ASSERT((STATE_SUCCESS == (State = TestSetCount(0))),
        return State, "Set count failed, result %d.", State);

    hTask = CreateTask(Name, TASK_TEST_THREAD01, NULL);
    
    TEST_CASE_ASSERT(INVALID_HANDLE_VALUE != hTask,
        return STATE_NOT_MATCH, "Create task %s failed !", Name);

    memset(Name, 0, sizeof(Name));

    State = GetTaskName(hTask, Name, sizeof(Name));

    TEST_CASE_ASSERT(STATE_SUCCESS == State,
        return State, "Get task name failed.", Name);

    TEST_CASE_ASSERT (0 == strcmp(Name, TASK_NAME_THREAD01),
        return STATE_NOT_MATCH, "Task name '%s' not expect '%s'",
        Name, TASK_NAME_THREAD01);

    while(0==g_TaskCount)
    {
        if (--i == 0)
        {
            Sleep(1000);
            LOG_ERROR(TRUE, "Wait '%s' time out.", Name);
            return STATE_TIME_OUT;
        }
    }

    Sleep(1000);
    
    return STATE_SUCCESS;
}



PUBLIC CODE_TEXT E_STATUS TASK_TEST_CASE(VOID)
{
    E_STATUS Result = STATE_SUCCESS;
    
    g_TestHandles[TEST_MUTEX_ID] = CreateMutex(_TEXT("TASKMTX"), FALSE);
    
    TEST_CASE_ASSERT(INVALID_HANDLE_VALUE != g_TestHandles[TEST_MUTEX_ID],
            return GetError();, "Create mutex failed, result %d !", GetError());
    
    g_TestHandles[TEST_EVENT_ID] = CreateEvent(_TEXT("TASKEVT"), TRUE, FALSE);
    
    TEST_CASE_ASSERT(INVALID_HANDLE_VALUE != g_TestHandles[TEST_EVENT_ID],
            return GetError();, "Create mutex failed, result %d !", GetError());
    
    TEST_CASE_SET_RESULT(Result, TASK_TEST_CASE00,);
    TEST_CASE_SET_RESULT(Result, TASK_TEST_CASE01,);
    
    CloseHandle(g_TestHandles[TEST_MUTEX_ID]);
    CloseHandle(g_TestHandles[TEST_EVENT_ID]);

    return Result;
}
