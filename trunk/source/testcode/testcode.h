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

#ifdef _UNICODE
#define     TSNPRINTF           wsnprintf
#define     TSTRCMP(s1, s2)     wcscmp(s1, s2)
#else
#define     TSNPRINTF           snprintf
#define     TSTRCMP(s1, s2)     strcmp(s1, s2)
#endif

#define     NAME_CASE00         _TEXT("CASE00")
#define     NAME_CASE01         _TEXT("CASE01")
#define     MAX_TEST_TASK       4

EXTERN CONST TCHAR     g_TaskCase01Name[];
EXTERN VOLATILE LPTSTR g_lpWakeupTaskName;
EXTERN VOLATILE DWORD  g_FinishedTaskCount;

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
    E_STATUS TEST_TASK_CREATE(HANDLE * hTask, DWORD Count, FNTASKMAIN fnMain, LPVOID lpParam);
    VOID TEST_TASK_CLEANUP(HANDLE * hTask, DWORD Count);
#ifdef __cplusplus
}
#endif  

#endif
