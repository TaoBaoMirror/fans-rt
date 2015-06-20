#include "stdio.h"
#include "string.h"
#include <fauser.h>
#include <fadefs.h>
#include <faerror.h>
#include <fatypes.h>
#include <fapi.h>
#include <famodule.h>

#include "testcode.h"

PUBLIC E_STATUS BASE_TESTCASE(VOID);
PUBLIC E_STATUS TASK_TESTCASE(VOID);

STATIC CODE_TEXT E_STATUS FansMain(LPMODULE_HEADER CONST lpModule)
{
    TEST_CASE_CALL(BASE_TESTCASE,);
    TEST_CASE_CALL(TASK_TESTCASE,);
    return STATE_SUCCESS;
}

DEFINE_MODULE(Application, NULL);

