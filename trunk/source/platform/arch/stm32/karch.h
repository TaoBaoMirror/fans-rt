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
    PUBLIC DWORD CORE_GetCPUNumbers(VOID);
    STATIC BOOL CORE_CpuSupportGlobalCoreStack(VOID) { return TRUE; }
    PUBLIC E_STATUS CORE_Switch2UserMode(VOID);
    PUBLIC LPVOID CORE_FillStack(LPVOID Position, LPVOID Entry, LPVOID lpArgument, HANDLE hTask);
    PUBLIC VOID CORE_SetArchContextParam(LPARCH_CONTEXT lpArchContext, LPVOID lpParam);
#ifdef __cplusplus
}
#endif

#endif

