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

#if (defined(CONFIG_BUILD_LOCAL_STORE) && (TRUE == CONFIG_BUILD_LOCAL_STORE))
#include "testcode.h"

STATIC RO_USER_CODE E_STATUS LSOT_TEST_CASE00(VOID)
{
    DWORD Value;
    E_STATUS Result;
    
    Result = SetLsotValue(0, 0x5aa555aa);
    
    TEST_CASE_ASSERT(STATE_SUCCESS != Result, return STATE_SYSTEM_FAULT,
            "Set free key value test failed.");
    
    Result = SetLsotValue(CONFIG_DEFAULT_SLOT_KEYS, 0x5aa555aa);
    
    TEST_CASE_ASSERT(STATE_SUCCESS != Result, return STATE_SYSTEM_FAULT,
            "Set over range key value test failed.");

    Result = GetLsotValue(CONFIG_DEFAULT_SLOT_KEYS-1, &Value);
    
    TEST_CASE_ASSERT(STATE_SUCCESS != Result, return STATE_SYSTEM_FAULT,
            "Get free key value test failed.");
    
    Result = GetLsotValue(CONFIG_DEFAULT_SLOT_KEYS, &Value);
    
    TEST_CASE_ASSERT(STATE_SUCCESS != Result, return STATE_SYSTEM_FAULT,
            "Get over range key value test failed.");
    
    return STATE_SUCCESS;
}

STATIC RO_USER_CODE E_STATUS LSOT_TEST_CASE01(VOID)
{
    INT i = 0;
    E_STATUS Result;
    LSOT_KEY_T LsotKey;
    LSOT_KEY_T KeyArray[CONFIG_DEFAULT_SLOT_KEYS];
    
    for (; i < CONFIG_DEFAULT_SLOT_KEYS; i ++)
    {
        KeyArray[i] = GetLsotKey();

        TEST_CASE_ASSERT(TASK_LSOTKEY_INVALID != KeyArray[i], return STATE_OVER_RANGE,
            "Get key failed, index is %u.", i);
    }

    LsotKey = GetLsotKey();
    
    TEST_CASE_ASSERT(TASK_LSOTKEY_INVALID == LsotKey, return STATE_OVER_RANGE,
            "The last key %u failed.", i);

    for (i = CONFIG_DEFAULT_SLOT_KEYS - 1; i >=0; i --)
    {
        Result = PutLsotKey(KeyArray[i]);
        
        TEST_CASE_ASSERT(STATE_SUCCESS == Result, return Result,
            "Put key %u failed, result %d.", KeyArray[i], Result);
    }

    return STATE_SUCCESS;
}

STATIC RO_USER_CODE E_STATUS LSOT_TEST_CASE02(VOID)
{
    INT i = 0;
    DWORD Value;
    E_STATUS Result;
    LSOT_KEY_T KeyArray[CONFIG_DEFAULT_SLOT_KEYS];
    
    for (; i < CONFIG_DEFAULT_SLOT_KEYS; i ++)
    {
        KeyArray[i] = GetLsotKey();
        
        TEST_CASE_ASSERT(TASK_LSOTKEY_INVALID != KeyArray[i], return STATE_OVER_RANGE,
            "Get key failed, index is %u.", i);
    }

    for (i = 0; i < CONFIG_DEFAULT_SLOT_KEYS-1; i ++)
    {
        Result = PutLsotKey(KeyArray[i]);
        
        TEST_CASE_ASSERT(STATE_SUCCESS == Result, return Result,
            "Put key %u failed, result %d.", KeyArray[i], Result);
    }
    
    Result = SetLsotValue(KeyArray[CONFIG_DEFAULT_SLOT_KEYS-1], 0x5aa555aa);
    
    TEST_CASE_ASSERT(STATE_SUCCESS == Result, return Result,
            "Set value to key %u failed.", KeyArray[CONFIG_DEFAULT_SLOT_KEYS-1]);
    
    Result = GetLsotValue(KeyArray[CONFIG_DEFAULT_SLOT_KEYS-1], &Value);
    
    TEST_CASE_ASSERT(STATE_SUCCESS == Result, return Result,
            "Get value from key %u failed, result %d.", KeyArray[CONFIG_DEFAULT_SLOT_KEYS-1], Result);
    
    TEST_CASE_ASSERT(0x5aa555aa == Value, return STATE_NOT_MATCH,
            "Get value from key %u failed, value 0x%x.", KeyArray[CONFIG_DEFAULT_SLOT_KEYS-1], Value);
    
    PutLsotKey(KeyArray[CONFIG_DEFAULT_SLOT_KEYS-1]);
    
    return STATE_SUCCESS;
}


STATIC RO_USER_DATA CONST TEST_CASE_DESCRIPTOR g_TestCase[] =
{
    DEFINE_TEST_CASE(LSOT_TEST_CASE00),
    DEFINE_TEST_CASE(LSOT_TEST_CASE01),
    DEFINE_TEST_CASE(LSOT_TEST_CASE02),
};

PUBLIC RO_USER_CODE E_STATUS LSOT_TEST_CASE(VOID)
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
PUBLIC RO_USER_CODE E_STATUS LSOT_TEST_CASE(VOID)
{
    return STATE_SUCCESS;
}
#endif

