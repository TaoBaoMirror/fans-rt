#include "stdio.h"
#include "string.h"
#include <fauser.h>
#include <fadefs.h>
#include <faerror.h>
#include <fatypes.h>
#include <fapi.h>
#include <famodule.h>

#include "testcode.h"

PUBLIC E_STATUS BASE_TEST_CASE(VOID);
PUBLIC E_STATUS TASK_TEST_CASE(VOID);

STATIC CONST TEST_CASE_DESCRIPTOR g_TestCase[] =
{
    DEFINE_TEST_CASE(BASE_TEST_CASE),
    DEFINE_TEST_CASE(TASK_TEST_CASE),
};

STATIC CODE_TEXT E_STATUS FansMain(LPMODULE_HEADER CONST lpModule)
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
            LOG_ERROR(TRUE, "%s .... [SUCCESS]", g_TestCase[i].lpTestName);
        }
    }
    
    return STATE_SUCCESS;
}

DEFINE_MODULE(Application, NULL);

