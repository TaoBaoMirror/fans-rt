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

#define NAME_CASE00     _TEXT("CASE00")
#ifdef _UNICODE
#define     TSTRCMP(s1, s2)     wcscmp(s1, s2)
#else
#define     TSTRCMP(s1, s2)     strcmp(s1, s2)
#endif

STATIC RO_USER_CODE E_STATUS SEMAPHORE_TEST_CASE00(VOID)
{
    E_STATUS Result;
    TCHAR Name[OBJECT_NAME_MAX];
    LPCTSTR lpName = NAME_CASE00;
    HANDLE handle = INVALID_HANDLE_VALUE;
    HANDLE hFind = INVALID_HANDLE_VALUE;

    handle = CreateSemaphore(lpName, 1, 100);
    
    TEST_CASE_ASSERT(INVALID_HANDLE_VALUE != handle, return GetError(),
            "Create semaphore %s failed.", lpName);
    
    Result = GetObjectName(handle, Name, OBJECT_NAME_MAX);
    
    TEST_CASE_ASSERT(STATE_SUCCESS == Result, return Result,
            "Get semaphore(0x%08X) name failed.", handle);
    
    TEST_CASE_ASSERT(0 == TSTRCMP(Name, lpName), return STATE_NOT_MATCH,
            "Object name not match !");

    hFind = TakeObject(Name);
    
    TEST_CASE_ASSERT(INVALID_HANDLE_VALUE != hFind, return GetError(),
            "take semaphore %s failed.", Name);
    
    Result = CloseHandle(handle);

    TEST_CASE_ASSERT(STATE_SUCCESS == Result, return Result,
            "Close semaphore(0x%08X - '%s') failed.", handle, Name);
            
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

