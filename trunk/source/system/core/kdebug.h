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
 *    2014-09-07     JiangYong       new file
 */
#ifndef __K_DEBUG_H
#define __K_DEBUG_H

#include <stdarg.h>
#include <fadefs.h>
#include <fatypes.h>
#include <faerror.h>
#include <fadebug.h>

#include "karch.h"

#define     DEBUG_LOG_CONDITION(lv)         ((kDebugGetMask() & (1 << (lv))) != 0)
#define     DEBUG_LOG_LEVEL(lv)             kDebugLevel2String(lv)

#if (defined(__DEBUG__) && (DEBUG_LOG_MASK & LOG_ERROR_MASK))
#define     CORE_ERROR(Enter, ...)          kDebugLog(Enter, __LINE__, __FUNCTION__, LOG_LEVEL_ERROR, __VA_ARGS__)
#else
#define     CORE_ERROR(Enter, ...)
#endif

#if (defined(__DEBUG__) && (DEBUG_LOG_MASK & LOG_INFOR_MASK))
#define     CORE_INFOR(Enter,...)           kDebugLog(Enter, __LINE__, __FUNCTION__, LOG_LEVEL_INFOR, __VA_ARGS__)
#else
#define     CORE_INFOR(Enter,...)
#endif

#if (defined(__DEBUG__) && (DEBUG_LOG_MASK & LOG_WARN_MASK) )
#define     CORE_WARN(Enter,...)            kDebugLog(Enter, __LINE__, __FUNCTION__, LOG_LEVEL_WARN, __VA_ARGS__)
#else
#define     CORE_WARN(Enter,...)
#endif

#if (defined(__DEBUG__) && (DEBUG_LOG_MASK & LOG_LEVEL_BUG))
#define     CORE_BUG(Enter,...)             kDebugLog(Enter, __LINE__, __FUNCTION__, LOG_LEVEL_BUG, __VA_ARGS__)
#else
#define     CORE_BUG(Enter,...)
#endif

#if (defined(__DEBUG__) && (DEBUG_LOG_MASK & LOG_LEVEL_FIX))
#define     CORE_FIX(Enter,...)             kDebugLog(Enter, __LINE__, __FUNCTION__, LOG_LEVEL_FIX, __VA_ARGS__)
#else
#define     CORE_FIX(Enter,...)
#endif

#if (defined(__DEBUG__) && (DEBUG_LOG_MASK & LOG_LEVEL_TRACE))
#define     CORE_TRACE(Enter,...)           kDebugLog(Enter, __LINE__, __FUNCTION__, LOG_LEVEL_TRACE, __VA_ARGS__)
#else
#define     CORE_TRACE(Enter,...)
#endif

#if (defined(__DEBUG__) && (DEBUG_LOG_MASK & LOG_LEVEL_DEBUG))
#define     CORE_DEBUG(Enter,...)           kDebugLog(Enter, __LINE__, __FUNCTION__, LOG_LEVEL_DEBUG, __VA_ARGS__)
#else
#define     CORE_DEBUG(Enter,...)
#endif

#if (defined(__DEBUG__) && (DEBUG_LOG_MASK & LOG_LEVEL_FATAL))
#define     CORE_FATAL(Enter, ...)          do{ kDebugLog(Enter, __LINE__, __FUNCTION__, LOG_LEVEL_FATAL, __VA_ARGS__); SYSTEM_CALL_OOPS(); } while(0)
#else
#define     CORE_FATAL(Enter, ...)          SYSTEM_CALL_OOPS();
#endif

#ifdef __DEBUG__
#define     CORE_ASSERT(Condition, Execute, ...)                        \
            do{                                                         \
                if (!(Condition))                                       \
                {                                                       \
                    CORE_ERROR(TRUE, __VA_ARGS__);                      \
                    Execute;                                            \
                }                                                       \
            }while(0)
#else  /* __DEBUG__ */
#define     CORE_ASSERT(Condition, Execute, ...)                        \
            do{                                                         \
                if (!(Condition));                                      \
            }while(0)
#endif /* __DEBUG__ */
            
#ifdef __DEBUG__
#define     DEBUG_SHOW_BUFFER(emLevel, lpBuffer, Length)                \
            kDebugShowData(emLevel, lpBuffer, Length)
#else
#define     DEBUG_SHOW_BUFFER(emLevel, lpBuffer, Length)
#endif

#ifdef __cplusplus
extern "C" {
#endif  
    EXPORT DWORD kDebugGetMask(VOID);
    EXPORT BOOL kDebugGetState(VOID);
#ifdef DEBUG_IRQ_MONITOR
    EXPORT VOID kDebugMonitorIRQ(VOID);
#else
    STATIC INLINE VOID kDebugMonitorIRQ(VOID) {}
#endif
    PUBLIC VOID kDebugShowStack(LPIRQ_REGISTER_STACK lpStack, BOOL IsStack);

    EXPORT VOID kDebugEnableLevel(E_LOG_LEVEL emLevel);
    EXPORT VOID kDebugDisableLevel(E_LOG_LEVEL emLevel);
    EXPORT LPSTR kDebugLevel2String(E_LOG_LEVEL emLevel);
    EXPORT VOID kDebugShowData(E_LOG_LEVEL emLevel, LPVOID lpBuffer, SIZE_T Length);
    EXPORT INT kDebugPrintf(BOOL Enter, CONST CHAR * Format,...);
    EXPORT INT kDebugVPrintf(BOOL Enter, CONST CHAR * Format, va_list Vargs);
    EXPORT INT kDebugLog(BOOL Enter, INT Line, CONST CHAR * Function, E_LOG_LEVEL emLevel, CONST CHAR * Format,...);
#ifdef __cplusplus
}
#endif  
#endif

