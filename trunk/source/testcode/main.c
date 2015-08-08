#include "stdio.h"
#include "string.h"
#include <fauser.h>
#include <fadefs.h>
#include <faerror.h>
#include <fatypes.h>
#include <fapi.h>
#include <famodule.h>

#include "testcode.h"

PUBLIC E_STATUS LSOT_TEST_CASE(VOID);
PUBLIC E_STATUS BASE_TEST_CASE(VOID);
PUBLIC E_STATUS TASK_TEST_CASE(VOID);
PUBLIC E_STATUS EVENT_TEST_CASE(VOID);
PUBLIC E_STATUS MUTEX_TEST_CASE(VOID);
PUBLIC E_STATUS SEMAPHORE_TEST_CASE(VOID);
PUBLIC E_STATUS SEMSET_TEST_CASE(VOID);

STATIC RO_USER_DATA CONST TEST_CASE_DESCRIPTOR g_TestCase[] =
{
    DEFINE_TEST_CASE(BASE_TEST_CASE),
    DEFINE_TEST_CASE(TASK_TEST_CASE),
    DEFINE_TEST_CASE(LSOT_TEST_CASE),
    DEFINE_TEST_CASE(EVENT_TEST_CASE),
    DEFINE_TEST_CASE(MUTEX_TEST_CASE),
    DEFINE_TEST_CASE(SEMAPHORE_TEST_CASE),
    DEFINE_TEST_CASE(SEMSET_TEST_CASE),
};

RO_USER_DATA CONST TCHAR     g_TaskCase01Name[]  = {NAME_CASE01};
RW_USER_DATA VOLATILE LPTSTR g_lpWakeupTaskName  = NULL;
RW_USER_DATA VOLATILE DWORD  g_FinishedTaskCount = 0;


RO_USER_CODE VOID TEST_TASK_CLEANUP(HANDLE * hTask, DWORD Count)
{
    DWORD j;
    
    for (j = 0; j < Count; j ++)
    {
        if (INVALID_HANDLE_VALUE != hTask[j]) KillTask(hTask[j]);
    }
}

RO_USER_CODE E_STATUS TEST_TASK_CREATE(HANDLE * hTask, DWORD Count, FNTASKMAIN fnMain, LPVOID lpParam)
{
    DWORD i = 0;

    for (i = 0; i < Count; i ++)
    {
        hTask[i] = INVALID_HANDLE_VALUE;
    }

    /* 创建普通优先级的测试任务 */
    for (i = 0; i < Count; i ++)
    {
        TCHAR Name[OBJECT_NAME_MAX];

        TSNPRINTF(Name, OBJECT_NAME_MAX, "TASK%02u", i);
        hTask[i] = CreateTask(Name, fnMain, (void *) lpParam);
        
        TEST_CASE_ASSERT(INVALID_HANDLE_VALUE != hTask[i],
            return STATE_SYSTEM_FAULT,
            "Create task %s failed !", Name);
        /* 休眠 100 ms 保证新创建的任务已经被对象阻塞 */
        Sleep(100);
    }
    
    return STATE_SUCCESS;
}

STATIC RO_USER_CODE E_STATUS FansMain(LPMODULE_HEADER CONST lpModule)
{
    LONG i = 0;

    for (i = 0; i < SIZEOF_ARRAY(g_TestCase); i ++)
    {
        if (STATE_SUCCESS != g_TestCase[i].fnTestCase())
        {
            LOG_ERROR(TRUE, "%s .... [FAILED]", g_TestCase[i].lpTestName);
        }
        else
        {
            LOG_INFOR(TRUE, "%s .... [SUCCESS]", g_TestCase[i].lpTestName);
        }
    }

    LOG_INFOR(TRUE, "The all testcase finished !");
    
    return STATE_SUCCESS;
}

DEFINE_MODULE(Application, NULL);

