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
 * �����������ͷš����ƻ�ȡ���ͷź��ٲ����Զ���������
 */
STATIC RO_USER_CODE E_STATUS SEMAPHORE_TEST_CASE00(VOID)
{
    E_STATUS Result;
    TCHAR Name[OBJECT_NAME_MAX];
    LPCTSTR lpName = NAME_CASE00;
    HANDLE handle = INVALID_HANDLE_VALUE;
    HANDLE hFind = INVALID_HANDLE_VALUE;

    /* ����������� */
    handle = CreateSemaphore(lpName, 1, 100);
    
    TEST_CASE_ASSERT(INVALID_HANDLE_VALUE != handle, return GetError(),
            "Create semaphore %s failed.", lpName);
    
    /* ���ƻ�ȡ���� */
    Result = GetObjectName(handle, Name, OBJECT_NAME_MAX);
    
    TEST_CASE_ASSERT(STATE_SUCCESS == Result, return Result,
            "Get semaphore(0x%08X) name failed.", handle);
    
    TEST_CASE_ASSERT(0 == TSTRCMP(Name, lpName), return STATE_NOT_MATCH,
            "Object name not match !");

    /* ���󲶻���� */
    hFind = TakeObject(Name);
    
    TEST_CASE_ASSERT(INVALID_HANDLE_VALUE != hFind, return GetError(),
            "take semaphore %s failed.", Name);
            
    TEST_CASE_ASSERT(handle == hFind, return GetError(),
            "Take semaphore %s failed.", Name);

    /* �����ͷŲ��� */
    Result = CloseHandle(handle);

    TEST_CASE_ASSERT(STATE_SUCCESS == Result, return Result,
            "Close semaphore(0x%08X - '%s') failed.", handle, Name);
    /* �ͷź��ٲ������ */
    hFind = TakeObject(Name);
    
    TEST_CASE_ASSERT(INVALID_HANDLE_VALUE == hFind, return GetError(),
            "Semaphore %s close failed.", Name);

    /* �����Զ��������� */
    handle = CreateSemaphore(NULL, 1, 100);
    
    TEST_CASE_ASSERT(INVALID_HANDLE_VALUE != handle, return GetError(),
            "Create noname semaphore failed.");

    /* ���ƻ�ȡ���� */
    Result = GetObjectName(handle, Name, OBJECT_NAME_MAX);
    
    TEST_CASE_ASSERT(STATE_SUCCESS == Result, return Result,
            "Get semaphore(0x%08X) name failed.", handle);

    /* �����ͷŲ��� */
    Result = CloseHandle(handle);

    TEST_CASE_ASSERT(STATE_SUCCESS == Result, return Result,
            "Close semaphore(0x%08X - '%s') failed, result %d.", handle, Name, Result);
            
    return STATE_SUCCESS;
}

STATIC RO_USER_CODE E_STATUS TEST_CASE01_TASK(LPVOID lpParam)
{
    E_STATUS Result;
    TASK_PRIORITY Priority;
    DWORD TaskID = 0;
    TCHAR Name[OBJECT_NAME_MAX];
    SHORT SignalID = WAIT_SIGNAL_INVALID;
    HANDLE hSemaphore = TakeObject(lpParam);
    
    GetTaskSelfName(Name, OBJECT_NAME_MAX);

    LOG_INFOR(TRUE, "Task '%s' startup ...", Name);

    TEST_CASE_ASSERT(INVALID_HANDLE_VALUE != hSemaphore, return GetError(),
            "Take mutex %s failed.", lpParam);
    /* 1. ��������˳����ԣ��ȴ��ź� */
    SignalID = WaitObject(hSemaphore, WAIT_INFINITE);

    TEST_CASE_ASSERT(WAIT_SIGNAL_ID_0 == SignalID, return GetError(),
            "Task '%s' wait semaphore %s failed, id %d, error %d.",
            Name, lpParam, SignalID, GetError());
    
    g_lpWakeupTaskName = Name;

    /* 1. ���ݻ���˳��ȡ TASK ID */
    TaskID = g_FinishedTaskCount ++;

    /* 1. ����������: ������Ϊ������ȼ����������Ȼ��� */
    Result = PostSemaphore(hSemaphore, 1);
    
    TEST_CASE_ASSERT(STATE_SUCCESS == Result, return Result,
            "Task '%s' post semaphore %s failed, result %d.", Name, lpParam, Result);

    LOG_INFOR(TRUE, "Task '%s' wakeup finished, task id is %d, conter is %d.",
            Name, TaskID, g_FinishedTaskCount);
    
    /* 2. ���ȼ�����˳����ԣ��ȴ�������������ȫ������ */
    while(g_FinishedTaskCount < 4);

    Sleep(1000);

    /* 2. �� ID ��������������� */
    while (TaskID + 1 != g_FinishedTaskCount) Sleep(100);

    /* 2. ��������һ��ʹ��һ����������������� */
    g_FinishedTaskCount --;

    /* 2. ���� ID �������ȼ��� ID Խ����������ȼ�Խ�� */
    Result = SetTaskSelfPriority(TaskID + 1);

    TEST_CASE_ASSERT(STATE_SUCCESS == Result, return GetError(),
            "'%s' set self priority to %u failed, result %d.", Name, TaskID + 1, Result);
    
    Priority = GetTaskSelfPriority();

    TEST_CASE_ASSERT(TaskID + 1 == Priority, return GetError(),
            "'%s' set self priority to %u failed, priority is %d.", Name, TaskID + 1, Priority);
            
    LOG_INFOR(TRUE, "Task '%s' begin to priority test, conter is %d, priority is %u.",
        Name, g_FinishedTaskCount, Priority);

    /* 2. ��ǰ����ȴ��ź� */
    SignalID = WaitObject(hSemaphore, WAIT_INFINITE);

    g_lpWakeupTaskName = Name;

    TEST_CASE_ASSERT(WAIT_SIGNAL_ID_0 == SignalID, return GetError(),
            "Task '%s' wait semaphore %s failed, error %d.", Name, lpParam, GetError());
    /* 2. ���������� */
    Result = PostSemaphore(hSemaphore, 1);
    
    TEST_CASE_ASSERT(STATE_SUCCESS == Result, return Result,
            "Task '%s' post semaphore %s failed, result %d.", Name, lpParam, Result);
            
    g_FinishedTaskCount ++;
    
    LOG_INFOR(TRUE, "******Task '%s' priority wakeup finished, conter is %d.",
        Name, g_FinishedTaskCount);
        
    /* 3.�ָ�Ĭ�����ȼ� */
    Result = SetTaskSelfPriority(TASK_PRIORITY_NORMAL);

    TEST_CASE_ASSERT(STATE_SUCCESS == Result, return GetError(),
            "'%s' set self priority to normal failed, result %d.", Name, Result);
            
    Priority = GetTaskSelfPriority();

    TEST_CASE_ASSERT(TASK_PRIORITY_NORMAL == Priority, return GetError(),
            "'%s' set self priority to normal failed, priority is %d.", Name, Priority);
    /* 3. �ȴ�������������ȫ������ */
    while(g_FinishedTaskCount < 4) Sleep(1);

    /* 3. �ȴ��ź� */
    SignalID = WaitObject(hSemaphore, WAIT_INFINITE);

    TEST_CASE_ASSERT(WAIT_SIGNAL_INVALID == SignalID, return GetError(),
            "Task '%s' wait semaphore %s failed, error %d.", Name, lpParam, GetError());

    g_FinishedTaskCount --;
    
    LOG_INFOR(TRUE, "Task '%s' test finished, conter is %d.", Name, g_FinishedTaskCount);

    return STATE_SUCCESS;
}


STATIC RO_USER_CODE E_STATUS SEMAPHORE_CASE01_POST(HANDLE hSemaphore, LPHANDLE hTask, DWORD Count)
{
    DWORD i = 0;
    SHORT SignalID = WAIT_SIGNAL_INVALID;
    E_STATUS Result = STATE_SUCCESS;
    TCHAR TaskName[OBJECT_NAME_MAX];

    GetTaskSelfName(TaskName, OBJECT_NAME_MAX);

    for (i = 0; i < Count; i ++)
    {
        TCHAR Name[OBJECT_NAME_MAX];
        
        TSNPRINTF(Name, OBJECT_NAME_MAX, "TASK%02u", i);

        Result = PostSemaphore(hSemaphore, 1);
        
        TEST_CASE_ASSERT(STATE_SUCCESS == Result, 
                return Result,
                "Task '%s' post semaphore %s failed, result %d.",
                TaskName, g_TaskCase01Name, Result);
        
        SignalID = WaitObject(hSemaphore, 1000);
        
        TEST_CASE_ASSERT(WAIT_SIGNAL_ID_0 == SignalID, 
                return STATE_SYSTEM_FAULT,
                "Task '%s' wait semaphore %s failed, error %d.",
                TaskName, g_TaskCase01Name, GetError());

        TEST_CASE_ASSERT(NULL != g_lpWakeupTaskName, 
                return STATE_SYSTEM_FAULT,
                "Task '%s' post semaphore %s failed, invalid wakeup task name.",
                TaskName, g_TaskCase01Name);

        TEST_CASE_ASSERT(0 == TSTRCMP(g_lpWakeupTaskName, Name),
                return STATE_SYSTEM_FAULT,
                "Task '%s' post semaphore %s failed, wakeup task '%s' out of expetation.",
                TaskName, g_TaskCase01Name, Name);
    }
    
    return STATE_SUCCESS;
}

STATIC RO_USER_CODE E_STATUS SEMAPHORE_TEST_CASE01(VOID)
{
    DWORD i;
    E_STATUS Result;
    HANDLE hSemaphore;
    TASK_PRIORITY Priority;
    HANDLE hTask[MAX_TEST_TASK];
    TCHAR TaskName[OBJECT_NAME_MAX];

    GetTaskSelfName(TaskName, OBJECT_NAME_MAX);
    
    g_FinishedTaskCount = 0;

    /* ����һ������ס�� MUTEX ����*/
    hSemaphore = CreateSemaphore(g_TaskCase01Name, 0, 127);

    TEST_CASE_ASSERT(INVALID_HANDLE_VALUE != hSemaphore, return GetError(),
            "Create semaphore %s failed.", g_TaskCase01Name);
    
    Result = TEST_TASK_CREATE(hTask, SIZEOF_ARRAY(hTask), TEST_CASE01_TASK, (void*)g_TaskCase01Name);
    
    TEST_CASE_ASSERT(STATE_SUCCESS == Result,
            TEST_TASK_CLEANUP(hTask, SIZEOF_ARRAY(hTask));
            CloseHandle(hSemaphore);
            return Result,
            "Create test task failed !");

    /* ������ǰ�������ȼ�Ϊʵʱ */
    Result = SetTaskSelfPriority(TASK_PRIORITY_REAL);

    TEST_CASE_ASSERT(STATE_SUCCESS == Result, return GetError(),
            "'%s' set self priority to realtime failed, result %d.", TaskName, Result);
    
    Priority = GetTaskSelfPriority();

    TEST_CASE_ASSERT(TASK_PRIORITY_REAL == Priority, return GetError(),
            "'%s' set self priority to realtime failed, priority is %d.", TaskName, Priority);
    
    Result = SEMAPHORE_CASE01_POST(hSemaphore, hTask, SIZEOF_ARRAY(hTask));
    
    TEST_CASE_ASSERT(STATE_SUCCESS == Result, 
                TEST_TASK_CLEANUP(hTask, SIZEOF_ARRAY(hTask));
                CloseHandle(hSemaphore);
                return Result,
                "Task '%s' test semaphore %s failed, result %d.",
                TaskName, g_TaskCase01Name, Result);

    while(g_FinishedTaskCount) Sleep(1);

    LOG_INFOR(TRUE, "Semaphore highest test ...");

    Result = SEMAPHORE_CASE01_POST(hSemaphore, hTask, SIZEOF_ARRAY(hTask));
    
    TEST_CASE_ASSERT(STATE_SUCCESS == Result, 
                TEST_TASK_CLEANUP(hTask, SIZEOF_ARRAY(hTask));
                CloseHandle(hSemaphore);
                return Result,
                "Task '%s' test semaphore %s failed, result %d.",
                TaskName, g_TaskCase01Name, Result);
                
    Sleep(1000);

        /* �ر� MUTEX ���� */
    Result = CloseHandle(hSemaphore);
    
    TEST_CASE_ASSERT(STATE_SUCCESS == Result, 
            TEST_TASK_CLEANUP(hTask, SIZEOF_ARRAY(hTask));
            SetTaskSelfPriority(TASK_PRIORITY_NORMAL);
            return Result,
            "Task '%s' close mutex %s failed, result %d.",
            TaskName, g_TaskCase01Name, Result);

    /* ����ɾ������һ��ʱ���ڣ������������������ȫ��������(���STATE_REMOVED�ķ���ֵ) */
    for (i = 0; i < 1000 && 0 != g_FinishedTaskCount; i ++)
    {
        Sleep(10);
    }
    /* ��������Ƿ��Ѿ���ȫ������ */
    TEST_CASE_ASSERT(0 == g_FinishedTaskCount, 
        TEST_TASK_CLEANUP(hTask, SIZEOF_ARRAY(hTask));
        return STATE_SYSTEM_FAULT,
        "Task '%s' unlock mutex %s failed, invalid conter %d.",
        TaskName, g_TaskCase01Name, g_FinishedTaskCount);
    
    TEST_TASK_CLEANUP(hTask, SIZEOF_ARRAY(hTask));
    
    /* ������ǰ�������ȼ�ΪĬ�� */
    Result = SetTaskSelfPriority(TASK_PRIORITY_NORMAL);

    TEST_CASE_ASSERT(STATE_SUCCESS == Result, return GetError(),
            "'%s' set self priority to normal failed, result %d.", TaskName, Result);
    
    Priority = GetTaskSelfPriority();

    TEST_CASE_ASSERT(TASK_PRIORITY_NORMAL == Priority, return GetError(),
            "'%s' set self priority to normal failed, priority is %d.", TaskName, Priority);

    return STATE_SUCCESS;
}

STATIC RO_USER_CODE E_STATUS SEMAPHORE_TEST_CASE02(VOID)
{
    return STATE_SUCCESS;
}


STATIC RO_USER_DATA CONST TEST_CASE_DESCRIPTOR g_TestCase[] =
{
    DEFINE_TEST_CASE(SEMAPHORE_TEST_CASE00),
//    DEFINE_TEST_CASE(SEMAPHORE_TEST_CASE01),
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

