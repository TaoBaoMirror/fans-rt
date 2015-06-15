#ifndef _GCC_STDINT_H
#define _GCC_STDINT_H

#include <limits.h>
/* 7.18.1.1  Exact-width integer types */
typedef signed char int8_t;
typedef signed char sint8_t;
typedef unsigned char uint8_t;
typedef signed short int16_t;
typedef signed short sint16_t;
typedef unsigned short uint16_t;
typedef signed int int32_t;
typedef signed int sint32_t;
typedef unsigned int uint32_t;
typedef signed long long int64_t;
typedef signed long long sint64_t;
typedef unsigned long long uint64_t;

/* 7.18.1.2  Minimum-width integer types */
typedef signed char int_least8_t;
typedef signed char sint_least8_t;
typedef unsigned char uint_least8_t;
typedef signed short int_least16_t;
typedef signed short sint_least16_t;
typedef unsigned short  uint_least16_t;
typedef signed int int_least32_t;
typedef signed int sint_least32_t;
typedef unsigned int uint_least32_t;
typedef signed long long int_least64_t;
typedef signed long long sint_least64_t;
typedef unsigned long long uint_least64_t;

/*  7.18.1.3  Fastest minimum-width integer types 
 *  Not actually guaranteed to be fastest for all purposes
 *  Here we use the exact-width types for 8 and 16-bit ints. 
 */
typedef signed char int_fast8_t;
typedef signed char sint_fast8_t;
typedef unsigned char uint_fast8_t;
typedef signed short int_fast16_t;
typedef signed short sint_fast16_t;
typedef unsigned short uint_fast16_t;
typedef signed int int_fast32_t;
typedef signed int sint_fast32_t;
typedef unsigned int uint_fast32_t;
typedef signed long long int_fast64_t;
typedef signed long long sint_fast64_t;
typedef unsigned long long uint_fast64_t;

/* 7.18.1.4  Integer types capable of holding object pointers */
typedef int intptr_t;
typedef unsigned int uintptr_t;

/* 7.18.1.5  Greatest-width integer types */
typedef long long  intmax_t;
typedef unsigned long long uintmax_t;

/* 7.18.2.2  Limits of minimum-width integer types */
#define INT_LEAST8_MIN INT8_MIN
#define INT_LEAST16_MIN INT16_MIN
#define INT_LEAST32_MIN INT32_MIN
#define INT_LEAST64_MIN INT64_MIN

#define INT_LEAST8_MAX INT8_MAX
#define INT_LEAST16_MAX INT16_MAX
#define INT_LEAST32_MAX INT32_MAX
#define INT_LEAST64_MAX INT64_MAX

#define UINT_LEAST8_MAX UINT8_MAX
#define UINT_LEAST16_MAX UINT16_MAX
#define UINT_LEAST32_MAX UINT32_MAX
#define UINT_LEAST64_MAX UINT64_MAX

/* 7.18.2.3  Limits of fastest minimum-width integer types */
#define INT_FAST8_MIN INT8_MIN
#define INT_FAST16_MIN INT16_MIN
#define INT_FAST32_MIN INT32_MIN
#define INT_FAST64_MIN INT64_MIN

#define INT_FAST8_MAX INT8_MAX
#define INT_FAST16_MAX INT16_MAX
#define INT_FAST32_MAX INT32_MAX
#define INT_FAST64_MAX INT64_MAX

#define UINT_FAST8_MAX UINT8_MAX
#define UINT_FAST16_MAX UINT16_MAX
#define UINT_FAST32_MAX UINT32_MAX
#define UINT_FAST64_MAX UINT64_MAX

#define INTPTR_MIN INT32_MIN
#define INTPTR_MAX INT32_MAX
#define UINTPTR_MAX UINT32_MAX

/* 7.18.3  Limits of other integer types */
#define PTRDIFF_MIN INTPTR_MIN
#define PTRDIFF_MAX INTPTR_MAX

#define SIG_ATOMIC_MIN INTPTR_MIN
#define SIG_ATOMIC_MAX INTPTR_MAX

#define SIZE_MAX UINTPTR_MAX

#ifndef WCHAR_MIN  /* also in wchar.h */ 
#define WCHAR_MIN 0
#define WCHAR_MAX 0xffff /* UINT16_MAX */
#endif

#endif


