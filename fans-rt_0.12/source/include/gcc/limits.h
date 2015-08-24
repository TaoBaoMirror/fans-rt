#ifndef _GCC_LIMITS_H_
#define _GCC_LIMITS_H_

/* Number of bits in a `char'.  */
#define CHAR_BIT __CHAR_BIT__

/* Maximum length of a multibyte character.  */
#ifndef MB_LEN_MAX
#define MB_LEN_MAX 1
#endif

/* Minimum and maximum values a `signed char' can hold.  */
#define SCHAR_MIN (-SCHAR_MAX - 1)
#define SCHAR_MAX __SCHAR_MAX__

/* Maximum value an `unsigned char' can hold.  (Minimum is 0).  */
#if __SCHAR_MAX__ == __INT_MAX__
# define UCHAR_MAX (SCHAR_MAX * 2U + 1U)
#else
# define UCHAR_MAX (SCHAR_MAX * 2 + 1)
#endif

/* Minimum and maximum values a `char' can hold.  */
#ifdef __CHAR_UNSIGNED__
# undef CHAR_MIN
# if __SCHAR_MAX__ == __INT_MAX__
#  define CHAR_MIN 0U
# else
#  define CHAR_MIN 0
# endif
# undef CHAR_MAX
# define CHAR_MAX UCHAR_MAX
#else
# undef CHAR_MIN
# define CHAR_MIN SCHAR_MIN
# undef CHAR_MAX
# define CHAR_MAX SCHAR_MAX
#endif

/* Minimum and maximum values a `signed short int' can hold.  */
#define SHRT_MIN (-SHRT_MAX - 1)
#define SHRT_MAX __SHRT_MAX__

/* Maximum value an `unsigned short int' can hold.  (Minimum is 0).  */
#if __SHRT_MAX__ == __INT_MAX__
# define USHRT_MAX (SHRT_MAX * 2U + 1U)
#else
# define USHRT_MAX (SHRT_MAX * 2 + 1)
#endif

/* Minimum and maximum values a `signed int' can hold.  */
#define INT_MIN (-INT_MAX - 1)
#define INT_MAX __INT_MAX__

/* Maximum value an `unsigned int' can hold.  (Minimum is 0).  */
#define UINT_MAX (INT_MAX * 2U + 1U)

/* Minimum and maximum values a `signed long int' can hold.
   (Same as `int').  */
#define LONG_MIN (-LONG_MAX - 1L)
#define LONG_MAX __LONG_MAX__

/* Maximum value an `unsigned long int' can hold.  (Minimum is 0).  */
#define ULONG_MAX (LONG_MAX * 2UL + 1UL)

#if defined (__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
/* Minimum and maximum values a `signed long long int' can hold.  */
# define LLONG_MIN (-LLONG_MAX - 1LL)
# define LLONG_MAX __LONG_LONG_MAX__

/* Maximum value an `unsigned long long int' can hold.  (Minimum is 0).  */
# define ULLONG_MAX (LLONG_MAX * 2ULL + 1ULL)
#endif

#if defined (__GNU_LIBRARY__) ? defined (__USE_GNU) : !defined (__STRICT_ANSI__)
/* Minimum and maximum values a `signed long long int' can hold.  */
# define LONG_LONG_MIN (-LONG_LONG_MAX - 1LL)
# define LONG_LONG_MAX __LONG_LONG_MAX__

/* Maximum value an `unsigned long long int' can hold.  (Minimum is 0).  */
# define ULONG_LONG_MAX (LONG_LONG_MAX * 2ULL + 1ULL)
#endif


#endif

