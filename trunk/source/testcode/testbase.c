#include "stdio.h"
#include "string.h"
#include <fauser.h>
#include <fadefs.h>
#include <faerror.h>
#include <fatypes.h>
#include <fapi.h>

#include "testcode.h"

STATIC CODE_TEXT E_STATUS TestCase00(VOID)
{
    
    signed int int_min = INT_MIN;
    signed int int_max = INT_MAX;
    signed short short_min = SHRT_MIN;
    signed short short_max = SHRT_MAX;
    
    TEST_CASE_ASSERT(int_max == INT_MAX, return STATE_NOT_MATCH,
        "The int_max %d not equal %d", int_max, INT_MAX);
    TEST_CASE_ASSERT(int_min == INT_MIN, return STATE_NOT_MATCH,
        "The int_min %d not equal %d", int_min, INT_MIN);
    TEST_CASE_ASSERT(short_min == SHRT_MIN, return STATE_NOT_MATCH,
        "The short_min %d not equal %d", short_min, SHRT_MIN);
    TEST_CASE_ASSERT(short_max == SHRT_MAX, return STATE_NOT_MATCH,
        "The short_max %d not equal %d", short_max, SHRT_MAX);
    
    return STATE_SUCCESS;
}

PUBLIC CODE_TEXT E_STATUS BASE_TEST_CASE(VOID)
{
    E_STATUS Result = STATE_SUCCESS;
    
    TEST_CASE_SET_RESULT(Result, TestCase00,);

    return Result;
}
