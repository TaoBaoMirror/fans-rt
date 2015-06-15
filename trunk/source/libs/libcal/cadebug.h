/*
 *    Copyright(C) 2013-2015, Fans-rt development team.
 *
 *    All rights reserved.
 *
 *    This is open source software.
 *    Learning and research can be unrestricted to modification, use and dissemination.
 *    If you need for commercial purposes, you should get the author's permission.
 *
 *    date           author          notes
 *    2014-09-07     JiangYong       new file
 */
#ifndef __NT_DEBUG_H
#define __NT_DEBUG_H

#include <fadefs.h>
#include <fatypes.h>
#include <faerror.h>
#include <fadebug.h>

#define     DEBUG_LOG_CONDITION(lv)         ((ntDebugGetMask() & (1 << (lv))) != 0)
#define     DEBUG_LOG_LEVEL(lv)             ntDebugLevel2String(lv)

#if (defined(__DEBUG__) && (DEBUG_LOG_MASK & LOG_ERROR_MASK))
#define     LOG_ERROR(Enter, ...)           ntDebugLog(Enter, __LINE__, __FUNCTION__, LOG_LEVEL_ERROR, __VA_ARGS__)
#else
#define     LOG_ERROR(Enter, ...)
#endif

#if (defined(__DEBUG__) && (DEBUG_LOG_MASK & LOG_INFOR_MASK))
#define     LOG_INFOR(Enter,...)            ntDebugLog(Enter, __LINE__, __FUNCTION__, LOG_LEVEL_INFOR, __VA_ARGS__)
#else
#define     LOG_INFOR(Enter,...)
#endif

#if (defined(__DEBUG__) && (DEBUG_LOG_MASK & LOG_WARN_MASK) )
#define     LOG_WARN(Enter,...)             ntDebugLog(Enter, __LINE__, __FUNCTION__, LOG_LEVEL_WARN, __VA_ARGS__)
#else
#define     LOG_WARN(Enter,...)
#endif

#if (defined(__DEBUG__) && (DEBUG_LOG_MASK & LOG_LEVEL_BUG))
#define     LOG_BUG(Enter,...)              ntDebugLog(Enter, __LINE__, __FUNCTION__, LOG_LEVEL_BUG, __VA_ARGS__)
#else
#define     LOG_BUG(Enter,...)
#endif

#if (defined(__DEBUG__) && (DEBUG_LOG_MASK & LOG_LEVEL_FIX))
#define     LOG_FIX(Enter,...)              ntDebugLog(Enter, __LINE__, __FUNCTION__, LOG_LEVEL_FIX, __VA_ARGS__)
#else
#define     LOG_FIX(Enter,...)
#endif

#if (defined(__DEBUG__) && (DEBUG_LOG_MASK & LOG_LEVEL_TRACE))
#define     LOG_TRACE(Enter,...)            ntDebugLog(Enter, __LINE__, __FUNCTION__, LOG_LEVEL_TRACE, __VA_ARGS__)
#else
#define     LOG_TRACE(Enter,...)
#endif

#if (defined(__DEBUG__) && (DEBUG_LOG_MASK & LOG_LEVEL_DEBUG))
#define     LOG_DEBUG(Enter,...)            ntDebugLog(Enter, __LINE__, __FUNCTION__, LOG_LEVEL_DEBUG, __VA_ARGS__)
#else
#define     LOG_DEBUG(Enter,...)
#endif

#if (defined(__DEBUG__) && (DEBUG_LOG_MASK & LOG_LEVEL_FATAL))
#define     LOG_FATAL(Enter, ...)           do{ ntDebugLog(Enter, __LINE__, __FUNCTION__, LOG_LEVEL_FATAL, __VA_ARGS__); SYSTEM_CALL_OOPS(); } while(0)
#else
#define     LOG_FATAL(Enter, ...)           SYSTEM_CALL_OOPS();
#endif

#ifdef __DEBUG__

#define     FANS_ASSERT(Condition, Execute, ...)                        \
            do{                                                         \
                if (!(Condition))                                       \
                {                                                       \
                    LOG_ERROR(TRUE, __VA_ARGS__);                       \
                    Execute;                                            \
                }                                                       \
            }while(0)

#endif /* __DEBUG__ */

#ifdef __cplusplus
extern "C" {
#endif  
    EXPORT DWORD ntDebugGetMask(VOID);
    EXPORT LPSTR ntDebugLevel2String(E_LOG_LEVEL emLevel);
    EXPORT VOID ntDebugShowData(E_LOG_LEVEL emLevel, LPVOID lpBuffer, SIZE_T Length);
    EXPORT int ntDebugPrintf(BOOL Enter, CONST CHAR * Format,...);
    EXPORT int ntDebugLog(BOOL Enter, INT Line, CONST CHAR * Function, E_LOG_LEVEL emLevel, CONST CHAR * Format,...);
#ifdef __cplusplus
}
#endif  
#endif

