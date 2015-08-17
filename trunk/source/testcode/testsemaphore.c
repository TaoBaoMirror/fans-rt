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
 *    2015-07-16     JiangYong       new file
 */

#include "stdio.h"
#include "string.h"
#include <fauser.h>
#include <fadefs.h>
#include <faerror.h>
#include <fatypes.h>
#include <fapi.h>


#if (defined(CONFIG_BUILD_IPC_SEMAPHORE) && (TRUE == CONFIG_BUILD_IPC_SEMAPHORE))
#include "testcode.h"



/*
 * 创建、捕获、释放、名称获取、释放后再捕获、自动命名测试
 */
STATIC RO_USER_CODE E_STATUS SEMAPHORE_TEST_CASE00(VOID)
{
    E_STATUS Result;
    TCHAR Name[OBJECT_NAME_MAX];
    HANDLE handle = INVALID_HANDLE_VALUE;
    HANDLE hFind = INVALID_HANDLE_VALUE;

    /* 创建对象测试 */
    handle = CreateSemaphore(g_TestSemapName, 1, 100);

    TEST_CASE_ASSERT(INVALID_HANDLE_VALUE != handle, return GetError(),
            "Create semaphore %s failed.", g_TestSemapName);

    /* 名称获取测试 */
    Result = GetObjectName(handle, Name, OBJECT_NAME_MAX);

    TEST_CASE_ASSERT(STATE_SUCCESS == Result, return Result,
            "Get semaphore(0x%08X) name failed.", handle);
    
    TEST_CASE_ASSERT(0 == TSTRCMP(Name, g_TestSemapName), return STATE_NOT_MATCH,
            "Object name not match !");

    /* 对象捕获测试 */
    hFind = TakeObject(Name);
    
    TEST_CASE_ASSERT(INVALID_HANDLE_VALUE != hFind, return GetError(),
            "take semaphore %s failed.", Name);
            
    TEST_CASE_ASSERT(handle == hFind, return GetError(),
            "Take semaphore %s failed.", Name);

    /* 对象释放测试 */
    Result = CloseHandle(handle);

    TEST_CASE_ASSERT(STATE_SUCCESS == Result, return Result,
            "Close semaphore(0x%08X - '%s') failed.", handle, Name);
    /* 释放后再捕获测试 */
    hFind = TakeObject(Name);
    
    TEST_CASE_ASSERT(INVALID_HANDLE_VALUE == hFind, return GetError(),
            "Semaphore %s close failed.", Name);

    /* 对象自动命名测试 */
    handle = CreateSemaphore(NULL, 1, 100);
    
    TEST_CASE_ASSERT(INVALID_HANDLE_VALUE != handle, return GetError(),
            "Create noname semaphore failed.");

    /* 名称获取测试 */
    Result = GetObjectName(handle, Name, OBJECT_NAME_MAX);
    
    TEST_CASE_ASSERT(STATE_SUCCESS == Result, return Result,
            "Get semaphore(0x%08X) name failed.", handle);

    /* 对象释放测试 */
    Result = CloseHandle(handle);

    TEST_CASE_ASSERT(STATE_SUCCESS == Result, return Result,
            "Close semaphore(0x%08X - '%s') failed, result %d.", handle, Name, Result);
            
    return STATE_SUCCESS;
}

STATIC RO_USER_CODE E_STATUS TEST_CASE01_TASK(LPVOID lpParam)
{
    E_STATUS Result;
    TCHAR Name[OBJECT_NAME_MAX];
    SHORT SignalID = WAIT_SIGNAL_INVALID;
	HANDLE hEvent = TakeObject(g_TestEventName);
	HANDLE hMutex = TakeObject(g_TestMutexName);
    HANDLE hSemap = TakeObject(g_TestSemapName);
    
    GetTaskSelfName(Name, OBJECT_NAME_MAX);

    LOG_INFOR(TRUE, "Task '%s' startup ...", Name);

    TEST_CASE_ASSERT(INVALID_HANDLE_VALUE != hSemap, return GetError(),
            "Take mutex %s failed.", lpParam);

    SignalID = WaitObject(hSemap, WAIT_INFINITE);

    TEST_CASE_ASSERT(WAIT_SIGNAL_ID_0 == SignalID, return GetError(),
            "Task '%s' wait semaphore %s failed, id %d, error %d.",
            Name, g_TestSemapName, SignalID, GetError());

    g_FinishedTaskHandle = GetCurrentTask();
    
    Result = PostEvent(hEvent);
    
    LOG_INFOR(TRUE, "Task '%s' wakeup ...", Name);

    TEST_CASE_ASSERT(STATE_SUCCESS == Result, return GetError(),
            "Task '%s' post event %s failed.",
            Name, g_TestEventName);
            
    while(!TestCancel()) Sleep(1);
    
    LOG_INFOR(TRUE, "Task '%s' finished ...", Name);

    Result = MutexLock(hMutex);

    TEST_CASE_ASSERT(STATE_SUCCESS == Result, return GetError(),
            "Task '%s' lock mutex %s failed.",
            Name, g_TestMutexName);

    g_FinishedTaskCount ++;

    Result = MutexUnlock(hMutex);

    TEST_CASE_ASSERT(STATE_SUCCESS == Result, return GetError(),
            "Task '%s' unlock mutex %s failed.",
            Name, g_TestMutexName);

    return STATE_SUCCESS;
}


STATIC RO_USER_CODE E_STATUS SEMAPHORE_TEST_CASE01(VOID)
{
    DWORD i;
    E_STATUS Result;
    SHORT SignalID;

    TCHAR TaskName[OBJECT_NAME_MAX];
    TCHAR PostName[OBJECT_NAME_MAX];
    TCHAR HopeName[OBJECT_NAME_MAX];

    HANDLE hEvent = INVALID_HANDLE_VALUE;
    HANDLE hMutex = INVALID_HANDLE_VALUE;
    HANDLE Handle = INVALID_HANDLE_VALUE;
    HANDLE hTask[MAX_TEST_TASK] = {INVALID_HANDLE_VALUE};
    
    g_FinishedTaskCount = 0;
    
    GetTaskSelfName(TaskName, OBJECT_NAME_MAX);
    
    hEvent = CreateEvent(g_TestEventName, TRUE, FALSE);
    
    TEST_CASE_ASSERT(INVALID_HANDLE_VALUE != hEvent,
            Result = GetError(); goto _exit,
            "Create event %s failed.", g_TestEventName);
    
    hMutex = CreateMutex(g_TestMutexName, FALSE);
    
    TEST_CASE_ASSERT(INVALID_HANDLE_VALUE != hMutex,
            Result = GetError(); goto _exit,
            "Create mutex %s failed.", g_TestMutexName);
    
    Handle = CreateSemaphore(g_TestSemapName, 0, 127);
    
    TEST_CASE_ASSERT(INVALID_HANDLE_VALUE != Handle,
            Result = GetError(); goto _exit,
            "Create semaphore %s failed.", g_TestSemapName);
    
    Result = TEST_TASK_CREATE(hTask, SIZEOF_ARRAY(hTask), TEST_CASE01_TASK, NULL);
    
    TEST_CASE_ASSERT(STATE_SUCCESS == Result,
            goto _exit,
            "Create test task failed !");
            
    for (i = 0; i < SIZEOF_ARRAY(hTask); i++)
    {
        Result = PostSemaphore(Handle, 1);

        TEST_CASE_ASSERT(STATE_SUCCESS == Result,
                goto _exit,
                "Task '%s' post semaphore %s failed, result %d.",
                TaskName, g_TestSemapName, Result);
        
        SignalID = WaitObject(hEvent, 20000);
        
        Result = GetError();

        GetObjectName(hTask[i], HopeName, OBJECT_NAME_MAX);

        GetObjectName(g_FinishedTaskHandle, PostName, OBJECT_NAME_MAX);

        TEST_CASE_ASSERT(WAIT_SIGNAL_ID_0 == SignalID,
                goto _exit,
                "Task '%s' wait event %s failed, result %d.",
                TaskName, g_TestEventName, Result);

        TEST_CASE_ASSERT(g_FinishedTaskHandle == hTask[i],
                goto _exit,
                "The task '%s' is out of expetation(%d).",
                PostName, HopeName);
    }

    for (i = 0; i < SIZEOF_ARRAY(hTask); i++)
    {
        Result = PostCancel(hTask[i]);
        
        TEST_CASE_ASSERT(STATE_SUCCESS == Result,
                goto _exit,
                "Task '%s' post semaphore %s failed, result %d.",
                TaskName, g_TestSemapName, Result);
    }

    for (i = 0; i < 100; i++)
    {
        if (g_FinishedTaskCount >= SIZEOF_ARRAY(hTask)) break;
        Sleep(100);
    }
    
    TEST_CASE_ASSERT(g_FinishedTaskCount >= SIZEOF_ARRAY(hTask),
                goto _exit,
                "Task '%s' wait the all test task failed.",
                TaskName);
    
_exit:
    CloseHandle(hEvent);
    CloseHandle(hMutex);
    CloseHandle(Handle);
    TEST_TASK_CLEANUP(hTask, SIZEOF_ARRAY(hTask));
    
    return Result;
}

STATIC RO_USER_CODE E_STATUS SEMAPHORE_TEST_CASE02(VOID)
{
    return STATE_SUCCESS;
}


STATIC RO_USER_DATA CONST TEST_CASE_DESCRIPTOR g_TestCase[] =
{
    DEFINE_TEST_CASE(SEMAPHORE_TEST_CASE00),
    DEFINE_TEST_CASE(SEMAPHORE_TEST_CASE01),
    DEFINE_TEST_CASE(SEMAPHORE_TEST_CASE02),
};

PUBLIC RO_USER_CODE E_STATUS SEMAPHORE_TEST_CASE(VOID)
{
    LONG i = 0;
    E_STATUS Temp;
    E_STATUS Result = STATE_SUCCESS;

    for (i = 0; i < SIZEOF_ARRAY(g_TestCase); i ++)
    {
        Temp = g_TestCase[i].fnTestCase();
        
        if (STATE_SUCCESS != Temp)
        {
            LOG_ERROR(TRUE, "%s .... [FAILED]", g_TestCase[i].lpTestName);
            Result = Temp;
        }
        else
        {
            LOG_ERROR(TRUE, "%s .... [SUCCESS]", g_TestCase[i].lpTestName);
        }
    }

    return Result;
}
#else
PUBLIC RO_USER_CODE E_STATUS SEMAPHORE_TEST_CASE(VOID)
{
    return STATE_SUCCESS;
}
#endif

