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
#ifndef __FA_DEFS_H
#define __FA_DEFS_H

#include "config.h"

#ifndef NULL
#define NULL ((VOID *) 0)
#endif

#ifndef __IN
#define __IN
#endif

#ifndef __OUT
#define __OUT
#endif

#ifndef FAR
#define FAR
#endif

#ifndef NEAR
#define NEAR
#endif

#define EXPORT_SYMBOL(sym)


#ifndef EXPORT
#define EXPORT
#endif

#ifndef PUBLIC
#define PUBLIC
#endif

#ifndef STATIC
#define STATIC static
#endif

#ifndef CONST
#define CONST const
#endif


#ifndef FANSAPI
#define FANSAPI EXPORT
#endif

#ifndef EXTERN
#define EXTERN  extern
#endif

#ifndef VOLATILE
#define VOLATILE volatile
#endif

#ifndef CALLBACK
#define CALLBACK
#endif

#define MAX(x, y)                   ((x) > (y) ? (x) : (y))
#define MIN(x, y)                   ((x) < (y) ? (x) : (y))

#define LOBYTE(x)                   (BYTE)((x) & 0xff)
#define LOWORD(x)                   (WORD)((x) & 0xffff)
#define HIBYTE(x)                   (BYTE)(((x) >> 8) & 0xff)
#define HIWORD(x)                   (WORD)(((x) >> 16) & 0xffff)

#define BOOT_TASK_NAME              "Boot"
#define IDLE_TASK_NAME              "Idle"
#define OBJECT_NAME_MAX             8
#define MAX_PATH                    128
#define MAX_NAME                    128

#define SET_BIT_VALUE(x, shift, boolean)                                        \
        do{                                                                     \
            (x) &= ~(1 << (shift));                                             \
            (x) |= ((boolean) << (shift));                                      \
        }while(0)


#define SIZEOF_ARRAY(Array)                                                     \
        (sizeof(Array)/sizeof(Array[0]))

#define OFFSET_OF(Type, Member)                                                 \
        ((SIZE_T)&(((Type *)0)->Member))

#define CONTAINER_OF(Ptr, Type, Member)                                         \
        ((LPVOID)(((LPBYTE)Ptr) - OFFSET_OF(Type, Member)))


#define IS_INCLUDE(sub, parent, parlen)                                         \
        (((DWORD)(sub) >= (DWORD)(parent)) && ((DWORD)(sub) <= ((DWORD)(parent)) + parlen))
#define IS_SUBSET(sub, sublen, parent, parlen)                                  \
        (IS_INCLUDE(sub, parent, parlen) && IS_INCLUDE(((DWORD)(sub) + (sublen)), parent, parlen))

#if defined(__LITTLE_ENDIAN__)
#define __ENDIAN__                          0x1234
#elif defined (__BIT_ENDIAN__)
#define __ENDIAN__                          0x4321
#else /* __LITTLE_ENDIAN__ */
#define __ENDIAN__                          0x1234
#define __LITTLE_ENDIAN__
#endif /* __LITTLE_ENDIAN__ */

#define SLB(x, bits)                        (((x) >> (bits)) & 0xff)


#if (__ENDIAN__== 0x1234)
#define LE32_TO_CPU(x)                      (x)
#define BE32_TO_CPU(x)                      ((SLB(x, 0) << 24) +    \
                                            (SLB(x, 8) << 16) +     \
                                            (SLB(x, 16) << 8) +     \
                                            (SLB(x, 24) << 0))

#define MAKE_DWORD(a, b, c)                 (((((DWORD) (a)) << 0) + \
                                            (((DWORD) (b)) << 8) + \
                                            (((DWORD) (c)) << 16)) & 0xffffff)
#else /* __ENDIAN__== 0x4321*/
#define BE32_TO_CPU(x)                      (x)
#define LE32_TO_CPU(x)                      ((SLB(x, 0) << 24) +    \
                                            (SLB(x, 8) << 16) +     \
                                            (SLB(x, 16) << 8) +     \
                                            (SLB(x, 24) << 0))

#define MAKE_DWORD(a, b, c)                 (((((DWORD) (a)) << 16) + \
                                            (((DWORD) (b)) << 8) + \
                                            (((DWORD) (c)) << 0)) & 0xffffff)
#endif /* __ENDIAN__*/

#if defined ( __CC_ARM   )
  #define __ASM          __asm             /*!< asm keyword for ARM Compiler          */
  #define INLINE         __inline          /*!< inline keyword for ARM Compiler       */

#elif defined ( __ICCARM__ )
  #define __ASM         __asm             /*!< asm keyword for IAR Compiler          */
  #define INLINE        inline            /*!< inline keyword for IAR Compiler. Only avaiable in High optimization mode! */

#elif defined   (  __GNUC__  )
  #define __ASM          __asm            /*!< asm keyword for GNU Compiler          */
  #define INLINE         inline           /*!< inline keyword for GNU Compiler       */
#elif defined   (  __TASKING__  )
  #define __ASM          __asm            /*!< asm keyword for TASKING Compiler      */
  #define INLINE         inline           /*!< inline keyword for TASKING Compiler   */
#elif defined   (_MSC_VER)
  #define __ASM          __asm            /*!< asm keyword for vc Compiler      */
  #define INLINE        __inline          /*!< inline keyword for vc Compiler       */
  #define __attribute__(x)
#else
  #error "Unknow compiler !"
#endif

#if (ARCH == 0x8086)
#define	BUILD_CODE_TYPE_SET_FOR_8086	__asm__(".code16gcc\n");
#else
#define	BUILD_CODE_TYPE_SET_FOR_8086
#endif
#if 1
#define SECTION(x)		        __attribute__ ((section(x)))
#define	SECTION_CODE16	        __attribute__ ((section(".code16")))
#define	SECTION_CODE32	        __attribute__ ((section(".code32")))
#define	SECTION_TEXT	        __attribute__ ((section(".text")))
#define RO_CORE_CODE            __attribute__ ((section(".CoreText")))
#define RW_CORE_DATA            __attribute__ ((section(".CoreRWdata")))
#define RW_CORE_STACK           __attribute__ ((section(".CoreStack")))
#define RW_IDLE_STACK           __attribute__ ((section(".IdleStack")))
#define RW_BOOT_STACK           __attribute__ ((section(".BootStack")))
#define RW_CORE_REGION0         __attribute__ ((section(".Region0")))
#define RW_CORE_REGION1         __attribute__ ((section(".Region1")))
#define RO_CORE_DATA            __attribute__ ((section(".CoreROdata")))
#define RO_USER_CODE            __attribute__ ((section(".UserText")))
#define RO_USER_DATA            __attribute__ ((section(".UserROdata")))
#define RW_USER_DATA            __attribute__ ((section(".UserRWdata")))
#define RW_USER_STACK           __attribute__ ((section(".UserStack")))
#else
#define RO_CORE_CODE
#define RW_CORE_DATA
#define RO_CORE_DATA
#define RO_USER_CODE
#define RW_USER_DATA
#define RO_USER_DATA
#endif
#ifndef BUILD_SECTION_MODE
#include <facore.h>
#endif

#if defined(BUILD_CORE_SECTION)
#define RW_DATA                 RW_CORE_DATA
#define RO_DATA                 RO_CORE_DATA
#define RO_CODE                 RO_CORE_CODE
#elif defined(BUILD_CORE_SECTION)
#define RW_DATA                 RW_USER_DATA
#define RO_DATA                 RO_USER_DATA
#define RO_CODE                 RO_USER_CODE
#else
#define RW_DATA
#define RO_DATA
#define RO_CODE
#endif

BUILD_CODE_TYPE_SET_FOR_8086


#endif


