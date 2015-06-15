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
#ifndef __KTABLE_H
#define __KTABLE_H

#include <fadefs.h>
#include <fatypes.h>


#if (CONFIG_BUILD_MINI_MAP == TRUE)

#define     GetByteHowManyBits(Value)       (g_HowManyBitsTable[((Value) >> 0) & 0xf] + g_HowManyBitsTable[((Value) >> 4) & 0xf])
#define     GetWordHowManyBits(Value)       (GetByteHowManyBits(((Value) >> 0) & 0xff) + GetByteHowManyBits(((Value) >> 8) & 0xff))
#define     GetDwordHowManyBits(Value)      (GetWordHowManyBits(((Value) >> 0) & 0xffff) + GetWordHowManyBits(((Value) >> 16) & 0xffff))

EXTERN CONST BYTE g_HowManyBitsTable[16];
EXTERN CONST BYTE g_HighestBitmapTable[16];
EXTERN CONST BYTE g_LowestBitTable[16];

STATIC INLINE BYTE GetByteHighestBit(BYTE Mask)
{
    BYTE Lb = g_HighestBitmapTable[(Mask >> 0) & 0xf];
    BYTE Hb = g_HighestBitmapTable[(Mask >> 4) & 0xf];

    if (Hb < 8) return Hb + 4;
    if (Lb < 8) return Lb;

    return 8;
}

STATIC INLINE BYTE GetByteLowestBit(BYTE Mask)
{
    BYTE Lb = g_LowestBitTable[(Mask >> 0) & 0xf];
    BYTE Hb = g_LowestBitTable[(Mask >> 4) & 0xf];

    if (Lb < 8) return Lb;
    if (Hb < 8) return Hb + 4;

    return 8;
}


#else

#define     GetByteHowManyBits(Value)       (g_HowManyBitsTable[Value])
#define     GetWordHowManyBits(Value)       (GetByteHowManyBits(((Value) >> 0) & 0xff) + GetByteHowManyBits(((Value) >> 8) & 0xff))
#define     GetDwordHowManyBits(Value)      (GetWordHowManyBits(((Value) >> 0) & 0xffff) + GetWordHowManyBits(((Value) >> 16) & 0xffff))

EXTERN CONST BYTE g_HowManyBitsTable[256];
EXTERN CONST BYTE g_HighestBitmapTable[256];
EXTERN CONST BYTE g_LowestBitTable[256];

STATIC INLINE BYTE GetByteHighestBit(BYTE Mask)
{
    return g_HighestBitmapTable[Mask];
}

STATIC INLINE BYTE GetByteLowestBit(BYTE Mask)
{
    return g_LowestBitTable[Mask];
}
#endif



STATIC INLINE BYTE GetWordLowestBit(WORD Mask)
{
    BYTE Lb = GetByteLowestBit(LOBYTE(Mask));
    BYTE Hb = GetByteLowestBit(HIBYTE(Mask));

    if (Lb < 8) return Lb;
    if (Hb < 8) return Hb + 8;

    return 16;
}

STATIC INLINE BYTE GetDwordLowestBit(DWORD Mask)
{
    BYTE Lb = GetWordLowestBit(LOWORD(Mask));
    BYTE Hb = GetWordLowestBit(HIWORD(Mask));

    if (Lb < 16) return Lb;
    if (Hb < 16) return Hb + 16;

    return 32;
}

STATIC INLINE BYTE GetWordHighestBit(WORD Mask)
{
    BYTE Lb = GetByteHighestBit(LOBYTE(Mask));
    BYTE Hb = GetByteHighestBit(HIBYTE(Mask));

    if (Hb < 8) return Hb + 8;
    if (Lb < 8) return Lb;

    return 16;
}

STATIC INLINE BYTE GetDwordHighestBit(DWORD Mask)
{
    BYTE Lb = GetWordHighestBit(LOWORD(Mask));
    BYTE Hb = GetWordHighestBit(HIWORD(Mask));

    if (Hb < 16) return Hb + 16;
    if (Lb < 16) return Lb;

    return 32;
}

#endif

