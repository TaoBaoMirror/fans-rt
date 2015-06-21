#ifndef __TESTCODE_H
#define __TESTCODE_H

#include <fadefs.h>
#include <faerror.h>
#include <fatypes.h>
#include <fapi.h>

#include "cadebug.h"

#define     TEST_CASE_CALL(Func, Execute)                                                   \
            do{                                                                             \
                if (STATE_SUCCESS != Func())                                                \
                {                                                                           \
                    LOG_ERROR(TRUE, "Run '%s' ...... [FAILED]", #Func);                     \
                    Execute;                                                                \
                }                                                                           \
                else                                                                        \
                {                                                                           \
                    LOG_INFOR(TRUE, "Run '%s' ...... [SUCCESS]", #Func);                    \
                }                                                                           \
            }while(0)
            
#define     TEST_CASE_SET_RESULT(Result, Func, Execute)                                     \
            do{                                                                             \
                E_STATUS State = Func();                                                    \
                Result = Result != STATE_SUCCESS ? Result : State;                          \
                if (STATE_SUCCESS != State)                                                 \
                {                                                                           \
                    LOG_ERROR(TRUE, "Run '%s' ...... [FAILED]", #Func);                     \
                    Execute;                                                                \
                }                                                                           \
                else                                                                        \
                {                                                                           \
                    LOG_INFOR(TRUE, "Run '%s' ...... [SUCCESS]", #Func);                    \
                }                                                                           \
            }while(0)

#define     TEST_CASE_ASSERT(Condition, Execute, ...)                                       \
            do{                                                                             \
                if (!(Condition))                                                           \
                {                                                                           \
                    LOG_ERROR(TRUE, __VA_ARGS__);                                           \
                    Execute;                                                                \
                }                                                                           \
            }while(0)

typedef E_STATUS (*TEST_CASE_FUNCTION)(VOID);
            
typedef struct tagTEST_CASE_DESCRIPTOR{
    TEST_CASE_FUNCTION          fnTestCase;
    LPCSTR                      lpTestName;
}TEST_CASE_DESCRIPTOR, * PTEST_CASE_DESCRIPTOR, FAR * LPTEST_CASE_DESCRIPTOR;

#define     DEFINE_TEST_CASE(fn)                                                       \
            {fn,    #fn}
#ifdef __cplusplus
extern "C" {
#endif
    
#ifdef __cplusplus
}
#endif  

#endif
