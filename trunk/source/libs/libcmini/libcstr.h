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
#include <fauser.h>
#include <fadefs.h>
#include <fatypes.h>

#if (BUILD_CHAR_WIDTH != 1)
#define STRING_CHAR_T WCHAR
#else
#define STRING_CHAR_T char
#endif

#if (BUILD_CHAR_WIDTH != 1)
#define strlen_lib  wcslen
#else
#define strlen_lib  strlen
#endif
EXPORT CODE_TEXT size_t strlen_lib(const STRING_CHAR_T *str)
{
    const STRING_CHAR_T *s = str;
    while (*s)s++;
    return s - str;
}

#if (BUILD_CHAR_WIDTH != 1)
#define strcpy_lib  wcscpy
#else
#define strcpy_lib  strcpy
#endif
EXPORT CODE_TEXT STRING_CHAR_T *strcpy_lib(STRING_CHAR_T *dst, const STRING_CHAR_T *src)
{
    STRING_CHAR_T *p = dst;
    while ('\0' != (*p++ = *src++));
    return dst;
}

#if (BUILD_CHAR_WIDTH != 1)
#define strncpy_lib wcsncpy
#else
#define strncpy_lib strncpy
#endif
EXPORT CODE_TEXT STRING_CHAR_T *strncpy_lib(STRING_CHAR_T *dst,const STRING_CHAR_T *src, size_t count)
{
    STRING_CHAR_T *p = dst;
    if ( count == 0) return NULL;
    while (count--) {*p++ = *src++;}
    return dst;
}


#if (BUILD_CHAR_WIDTH != 1)
#define strcmp_lib  wcscmp
#else
#define strcmp_lib  strcmp
#endif
EXPORT CODE_TEXT int strcmp_lib(const STRING_CHAR_T *str1, const STRING_CHAR_T *str2)
{
    while (*str1 && (*str1 == *str2)) { str1++; str2++; }
    return *str1 - *str2;
}

#if (BUILD_CHAR_WIDTH != 1)
#define strncmp_lib wcsncmp
#else
#define strncmp_lib strncmp
#endif
EXPORT CODE_TEXT int strncmp_lib(const STRING_CHAR_T *str1, const STRING_CHAR_T *str2, int n)
{
    if (n <= 0) return 0;
    while ((--n > 0) && *str1 && (*str1 == *str2)) { str1++; str2++; }
    return *str1 - *str2;
}

#if (BUILD_CHAR_WIDTH != 1)
#define strcat_lib  wcscat
#else
#define strcat_lib  strcat
#endif
EXPORT CODE_TEXT STRING_CHAR_T *strcat_lib(STRING_CHAR_T *dst, const STRING_CHAR_T *src)
{
    strcpy_lib( dst + strlen_lib(dst), src );
    return dst;
}

#if (BUILD_CHAR_WIDTH != 1)
#define strncat_lib wcsncat
#else
#define strncat_lib strncat
#endif
EXPORT CODE_TEXT STRING_CHAR_T * strncat_lib(STRING_CHAR_T *dst, const STRING_CHAR_T *src, size_t length)
{
    strncpy_lib(dst + strlen_lib(dst), src, length );
    return dst;
}


#if (BUILD_CHAR_WIDTH != 1)
#define strchr_lib  wcschr
#else
#define strchr_lib  strchr
#endif
EXPORT CODE_TEXT STRING_CHAR_T *strchr_lib(const STRING_CHAR_T *str, int ch)
{
    do { if (*str == ch) return (STRING_CHAR_T *)str; } while (*str++);
    return NULL;
}


#if (BUILD_CHAR_WIDTH != 1)
#define strrchr_lib wcsrchr
#else
#define strrchr_lib strrchr
#endif
EXPORT CODE_TEXT STRING_CHAR_T *strrchr_lib(const STRING_CHAR_T *str, int ch)
{
    STRING_CHAR_T *ret = NULL;
    do { if (*str == ch) ret = (STRING_CHAR_T *)str; } while (*str++);
    return ret;
}


#if (BUILD_CHAR_WIDTH != 1)
#define strdup_lib  wcsdup
#else
#define strdup_lib  strdup
#endif
EXPORT CODE_TEXT STRING_CHAR_T *strdup_lib(const STRING_CHAR_T *str)
{
    STRING_CHAR_T *newstr;
    newstr = malloc((strlen_lib(str)+1) * sizeof(STRING_CHAR_T) );
    if ( newstr == NULL ) return NULL;
    memset(newstr,0,(strlen_lib(str)+1) * sizeof(STRING_CHAR_T));
    strcpy_lib(newstr,str);
    return newstr;
}

#if (BUILD_CHAR_WIDTH != 1)
#define strpbrk_lib wcspbrk
#else
#define strpbrk_lib strpbrk
#endif
EXPORT CODE_TEXT STRING_CHAR_T *strpbrk_lib(const STRING_CHAR_T *str, const STRING_CHAR_T *accept)
{
    for ( ; *str; str++) if (strchr_lib( accept, *str )) return (STRING_CHAR_T *)str;
    return NULL;
}

#if (BUILD_CHAR_WIDTH != 1)
#define strspn_lib  wcsspn
#else
#define strspn_lib  strspn
#endif
EXPORT CODE_TEXT size_t strspn_lib(const STRING_CHAR_T *str, const STRING_CHAR_T *accept)
{
    const STRING_CHAR_T *ptr;
    for (ptr = str; *ptr; ptr++) if (!strchr_lib( accept, *ptr )) break;
    return ptr - str;
}

#if (BUILD_CHAR_WIDTH != 1)
#define strcspn_lib wcscspn
#else
#define strcspn_lib strcspn
#endif
EXPORT CODE_TEXT size_t strcspn_lib(const STRING_CHAR_T *str, const STRING_CHAR_T *reject)
{
    const STRING_CHAR_T *ptr;
    for (ptr = str; *ptr; ptr++) if (strchr_lib( reject, *ptr )) break;
    return ptr - str;
}


#if (BUILD_CHAR_WIDTH != 1)
#define memchr_lib  wmemchr
#else
#define memchr_lib  memchr
#endif
EXPORT CODE_TEXT void *memchr_lib(const STRING_CHAR_T *ptr, STRING_CHAR_T ch, size_t n)
{
    const STRING_CHAR_T *end;
    for (end = ptr + n; ptr < end; ptr++) if (*ptr == ch) return (STRING_CHAR_T *)ptr;
    return NULL;
}


#if (BUILD_CHAR_WIDTH != 1)
#define memrchr_lib wmemrchr
#else
#define memrchr_lib memrchr
#endif
EXPORT CODE_TEXT void *memrchr_lib(const STRING_CHAR_T *ptr, STRING_CHAR_T ch, size_t n)
{
    const STRING_CHAR_T *end;
    STRING_CHAR_T *ret = NULL;
    for (end = ptr + n; ptr < end; ptr++) if (*ptr == ch) ret = (STRING_CHAR_T *)ptr;
    return ret;
}



#if (BUILD_CHAR_WIDTH == 1)
#ifndef __CC_ARM
STATIC CODE_TEXT void * fast_memcpy(void * dst, const void * src, size_t length)
{
    union{
        LPSTR lpstr;
        LPDWORD lpint;
    }s;
    union{
        LPSTR lpstr;
        LPDWORD lpint;
    }d;
    
    LPSTR suffix = (void *)(((MMADDR)src) + length);
    LPSTR prefix = (void *)(((MMADDR)src) & (~(sizeof(DWORD)-1)));
    LPDWORD middle = (void *)(((MMADDR)suffix) & (~(sizeof(DWORD)-1)));

    
    s.lpstr = (void *) src;
    d.lpstr = (void *) dst;
    
    if (prefix != src)
    {
        while(s.lpstr < prefix + sizeof(DWORD))
        {
            * d.lpstr++ = * s.lpstr ++;
        }
    }

    while(s.lpint < middle - (sizeof(DWORD) * 8))
    {
        * d.lpint++ = * s.lpint ++;
        * d.lpint++ = * s.lpint ++;
        * d.lpint++ = * s.lpint ++;
        * d.lpint++ = * s.lpint ++;
        * d.lpint++ = * s.lpint ++;
        * d.lpint++ = * s.lpint ++;
        * d.lpint++ = * s.lpint ++;
        * d.lpint++ = * s.lpint ++;
    }

    while(s.lpint < middle - (sizeof(DWORD) * 4))
    {
        * d.lpint++ = * s.lpint ++;
        * d.lpint++ = * s.lpint ++;
        * d.lpint++ = * s.lpint ++;
        * d.lpint++ = * s.lpint ++;
    }

    while(s.lpint < middle - (sizeof(DWORD) * 2))
    {
        * d.lpint++ = * s.lpint ++;
        * d.lpint++ = * s.lpint ++;
    }

    while(s.lpint < middle) * d.lpint++ = * s.lpint ++;
    while(s.lpstr < suffix) * d.lpstr++ = * s.lpstr ++;

    return dst;    
}

EXPORT CODE_TEXT void * memcpy(void * dst, const void * src, size_t length)
{
    if ((((MMADDR)src) & (~(sizeof(DWORD)-1))) != (((MMADDR)dst) & (~(sizeof(DWORD)-1))))
    {
        LPSTR lpSrc = (void *) src;
        LPSTR lpDst = (void *)dst;
        LPSTR lpTail = lpSrc + length;
        
        while(lpSrc < lpTail) *lpDst ++ = *lpSrc ++;
        
        return dst;
    }
    
    return fast_memcpy(dst, src, length);
}

EXPORT CODE_TEXT void * memset(void * ptr, int ch, size_t length)
{
    DWORD cv = (ch & 0xff);
    register DWORD w4 = (cv << 24) + (cv << 16) + (cv <<8) + (cv << 0);
    LPSTR suffix = (void *)(((MMADDR)ptr) + length);
    LPSTR prefix = (void *)(((MMADDR)ptr) & (~(sizeof(DWORD)-1)));
    LPDWORD middle = (void *)(((MMADDR)suffix) & (~(sizeof(DWORD)-1)));
    register union{
        LPSTR lpstr;
        LPDWORD lpint;
    }u;
   
    u.lpstr = ptr;
    
    if (prefix != ptr)
    {
        while(u.lpstr < prefix + sizeof(DWORD))
        {
            *u.lpstr++ = cv;
        }
    }
    while(u.lpint < middle - (sizeof(DWORD) * 8))
    {
        *u.lpint++ = w4;
        *u.lpint++ = w4;
        *u.lpint++ = w4;
        *u.lpint++ = w4;
        *u.lpint++ = w4;
        *u.lpint++ = w4;
        *u.lpint++ = w4;
        *u.lpint++ = w4;
    }
    while(u.lpint < middle - (sizeof(DWORD) * 4))
    {
        *u.lpint++ = w4;
        *u.lpint++ = w4;
        *u.lpint++ = w4;
        *u.lpint++ = w4;
    }

    while(u.lpint < middle - (sizeof(DWORD) * 2))
    {
        *u.lpint++ = w4;
        *u.lpint++ = w4;
    }
    while(u.lpint < middle) *u.lpint++ = w4;
    while(u.lpstr < suffix) *u.lpstr++ = cv;

    return ptr;
}
#endif
#endif
