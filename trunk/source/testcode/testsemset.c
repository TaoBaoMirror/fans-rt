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


#if (defined(CONFIG_BUILD_IPC_SEMSET) && (TRUE == CONFIG_BUILD_IPC_SEMSET))
#include "testcode.h"

STATIC RO_USER_CODE E_STATUS SEMSET_TEST_CASE00(VOID)
{   
    E_STATUS Result;
    TCHAR Name[OBJECT_NAME_MAX];
    LPCTSTR lpName = NAME_CASE00;
    HANDLE handle = INVALID_HANDLE_VALUE;
    HANDLE hFind = INVALID_HANDLE_VALUE;

    /* 创建对象测试 */
    handle = CreateSemset(lpName, 4, TRUE);
    
    Result = GetError();
    TEST_CASE_ASSERT(INVALID_HANDLE_VALUE != handle, return Result,
            "Create semset %s failed %d.", lpName, Result);
    
    /* 名称获取测试 */
    Result = GetObjectName(handle, Name, OBJECT_NAME_MAX);
    
    TEST_CASE_ASSERT(STATE_SUCCESS == Result, return Result,
            "Get semset(0x%08X) name failed.", handle);
    
    TEST_CASE_ASSERT(0 == TSTRCMP(Name, lpName), return STATE_NOT_MATCH,
            "Object name not match !");

    /* 对象捕获测试 */
    hFind = TakeObject(Name);
    
    Result = GetError();
    
    TEST_CASE_ASSERT(INVALID_HANDLE_VALUE != hFind, return Result,
            "take semset %s failed.", Name);

    TEST_CASE_ASSERT(handle == hFind, return STATE_NOT_EXIST,
            "Take semset %s failed, not found.", Name);

    /* 对象释放测试 */
    Result = CloseHandle(handle);

    TEST_CASE_ASSERT(STATE_SUCCESS == Result, return Result,
            "Close semset(0x%08X - '%s') failed.", handle, Name);
    /* 释放后再捕获测试 */
    hFind = TakeObject(Name);
    
    Result = GetError();
    
    TEST_CASE_ASSERT(INVALID_HANDLE_VALUE == hFind, return Result,
            "Semaphore %s close failed, result %d.", Name, Result);

    /* 对象自动命名测试 */
    handle = CreateSemset(NULL, 4, FALSE);
    
    Result = GetError();
    
    TEST_CASE_ASSERT(INVALID_HANDLE_VALUE != handle, return Result,
            "Create noname semset failed, result %d.", Result);

    /* 名称获取测试 */
    Result = GetObjectName(handle, Name, OBJECT_NAME_MAX);
    
    TEST_CASE_ASSERT(STATE_SUCCESS == Result, return Result,
            "Get semset(0x%08X) name failed.", handle);

    /* 对象释放测试 */
    Result = CloseHandle(handle);

    TEST_CASE_ASSERT(STATE_SUCCESS == Result, return Result,
            "Close semset(0x%08X - '%s') failed, result %d.", handle, Name, Result);
            
    return STATE_SUCCESS;
}

#define     FULL_NAME       _TEXT("FullSet")

STATIC RO_USER_CODE E_STATUS TEST_CASE01_TASK(LPVOID lpParam)
{
    E_STATUS Result;
    TASK_PRIORITY Priority;
    DWORD TaskID = 0;
    TCHAR Name[OBJECT_NAME_MAX];
    SHORT SignalID = WAIT_SIGNAL_INVALID;
    HANDLE handle = TakeObject(lpParam);
    HANDLE hFull = TakeObject(FULL_NAME);
    
    GetTaskSelfName(Name, OBJECT_NAME_MAX);

    LOG_INFOR(TRUE, "Task '%s' startup ...", Name);
    
    /* 调整当前任务优先级为实时 */
    Result = SetTaskSelfPriority(TASK_PRIORITY_REAL);

    TEST_CASE_ASSERT(STATE_SUCCESS == Result, return Result,
            "'%s' set self priority to realtime failed, result %d.", Name, Result);
    
    Priority = GetTaskSelfPriority();

    Result = GetError();

    TEST_CASE_ASSERT(TASK_PRIORITY_REAL == Priority, return Result,
            "'%s' set self priority to realtime failed, priority is %d.", Name, Priority);

    TaskID = g_FinishedTaskCount ++;

    TEST_CASE_ASSERT(INVALID_HANDLE_VALUE != handle, return GetError(),
            "Take mutex %s failed.", lpParam);
    /* 1. 阻塞队列顺序测试，等待信号 */
    SignalID = WaitObject(handle, WAIT_INFINITE);
    
    LOG_INFOR(TRUE, "Task '%s' wakeup ...", Name);

    TEST_CASE_ASSERT(TaskID == SignalID, return GetError(),
            "Task '%s' wait object %s failed, id %d, error %d.",
            Name, lpParam, SignalID, GetError());
    
    g_lpWakeupTaskName = Name;
    
    Sleep(5000);
    
    g_FinishedTaskCount --;

    Result = PostSemset(hFull, (SHORT) TaskID);
    
    TEST_CASE_ASSERT(STATE_SUCCESS == Result, return Result,
            "'%s' post semset failed, result %d.", Name, Result);
            
    return STATE_SUCCESS;
}

STATIC RO_USER_CODE E_STATUS SEMSET_TEST_CASE01(VOID)
{
    DWORD i;
    E_STATUS Result;
    HANDLE handle;
    HANDLE hFull;
    SHORT SignalID;
    HANDLE hTask[MAX_TEST_TASK];
    TCHAR TaskName[OBJECT_NAME_MAX];

    GetTaskSelfName(TaskName, OBJECT_NAME_MAX);

    handle = CreateSemset(g_TaskCase01Name, 4, FALSE);

    TEST_CASE_ASSERT(INVALID_HANDLE_VALUE != handle, return GetError(),
            "Create semaphore %s failed.", g_TaskCase01Name);
    
    hFull = CreateSemset(FULL_NAME, 4, TRUE);

    g_FinishedTaskCount = 0;
    
    Result = TEST_TASK_CREATE(hTask, SIZEOF_ARRAY(hTask), TEST_CASE01_TASK, (void*)g_TaskCase01Name);
    
    TEST_CASE_ASSERT(STATE_SUCCESS == Result,
            TEST_TASK_CLEANUP(hTask, SIZEOF_ARRAY(hTask));
            CloseHandle(handle);
            return Result,
            "Create test task failed !");
            
    for (i = 0; i < SIZEOF_ARRAY(hTask); i ++)
    {
        TCHAR Name[OBJECT_NAME_MAX];
        
        TSNPRINTF(Name, OBJECT_NAME_MAX, "TASK%02u", i);
        
        Result = PostSemset(handle, (SHORT) i);

        TEST_CASE_ASSERT(STATE_SUCCESS == Result,
                TEST_TASK_CLEANUP(hTask, SIZEOF_ARRAY(hTask));
                CloseHandle(handle);
                CloseHandle(hFull);
                return Result,
                "'%s' post semset failed, result %d.", TaskName, Result);
        TEST_CASE_ASSERT(NULL != g_lpWakeupTaskName, 
                TEST_TASK_CLEANUP(hTask, SIZEOF_ARRAY(hTask));
                CloseHandle(handle);
                CloseHandle(hFull);
                return STATE_SYSTEM_FAULT,
                "Task '%s' post semset %s failed, invalid wakeup task name.",
                TaskName, g_TaskCase01Name);
        /* 检查唤醒任务的顺序是否符合要求 */
        TEST_CASE_ASSERT(0 == TSTRCMP(g_lpWakeupTaskName, Name),
                TEST_TASK_CLEANUP(hTask, SIZEOF_ARRAY(hTask));
                CloseHandle(handle);
                CloseHandle(hFull);
                return STATE_SYSTEM_FAULT,
                "Task '%s' post semset %s failed, wakeup task out of expetation.",
                TaskName, g_TaskCase01Name);
    }
    
    SignalID = WaitObject(hFull, 6000);
    
    Result = GetError();
    
    TEST_CASE_ASSERT(WAIT_SIGNAL_INVALID != SignalID, return Result,
            "Task '%s' wait object %s failed, id %d, error %d.",
            TaskName, FULL_NAME, SignalID, Result);
            
    return STATE_SUCCESS;
}

STATIC RO_USER_CODE E_STATUS SEMSET_TEST_CASE02(VOID)
{
    return STATE_SUCCESS;
}


STATIC RO_USER_DATA CONST TEST_CASE_DESCRIPTOR g_TestCase[] =
{
    DEFINE_TEST_CASE(SEMSET_TEST_CASE00),
    DEFINE_TEST_CASE(SEMSET_TEST_CASE01),
    DEFINE_TEST_CASE(SEMSET_TEST_CASE02),
};

PUBLIC RO_USER_CODE E_STATUS SEMSET_TEST_CASE(VOID)
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
PUBLIC RO_USER_CODE E_STATUS SEMSET_TEST_CASE(VOID)
{
    return STATE_SUCCESS;
}
#endif

