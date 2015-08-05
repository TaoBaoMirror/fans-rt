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


#if (defined(CONFIG_BUILD_IPC_MUTEX) && (TRUE == CONFIG_BUILD_IPC_MUTEX))
#include "testcode.h"

#ifdef _UNICODE
#define     TSNPRINTF           wsnprintf
#define     TSTRCMP(s1, s2)     wcscmp(s1, s2)
#else
#define     TSNPRINTF           snprintf
#define     TSTRCMP(s1, s2)     strcmp(s1, s2)
#endif

#define     NAME_CASE00         _TEXT("CASE00")
/*
 * 创建、捕获、释放、名称获取、释放后再捕获、自动命名测试
 */
STATIC RO_USER_CODE E_STATUS MUTEX_TEST_CASE00(VOID)
{
    E_STATUS Result;
    TCHAR Name[OBJECT_NAME_MAX];
    LPCTSTR lpName = NAME_CASE00;
    HANDLE handle = INVALID_HANDLE_VALUE;
    HANDLE hFind = INVALID_HANDLE_VALUE;

    /* 创建对象测试 */
    handle = CreateMutex(lpName, FALSE);
    
    TEST_CASE_ASSERT(INVALID_HANDLE_VALUE != handle, return GetError(),
            "Create mutex %s failed.", lpName);
    
    /* 名称获取测试 */
    Result = GetObjectName(handle, Name, OBJECT_NAME_MAX);
    
    TEST_CASE_ASSERT(STATE_SUCCESS == Result, return Result,
            "Get mutex(0x%08X) name failed.", handle);
    
    TEST_CASE_ASSERT(0 == TSTRCMP(Name, lpName), return STATE_NOT_MATCH,
            "Object name not match !");

    /* 对象捕获测试 */
    hFind = TakeObject(Name);
    
    TEST_CASE_ASSERT(INVALID_HANDLE_VALUE != hFind, return GetError(),
            "Take mutex %s failed.", Name);
            
    TEST_CASE_ASSERT(handle == hFind, return GetError(),
            "Take mutex %s failed.", Name);

    /* 对象释放测试 */
    Result = CloseHandle(handle);

    TEST_CASE_ASSERT(STATE_SUCCESS == Result, return Result,
            "Close mutex(0x%08X - '%s') failed.", handle, Name);

    /* 释放后再捕获测试 */
    hFind = TakeObject(Name);
    
    TEST_CASE_ASSERT(INVALID_HANDLE_VALUE == hFind, return GetError(),
            "Mutex %s close failed.", Name);

    /* 对象自动命名测试 */
    handle = CreateMutex(NULL, FALSE);
    
    TEST_CASE_ASSERT(INVALID_HANDLE_VALUE != handle, return GetError(),
            "Create noname mutex failed.");

    /* 名称获取测试 */
    Result = GetObjectName(handle, Name, OBJECT_NAME_MAX);
    
    TEST_CASE_ASSERT(STATE_SUCCESS == Result, return Result,
            "Get mutex(0x%08X) name failed.", handle);

    /* 对象释放测试 */
    Result = CloseHandle(handle);

    TEST_CASE_ASSERT(STATE_SUCCESS == Result, return Result,
            "Close mutex(0x%08X - '%s') failed, result %d.", handle, Name, Result);
    
    return STATE_SUCCESS;
}

#define     NAME_CASE01         _TEXT("CASE01")
STATIC RO_USER_DATA CONST TCHAR     g_TaskCase01MutexName[] = {NAME_CASE01};
STATIC RW_USER_DATA VOLATILE LPTSTR g_lpWakeupTaskName  = NULL;
STATIC RW_USER_DATA VOLATILE DWORD  g_FinishedTaskCount = 0;

STATIC RO_USER_CODE E_STATUS TEST_CASE01_TASK(LPVOID lpParam)
{
    E_STATUS Result;
    TASK_PRIORITY Priority;
    TCHAR Name[OBJECT_NAME_MAX];
    HANDLE hMutex = TakeObject(lpParam);
    
    GetTaskSelfName(Name, OBJECT_NAME_MAX);

    LOG_DEBUG(TRUE, "Task '%s' startup ...", Name);

    TEST_CASE_ASSERT(INVALID_HANDLE_VALUE != hMutex, return GetError(),
            "Take mutex %s failed.", lpParam);
    
    Result = MutexLock(hMutex);

    g_lpWakeupTaskName = Name;

    TEST_CASE_ASSERT(STATE_SUCCESS == Result, return Result,
            "Task '%s' lock mutex %s failed, result %d.", Name, lpParam, Result);
    
    Priority = GetTaskSelfPriority();

    TEST_CASE_ASSERT(TASK_PRIORITY_REAL == Priority, return STATE_SYSTEM_FAULT,
            "Task '%s' lock mutex %s failed, priority is %d.",
            Name, lpParam, Priority);
    
    Result = MutexUnlock(hMutex);
    
    TEST_CASE_ASSERT(STATE_SUCCESS == Result, return Result,
            "Task '%s' unlock mutex %s failed, result %d.", Name, lpParam, Result);
    
    Priority = GetTaskSelfPriority();
    
    TEST_CASE_ASSERT(TASK_PRIORITY_NORMAL == Priority, return STATE_SYSTEM_FAULT,
            "Task '%s' lock mutex %s failed, priority is %d, normal is %d.",
            Name, lpParam, Priority, TASK_PRIORITY_NORMAL);
            
    LOG_DEBUG(TRUE, "Task '%s' will be lock mutex '%s'.", Name, lpParam);

    Result = MutexLock(hMutex);
    
    TEST_CASE_ASSERT(STATE_REMOVED == Result, return Result,
            "Task '%s' lock mutex %s failed, result %d.", Name, lpParam, Result);

    g_FinishedTaskCount ++;

    return STATE_SUCCESS;
}

STATIC RO_USER_CODE VOID MUTEX_CASE01_CLEANUP(HANDLE * hTask, DWORD Count)
{
    DWORD j;
    
    for (j = 0; j < Count; j ++)
    {
        if (INVALID_HANDLE_VALUE != hTask[j]) KillTask(hTask[j]);
    }
}

/*
 * 加锁、解锁、倒挂、阻塞后释放测试
 */

STATIC RO_USER_CODE E_STATUS MUTEX_TEST_CASE01(VOID)
{
    DWORD i;
    HANDLE hMutex;
    HANDLE hTask[4];
    E_STATUS Result;
    TASK_PRIORITY Priority;
    TCHAR TaskName[OBJECT_NAME_MAX];

    GetTaskSelfName(TaskName, OBJECT_NAME_MAX);

    /* 创建一个被锁住的 MUTEX 对象*/
    hMutex = CreateMutex(g_TaskCase01MutexName, TRUE);
    
    TEST_CASE_ASSERT(INVALID_HANDLE_VALUE != hMutex, return GetError(),
            "Create mutex %s failed.", g_TaskCase01MutexName);
    
    for (i = 0; i < SIZEOF_ARRAY(hTask); i ++)
    {
        hTask[i] = INVALID_HANDLE_VALUE;
    }

    /* 创建普通优先级的测试任务 */
    for (i = 0; i < SIZEOF_ARRAY(hTask); i ++)
    {
        TCHAR Name[OBJECT_NAME_MAX];

        TSNPRINTF(Name, OBJECT_NAME_MAX, "TASK%02u", i);
        hTask[i] = CreateTask(Name, TEST_CASE01_TASK, (void *) g_TaskCase01MutexName);
        
        TEST_CASE_ASSERT(INVALID_HANDLE_VALUE != hTask[i],
            MUTEX_CASE01_CLEANUP(hTask, SIZEOF_ARRAY(hTask));
            CloseHandle(hMutex);
            return STATE_SYSTEM_FAULT,
            "Create task %s failed !", Name);
        /* 休眠 100 ms 保证新创建的任务已经被 MUTEX 对象阻塞 */
        Sleep(100);
    }
    /* 调整当前任务优先级为实时 */
    Result = SetTaskSelfPriority(TASK_PRIORITY_REAL);

    TEST_CASE_ASSERT(STATE_SUCCESS == Result, return GetError(),
            "'%s' set self priority to realtime failed, result %d.", TaskName, Result);
    
    Priority = GetTaskSelfPriority();

    TEST_CASE_ASSERT(TASK_PRIORITY_REAL == Priority, return GetError(),
            "'%s' set self priority to realtime failed, priority is %d.", TaskName, Priority);

    /* 阻塞和倒挂测试 */
    for (i = 0; i < SIZEOF_ARRAY(hTask); i ++)
    {
        TCHAR Name[OBJECT_NAME_MAX];
        
        TSNPRINTF(Name, OBJECT_NAME_MAX, "TASK%02u", i);
        /* 解锁 MUTEX 对象，当前任务为实时任务，阻塞任务被唤醒，但不能立即得到执行 */
        Result = MutexUnlock(hMutex);
        
        TEST_CASE_ASSERT(STATE_SUCCESS == Result, 
                MUTEX_CASE01_CLEANUP(hTask, SIZEOF_ARRAY(hTask));
                CloseHandle(hMutex);
                SetTaskSelfPriority(TASK_PRIORITY_NORMAL);
                return Result,
                "Task '%s' lock mutex %s failed, result %d.",
                TaskName, g_TaskCase01MutexName, Result);

        /* 锁住 MUTEX 对象，当前任务为实时任务，下次解锁 MUTEX 时，当前任务优先唤醒 */
        Result = MutexLock(hMutex);
        
        TEST_CASE_ASSERT(STATE_SUCCESS == Result, 
                MUTEX_CASE01_CLEANUP(hTask, SIZEOF_ARRAY(hTask));
                CloseHandle(hMutex);
                SetTaskSelfPriority(TASK_PRIORITY_NORMAL);
                return Result,
                "Task '%s' unlock mutex %s failed, result %d.",
                TaskName, g_TaskCase01MutexName, Result);

        TEST_CASE_ASSERT(NULL != g_lpWakeupTaskName, 
                MUTEX_CASE01_CLEANUP(hTask, SIZEOF_ARRAY(hTask));
                CloseHandle(hMutex);
                SetTaskSelfPriority(TASK_PRIORITY_NORMAL);
                return STATE_SYSTEM_FAULT,
                "Task '%s' unlock mutex %s failed, invalid wakeup task name.",
                TaskName, g_TaskCase01MutexName);
        /* 检查唤醒任务的顺序是否符合要求 */
        TEST_CASE_ASSERT(0 == TSTRCMP(g_lpWakeupTaskName, Name),
                MUTEX_CASE01_CLEANUP(hTask, SIZEOF_ARRAY(hTask));
                CloseHandle(hMutex);
                SetTaskSelfPriority(TASK_PRIORITY_NORMAL);
                return STATE_SYSTEM_FAULT,
                "Task '%s' unlock mutex %s failed, wakeup task out of expetation.",
                TaskName, g_TaskCase01MutexName);
    }

    /* 恢复为默认优先级 */
    Result = SetTaskSelfPriority(TASK_PRIORITY_NORMAL);

    TEST_CASE_ASSERT(STATE_SUCCESS == Result, return GetError(),
            "'%s' set self priority to realtime failed, result %d.", TaskName, Result);
    
    Priority = GetTaskSelfPriority();

    TEST_CASE_ASSERT(TASK_PRIORITY_NORMAL == Priority, return GetError(),
            "'%s' set self priority to realtime failed, priority is %d.", TaskName, Priority);

    Sleep(1000);

    /* 关闭 MUTEX 对象 */
    Result = CloseHandle(hMutex);
    
    TEST_CASE_ASSERT(STATE_SUCCESS == Result, 
            MUTEX_CASE01_CLEANUP(hTask, SIZEOF_ARRAY(hTask));
            SetTaskSelfPriority(TASK_PRIORITY_NORMAL);
            return Result,
            "Task '%s' close mutex %s failed, result %d.",
            TaskName, g_TaskCase01MutexName, Result);

    /* 对象被删除后，在一定时间内，所有阻塞的任务必须全部被唤醒(获得STATE_REMOVED的返回值) */
    for (i = 0; i < 1000 && g_FinishedTaskCount < SIZEOF_ARRAY(hTask); i ++)
    {
        Sleep(10);
    }
    /* 检查任务是否已经被全部唤醒 */
    TEST_CASE_ASSERT(SIZEOF_ARRAY(hTask) == g_FinishedTaskCount, 
        MUTEX_CASE01_CLEANUP(hTask, SIZEOF_ARRAY(hTask));
        return STATE_SYSTEM_FAULT,
        "Task '%s' unlock mutex %s failed, invalid wakeup task name.",
        TaskName, g_TaskCase01MutexName, Result);
    
    MUTEX_CASE01_CLEANUP(hTask, SIZEOF_ARRAY(hTask));
    
    return STATE_SUCCESS;
}

/*
 * 加锁后释放、释放后加锁、释放后解锁
 */
STATIC RO_USER_CODE E_STATUS MUTEX_TEST_CASE02(VOID)
{
    return STATE_SUCCESS;
}


STATIC RO_USER_DATA CONST TEST_CASE_DESCRIPTOR g_TestCase[] =
{
    DEFINE_TEST_CASE(MUTEX_TEST_CASE00),
//    DEFINE_TEST_CASE(MUTEX_TEST_CASE01),
    DEFINE_TEST_CASE(MUTEX_TEST_CASE02),
};

PUBLIC RO_USER_CODE E_STATUS MUTEX_TEST_CASE(VOID)
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
PUBLIC RO_USER_CODE E_STATUS MUTEX_TEST_CASE(VOID)
{
    return STATE_SUCCESS;
}
#endif

