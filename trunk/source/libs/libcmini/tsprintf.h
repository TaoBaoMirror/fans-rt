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
#include <limits.h>
#include <stddef.h>
#include <stdarg.h>
#include <wchar.h>
#include <string.h>
#include <stdio.h>

typedef int (* FNPUTCHAR)(int Ch, void * lpPrivate);

#define F_UNSET  (0)
#define F_MARK   (1 << 0)
#define F_MINUS  (1 << 1)
#define F_PLUS   (1 << 2)
#define F_SPACE  (1 << 3)
#define F_HASH   (1 << 4)
#define F_DOT    (1 << 5)
#define F_ZERO   (1 << 6)
#define F_LONG   (1 << 7)
#define F_LLONG  (1 << 8)
#define N_BUFFER_SIZE   64

#ifdef _TEXT
#undef _TEXT
#endif

#if (BUILD_CHAR_WIDTH != 1)
#define STRING_CHAR_T       WCHAR
#define _TEXT(text)         (void *)(L ##text)
#define tslen(x)            wcslen(x)
#define tscpy(src, dst)     wcscpy(src, dst)
#else
#define STRING_CHAR_T       char
#define tslen(x)            strlen(x)
#define tscpy(src, dst)     strcpy(src, dst)
#define _TEXT(text)         text
#endif

#if defined (__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
typedef long long SLL_TYPE;
typedef unsigned long long ULL_TYPE;
#define MAX_BITS_WIDTH      64
#else
typedef long SLL_TYPE;
typedef unsigned long ULL_TYPE;
#define MAX_BITS_WIDTH      32
#endif

typedef double FLL_TYPE;

static const STRING_CHAR_T * lcharset = _TEXT("0123456789abcdef");
static const STRING_CHAR_T * ucharset = _TEXT("0123456789ABCDEF");

static ULL_TYPE __std_ulldiv_mod(ULL_TYPE divisor,ULL_TYPE dividend, ULL_TYPE * remainder)
{
    unsigned Bits = MAX_BITS_WIDTH;
    ULL_TYPE business = 0;
    
    do{
        Bits --;
        if((dividend >> Bits)>= divisor)
        {
            dividend -= (divisor<<Bits);
            business +=(1ULL<<Bits);
        }
    }while(Bits);

    *remainder = dividend;
    
    return business;
}


static int fa_p_itoa(ULL_TYPE number, STRING_CHAR_T * buffer, size_t length,
                    int precision, const STRING_CHAR_T * charset, int radix, int prefix)
{
    int count = 0;
    ULL_TYPE dividend = number;
    ULL_TYPE offset = 0ULL;
    
    STRING_CHAR_T * string = buffer + length - 1;
        
    *(string--) = '\0';

    if (!dividend)
    {
        *(string--) = '0';
        count ++;
    }
    else
    {
        while(dividend && count < length)
        {
            dividend = __std_ulldiv_mod(radix, dividend, &offset);
            *(string--) = charset[offset];
            count ++;
        }
    }

    while(count < precision)
    {
        *(string--) = '0';
        count ++;
    }

    if (prefix)
    {
        *(string--) = prefix;
        count ++;
    }
#if 0
    else if(subsidiary)
    {
        *(string--) = prefix;
        count ++;
    }
#endif

    return count;
}

static int fa_p_show_string(FNPUTCHAR fnPut, LPVOID lpPrivate, STRING_CHAR_T type,
    int width, size_t max_length, int scale, STRING_CHAR_T * string, int flags,
    STRING_CHAR_T prefix, STRING_CHAR_T suffix)
{
    int i;
    int old_scale = scale;
    int count = tslen(string);
    
    if (0 == (flags & F_MINUS))
    {
        for (i = 0; i < width - count; i ++, scale ++)
        {
            if (scale >= max_length - 1)
            {
                return scale - old_scale;
            }

            if (EOF == fnPut (prefix, lpPrivate))
            {
                return -1;
            }
        }
    }

    for(;*string; scale ++)
    {
        if (scale >= max_length - 1)
        {
            break;
        }
        
        if (EOF == fnPut (*string++, lpPrivate))
        {
            return -1;
        }
    }

    if (0 != (flags & F_MINUS))
    {
        for (i = 0; i < width - count; i ++, scale ++)
        {
            if (scale >= max_length - 1)
            {
                return scale - old_scale;
            }

            if (EOF == fnPut (suffix, lpPrivate))
            {
                return -1;
            }
        }
    }
    
    return scale - old_scale;
}


static int fa_p_show_number(FNPUTCHAR fnPut, LPVOID lpPrivate, STRING_CHAR_T type,
    int precision, size_t max_length, int scale, SLL_TYPE number, int flags)
{
    int width = 0;
    int radix = 10;
    int count = 0;
    int prefix = 0;
    STRING_CHAR_T fill = (flags & F_ZERO) ? '0' : ' ';
    STRING_CHAR_T * string;
    ULL_TYPE unumber = (ULL_TYPE) number;
    const STRING_CHAR_T * charset = lcharset;
    STRING_CHAR_T buffer[N_BUFFER_SIZE];
    
    switch (type)
    {
    case 'd':
    case 'i':
        radix = 10;
        unumber = abs(number);
        if (number < 0)
            prefix = '-';
        else if (flags & F_PLUS)
            prefix = '+';
        else if (flags & F_SPACE)
            prefix = ' ';
//        else if (flags & F_ZERO)
//            prefix = '0';
        break;
    case 'u':
        radix = 10;
        if (flags & F_PLUS)
            prefix = '+';
        else if (flags & F_SPACE)
            prefix = ' ';
//        else if (flags & F_ZERO)
//            prefix = '0';
        break;
    case 'o':
        radix = 8;
        break;
    case 'x':
        charset = lcharset;
        radix = 16;
        break;
    case 'X':
        charset = ucharset;
        radix = 16;
        break;
    }

    if (precision >= N_BUFFER_SIZE-1)
    {
        precision = N_BUFFER_SIZE-1;
    }
        
    if ('0' != fill)
    {   
        width = precision;
        precision = 0;
    }

    count = fa_p_itoa(unumber, buffer, N_BUFFER_SIZE, precision, charset, radix, prefix);

    string = buffer + (N_BUFFER_SIZE - count - 1);
    
    return fa_p_show_string(fnPut, lpPrivate, type, width, max_length, scale, string, flags, ' ', ' ');
}

static int fa_p_show_address(FNPUTCHAR fnPut, LPVOID lpPrivate, STRING_CHAR_T type,
    int width, size_t max_length, int scale, SLL_TYPE number, int flags)
{
    int count = 0;
    STRING_CHAR_T * string;
    const STRING_CHAR_T * charset = lcharset;
    STRING_CHAR_T prefix = ' ';
    STRING_CHAR_T buffer[N_BUFFER_SIZE];
    
    if ('P' == type)
    {
        charset = ucharset;
    }
    
    if (flags & F_ZERO)
    {
        prefix = '0';
    }

    count = fa_p_itoa((ULL_TYPE) number, buffer, N_BUFFER_SIZE, sizeof(void*) * 2, charset, 16, 0);
    
    string = buffer + (N_BUFFER_SIZE - count - 1);
    
    return fa_p_show_string(fnPut, lpPrivate, type, width, max_length, scale, string, flags, prefix, ' ');
}

#define IEEE754_MANTISSA_MAGIC      0x0010000000000000LL
#define IEEE754_MANTISSA_MASK       0x000fffffffffffffLL   /* 尾数 */
#define IEEE754_MANTISSA_SHIFT      0
#define IEEE754_EXPONENT_MAGIC      1023
#define IEEE754_EXPONENT_MASK       0x7ffLL                 /* 指数 */
#define IEEE754_EXPONENT_SHIFT      52
#define IEEE754_SIGNBIT_MASK        1
#define IEEE754_SIGNBIT_SHIFT       63


#define IEEE754_SIGN(x)             (((x) >> IEEE754_SIGNBIT_SHIFT) & IEEE754_SIGNBIT_MASK)
#define IEEE754_EXPONENT(x)         ((((x) >> IEEE754_EXPONENT_SHIFT) & IEEE754_EXPONENT_MASK) - IEEE754_EXPONENT_MAGIC)
#define IEEE754_MANTISSA(x)         ((((x) >> IEEE754_MANTISSA_SHIFT) & IEEE754_MANTISSA_MASK) | IEEE754_MANTISSA_MAGIC)
#define IEEE754_INT_SHIFT(x)        (IEEE754_EXPONENT_SHIFT - IEEE754_EXPONENT(x))
#define IEEE754_DECIMAL_LENGTH      16
#define IEEE754_DECIMAL_BASE        5000000000000000LL
#define IEEE754_DEFAULT_PRECISION   6

#if defined (__STDC_VERSION__) && __STDC_VERSION__ >= 199901L

static ULL_TYPE fa_p_decpart(ULL_TYPE number)
{
    int Bits;
    ULL_TYPE base = IEEE754_DECIMAL_BASE;
    ULL_TYPE result = 0ULL;

    for (Bits = MAX_BITS_WIDTH - 1; Bits > 0; Bits --)
    {
        if (0 != ((number >> Bits) & 0x1))
        {
            result += base;
        }

        base >>= 1;
    }

    return result;
}

static int fa_p_ftoa(int shift, ULL_TYPE mantissa, STRING_CHAR_T * buffer, int length,
                    int precision, const STRING_CHAR_T * charset, int prefix)
{
    int carry = 0;
    int int_count = 0;
    int dec_count = 0;
    int position;
    int decimal_shift = 64 - shift;
    STRING_CHAR_T * dec_offset;
    STRING_CHAR_T * int_buffer = buffer;
    STRING_CHAR_T * dec_buffer = int_buffer + length/2;
    
    ULL_TYPE int_part = (mantissa >> shift);
    ULL_TYPE dec_part = (mantissa << decimal_shift);
    
    int_count = fa_p_itoa(int_part, int_buffer, length/2, 0, charset, 10, prefix);
    
    dec_part = fa_p_decpart(dec_part);
    
    dec_count = fa_p_itoa(dec_part, dec_buffer, length/2, 16, charset, 10, 0);

    dec_offset = int_buffer + (length - dec_count - 1);
    
    *(--dec_offset) = '.';

    tscpy(dec_buffer - 1, dec_offset);

    if (0 == precision)
    {
        position = length/2 + IEEE754_DEFAULT_PRECISION - 1;
    }
    else if (precision > IEEE754_DECIMAL_LENGTH)
    {
        position = length/2 + IEEE754_DECIMAL_LENGTH - 1;
    }
    else
    {
        position = length/2 + precision - 1;
    }

    if (position < length/2 + IEEE754_DECIMAL_LENGTH - 1)
    {
        if (buffer[position + 1] > '4')
        {
            carry = 1;
        }
        buffer[position + 1] = '\0';
    }

    while(carry)
    {
        if (buffer[position] == '.')
        {
            position --;
        }
        else if (buffer[position] >= '0' && buffer[position] <= '9')
        {
            buffer[position] ++;

            if (buffer[position] > '9')
            {
                buffer[position] = '0';
                carry = 1;
            }
            else
            {
                carry = 0;
            }
            
            position --;
        }
        else
        {
            buffer[position - 1] = buffer[position];
            buffer[position] = '1';
            carry = 0;
            int_count ++;
        }
    }

    return int_count;
}

static int fa_p_show_float(FNPUTCHAR fnPut, LPVOID lpPrivate, STRING_CHAR_T type,
    int width, int precision, size_t max_length, int scale, FLL_TYPE fnumber, int flags)
{
    int sign;
    int count = 0;
    int prefix = 0;
    int int_shift;
    ULL_TYPE mantissa;
    STRING_CHAR_T * string;
    STRING_CHAR_T buffer[N_BUFFER_SIZE];
    union {
        FLL_TYPE    fnumber;
        ULL_TYPE    unumber;
    }un;

    un.fnumber = fnumber;

    sign = IEEE754_SIGN(un.unumber);                            /* 符号 */
    int_shift = IEEE754_INT_SHIFT(un.unumber);                  /* 整数部分位移 */
    mantissa = (ULL_TYPE) IEEE754_MANTISSA(un.unumber);    /* 尾数 */

    if (sign) prefix = '-';
    else if (flags & F_PLUS) prefix = '+';
    
    count = fa_p_ftoa(int_shift, mantissa, buffer, sizeof(buffer), precision, lcharset, prefix);

    string = buffer + (N_BUFFER_SIZE/2 - count - 1);

    return fa_p_show_string(fnPut, lpPrivate, type, width, max_length, scale, string, flags, ' ', ' ');
}
#endif


#if (BUILD_CHAR_WIDTH  != 1)
#define fa_vxnprintf        fa_wvxnprintf
#endif
int fa_vxnprintf(FNPUTCHAR fnPut, LPVOID lpPrivate, size_t length, const STRING_CHAR_T * format, va_list args)
{
    int ch;
    int result = 0;
    int flags = F_UNSET;
    int count = 0;          /* 总共已输出字符数 */
    int width = 0;          /* 输出字符串宽度 */
    int precision = 0;      /* 输出数据精度 */
    SLL_TYPE number = 0;
    FLL_TYPE fnumber = 0.0f;
    STRING_CHAR_T * string = NULL;
    
    while(*format)
    {
        if (!flags && '%' != *format)
        {
            if (EOF == fnPut (*format++, lpPrivate))
            {
                return -1;
            }
            
            if (length - 1 == count ++)
            {
                if (EOF == fnPut ('\0', lpPrivate))
                {
                    return -1;
                }
                
                return count + 1;
            }
            
            continue;
        }
        /* else */
        //
        /* flags != 0 or format = '%' */
        switch(*format)
        {
        case '-':
            flags |= F_MINUS;
            break;
        case '+':
            flags |= F_PLUS;
            break;
        case '#':
            flags |= F_HASH;
            break;
        case ' ':
            flags |= F_SPACE;
            break;
        case '.':
            flags |= F_DOT;
            precision = 0;
            break;
        case '0':
            if ((0 == width) && (0 == (flags & F_DOT)))
            {
                flags |= F_ZERO;
                precision = 0;
                break;
            }
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            if ((flags & F_ZERO) || (flags & F_DOT))
            {
                precision = precision * 10 + (*format) - '0';
            }
            else
            {
                width = width * 10 + (*format) - '0';
            }
            break;
        case '*':
            break;
        case 'h':
            break;
        case 'l':
            if (flags & F_LONG) { flags |= F_LLONG; }
            else { flags |= F_LONG; }
            break;
        case 'd':
        case 'i':
        case 'u':
        case 'o':
        case 'x':
        case 'X':
#if defined (__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
            if (flags & F_LLONG)
            {
                number = va_arg (args, long long);
            }
            else
#endif
            if (flags & F_LONG)
            {
                number = (SLL_TYPE)(va_arg (args, long));
#if (LONG_MAX == 0x7fffffffL)
                if ('u' == *format || 'o' == *format ||
                    'x' == *format || 'X' == *format)
                {
                    number = (((ULL_TYPE)number) & ULONG_MAX);
                }
#endif
            }
            else
            {
                number = (SLL_TYPE)(va_arg (args, int));
                if ('u' == *format || 'o' == *format ||
                    'x' == *format || 'X' == *format)
                {
                    number = (((ULL_TYPE)number) & UINT_MAX);
                }
            }
          
            result = fa_p_show_number(fnPut, lpPrivate, *format, 
                                    precision ? precision : width,
                                    length, count, (SLL_TYPE) number, flags);
            
            if (EOF == result)
            {
                return -1;
            }
            
            count += result;
            flags = F_UNSET;
            precision = 0;
            width = 0;
            break;
        case 'p':
        case 'P':
#if defined (__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
            if (8 == sizeof(void *))
            {
                number = (SLL_TYPE)(va_arg (args, long long));
            }
            else
#endif
            if (2 == sizeof(void*))
            {
                number = (SLL_TYPE)(va_arg (args, int));
                number &= 0xffff;
            }
            else /*if (4 == sizeof(void *))*/
            {
                number = (SLL_TYPE)(va_arg (args, long));
                number &= 0xffffffff;
            }

            result = fa_p_show_address(fnPut, lpPrivate, *format, 
                                precision ? precision : width,
                                length, count, number, flags);
            
            if (EOF == result)
            {
                return -1;
            }
            
            count += result;
            flags = F_UNSET;
            precision = 0;
            width = 0;
            break;
        case 'f':
            fnumber = (FLL_TYPE)(va_arg (args, FLL_TYPE));
#if defined (__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
            result = fa_p_show_float(fnPut, lpPrivate, *format, 
                                width, precision,
                                length, count, fnumber, flags);

            if (EOF == result)
            {
                return -1;
            }
            
            count += result;
#endif
            flags = F_UNSET;
            precision = 0;
            width = 0;

            break;
        case 's':
            string = va_arg (args, STRING_CHAR_T*);

            if (NULL == string)
            {
                string =_TEXT("{NULL}");
            }

            result = fa_p_show_string(fnPut, lpPrivate, *format,
                            width, length, count, string, flags,
                            ' ', ' ');
            
            if (EOF == result)
            {
                return -1;
            }
            
            count += result;
            flags = F_UNSET;
            precision = 0;
            width = 0;
            break;
        case '%':
            if (flags & F_MARK)
            {
                ch = '%';
                result = fa_p_show_string(fnPut, lpPrivate, *format,
                                width, length, count, (STRING_CHAR_T*)&ch, flags,
                                ' ', ' ');
                
                if (EOF == result)
                {
                    return -1;
                }
                
                count += result;
                flags = F_UNSET;
                precision = 0;
                width = 0;
            }
            else
            {
                flags = F_MARK;
            }
            break;
        case 'c':
            ch = va_arg (args, int);

            result = fa_p_show_string(fnPut, lpPrivate, *format,
                            width, length, count, (STRING_CHAR_T*)&ch, flags,
                            ' ', ' ');
            
            if (EOF == result)
            {
                return -1;
            }
            
            count += result;
            flags = F_UNSET;
            precision = 0;
            width = 0;
            break;
        default:
            break;
        }
        
        format ++;
    }

    if (EOF == fnPut ('\0', lpPrivate))
    {
        return -1;
    }
    
    return count + 1;
}

#if (BUILD_CHAR_WIDTH != 1)
#define fa_xnprintf fa_wxnprintf
#endif

int fa_xnprintf(FNPUTCHAR fnPut, LPVOID lpPrivate, size_t size, const STRING_CHAR_T * format, ...)
{
    int length;
    va_list vargs;
    
    va_start (vargs, format);
    length = fa_vxnprintf(fnPut, lpPrivate, size, format, vargs);
    va_end(vargs);
    
    return length;
}
EXPORT_SYMBOL(fa_xnprintf);

typedef struct tagSTRING_BUFFER STRING_BUFFER;
typedef struct tagSTRING_BUFFER * PSTRING_BUFFER;
typedef struct tagSTRING_BUFFER FAR * LPSTRING_BUFFER;

struct tagSTRING_BUFFER
{
    size_t          length;
    size_t          position;
    STRING_CHAR_T * buffer;
};


static int put_string_char(int ch, LPVOID lpPrivate)
{
    LPSTRING_BUFFER string = lpPrivate;
    
    if (string->position < string->length)
    {
        string->buffer[string->position ++] = ch;
    }
    
    return ch;
}

#if (BUILD_CHAR_WIDTH != 1)
#undef vsnprintf
#define vsnprintf wvsnprintf
#endif
int vsnprintf(STRING_CHAR_T * buffer, size_t count, const STRING_CHAR_T * format, va_list vargs)
{
    STRING_BUFFER String;

    String.length = count;
    String.position = 0;
    String.buffer = buffer;

    return fa_vxnprintf(put_string_char, &String, count, format, vargs);
}

#if (BUILD_CHAR_WIDTH != 1)
#undef vsprintf
#define vsprintf wvsprintf
#endif
int vsprintf(STRING_CHAR_T * buffer, const STRING_CHAR_T * format, va_list vargs)
{
    return vsnprintf(buffer, ~0, format, vargs);
}

#if (BUILD_CHAR_WIDTH != 1)
#undef sprintf
#define sprintf wsprintf
#endif
int sprintf(STRING_CHAR_T * buffer, const STRING_CHAR_T * format,...)
{
    va_list vargs;
    int string_length;

    va_start(vargs,format);
    string_length = vsnprintf(buffer, ~0, format, vargs);
    va_end(vargs);

    return string_length;
}

#if (BUILD_CHAR_WIDTH != 1)
#undef snprintf
#define snprintf wsnprintf
#endif
int snprintf(STRING_CHAR_T * buffer,size_t count, const STRING_CHAR_T * format,...)
{
    int length;
    va_list vargs;

    va_start(vargs, format);
    length = vsnprintf(buffer, count, format, vargs);;
    va_end(vargs);

    return length;
}


