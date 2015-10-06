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
#ifndef __K_ARCH_H
#define __K_ARCH_H

#include <fadefs.h>
#include <fatypes.h>

#include "kstack.h"

typedef volatile unsigned char SPIN_LOCK_T;
typedef volatile unsigned char * PSPIN_LOCK_T;
typedef volatile unsigned char FAR * LPSPIN_LOCK_T;

typedef union tagINSTRUCTION_BIT_FIELD INSTRUCTION_BIT_FIELD;
typedef union tagINSTRUCTION_BIT_FIELD * PINSTRUCTION_BIT_FIELD;
typedef union tagINSTRUCTION_BIT_FIELD FAR * LPINSTRUCTION_BIT_FIELD;

union tagINSTRUCTION_BIT_FIELD{
    struct{
        WORD        Imm:11;
        WORD        OpCode:5;
    }iucb;
    struct{
        WORD        Other:13;
        WORD        Mark:3;
    }i32b;
};

typedef struct tagSTACK_DUMP STACK_DUMP;
typedef struct tagSTACK_DUMP * PSTACK_DUMP;
typedef struct tagSTACK_DUMP FAR * LPSTACK_DUMP;

struct tagSTACK_DUMP
{
    DWORD           R4;
    DWORD           R5;
    DWORD           R6;
    DWORD           R7;
    DWORD           R8;
    DWORD           R9;
    DWORD           R10;
    DWORD           R11;
    DWORD           ALIGN;
    DWORD           BP;             /* IRQ Break */
    DWORD           R0;
    DWORD           R1;
    DWORD           R2;
    DWORD           R3;
    DWORD           R12;
    DWORD           LR;
    DWORD           PC;
    DWORD           xPSR;
};

typedef struct tagARCH_CONTEXT ARCH_CONTEXT;
typedef struct tagARCH_CONTEXT * PARCH_CONTEXT;
typedef struct tagARCH_CONTEXT FAR * LPARCH_CONTEXT;


struct tagARCH_CONTEXT{
    STACK_DESCRIPTOR            Stack[TASK_PERMISSION_MAX];
};

#define     GetArchSD(lpAC, Permission)     (&((lpAC)->Stack[Permission]))
#define     GetArchUserSD(lpAC)             (&((lpAC)->Stack[TASK_PERMISSION_USER]))
#define     GetArchCoreSD(lpAC)             (&((lpAC)->Stack[TASK_PERMISSION_CORE]))

#ifdef __cplusplus
extern "C" {
#endif
    PUBLIC VOID CORE_SystemHang(VOID);
    PUBLIC VOID CORE_EnableIRQ(VOID);
    PUBLIC VOID CORE_RestoreIRQ(DWORD dwFlags);
    PUBLIC DWORD CORE_DisableIRQ(VOID);
    PUBLIC DWORD CORE_SaveIRQFlags(VOID);
    PUBLIC E_STATUS CORE_Switch2UserMode(VOID);
    PUBLIC LPVOID CORE_FillStack(LPVOID Position, LPVOID fnMain, LPVOID lpArgument,
                                 LPVOID lpTaskContext, HANDLE hTask,
                                 E_TASK_PERMISSION Permission);
    PUBLIC VOID CORE_SetArchContextParam(LPARCH_CONTEXT lpArchContext, LPVOID lpParam);

#define     CORE_SPIN_LOCK(lock)                    CORE_SpinLock(&lock)
#define     CORE_SPIN_UNLOCK(lock)                  CORE_SpinUnlock(&lock)
#define     CORE_SPIN_LOCK_IRQ(lock, flags)         do { flags = CORE_DisableIRQ(); } while(0)
#define     CORE_SPIN_UNLOCK_IRQ(lock, flags)       do { CORE_RestoreIRQ(flags); } while(0)
    
    PUBLIC VOID CORE_SpinLock(LPSPIN_LOCK_T Lock);
    PUBLIC VOID CORE_SpinUnlock(LPSPIN_LOCK_T Lock);
    PUBLIC VOID CORE_SpinLockIRQ(LPSPIN_LOCK_T Lock, DWORD_PTR dwFlags);
    PUBLIC VOID CORE_SpinUnlockIRQ(LPSPIN_LOCK_T Lock, DWORD_PTR dwFlags);
    
#ifdef __cplusplus
}
#endif

#endif

