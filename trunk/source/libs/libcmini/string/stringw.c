#include <string.h>
#include <stdlib.h>
#include <fadefs.h>
#include <fatypes.h>

  
size_t strlenW( const WCHAR *str )
{
    const WCHAR *s = str;
    while (*s)s++;
    return s - str;
}

WCHAR *strcpyW( WCHAR *dst, const WCHAR *src )
{
    WCHAR *p = dst;
    while ('\0' != (*p++ = *src++));
    return dst;
}

WCHAR *strncpyW(WCHAR *dst,const WCHAR *src,size_t count)
{
    WCHAR *p = dst;
    if ( count == 0) return NULL;
    while (count--) {*p++ = *src++;}
    return dst;
}

/* strncpy doesn't do what you think, don't use it */

int strcmpW(const WCHAR *str1, const WCHAR *str2)
{
    while (*str1 && (*str1 == *str2)) { str1++; str2++; }
    return *str1 - *str2;
}

int strncmpW(const WCHAR *str1, const WCHAR *str2, int n)
{
    if (n <= 0) return 0;
    while ((--n > 0) && *str1 && (*str1 == *str2)) { str1++; str2++; }
    return *str1 - *str2;
}

WCHAR *strcatW(WCHAR *dst, const WCHAR *src)
{
    strcpyW( dst + strlenW(dst), src );
    return dst;
}

WCHAR * strncatW(WCHAR *dst, const WCHAR *src, size_t length)
{
    strncpyW(dst + strlenW(dst), src, length );
    return dst;
}


WCHAR *strchrW(const WCHAR *str, WCHAR ch)
{
    do { if (*str == ch) return (WCHAR *)str; } while (*str++);
    return NULL;
}

WCHAR *strrchrW(const WCHAR *str, WCHAR ch)
{
    WCHAR *ret = NULL;
    do { if (*str == ch) ret = (WCHAR *)str; } while (*str++);
    return ret;
}


WCHAR *strdupW(const WCHAR *str)
{
    WCHAR *newstr;
    newstr = malloc((strlenW(str)+1) * sizeof(WCHAR) );
    if ( newstr == NULL ) return NULL;
    memset(newstr,0,(strlenW(str)+1) * sizeof(WCHAR));
    strcpyW(newstr,str);
    return newstr;
}


WCHAR *strpbrkW(const WCHAR *str, const WCHAR *accept)
{
    for ( ; *str; str++) if (strchrW( accept, *str )) return (WCHAR *)str;
    return NULL;
}

size_t strspnW(const WCHAR *str, const WCHAR *accept)
{
    const WCHAR *ptr;
    for (ptr = str; *ptr; ptr++) if (!strchrW( accept, *ptr )) break;
    return ptr - str;
}

size_t strcspnW(const WCHAR *str, const WCHAR *reject)
{
    const WCHAR *ptr;
    for (ptr = str; *ptr; ptr++) if (strchrW( reject, *ptr )) break;
    return ptr - str;
}

WCHAR *memchrW(const WCHAR *ptr, WCHAR ch, size_t n)
{
    const WCHAR *end;
    for (end = ptr + n; ptr < end; ptr++) if (*ptr == ch) return (WCHAR *)ptr;
    return NULL;
}

WCHAR *memrchrW(const WCHAR *ptr, WCHAR ch, size_t n)
{
    const WCHAR *end;
    WCHAR *ret = NULL;
    for (end = ptr + n; ptr < end; ptr++) if (*ptr == ch) ret = (WCHAR *)ptr;
    return ret;
}
