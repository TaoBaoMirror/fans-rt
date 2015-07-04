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
#ifndef __KHASH_H
#define __KHASH_H

#include <fadefs.h>
#include <fatypes.h>

#define     BITS00_07(x)                        ((((x) >> 0) & 0xff) - 'A')
#define     BITS08_15(x)                        ((((x) >> 8) & 0xff) - 'A')
#define     BITS16_23(x)                        ((((x) >> 16) & 0xff) - 'A')
#define     BITS24_31(x)                        ((((x) >> 16) & 0xff) - 'A')

#if (__ENDIAN__ == 0x1234)
#define     Magic2RequestIDKey(Magic)                                \
            ((BITS08_15(Magic))% CONFIG_LPC_SERVICE_MAX)

#define     Magic2RequestID(Magic)                                    \
            (Magic2RequestIDKey(Magic) % CONFIG_LPC_SERVICE_MAX)
            
#define     Magic2ClassIDKey(Magic)                                 \
            (((BITS16_23(Magic)<<0) |                               \
            (BITS08_15(Magic)<<5)))

#define     Magic2ClassID(Magic)                                    \
            (Magic2ClassIDKey(Magic) % CONFIG_SYSTEM_CLASS_MAX)

#else
#define     Magic2RequestID(Magic)                                \
            (BITS08_15(Magic)% CONFIG_LPC_SERVICE_MAX)

#define     Magic2ClassIDKey(Magic)                                 \
            (((BITS16_23(Magic)<<0) |                               \
            (BITS08_15(Magic)<<5)))

#define     Magic2ClassID(Magic)                                    \
            (Magic2ClassIDKey(Magic) % CONFIG_SYSTEM_CLASS_MAX)

#endif

#ifdef __cplusplus
extern "C" {
#endif
    typedef DWORD (*FNHASHKEY)(DWORD Magic);
    typedef DWORD (*FNHASHLOADMAGIC)(LPVOID lpPrivate, DWORD Id);
    PUBLIC DWORD CORE_HashSetArray(FNHASHLOADMAGIC fnLoadMagic, LPVOID lpPrivate,
                                LPDWORD MagicArray, DWORD LastMagic, DWORD Count);
    PUBLIC VOID CORE_HashDebug(FNHASHKEY fnHashKey, LPDWORD MagicArray,
                                DWORD Count, DWORD DivLow, DWORD DivHigh,
                                LPCSTR Config, LPCSTR HashFunc);
    
#ifdef __cplusplus
}
#endif
#endif
