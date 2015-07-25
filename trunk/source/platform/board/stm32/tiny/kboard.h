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
#ifndef __K_BOARD_H
#define __K_BOARD_H

#include <fadefs.h>
#include <fatypes.h>
#include <faerror.h>

#ifdef __cplusplus
extern "C" {
#endif
#define     CORE_GetRegionID(x)                  0
    PUBLIC LPCSTR CORE_GetBoardName(VOID);
    PUBLIC VOID CORE_DebugWriteByte(DWORD Data);
    PUBLIC LPCSTR CORE_GetIRQNameString(DWORD IrqID);

    PUBLIC VOID CORE_ActiveSwitchIRQ(VOID);
    PUBLIC VOID CORE_InitBoardFirst(VOID);
#define     CORE_GetBootStackLength()     CONFIG_BOOT_STACK_SIZE
#define     CORE_GetIdleStackLength()     CONFIG_IDLE_STACK_SIZE
    PUBLIC LPVOID CORE_GetCoreStackBuffer(VOID);
    PUBLIC SIZE_T CORE_GetCoreStackLength(VOID);
    PUBLIC LPVOID CORE_GetBootStackBuffer(VOID);
    PUBLIC LPVOID CORE_GetIdleStackBuffer(DWORD CpuID);
#define     CORE_ScanMemoryRegion()         STATE_SUCCESS
#ifdef __cplusplus
}
#endif

#endif

