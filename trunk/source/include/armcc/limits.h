/* limits.h: ANSI 'C' (X3J11 Oct 88) library header, section 2.2.4.2 */
/* Copyright (C) Codemist Ltd., 1988                            */
/* Copyright 1991-1997 ARM Limited. All rights reserved         */

/*
 * RCS $Revision: 178085 $
 * Checkin $Date: 2012-12-11 14:54:17 +0000 (Tue, 11 Dec 2012) $
 * Revising $Author: drodgman $
 */

#ifndef __limits_h
#define __limits_h

#define CHAR_BIT 8
#define SCHAR_MIN (-128)
#define SCHAR_MAX 127
#define UCHAR_MAX 255
#ifdef __FEATURE_SIGNED_CHAR
  #define CHAR_MIN (-128)
  #define CHAR_MAX 127
#else
  #define CHAR_MIN 0
  #define CHAR_MAX 255
#endif

#define MB_LEN_MAX 6

#define SHRT_MIN  (-0x8000)
#define SHRT_MAX  0x7fff
#define USHRT_MAX 0xffff

#define INT_MAX   0x7fffffff
#define INT_MIN   (-0x80000000)
#define UINT_MAX  0xffffffffUL

#define LONG_MAX  0x7fffffffL
#define LONG_MIN  (-0x80000000L)
#define ULONG_MAX 0xffffffffUL

#if defined (__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
#define LLONG_MAX       0x7fffffffffffffffLL
#define LLONG_MIN       (0LL - 0x8000000000000000LL)
#define ULLONG_MAX      0xffffffffffffffffULL
#endif

#endif

/* end of limits.h */

