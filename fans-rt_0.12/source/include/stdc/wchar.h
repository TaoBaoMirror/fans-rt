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
#ifndef __WCHAR_H
#define __WCHAR_H

/* All the headers include this file. */
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif
    size_t     wcslen(const wchar_t* str);
    wchar_t *  wcscpy(wchar_t *dst, const wchar_t *src);
    wchar_t *  wcsncpy(wchar_t *dst, const wchar_t *src, size_t count);
    int        wcscmp(const wchar_t *str1, const wchar_t *str2);
    int        wcsncmp(const wchar_t *str1, const wchar_t *str2, int count);
    wchar_t *  wcscat(wchar_t *dst, const wchar_t *src);
    wchar_t *  wcsncat(wchar_t *dst, const wchar_t *src, size_t length);
    wchar_t *  wcschr(const wchar_t *str, int ch);
    wchar_t *  wcsrchr(const wchar_t *str, int ch);
    wchar_t *  wcsdup(const wchar_t *str);
    wchar_t *  wcspbrk(const wchar_t *str, const wchar_t *accept);
    size_t     wcsspn(const wchar_t *str, const wchar_t *accept);
    size_t     wcscspn(const wchar_t *str, const wchar_t *reject);
    void *     wmemchr(const wchar_t *ptr, wchar_t ch, size_t n);
    void *     wmemrchr(const wchar_t *ptr, wchar_t ch, size_t n);
#ifdef __cplusplus
}
#endif

#endif

