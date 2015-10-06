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
#ifndef __LIBCMINI_H
#define __LIBCMINI_H

#include <stddef.h>
#include <stdarg.h>
#include <fauser.h>
#include <fadefs.h>
#include <fatypes.h>

#ifdef __cplusplus
extern "C" {
#endif
    typedef int (FAR * FNPUTCHAR)(INT Ch, LPVOID lpPrivate);
    extern int fa_wvxnprintf(FNPUTCHAR fnPut, LPVOID lpPrivate, size_t size, const WCHAR * format, va_list args);
    extern int fa_wxnprintf(FNPUTCHAR fnPut, LPVOID lpPrivate, size_t size, const WCHAR * format, ...);
    extern int fa_vxnprintf(FNPUTCHAR fnPut, LPVOID lpPrivate, size_t size, const char * format, va_list args); 
    extern int fa_xnprintf(FNPUTCHAR fnPut, LPVOID lpPrivate, size_t size, const char * format, ...);
#ifdef __cplusplus
}
#endif
#endif

