#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <fatypes.h>

static void * fast_memcpy(void * dst, const void * src, size_t length)
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

void * memcpy(void * dst, const void * src, size_t length)
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

void * memset(void * ptr, int ch, size_t length)
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


size_t strlen(const char* str)
{
    const char * s = str;

    while(*s) s++;

    return (size_t)(s-str);
}


char * strcpy(char *dst, const char *src)
{
    char *p = dst;

    while('\0' != (*p++=*src++));

    return dst;
}

char * strncpy(char *dst, const char *src, size_t count)
{
    char *p = dst;

    while (count-- && '\0' != (*p++ = *src++));

    return dst;
}

int strcmp(const char *str1, const char *str2)
{
    while (*str1 && (*str1 == *str2))
    {
        str1++; str2++;
    }
    return *str1 - *str2;
}

int strncmp(const char *str1, const char *str2, size_t count)
{
    if (!count)
        return 0;
    
    while (0 != count && *str1 && (*str1 == *str2))
    {
        str1++; str2++;
        count --;
    }
    
    return *str1 - *str2;
}

char * strcat(char *dst, const char *src)
{
    char * p = dst;
    
    while(*dst) dst ++;
    while ('\0' != (*dst++ = *src++));

    return p;
}

char * strncat(char *dst, const char *src, size_t length)
{
    strncpy(dst + strlen(dst), src, length );
    return dst;
}


char * strchr(const char *str, int ch)
{
    do{
        if (*str == ch)
            return (char *)str;
    }while (*str++);

    return NULL;
}

char * strrchr(const char *str, int ch)
{
    char *p = NULL;
    do{
        if (*str == ch)
            p = (char *)str;
    }while (*str++);
    return p;
}


char * strdup(const char *str)
{
    char *newstr;
    newstr = malloc((strlen(str)+1) * sizeof(char) );
    if ( newstr == NULL ) return NULL;
    memset(newstr,0,(strlen(str)+1) * sizeof(char));
    strcpy(newstr,str);
    return newstr;
}


char * strpbrk(const char *str, const char *accept)
{
    for ( ; *str; str++) if (strchr( accept, *str )) return (char *)str;
    return NULL;
}

size_t strspn(const char *str, const char *accept)
{
    const char *ptr;
    for (ptr = str; *ptr; ptr++) if (!strchr( accept, *ptr )) break;
    return ptr - str;
}

size_t strcspn(const char *str, const char *reject)
{
    const char *ptr;
    for (ptr = str; *ptr; ptr++) if (strchr( reject, *ptr )) break;
    return ptr - str;
}


void * memchr(const char *ptr, char ch, size_t n)
{
    const char *end;
    for (end = ptr + n; ptr < end; ptr++) if (*ptr == ch) return (char *)ptr;
    return NULL;
}

void * memrchr(const char *ptr, char ch, size_t n)
{
    const char *end;
    char *ret = NULL;
    for (end = ptr + n; ptr < end; ptr++) if (*ptr == ch) ret = (char *)ptr;
    return ret;
}
