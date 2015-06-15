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

#include "birq.h"

#ifdef __cplusplus
extern "C" {
#endif
#define     BSP_GetRegionID(x)                  0
    PUBLIC LPSTR BSP_GetBoardName(VOID);
    PUBLIC VOID BSP_DebugWriteByte(DWORD Data);
    PUBLIC LPSTR BSP_GetIRQNameString(E_IRQ_ID Id);
#if (CONFIG_MEM_REGION_MAX != 0)
    PUBLIC E_STATUS BSP_ScanMemoryRegion(VOID);
#else
#define     BSP_ScanMemoryRegion()
#endif

#ifdef __cplusplus
}
#endif

#endif

