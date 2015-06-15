#include "config.h"
#include <limits.h>
#include <stddef.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <fatypes.h>

#include "libcmini.h"

#ifdef RETURN
#undef RETURN
#endif

#define RETURN return

#define CONFIG_BUILD_MINILIBC 1
#if CONFIG_BUILD_MINILIBC==1
#define CVT_BUF_SIZE 128

#define UNSET    -1
#define FPERCENT 0x01
#define FMINUS   0x02
#define FPLUS    0x04
#define FSPACE   0x08
#define FHASH    0x10
#define FDOT     0x20
#define FZERO    0x40
#define FSHORT   0x80
#define FLONG    0x100
#define FDLONG   0x200


#if defined(__v800) && defined(CA732_BUG)
STATIC CONST CHAR * lcset = "0123456789abcdef";
STATIC CONST CHAR * ucset = "0123456789ABCDEF";
#else
STATIC CONST CHAR * CONST lcset = "0123456789abcdef";
STATIC CONST CHAR * CONST ucset = "0123456789ABCDEF";
#endif

#if defined (__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
static char *_llicvt(char * buffer, char type, long long value)
{
    int base, i;

    i = CVT_BUF_SIZE - 1;
    buffer[i--] = '\0';

    switch (type)
    {
    case 'd':
    case 'i':
        base = 10;

        if (value < 0)
        {
            if (value == LLONG_MIN)
            {           /* Special case */
                long temp;
                temp = (LLONG_MAX % base) + 1;
                value = LLONG_MAX / base;
                if (temp == base)
                {
                    value += 1;
                    temp = 0;
                }
                buffer[i--] = lcset[temp];
            }
            else
                value = -value;
        }

        while (i >= 0)
        {
            buffer[i--] = lcset[value % base];

            if ((value /= base) == 0)
                break;
        }
        break;

    case 'u':
    case 'o':
    case 'x':
    case 'X':
        if (type == 'u')
            base = 10;
        else if (type == 'o')
            base = 8;
        else
            base = 16;

        while (i >= 0)
        {
            if (type == 'X')
                buffer[i--] = ucset[((unsigned long) value) % base];
            else
                buffer[i--] = lcset[((unsigned long) value) % base];

            if ((value = ((unsigned long) value) / base) == 0)
                break;
        }
        break;

    default:
        break;
    }

    RETURN ((&buffer[++i]));
}
#endif

static char *_licvt(char * buffer, char type, long value)
{
    int base, i;

    i = CVT_BUF_SIZE - 1;
    buffer[i--] = '\0';

    switch (type)
    {
    case 'd':
    case 'i':
        base = 10;

        if (value < 0)
        {
            if (value == LONG_MIN)
            {           /* Special case */
                long temp;
                temp = (LONG_MAX % base) + 1;
                value = LONG_MAX / base;
                if (temp == base)
                {
                    value += 1;
                    temp = 0;
                }
                buffer[i--] = lcset[temp];
            }
            else
                value = -value;
        }

        while (i >= 0)
        {
            buffer[i--] = lcset[value % base];

            if ((value /= base) == 0)
                break;
        }
        break;

    case 'u':
    case 'o':
    case 'x':
    case 'X':
        if (type == 'u')
            base = 10;
        else if (type == 'o')
            base = 8;
        else
            base = 16;

        while (i >= 0)
        {
            if (type == 'X')
                buffer[i--] = ucset[((unsigned long) value) % base];
            else
                buffer[i--] = lcset[((unsigned long) value) % base];

            if ((value = ((unsigned long) value) / base) == 0)
                break;
        }
        break;

    default:
        break;
    }

    RETURN ((&buffer[++i]));
}


static char * _icvt(char FAR * buffer, char type, int value)
{
    int base, i;

    i = CVT_BUF_SIZE - 1;
    buffer[i--] = '\0';

    switch (type)
    {
    case 'd':
    case 'i':
        base = 10;

        if (value < 0)
        {
            if (value == INT_MIN)
            {           /* Special case */
                int temp;
                temp = (INT_MAX % base) + 1;
                value = INT_MAX / base;
                if (temp == base)
                {
                    value += 1;
                    temp = 0;
                }
                buffer[i--] = lcset[temp];
            }
            else
                value = -value;
        }

        while (i >= 0)
        {
            buffer[i--] = lcset[value % base];

            if ((value /= base) == 0)
                break;
        }
        break;

    case 'u':
    case 'o':
    case 'x':
    case 'X':
        if (type == 'u')
            base = 10;
        else if (type == 'o')
            base = 8;
        else
            base = 16;

        while (i >= 0)
        {
            if (type == 'X')
                buffer[i--] = ucset[((unsigned) value) % base];
            else
                buffer[i--] = lcset[((unsigned) value) % base];

            if ((value = ((unsigned) value) / base) == 0)
                break;
        }
        break;

    default:
        break;
    }

    RETURN ((&buffer[++i]));
}

 
static int  out_string (char * s,int size, int n_chars, int flags,
                        int n_zero, int n_pad, char *prefix,
                        FNPUTCHAR fnPut, LPVOID lpPrivate)
{
    int i=0;
    int count=0;
    if (n_pad && !(flags & FMINUS))
    {
        if (prefix && (flags & FZERO))
        {
            for (i=0; prefix[i]&0xff; i++)
            {
                if (fnPut(prefix[i], lpPrivate) == EOF)
                {
                    RETURN (-1);
                }
                if (count++==size-1)
                {
                    if (fnPut ('\0', lpPrivate) == EOF)
                    {
                        RETURN (-1);
                    }
                    RETURN (count+1);
                }
            }
        }
        for (i = 0; i < n_pad; i++)
        {
            if (flags & FZERO)
            {
                if (fnPut ('0', lpPrivate) == EOF)
                {
                    RETURN (-1);
                }
                if (count++==size-1)
                {
                    if (fnPut ('\0', lpPrivate) == EOF)
                    {
                        RETURN (-1);
                    }
                    RETURN (count+1);
                }
            }
            else
            {
                if (fnPut (' ', lpPrivate) == EOF)
                {
                    RETURN (-1);
                }
                if (count++==size-1)
                {
                    if (fnPut ('\0', lpPrivate) == EOF)
                    {
                        RETURN (-1);
                    }
                    RETURN (count+1);
                }
            }
        }

        if (prefix && !(flags & FZERO))
        {
            for (i=0; prefix[i]&0xff; i++)
            {
                if (fnPut (prefix[i], lpPrivate) == EOF)
                {
                    RETURN (-1);
                }
                if (count++==size-1)
                {
                    if (fnPut ('\0', lpPrivate) == EOF)
                    {
                        RETURN (-1);
                    }
                    RETURN (count+1);
                }
            }
        }
    }
    else if (prefix)
    {
        for (i=0; prefix[i]&0xff; i++)
        {
            if (fnPut (prefix[i], lpPrivate) == EOF)
            {
                RETURN (-1);
            }
            if (count++==size-1)
            {
                if (fnPut ('\0', lpPrivate) == EOF)
                {
                    RETURN (-1);
                }
                RETURN (count+1);
            }
        }
    }
    for (i = 0; i < n_zero; i++)
    {
        if (fnPut ('0', lpPrivate) == EOF)
        {
            RETURN (-1);
        }
        if (count++==size-1)
        {
            if (fnPut ('\0', lpPrivate) == EOF)
            {
                RETURN (-1);
            }
            RETURN (count+1);
        }
    }
    for (i = 0; i < n_chars; i++)
    {

        if (fnPut (*s++, lpPrivate) == EOF)
        {
            RETURN (-1);
        }
        if (count++==size-1)
        {
            if (fnPut ('\0', lpPrivate) == EOF)
            {
                RETURN (-1);
            }
            RETURN (count+1);
        }
    }

    if (n_pad && (flags & FMINUS))
    {
        for (i = 0; i < n_pad; i++)
        {
            if (fnPut (' ', lpPrivate) == EOF)
            {
                RETURN (-1);
            }
            if (count++==size-1)
            {
                if (fnPut ('\0', lpPrivate) == EOF)
                {
                    RETURN (-1);
                }
                RETURN (count+1);
            }
        }
    }
    RETURN (n_chars + n_zero + n_pad + (prefix ? strlen (prefix) : 0));
}

int t_fa_vxnprintf(FNPUTCHAR fnPut, LPVOID lpPrivate, size_t size, const char * format, va_list args)
{
    char buffer[CVT_BUF_SIZE];
    int count = 0;
    int flags = 0;
    int field_width = 0;
    int precision = UNSET;
    int *ip;
    long int i;
    char *s;
    char c;
    int sign;

    char f_type, *prefix, *fld;
    int n_chars, n_zero, n_pad;

    for (;;)
    {
        if (!flags)
        {
            while (*format && *format != '%')
            {
                if (fnPut (*format++, lpPrivate) == EOF) RETURN (-1);
                if (count++==size-1)
                {
                    if (fnPut ('\0', lpPrivate) == EOF) RETURN (-1);
                    RETURN (count+1);
                }
            }
            if (*format)
            {
                flags |= FPERCENT;
                format++;
            }
            else
                RETURN (count);
        }
        switch (*format++)
        {
        case '-':
            flags |= FMINUS;
            break;
        case '+':
            flags |= FPLUS;
            break;
        case ' ':
            flags |= FSPACE;
            break;
        case '#':
            flags |= FHASH;
            break;

        case '.':
            flags |= FDOT;
            precision = 0;
            break;
        case '0':

            if (field_width == 0 && !(flags & FDOT))
            {
                precision=0;
                flags |= FZERO;
                break;
            }
            /* fall through */
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            if (flags & FDOT || flags&FZERO)
                precision = precision * 10 +
                *(format - 1) - '0';
            else
                field_width = field_width * 10 +
                *(format - 1) - '0';
            break;
        case '*':
            if (flags & FDOT)
                precision = va_arg (args, int);
            else
            {
                if ((field_width = va_arg (args, int)) < 0)
                {
                    flags |= FMINUS;
                    field_width = -field_width;
                }
            }
            break;

        case 'h':
            flags &= ~FLONG;
            break;
        case 'l':
            if (flags & FLONG)
            {
                flags |= FDLONG;
            }
            else
            {
                flags |= FLONG;
            }
            break;

        case 'd':
        case 'i':
        case 'u':
        case 'o':
        case 'x':
        case 'X':
            /* Newlib must be compilable by compilers other than gcc, and
            making `i' long long would simplify a lot of the following,
            but it would unnecessarily slow things down.  So instead we
            only use long long values when we absolutely have to.  Some
            targets have options to set the sizes of the various ints, but
            that won't bother us.
            `sign' records the signed-ness of the value so we don't have
            any more conditional compilation than we need.  */

            if (flags & FDLONG)
                i = va_arg (args, long long);
            else if (flags & FLONG)
                i = va_arg (args, long);
            else
                i = va_arg (args, int);
            sign = (i > 0) - (i < 0);
            /* If precision is specified, 0 flag is ignored */
            if (precision != UNSET)
                flags &= ~FZERO;

            if (precision == UNSET)
                precision = 1;

            f_type = *(format - 1);

            if (precision || sign)
            {
#if defined (__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
                if (flags & FDLONG)
                    fld = _llicvt (buffer, f_type, (long long) i);
                else
#endif
#if LONG_MAX != INT_MAX
                if (flags & FLONG)
                    fld = _licvt (buffer, f_type, (long) i);
                else
#endif
                    fld = _licvt (buffer, f_type, (long) i);
            }
            else
            {
                /* zero precision && zero value */

                /* reset flags, ... etc */
                flags = 0;
                field_width = 0;
                precision = UNSET;

                break;
            }
#if 1
            n_chars = (fld ? strlen (fld) : 0);
            n_zero = 0;
            if (precision > n_chars)
                n_zero = precision - n_chars;

            n_pad = 0;
            if (field_width > n_chars + n_zero)
                n_pad = field_width - n_chars - n_zero;

            if (f_type == 'd' || f_type == 'i')
            {
                if (sign < 0)
                    prefix = "-";
                else if (flags & FPLUS)
                    prefix = "+";
                else if (flags & FSPACE)
                    prefix = " ";
                else
                    prefix = NULL;
            }
            else if (flags & FHASH)
            {
                if (f_type == 'o')
                    prefix = "0";
                else if (f_type == 'x')
                    prefix = "0x";
                else if (f_type == 'X')
                    prefix = "0X";
                else
                    prefix = NULL;
            }
            else
                prefix = NULL;

            if (n_pad && prefix)
                n_pad -= strlen (prefix);

            if ((n_chars = out_string (fld, size-count, n_chars, flags,
                n_zero, n_pad, prefix, fnPut, lpPrivate)) == -1)
            {
                RETURN (-1);
            }
            else
            {
                count += n_chars;
            }
#endif
            /* reset flags, ... etc */
            flags = 0;
            field_width = 0;
            precision = UNSET;

            break;

        case 's':
            s = va_arg (args, char FAR *);

            if (s == NULL)
                s = "{null}";

            n_chars = strlen (s);
            if (precision != UNSET && precision < n_chars)
                n_chars = precision;

            n_pad = 0;
            if (field_width > n_chars)
            {
                n_pad = field_width - n_chars;
            }
            if ((n_chars = out_string (s, size-count,n_chars, flags, 0, n_pad,
                "", fnPut, lpPrivate)) == -1)
            {
                RETURN (-1);
            }
            else
            {
                count += n_chars;
            }

            /* reset flags, ... etc */
            flags = 0;
            field_width = 0;
            precision = UNSET;
            break;

        case 'c':
            c = va_arg (args, int);

            n_pad = 0;
            if (field_width > 1)
            {
                n_pad = field_width - 1;
            }

            if ((n_chars = out_string (&c,size-count, 1, flags, 0, n_pad, "",
                fnPut, lpPrivate)) == -1)
            {
                RETURN (-1);
            }
            else
            {
                count += n_chars;
            }

            /* reset flags, ... etc */
            flags = 0;
            field_width = 0;
            precision = UNSET;
            break;

        case 'p':
            ip = va_arg (args, int *);

            fld = _icvt (buffer, (long int) ip, 'x');

            precision = 2 * sizeof (ip);
            n_chars = (fld ? strlen (fld) : 0);

            flags = FZERO;

            n_zero = 0;
            if (precision > n_chars)
            {
                n_zero = precision - n_chars;
            }
            prefix = "0x";

            if ((n_chars = out_string (fld, size-count, n_chars, flags,
                n_zero, 0, prefix, fnPut, lpPrivate)) == -1)
            {
                RETURN (-1);
            }
            else
            {
                count += n_chars;
            }

            /* reset flags, ... etc */
            flags = 0;
            field_width = 0;
            precision = UNSET;
            prefix = NULL;
            break;

        case 'n':
            if (flags & FSHORT)
                *(va_arg (args, short *)) = count;
            else
                *(va_arg (args, int *)) = count;

            /* reset flags, ... etc */
            flags = 0;
            field_width = 0;
            precision = UNSET;
            break;

        case '%':
            n_pad = 0;
            if (field_width > 1)
            {
                n_pad = field_width - 1;
            }
            if ((n_chars = out_string ("%", size-count,1, flags, 0, n_pad, "",
                fnPut, lpPrivate)) == -1)
            {
                RETURN (-1);
            }
            else
            {
                count += n_chars;
            }

            /* reset flags, ... etc */
            flags = 0;
            field_width = 0;
            precision = UNSET;
            break;

        case '\0':
            /* reset flags, ... etc */
            flags = 0;
            field_width = 0;
            precision = UNSET;
            break;
        }
    }
}
EXPORT_SYMBOL(fa_vxnprintf);

int t_fa_xnprintf(FNPUTCHAR fnPut, LPVOID lpPrivate, size_t size, const char * format, ...)
{
    int length;
    va_list vargs;
    
    va_start (vargs, format);
    length = fa_vxnprintf(fnPut, lpPrivate, size, format, vargs);
    va_end(vargs);
    
    return length;
}

PUBLIC VOID BSP_DebugWriteByte(DWORD Data);

static int put_kernel_char(INT Ch, LPVOID lpPrivate)
{
    if ('\n' == Ch)
    {
        BSP_DebugWriteByte('\r');
    }
    
    BSP_DebugWriteByte(Ch);

    return Ch;
}

EXPORT int t_bugprintf(CONST CHAR * Format,...)
{
    va_list vargs;
    int length;

    va_start (vargs, Format);
    
    //DEBUG_SHOW_BUFFER(LOG_LEVEL_ERROR, &Format, 0x10);
    length = t_fa_vxnprintf(put_kernel_char, NULL, ~0, Format, vargs);
    va_end(vargs);

    return length;
}
EXPORT_SYMBOL(t_bugprintf);


#endif

