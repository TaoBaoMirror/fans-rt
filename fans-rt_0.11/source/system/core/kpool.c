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
#include <string.h>
#include <fadefs.h>
#include <faerror.h>
#include <fatypes.h>

#include "kmem.h"
#include "kcore.h"
#include "kpool.h"
#include "kdebug.h"
#include "ktable.h"

#if (KPOOL_DEBUG_ENABLE == TRUE)
#define     KPOOL_DEBUG(Enter, ...)                            CORE_DEBUG(Enter, __VA_ARGS__)
#define     KPOOL_INFOR(Enter, ...)                            CORE_INFOR(Enter, __VA_ARGS__)
#define     KPOOL_ERROR(Enter, ...)                            CORE_ERROR(Enter, __VA_ARGS__)
#ifndef _MSC_VER
#define     KPOOL_ASSERT(condition, code, ...)                 CORE_ASSERT(condition, code, __VA_ARGS__)
#else
#define     KPOOL_ASSERT(condition, code, ...)
#endif
#else
#define     KPOOL_DEBUG(...)
#define     KPOOL_INFOR(...)
#define     KPOOL_ERROR(...)
#define     KPOOL_ASSERT(condition, code, ...)
#endif

STATIC E_STATUS CreatePool(LPCORE_POOL lpCorePool)
{
#if (CONFIG_MEM_REGION_MAX == 0)
    return STATE_OUT_OF_MEMORY;
#else
    SIZE_T Length = 0;
    LPVOID lpContainer = NULL;
    
    KPOOL_DEBUG(TRUE, "Create core pool %p.", lpCorePool);
    
    if (INVALID_MAGIC != GetPoolMagic(lpCorePool))
    {
        KPOOL_DEBUG(TRUE, "Create pool failed !");
        return STATE_EXISTING;
    }
    
    Length = GetPoolTotalBlocks(lpCorePool) * GetPoolBlockLength(lpCorePool);

    lpContainer = CORE_PageAlloc(Length);
    
    if (NULL == lpContainer)
    {
        KPOOL_DEBUG(TRUE, "Malloc %d bytes to create pool failed, container is 0x%p !",
                Length, lpContainer);
        
        return STATE_OUT_OF_MEMORY;
    }

    SetPoolBuffer(lpCorePool, lpContainer);
    SetPoolMagic(lpCorePool, POL_MAGIC);

    return STATE_SUCCESS;
#endif
}


EXPORT E_STATUS CORE_CreatePoolContainer(LPCORE_CONTAINER lpManager, LPCSTR lpName,
                                BYTE_PTR lpTable, BYTE Pools, BYTE BlockPrePool, 
                                SIZE_T BytePreBlock, BOOL AllocForInit)
{
    KPOOL_ID_T Pid = 0;

    memset(lpManager, 0, sizeof(CONTAINER_ATTRIBUTE) + sizeof(CORE_POOL) * Pools);
    
    SetContainerName(lpManager, lpName);
    SetTotalPools(lpManager, Pools);
    SetBlocksPrePool(lpManager, BlockPrePool);
    SetFreeBitmapMask2Container(lpManager, GetBitsMaskValue(Pools));
    
    for (; Pid < Pools; Pid ++)
    {
        LPCORE_POOL lpCorePool = GetPoolForID(lpManager, Pid);

        SetPoolMagic(lpCorePool, POOL_INITIALIZE_MAGIC);
        
        SetPoolTotalBlocks(lpCorePool, BlockPrePool);
        SetPoolRemainBlocks(lpCorePool, BlockPrePool);
        SetPoolBlockLength(lpCorePool, BytePreBlock);
        
        SetFreeBitmapMask2Pool(lpCorePool, GetBitsMaskValue(BlockPrePool));

        if (NULL != lpTable)
        {
            SetPoolBuffer(lpCorePool, POOL_INITIALIZE_TABLE(lpTable, Pid, BytePreBlock));
        }
        else
        {
            E_STATUS Result;
            
            if (TRUE == AllocForInit && STATE_SUCCESS != (Result = CreatePool(lpCorePool)))
            {
                KPOOL_ERROR(TRUE, "Malloc page to create pool '%s' failed.",
                    GetContainerName(lpManager));
                return Result;
            }
        }
        
        KPOOL_INFOR(TRUE, "Create pool %d(0x%P) for '%s'(0x%P), have blocks %d.",
                            Pid, lpCorePool, GetContainerName(lpManager), 
                            lpManager, GetBlocksPrePool(lpManager));
    }
    
    return STATE_SUCCESS;
}
EXPORT_SYMBOL(CORE_CreatePoolContainer)

EXPORT KCONTAINER_ID_T CORE_PoolMallocBlock(LPCORE_CONTAINER lpManager)
{
    KPOOL_ID_T Pid;
    KBLOCK_ID_T Bid;
    KCONTAINER_ID_T Kid;
    LPCORE_POOL lpCorePool = NULL;

    Pid = GetFreePoolID(lpManager);

    KPOOL_DEBUG(TRUE, "Malloc block in manager '%s' ...", GetContainerName(lpManager));
    
    if (Pid >= GetTotalPools(lpManager))
    {
        KPOOL_ERROR(TRUE, "The pool id(%d) out of range(%d).", Pid, GetTotalPools(lpManager));
        return INVALID_CONTAINER_ID;
    }
    
    lpCorePool = GetPoolForID(lpManager, Pid);
    
    if (INVALID_MAGIC == GetPoolMagic(lpCorePool))
    {
        if (STATE_SUCCESS != CreatePool(lpCorePool))
        {
            KPOOL_ERROR(TRUE, "Invalid pool magic %p.", lpCorePool);
            return INVALID_CONTAINER_ID;
        }
    }
    
    if (POL_MAGIC != GetPoolMagic(lpCorePool))
    {
        KPOOL_ERROR(TRUE, "Invalid pool magic %p.", lpCorePool);
        return INVALID_CONTAINER_ID;
    }

    Bid = GetFreeBlockID(lpCorePool);
 
    if (Bid >= GetPoolTotalBlocks(lpCorePool))
    {
        KPOOL_ERROR(TRUE, "No free block in pool %d for '%s', bitmap(0x%08x), Bid(%d).",
            Pid, GetContainerName(lpManager), GetFreeBitmapMask4mPool(lpCorePool), Bid);
        return INVALID_CONTAINER_ID;
    }

    GetBlock4mPoolFreeBitmap(lpCorePool, Bid);
    SetFreeBitmap4mPool2Container(lpManager, Pid, !!GetFreeBitmapMask4mPool(lpCorePool));
    
    Kid = GetContainerID(lpManager, Pid, Bid);

    KPOOL_DEBUG(TRUE, "Malloc pool 0x%P Pid %u, Bid %u, Kid %u, for container '%s' ...",
            lpCorePool, Pid, Bid, Kid, GetContainerName(lpManager));
    
    return Kid;
}
EXPORT_SYMBOL(CORE_PoolMallocMapFast)

EXPORT LPVOID CORE_PoolTakeBlock(LPCORE_CONTAINER lpManager, KCONTAINER_ID_T Kid)
{
    LPCORE_POOL lpCorePool;
    KPOOL_ID_T Pid = ContainerID2PoolID(lpManager, Kid);
    KBLOCK_ID_T Bid = ContainerID2BlockID(lpManager, Kid);

    if (Pid >= GetTotalPools(lpManager))
    {
        KPOOL_ERROR(TRUE, "The handle pid(%d) max than manager total(%d) pools.",
            Pid, GetTotalPools(lpManager));
        return NULL;
    }
    
    lpCorePool = GetPoolForID(lpManager, Pid);
    
    if (POL_MAGIC != GetPoolMagic(lpCorePool))
    {
        KPOOL_ERROR(TRUE, "Invalid pool magic %p.", lpCorePool);
        return NULL;
    }
    
    if (Bid >= GetPoolTotalBlocks(lpCorePool))
    {
        KPOOL_ERROR(TRUE, "The handle bid(%d) max than pool total(%d) items.",
            Bid, GetPoolTotalBlocks(lpCorePool));
        return NULL;
    }

    return GetPoolBlockForID(lpCorePool, Bid);

}

PUBLIC E_STATUS CORE_PoolFreeBlock(LPCORE_CONTAINER lpManager, KCONTAINER_ID_T Kid)
{
    LPCORE_POOL lpCorePool;
    KPOOL_ID_T Pid = ContainerID2PoolID(lpManager, Kid);
    KBLOCK_ID_T Bid = ContainerID2BlockID(lpManager, Kid);
    
    if (Pid >= GetTotalPools(lpManager))
    {
        return STATE_OVER_RANGE;
    }
    
    lpCorePool = GetPoolForID(lpManager, Pid);
    
    if (POL_MAGIC != GetPoolMagic(lpCorePool))
    {
        KPOOL_ERROR(TRUE, "Container %s(0x%P) pool %p Pid %u, Bid %u, Kid %u magic invalid.",
            GetContainerName(lpManager), lpManager, lpCorePool, Pid, Bid, Kid);
        return STATE_SYSTEM_FAULT;
    }

    PutBlock2PoolFreeBitmap(lpCorePool, Bid);
    SetFreeBitmap4mPool2Container(lpManager, Pid,  !!GetFreeBitmapMask4mPool(lpCorePool));

    return STATE_SUCCESS;
}

