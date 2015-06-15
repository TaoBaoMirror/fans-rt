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
#ifndef __STRING_H
#define __STRING_H

/* All the headers include this file. */
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif
    void*   memset(void*, int, size_t);
    void*   memcpy(void*, const void*, size_t);
    size_t  strlen(const char* str);
    char *  strcpy(char *dst, const char *src);
    char *  strncpy(char *dst, const char *src, size_t count);
    int     strcmp(const char *str1, const char *str2);
    int     strncmp(const char *str1, const char *str2, int count);
    char *  strcat(char *dst, const char *src);
    char *  strncat(char *dst, const char *src, size_t length);
    char *  strchr(const char *str, int ch);
    char *  strrchr(const char *str, int ch);
    char *  strdup(const char *str);
    char *  strpbrk(const char *str, const char *accept);
    size_t  strspn(const char *str, const char *accept);
    size_t  strcspn(const char *str, const char *reject);
    void *  memchr(const char *ptr, char ch, size_t n);
    void *  memrchr(const char *ptr, char ch, size_t n);
#ifdef __cplusplus
}
#endif

#endif


