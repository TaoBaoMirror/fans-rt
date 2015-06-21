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

STATIC CODE_TEXT E_STATUS FansMain(LPMODULE_HEADER CONST lpModule)
{
    TEST_CASE_CALL(BASE_TEST_CASE,);
    TEST_CASE_CALL(TASK_TEST_CASE,);
    return STATE_SUCCESS;
}

DEFINE_MODULE(Application, NULL);

