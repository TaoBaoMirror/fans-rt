/*
 *    Copyright(C) 2013-2015, Fans-rt development team.
 *
 *    All rights reserved.
 *
 *    This is open source software.
 *    Learning and research can be unrestricted to  modification, use and dissemination.
 *    If you need for commercial purposes, you should get the author's permission.
 *
 *    date           author          notes
 *    2014-09-07     JiangYong       new file
 */
#include <fadefs.h>
#include <faerror.h>
#include <fatypes.h>

#include "kmem.h"
#include "khash.h"
#include "ktask.h"
#include "kcore.h"
#include "kpool.h"
#include "kdebug.h"
#include "ktable.h"

STATIC E_STATUS CreatePool(LPCORE_POOL lpCorePool)
{
#if (CONFIG_MEM_REGION_MAX == 0)
    return STATE_OUT_OF_MEMORY;
#else
    SIZE_T Length = 0;
    LPVOID lpContainer = NULL;
    
    if (INVALID_MAGIC != GetPoolMagic(lpCorePool))
    {
        CORE_DEBUG(TRUE, "Create pool failed !");
        return STATE_EXISTING;
    }
    
    Length = GetPoolTotalBlocks(lpCorePool) * GetPoolBlockLength(lpCorePool);

    lpContainer = CORE_PageAlloc(Length);
    
    SetPoolBuffer(lpCorePool, lpContainer);
    
    if (NULL == GetPoolBuffer(lpCorePool))
    {
        CORE_DEBUG(TRUE, "Malloc %d bytes to create pool failed, container is 0x%p !", Length, lpContainer);
        
        return STATE_OUT_OF_MEMORY;
    }

    SetPoolMagic(lpCorePool, POL_MAGIC);

    return STATE_SUCCESS;
#endif
}


EXPORT VOID CORE_CreatePoolContainer(LPCORE_CONTAINER lpManager, LPCSTR lpName,
                                LPBYTE lpTable, BYTE Pools, BYTE BlockPrePool, 
                                SIZE_T BytePreBlock, BOOL SearchForHash)
{
    BYTE Pid, Bid;
    
    memset(lpManager, 0, sizeof(CORE_CONTAINER));
    
    SetContainerName(lpManager, lpName);
    SetTotalPools(lpManager, Pools);
    SetBlocksPrePool(lpManager, BlockPrePool);
    
    CORE_INFOR(TRUE, "Create manager '%s', have pools %d, entries pre pool %d.",
            GetContainerName(lpManager), GetTotalPools(lpManager), GetBlocksPrePool(lpManager));
    
    for (Pid = 0; Pid < CONFIG_CORE_POOL_MAX; Pid ++)
    {
        LPCORE_POOL lpCorePool = GetPoolForID(lpManager, Pid);

        AddContainerBitmap(lpManager, ((MANA_MAP_T) (1 << Pid)));
        SetPoolMagic(lpCorePool, POOL_INITIALIZE_MAGIC);
        SetPoolBuffer(lpCorePool, POOL_INITIALIZE_TABLE(lpTable, Pid, BytePreBlock));
        SetPoolTotalBlocks(lpCorePool, BlockPrePool);
        SetPoolRemainBlocks(lpCorePool, BlockPrePool);
        SetPoolBlockLength(lpCorePool, BytePreBlock);

        for (Bid = 0; Bid < CONFIG_POOL_BLOCK_MAX; Bid ++)
        {
            AddPoolBitmap(lpCorePool, ((POOL_MAP_T) (1 << Bid)));
        }
    }
}
EXPORT_SYMBOL(CORE_CreatePoolContainer)

EXPORT KCONTAINER_ID_T CORE_PoolMallocBlock(LPCORE_CONTAINER lpManager)
{
    BYTE Pid, Bid;
    LPCORE_POOL lpCorePool = NULL;

    Pid = GetFreePoolID(lpManager);

    CORE_DEBUG(TRUE, "Malloc '0x%08X' in manager '%s' ...", Magic, GetContainerName(lpManager));
    
    if (Pid >= GetTotalPools(lpManager))
    {
        CORE_ERROR(TRUE, "The pool id(%d) out of range(%d).", Pid, GetTotalPools(lpManager));
        return INVALID_CONTAINER_ID;
    }
    
    lpCorePool = GetPoolForID(lpManager, Pid);
    
    if (INVALID_MAGIC == GetPoolMagic(lpCorePool))
    {
        if (STATE_SUCCESS != CreatePool(lpCorePool))
        {
            CORE_ERROR(TRUE, "Invalid TSS magic %p.", lpCorePool);
            return INVALID_CONTAINER_ID;
        }
    }
    
    if (POL_MAGIC != GetPoolMagic(lpCorePool))
    {
        CORE_ERROR(TRUE, "Invalid pool magic %p.", lpCorePool);
        return INVALID_CONTAINER_ID;
    }

    Bid = GetFreeBlockID(lpCorePool);
 
    if (Bid >= GetPoolTotalBlocks(lpCorePool))
    {
        CORE_ERROR(TRUE, "Can not found free block in pool %d, bitmap is 0x%08x.",
            Pid, GetPoolBitmap(lpCorePool));
        return INVALID_CONTAINER_ID;
    }

    SubPoolBitmap(lpCorePool, 1 << Bid);
    SetContainerBitmap(lpManager, Pid, TRUE && GetPoolBitmap(lpCorePool));
    
    return GetContainerID(lpManager, Pid, Bid);
}
EXPORT_SYMBOL(CORE_PoolMallocMapFast)

EXPORT LPVOID CORE_PoolTakeBlock(LPCORE_CONTAINER lpManager, KCONTAINER_ID_T Kid)
{
    LPCORE_POOL lpCorePool;
    BYTE Pid = ContainerID2PoolID(lpManager, Kid);
    BYTE Bid = ContainerID2BlockID(lpManager, Kid);

    if (Pid >= GetTotalPools(lpManager))
    {
        CORE_ERROR(TRUE, "The handle pid(%d) max than manager total(%d) pools.",
            Pid, GetTotalPools(lpManager));
        return NULL;
    }
    
    lpCorePool = GetPoolForID(lpManager, Pid);
    
    if (POL_MAGIC != GetPoolMagic(lpCorePool))
    {
        CORE_ERROR(TRUE, "Invalid pool magic %p.", lpCorePool);
        return NULL;
    }
    
    if (Bid >= GetPoolTotalBlocks(lpCorePool))
    {
        CORE_ERROR(TRUE, "The handle bid(%d) max than pool total(%d) items.",
            Bid, GetPoolTotalBlocks(lpCorePool));
        return NULL;
    }

    return GetPoolBlockForID(lpCorePool, Bid);

}

PUBLIC E_STATUS CORE_PoolFreeBlock(LPCORE_CONTAINER lpManager, KCONTAINER_ID_T Kid)
{
    LPCORE_POOL lpCorePool;
    BYTE Pid = ContainerID2PoolID(lpManager, Kid);
    BYTE Bid = ContainerID2BlockID(lpManager, Kid);
    
    lpCorePool = GetPoolForID(lpManager, Pid);
    
    if (POL_MAGIC != GetPoolMagic(lpCorePool))
    {
        CORE_DEBUG(TRUE, "Invalid pool magic %p.", lpCorePool);
        return STATE_SYSTEM_FAULT;
    }

    AddPoolBitmap(lpCorePool, 1 << Bid);
    SetContainerBitmap(lpManager, Pid, TRUE && GetPoolBitmap(lpCorePool));

    return STATE_SUCCESS;
}

