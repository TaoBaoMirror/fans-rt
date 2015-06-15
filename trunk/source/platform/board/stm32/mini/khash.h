#ifndef __KHASH_H
#define __KHASH_H

#include <fadefs.h>
#include <fatypes.h>

#define     POOL_HASH_MAX                       4
#define     POOL_ID1_HASH_OFFSET                1
#define     POOL_ID0_HASH_OFFSET                0

#define     GET_POOL_HASH_MAGIC(Magic)          (((Magic)[POOL_ID1_HASH_OFFSET]) >> 16)
#define     GET_BLOCK_HASH_MAGIC(Magic)         (((Magic)[POOL_ID1_HASH_OFFSET]) & 0xffff)

#define     BITS00_07(x)                        ((((x) >> 0) & 0xff) - 'A')
#define     BITS08_15(x)                        ((((x) >> 8) & 0xff) - 'A')
#define     BITS16_23(x)                        ((((x) >> 16) & 0xff) - 'A')
#define     BITS24_31(x)                        ((((x) >> 16) & 0xff) - 'A')

#define     KHASH32_0(Magic, Mod)                               \
            ((((BITS00_07(Magic) & 0x03) << 2) +                \
            ((BITS08_15(Magic) & 0x03) << 0) +                  \
            ((BITS16_23(Magic) & 0x0f) << 4)) % Mod)

#define     KHASH32_1(Magic, Mod)                               \
            ((((BITS00_07(Magic) & 0x0f) << 4) +                \
            ((BITS08_15(Magic) & 0x03) << 0) +                  \
            ((BITS16_23(Magic) & 0x03) << 2)) % Mod)
            
#define     KHASH32_2(Magic, Mod)                               \
            ((((BITS00_07(Magic) & 0x3) << 0) +                 \
            ((BITS08_15(Magic) & 0x0f) << 2) +                  \
            ((BITS16_23(Magic) & 0x03) << 6)) % Mod)

#define     KHASH32_3(Magic, Mod)                               \
            ((((BITS00_07(Magic) & 0xc) >> 2) +                 \
            ((BITS08_15(Magic) & 0x0c) << 4) +                  \
            ((BITS16_23(Magic) & 0x1e) << 1)) % Mod)

#if (__ENDIAN__ == 0x1234)
#define     Magic2Hash00(Magic, Mod)                              \
            (BITS00_07(Magic)% Mod)
#define     Magic2Hash01(Magic, Mod)                              \
            (BITS08_15(Magic)% Mod)
#define     Magic2Hash09(Magic, Mod)                              \
            (((BITS00_07(Magic)<<10) |                            \
            (BITS08_15(Magic)<<5) |                               \
            (BITS16_23(Magic)<<0)) % Mod)
#else
#define     Magic2Hash00(Magic, Mod)                              \
            (BITS24_31(Magic)% Mod)
#define     Magic2Hash01(Magic, Mod)                              \
            (BITS16_23(Magic)% Mod)
#endif
            
#endif
