#ifndef __FANS_BSP_H
#define __FANS_BSP_H

#include <fadefs.h>
#include <fatypes.h>
#include <faerror.h>

#include "birq.h"

#ifdef __cplusplus
extern "C" {
#endif
#define     BSP_GetRegionID(x)                  0
    PUBLIC LPSTR CORE_GetBoardName(VOID);
    PUBLIC VOID CORE_DebugWriteByte(DWORD Data);
    PUBLIC LPSTR CORE_GetIRQNameString(E_IRQ_ID Id);
#if (CONFIG_MEM_REGION_MAX != 0)
    PUBLIC E_STATUS BSP_ScanMemoryRegion(VOID);
#else
#define     BSP_ScanMemoryRegion()
#endif

#ifdef __cplusplus
}
#endif

#endif

