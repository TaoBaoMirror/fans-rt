//#include <string.h>
//#include <stdio.h>
#include <stm32f10x.h>
#include <fadefs.h>
#include <faerror.h>
#include <fatypes.h>

#include "birq.h"

PUBLIC LPSTR BSP_GetBoardName(VOID)
{
    return "stm32-mini";
}

PUBLIC LPSTR BSP_GetIRQNameString(E_IRQ_ID Id)
{
    switch(Id)
    {
    case E_CORE_SYSTICK_IRQ_ID:
        return _TEXT("SysTick");
    case E_CORE_SYSCALL_IRQ_ID:
        return _TEXT("caSystemCall");
    default:
        return _TEXT("Unknow");
    }
}

#if (CONFIG_MEM_REGION_MAX != 0)
PUBLIC E_STATUS BSP_ScanMemoryRegion(VOID)
{
    return STATE_SUCCESS;
}
#endif

