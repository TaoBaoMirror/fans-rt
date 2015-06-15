#include <string.h>
#include <stdio.h>
#include <fadefs.h>
#include <faerror.h>
#include <fatypes.h>

#define     SYSTICK_BASE_REGISTER               (0xE000E000 + 0x0010 + 0x4)
#define     SYSTICK_TIME_REGISTER               (0xE000E000 + 0x0010 + 0x8)

PUBLIC DWORD FW_GetTickCount(VOID)
{
    return *((VOLATILE DWORD *) SYSTICK_TIME_REGISTER);
}

PUBLIC DWORD FW_GetTickBase(VOID)
{
    return *((VOLATILE DWORD *) SYSTICK_BASE_REGISTER);
}

