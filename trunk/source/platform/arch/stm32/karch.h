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

typedef struct tagIRQ_REGISTER_STACK IRQ_REGISTER_STACK;
typedef struct tagIRQ_REGISTER_STACK * PIRQ_REGISTER_STACK;
typedef struct tagIRQ_REGISTER_STACK FAR * LPIRQ_REGISTER_STACK;

struct tagIRQ_REGISTER_STACK
{
    DWORD           R4;
    DWORD           R5;
    DWORD           R6;
    DWORD           R7;
    DWORD           R8;
    DWORD           R9;
    DWORD           R10;
    DWORD           R11;
    DWORD           LR1;
    DWORD           R0;
    DWORD           R1;
    DWORD           R2;
    DWORD           R3;
    DWORD           R12;
    DWORD           LR0;
    DWORD           R15;
    DWORD           xPSR;
    LPVOID          SP;
};

#ifdef __cplusplus
extern "C" {
#endif
#if (CONFIG_ARCH_SUPPORT_SCHEDULE == TRUE)
    PUBLIC VOID CORE_ActiveSwitchIRQ(VOID);
#else
#define     CORE_ActiveSwitchIRQ()
#endif
    PUBLIC VOID CORE_SystemHang(VOID);
    PUBLIC VOID CORE_EnableIRQ(VOID);
    PUBLIC VOID CORE_RestoreIRQ(DWORD dwFlags);
    PUBLIC DWORD CORE_DisableIRQ(VOID);
    PUBLIC DWORD CORE_SaveIRQFlags(VOID);
    PUBLIC LPVOID CORE_GetBootStackBuffer(VOID);
    PUBLIC E_STATUS CORE_EnableKernelStack(LPVOID lpKStack);
    PUBLIC LPVOID CORE_FillStack(LPVOID Position, LPVOID Entry, LPVOID lpArgument, HANDLE hTask);
#ifdef __cplusplus
}
#endif

#endif

