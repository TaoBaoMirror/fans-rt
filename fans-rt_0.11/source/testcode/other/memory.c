#if 0
#include <fadefs.h>
#include <faklist.h>
#include <faerror.h>
#include <fatypes.h>
#include <fanscore.h>
#include <kdebug.h>
#include <fapi.h>
#include <fapp.h>

#define MEMORY_BLOCKS 10
STATIC LPVOID * * Array = NULL;
STATIC LPVOID * * Buffer = NULL;

DWORD Count = 0;
DWORD ABC[32];

E_STATUS FansMain(VOID)
{
    int i = 0;
    int j = 0;
    LPDWORD TestDword = (LPDWORD) 0x8000000;
    
    DWORD Tick1 = 0;
    DWORD Tick0 = FW_GetTickCount();
    
    for (i = 0; i < 32; i ++)
    {
        ABC[i] = 0;
    }
    
    Tick1 = FW_GetTickCount();
    
    CORE_INFOR(TRUE, "Tick0 = %d  Tick1 = %d", Tick0, Tick1);
    
    Array = MemoryAllocate(MEMORY_BLOCKS * sizeof(LPVOID) * 2);
    Buffer = Array + MEMORY_BLOCKS;
    
    CORE_INFOR(TRUE, "Array buffer %p.", Array);
    
    for (i = 0; i < MEMORY_BLOCKS; i ++)
    {
        Array[i] = MemoryAllocate(1);
        CORE_INFOR(TRUE, "Alloc buffer id %d pointer is %p.", i, Array[i]);
    }
    
    CORE_INFOR(TRUE, "Start to test memory...", Array);
    
    for (j = 0; j < 10; j ++)
    {
        for (i = 0; i < MEMORY_BLOCKS; i ++)
        {
            Buffer[i] = MemoryAllocate(1);
        }

        for (i = 0; i < MEMORY_BLOCKS; i ++)
        {
            MemoryFree(Buffer[i]);
        }
    }
    
    CORE_INFOR(TRUE, "Memory test finish !");
    
    * TestDword = 1;
    
    return STATE_SUCCESS;
}
#endif
