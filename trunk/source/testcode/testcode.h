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

#ifdef __cplusplus
extern "C" {
#endif
    
#ifdef __cplusplus
}
#endif  

#endif
