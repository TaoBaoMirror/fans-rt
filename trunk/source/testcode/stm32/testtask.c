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

STATIC E_STATUS TestCase00(VOID)
{
    SetError(STATE_INVALID_PRIORITY);
    TEST_CASE_ASSERT(STATE_INVALID_PRIORITY == GetError(), return STATE_NOT_MATCH,
        "The error code(%d) not expect(%d).", GetError(), STATE_INVALID_PRIORITY);
    return STATE_SUCCESS;
}

STATIC E_STATUS TestTask01(LPVOID lpArgment)
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

#define TEST02_TASK_NAME    "Task02"

STATIC E_STATUS TestCase01(VOID)
{
    DWORD i = 0x10000000;
    HANDLE hTask;
    E_STATUS State;
    TICK StartTick = GetSystemTick();
    CHAR Name[OBJECT_NAME_MAX] = {TEST02_TASK_NAME};

    TEST_CASE_ASSERT((STATE_SUCCESS == (State = TestSetCount(0))),
        return State, "Set count failed, result %d.", State);

    hTask = CreateTask(Name, TestTask01, NULL);
    
    TEST_CASE_ASSERT(INVALID_HANDLE_VALUE != hTask,
        return STATE_NOT_MATCH, "Create task %s failed !", Name);

    memset(Name, 0, sizeof(Name));

    State = GetTaskName(hTask, Name, sizeof(Name));

    TEST_CASE_ASSERT(STATE_SUCCESS == State,
        return State, "Get task name failed.", Name);

    TEST_CASE_ASSERT (0 == strcmp(Name, TEST02_TASK_NAME),
        return STATE_NOT_MATCH, "Task name '%s' not expect '%s'",
        Name, TEST02_TASK_NAME);

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



PUBLIC CODE_TEXT E_STATUS TASK_TESTCASE(VOID)
{
    E_STATUS Result = STATE_SUCCESS;
    
    g_TestHandles[TEST_MUTEX_ID] = CreateMutex(_TEXT("TASKMTX"), FALSE);
    
    TEST_CASE_ASSERT(INVALID_HANDLE_VALUE != g_TestHandles[TEST_MUTEX_ID],
            return GetError();, "Create mutex failed, result %d !", GetError());
    
    g_TestHandles[TEST_EVENT_ID] = CreateEvent(_TEXT("TASKEVT"), TRUE, FALSE);
    
    TEST_CASE_ASSERT(INVALID_HANDLE_VALUE != g_TestHandles[TEST_EVENT_ID],
            return GetError();, "Create mutex failed, result %d !", GetError());
    
    TEST_CASE_SET_RESULT(Result, TestCase00,);
    TEST_CASE_SET_RESULT(Result, TestCase01,);
    
    CloseHandle(g_TestHandles[TEST_MUTEX_ID]);
    CloseHandle(g_TestHandles[TEST_EVENT_ID]);

    return Result;
}
