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
#ifndef __STDIO_H
#define __STDIO_H
#include <stddef.h>
#include <stdarg.h>
#include <fatypes.h>

#define EOF (-1)

typedef struct __FILE FILE;

extern FILE * stdin;
extern FILE * stdout;
extern FILE * stderr;

#ifdef __CC_ARM
#define printf          __c_libc_printf
#define vprintf         __c_libc_vprintf
#define fprintf         __c_libc_fprintf
#define vfprintf        __c_libc_vfprintf
#define vfnprintf       __c_libc_vfnprintf
#define vsprintf        __c_libc_vsprintf
#define sprintf         __c_libc_sprintf
#define snprintf        __c_libc_snprintf
#define vsnprintf       __c_libc_vsnprintf

#define wprintf         __w_libc_printf
#define wvprintf        __w_libc_vprintf
#define wfprintf        __w_libc_fprintf
#define wvfprintf       __w_libc_vfprintf
#define wvfnprintf      __w_libc_vfnprintf
#define wvsprintf       __w_libc_vsprintf
#define wsprintf        __w_libc_sprintf
#define wsnprintf       __w_libc_snprintf
#define wvsnprintf      __w_libc_vsnprintf
#endif

#ifdef __cplusplus
extern "C" {
#endif

    extern int fputc(int ch, FILE * stream);
    
    extern int printf(const char * format, ...);
    extern int vprintf(const char * format,va_list vargs);
    extern int fprintf(FILE * stream, const char * format, ...);
    extern int vfprintf(FILE * stream, const char * format, va_list vargs);
    extern int vfnprintf(FILE * stream, size_t count, const char * format, va_list vargs);
    extern int vsprintf(char * buffer, const char * format, va_list vargs);
    extern int sprintf(char * buffer, const char * format, ...);
    extern int snprintf(char * buffer, size_t count, const char * format, ...);
    extern int vsnprintf(char * buffer, size_t count, const char * format, va_list vargs);

    extern int wfputc(int ch, FILE * stream);
    
    extern int wprintf(const WCHAR * format, ...);
    extern int wvprintf(const WCHAR * format,va_list vargs);
    extern int wfprintf(FILE * stream, const WCHAR * format, ...);
    extern int wvfprintf(FILE * stream, const WCHAR * format, va_list vargs);
    extern int wvfnprintf(FILE * stream, size_t count, const WCHAR * format, va_list vargs);
    extern int wsprintf(WCHAR * buffer, const WCHAR * format, ...);
    extern int wvsprintf(WCHAR * buffer, const WCHAR * format, va_list vargs);
    extern int wsnprintf(WCHAR * buffer, size_t count, const WCHAR * format, ...);
    extern int wvsnprintf(WCHAR * buffer, size_t count, const WCHAR * format, va_list vargs);
#ifdef __cplusplus
}
#endif

#endif

