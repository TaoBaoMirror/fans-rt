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
 * �����������ͷš����ƻ�ȡ���ͷź��ٲ����Զ���������
 */
STATIC RO_USER_CODE E_STATUS MUTEX_TEST_CASE00(VOID)
{
    E_STATUS Result;
    TCHAR Name[OBJECT_NAME_MAX];
    LPCTSTR lpName = NAME_CASE00;
    HANDLE handle = INVALID_HANDLE_VALUE;
    HANDLE hFind = INVALID_HANDLE_VALUE;

    /* ����������� */
    handle = CreateMutex(lpName, FALSE);
    
    TEST_CASE_ASSERT(INVALID_HANDLE_VALUE != handle, return GetError(),
            "Create mutex %s failed.", lpName);
    
    /* ���ƻ�ȡ���� */
    Result = GetObjectName(handle, Name, OBJECT_NAME_MAX);
    
    TEST_CASE_ASSERT(STATE_SUCCESS == Result, return Result,
            "Get mutex(0x%08X) name failed.", handle);
    
    TEST_CASE_ASSERT(0 == TSTRCMP(Name, lpName), return STATE_NOT_MATCH,
            "Object name not match !");

    /* ���󲶻���� */
    hFind = TakeObject(Name);
    
    TEST_CASE_ASSERT(INVALID_HANDLE_VALUE != hFind, return GetError(),
            "Take mutex %s failed.", Name);
            
    TEST_CASE_ASSERT(handle == hFind, return GetError(),
            "Take mutex %s failed.", Name);

    /* �����ͷŲ��� */
    Result = CloseHandle(handle);

    TEST_CASE_ASSERT(STATE_SUCCESS == Result, return Result,
            "Close mutex(0x%08X - '%s') failed.", handle, Name);

    /* �ͷź��ٲ������ */
    hFind = TakeObject(Name);
    
    TEST_CASE_ASSERT(INVALID_HANDLE_VALUE == hFind, return GetError(),
            "Mutex %s close failed.", Name);

    /* �����Զ��������� */
    handle = CreateMutex(NULL, FALSE);
    
    TEST_CASE_ASSERT(INVALID_HANDLE_VALUE != handle, return GetError(),
            "Create noname mutex failed.");

    /* ���ƻ�ȡ���� */
    Result = GetObjectName(handle, Name, OBJECT_NAME_MAX);
    
    TEST_CASE_ASSERT(STATE_SUCCESS == Result, return Result,
            "Get mutex(0x%08X) name failed.", handle);

    /* �����ͷŲ��� */
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
 * ���������������ҡ��������ͷŲ���
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

    /* ����һ������ס�� MUTEX ����*/
    hMutex = CreateMutex(g_TaskCase01MutexName, TRUE);
    
    TEST_CASE_ASSERT(INVALID_HANDLE_VALUE != hMutex, return GetError(),
            "Create mutex %s failed.", g_TaskCase01MutexName);
    
    for (i = 0; i < SIZEOF_ARRAY(hTask); i ++)
    {
        hTask[i] = INVALID_HANDLE_VALUE;
    }

    /* ������ͨ���ȼ��Ĳ������� */
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
        /* ���� 100 ms ��֤�´����������Ѿ��� MUTEX �������� */
        Sleep(100);
    }
    /* ������ǰ�������ȼ�Ϊʵʱ */
    Result = SetTaskSelfPriority(TASK_PRIORITY_REAL);

    TEST_CASE_ASSERT(STATE_SUCCESS == Result, return GetError(),
            "'%s' set self priority to realtime failed, result %d.", TaskName, Result);
    
    Priority = GetTaskSelfPriority();

    TEST_CASE_ASSERT(TASK_PRIORITY_REAL == Priority, return GetError(),
            "'%s' set self priority to realtime failed, priority is %d.", TaskName, Priority);

    /* �����͵��Ҳ��� */
    for (i = 0; i < SIZEOF_ARRAY(hTask); i ++)
    {
        TCHAR Name[OBJECT_NAME_MAX];
        
        TSNPRINTF(Name, OBJECT_NAME_MAX, "TASK%02u", i);
        /* ���� MUTEX ���󣬵�ǰ����Ϊʵʱ�����������񱻻��ѣ������������õ�ִ�� */
        Result = MutexUnlock(hMutex);
        
        TEST_CASE_ASSERT(STATE_SUCCESS == Result, 
                MUTEX_CASE01_CLEANUP(hTask, SIZEOF_ARRAY(hTask));
                CloseHandle(hMutex);
                SetTaskSelfPriority(TASK_PRIORITY_NORMAL);
                return Result,
                "Task '%s' lock mutex %s failed, result %d.",
                TaskName, g_TaskCase01MutexName, Result);

        /* ��ס MUTEX ���󣬵�ǰ����Ϊʵʱ�����´ν��� MUTEX ʱ����ǰ�������Ȼ��� */
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
        /* ��黽�������˳���Ƿ����Ҫ�� */
        TEST_CASE_ASSERT(0 == TSTRCMP(g_lpWakeupTaskName, Name),
                MUTEX_CASE01_CLEANUP(hTask, SIZEOF_ARRAY(hTask));
                CloseHandle(hMutex);
                SetTaskSelfPriority(TASK_PRIORITY_NORMAL);
                return STATE_SYSTEM_FAULT,
                "Task '%s' unlock mutex %s failed, wakeup task out of expetation.",
                TaskName, g_TaskCase01MutexName);
    }

    /* �ָ�ΪĬ�����ȼ� */
    Result = SetTaskSelfPriority(TASK_PRIORITY_NORMAL);

    TEST_CASE_ASSERT(STATE_SUCCESS == Result, return GetError(),
            "'%s' set self priority to realtime failed, result %d.", TaskName, Result);
    
    Priority = GetTaskSelfPriority();

    TEST_CASE_ASSERT(TASK_PRIORITY_NORMAL == Priority, return GetError(),
            "'%s' set self priority to realtime failed, priority is %d.", TaskName, Priority);

    Sleep(1000);

    /* �ر� MUTEX ���� */
    Result = CloseHandle(hMutex);
    
    TEST_CASE_ASSERT(STATE_SUCCESS == Result, 
            MUTEX_CASE01_CLEANUP(hTask, SIZEOF_ARRAY(hTask));
            SetTaskSelfPriority(TASK_PRIORITY_NORMAL);
            return Result,
            "Task '%s' close mutex %s failed, result %d.",
            TaskName, g_TaskCase01MutexName, Result);

    /* ����ɾ������һ��ʱ���ڣ������������������ȫ��������(���STATE_REMOVED�ķ���ֵ) */
    for (i = 0; i < 1000 && g_FinishedTaskCount < SIZEOF_ARRAY(hTask); i ++)
    {
        Sleep(10);
    }
    /* ��������Ƿ��Ѿ���ȫ������ */
    TEST_CASE_ASSERT(SIZEOF_ARRAY(hTask) == g_FinishedTaskCount, 
        MUTEX_CASE01_CLEANUP(hTask, SIZEOF_ARRAY(hTask));
        return STATE_SYSTEM_FAULT,
        "Task '%s' unlock mutex %s failed, invalid wakeup task name.",
        TaskName, g_TaskCase01MutexName, Result);
    
    MUTEX_CASE01_CLEANUP(hTask, SIZEOF_ARRAY(hTask));
    
    return STATE_SUCCESS;
}

/*
 * �������ͷš��ͷź�������ͷź����
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

