#include <string.h>
#include <fadefs.h>
#include <faerror.h>
#include <fatypes.h>

#include "khash.h"
#include "kcore.h"
#include "kdebug.h"

#ifdef __DEBUG__

STATIC LONG CheckRepeat(BYTE * Array,LONG Count, LONG * Result)
{
    LONG i = 0, j = 0;

    for (i = 0; i < Count; i++)
    {
        for (j = 0; j < Count; j++)
        {
            if (i != j && Array[i] == Array[j])
            {
                *Result = j;
                return i;
            }
        }
    }

    return -1;
}

PUBLIC DWORD CORE_HashSetArray(FNHASHLOADMAGIC fnLoadMagic, LPVOID lpPrivate,
                               LPDWORD MagicArray, DWORD LastMagic, DWORD Count)
{
    DWORD i = 0, j = 0;
    
    memset(MagicArray, 0, sizeof(DWORD) * Count);

    for (i = 0; i < Count; i ++)
    {
        MagicArray[j] = fnLoadMagic(lpPrivate, i);
        j += (!!MagicArray[j]);
    }
    
    if (j < Count - 1)
    {
        MagicArray[j] = LastMagic;
    }
    else
    {
        CORE_ERROR(TRUE, "Please check you array size.");
        SYSTEM_CALL_OOPS();
    }
    
    return j+1;
}

PUBLIC VOID CORE_HashDebug(FNHASHKEY fnHashKey, LPDWORD MagicArray, DWORD Count,
                            DWORD DivLow, DWORD DivHigh, LPCSTR Config, LPCSTR HashFunc)
{
    LONG Repaet0 = -1, Repaet1 = -1;
    DWORD Divider = 0, Id = 0;
    BYTE ModArray[256] = {0};

    if (Count >= sizeof(ModArray))
    {
        CORE_ERROR(TRUE, "Hash count(%d) error !", Count);
        SYSTEM_CALL_OOPS();
    }

    for (Divider = DivLow; Divider <= DivHigh; Divider ++)
    {
        CORE_INFOR(TRUE, "*****Divider %d check *******", Divider);
        
        for (Id = 0; Id < Count; Id ++)
        {
            DWORD Magic = MagicArray[Id];
            LPSTR lpStr = (LPVOID) &Magic;

            ModArray[Id] = (fnHashKey(Magic) % Divider);
            
            CORE_INFOR(TRUE, "Magic 0x%08X(%s) key is %u, result is %u.",
                Magic, lpStr, fnHashKey(Magic), ModArray[Id]);
        }
        
        if (-1 != (Repaet0 = CheckRepeat(ModArray,Count, &Repaet1)))
        {
            DWORD Magic0 = MagicArray[Repaet0];
            DWORD Magic1 = MagicArray[Repaet1];
            LPSTR lpStr0 = (LPVOID) &Magic0;
            LPSTR lpStr1 = (LPVOID) &Magic1;
            
            CORE_INFOR(TRUE, "Magic0 0x%08X(%s) key is %u, result is %u.",
                Magic0, lpStr0, fnHashKey(Magic0), ModArray[Repaet0]);
            CORE_INFOR(TRUE, "Magic1 0x%08X(%s) key is %u, result is %u.",
                Magic1, lpStr1, fnHashKey(Magic1), ModArray[Repaet1]);
        }
        else
        {
            CORE_INFOR(TRUE, "**** Please set '%s' to '%d' ****", Config, Divider);
            SYSTEM_CALL_OOPS();
        }
    }

    CORE_INFOR(TRUE, "**** '%s' check failed, please modify "
                     "hash function '%s' ****", Config, HashFunc);
    SYSTEM_CALL_OOPS();
}

#endif
