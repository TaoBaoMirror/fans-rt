#include <limits.h>
#include <stddef.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <fauser.h>
#include <fadefs.h>
#include <fatypes.h>

#include "libcmini.h"

#ifdef __CC_ARM
#undef printf
#undef vprintf
#undef fprintf
#undef vfprintf
#undef vfnprintf
#undef vsprintf
#undef sprintf
#undef snprintf
#undef vsnprintf

#undef wprintf
#undef wvprintf
#undef wfprintf
#undef wvfprintf
#undef wvfnprintf
#undef wvsprintf
#undef wsprintf
#undef wsnprintf
#undef wvsnprintf

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


#if (BUILD_CHAR_WIDTH != 1)
#define STRING_CHAR_T       WCHAR
#define vfnprintf           wvfnprintf
#define vfprintf            wvfprintf
#define fnprintf            wfnprintf
#define fprintf             wfprintf
#define vprintf             wvprintf
#define printf              wprintf
#define fa_vxnprintf        fa_wvxnprintf
#define fputc               wfputc
#else
#define STRING_CHAR_T       char
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
#endif
#endif



STATIC CODE_TEXT  int put_stream_char(int ch, LPVOID lpPrivate)
{
    return fputc(ch, lpPrivate);
}

EXPORT CODE_TEXT int vfnprintf(FILE * stream, size_t count, const STRING_CHAR_T * format, va_list vargs)
{
    return fa_vxnprintf(put_stream_char, stream, count, format, vargs);
}
EXPORT_SYMBOL(vfnprintf);

EXPORT CODE_TEXT int vfprintf(FILE * stream, const STRING_CHAR_T * format,va_list vargs)
{
    return fa_vxnprintf(put_stream_char, stream, ~0, format, vargs);
}
EXPORT_SYMBOL(vfprintf);

EXPORT CODE_TEXT int fnprintf(FILE * stream, size_t count, const STRING_CHAR_T * format,...)
{
    int length;
    va_list vargs;

    va_start(vargs, format);
    length = fa_vxnprintf(put_stream_char, stdout, count, format, vargs);
    va_end(vargs);

    return (length);
}
EXPORT CODE_TEXT int fprintf(FILE * stream, const STRING_CHAR_T * format,...)
{
    int length;
    va_list vargs;

    va_start(vargs, format);
    length = fa_vxnprintf(put_stream_char, stdout, ~0, format, vargs);
    va_end(vargs);

    return (length);
}
EXPORT_SYMBOL(fprintf);

EXPORT CODE_TEXT int vprintf(const STRING_CHAR_T * format, va_list vargs)
{
    return fa_vxnprintf(put_stream_char, stdout, ~0, format, vargs);
}
EXPORT_SYMBOL(vprintf);

EXPORT CODE_TEXT int printf(const STRING_CHAR_T * format, ...)
{
    int length;
    va_list vargs;

    va_start(vargs,format);
    length = fa_vxnprintf(put_stream_char, stdout, ~0, format, vargs);
    va_end(vargs);

    return (length);
}
EXPORT_SYMBOL(printf);


