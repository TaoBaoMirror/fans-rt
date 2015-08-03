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
STATIC RO_USER_CODE E_STATUS SEMAPHORE_TEST_CASE00(VOID)
{
    E_STATUS Result;
    TCHAR Name[OBJECT_NAME_MAX];
    LPCTSTR lpName = NAME_CASE00;
    HANDLE handle = INVALID_HANDLE_VALUE;
    HANDLE hFind = INVALID_HANDLE_VALUE;

    /* 创建对象测试 */
    handle = CreateSemaphore(lpName, 1, 100);
    
    TEST_CASE_ASSERT(INVALID_HANDLE_VALUE != handle, return GetError(),
            "Create semaphore %s failed.", lpName);
    
    /* 名称获取测试 */
    Result = GetObjectName(handle, Name, OBJECT_NAME_MAX);
    
    TEST_CASE_ASSERT(STATE_SUCCESS == Result, return Result,
            "Get semaphore(0x%08X) name failed.", handle);
    
    TEST_CASE_ASSERT(0 == TSTRCMP(Name, lpName), return STATE_NOT_MATCH,
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

STATIC RO_USER_CODE E_STATUS SEMAPHORE_TEST_CASE01(VOID)
{
    return STATE_SUCCESS;
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

