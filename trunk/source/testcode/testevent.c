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


#if (defined(CONFIG_BUILD_IPC_EVENT) && (TRUE == CONFIG_BUILD_IPC_EVENT))
#include "testcode.h"

STATIC RO_USER_CODE E_STATUS EVENT_TEST_CASE00(VOID)
{   
    return STATE_SUCCESS;
}

STATIC RO_USER_CODE E_STATUS EVENT_TEST_CASE01(VOID)
{
    return STATE_SUCCESS;
}

STATIC RO_USER_CODE E_STATUS EVENT_TEST_CASE02(VOID)
{
    return STATE_SUCCESS;
}


STATIC RO_USER_DATA CONST TEST_CASE_DESCRIPTOR g_TestCase[] =
{
    DEFINE_TEST_CASE(EVENT_TEST_CASE00),
    DEFINE_TEST_CASE(EVENT_TEST_CASE01),
    DEFINE_TEST_CASE(EVENT_TEST_CASE02),
};

PUBLIC RO_USER_CODE E_STATUS EVENT_TEST_CASE(VOID)
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
PUBLIC RO_USER_CODE E_STATUS EVENT_TEST_CASE(VOID)
{
    return STATE_SUCCESS;
}
#endif

