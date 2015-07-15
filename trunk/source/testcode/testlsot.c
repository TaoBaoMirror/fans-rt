#include "stdio.h"
#include "string.h"
#include <fauser.h>
#include <fadefs.h>
#include <faerror.h>
#include <fatypes.h>
#include <fapi.h>

#include "testcode.h"

STATIC RO_CODE E_STATUS TestCase00(VOID)
{
    LSOT_KEY_T LsotKey = GetLsotKey();

    LOG_INFOR(TRUE, "LsotKey = %u", LsotKey);

    return STATE_SUCCESS;
}

PUBLIC RO_CODE E_STATUS LSOT_TEST_CASE(VOID)
{
    E_STATUS Result = STATE_SUCCESS;
    
    TEST_CASE_SET_RESULT(Result, TestCase00,);

    return Result;
}

