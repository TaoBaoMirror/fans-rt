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
#ifndef __K_POOL_H
#define __K_POOL_H

#include <fadefs.h>
#include <faerror.h>
#include <fatypes.h>

#include "ktable.h"

typedef struct tagCORE_POOL CORE_POOL;
typedef struct tagCORE_POOL * PCORE_POOL;
typedef struct tagCORE_POOL FAR * LPCORE_POOL;
typedef struct tagCORE_CONTAINER CORE_CONTAINER;
typedef struct tagCORE_CONTAINER * PCORE_CONTAINER;
typedef struct tagCORE_CONTAINER FAR * LPCORE_CONTAINER;

#if (CONFIG_POOL_BLOCK_MAX > 16)
typedef     DWORD                       POOL_MAP_T;
#define     GetFreeBlockID(lpP)          GetDwordLowestBit((lpP)->PoolBitmap)
#elif (CONFIG_POOL_BLOCK_MAX > 8)
typedef     WORD                        POOL_MAP_T;
#define     GetFreeBlockID(lpP)          GetWordLowestBit((lpP)->PoolBitmap)
#else
typedef     BYTE                        POOL_MAP_T;
#define     GetFreeBlockID(lpP)          GetByteLowestBit((lpP)->PoolBitmap)
#endif

#if (CONFIG_CORE_POOL_MAX > 16)
typedef     DWORD                       MANA_MAP_T;
#define     GetFreePoolID(lpM)          GetDwordLowestBit((lpM)->Attribute.ContainerBitmap)
#elif (CONFIG_CORE_POOL_MAX > 8)
typedef     WORD                        MANA_MAP_T;
#define     GetFreePoolID(lpM)          GetWordLowestBit((lpM)->Attribute.ContainerBitmap)
#elif (CONFIG_CORE_POOL_MAX > 1)
typedef     BYTE                        MANA_MAP_T;
#define     GetFreePoolID(lpM)          GetByteLowestBit((lpM)->Attribute.ContainerBitmap)
#else
typedef     BYTE                        MANA_MAP_T;
#define     GetFreePoolID(lpM)          0
#endif

struct tagCORE_POOL{
    DWORD               Magic;
    BYTE                TotalBlocks;
    BYTE                RemainBlocks;
    POOL_MAP_T          PoolBitmap;                                 /**< 位图-记录空闲项 */
    SIZE_T              BlockLength;
    LPBYTE              lpBuffer;
};

typedef struct tagCONTAINER_ATTRIBUTE{
    LPCSTR              lpContainerName;                            /**< 管理器名字 */
    BYTE                BlocksPrePool;                              /**< 每个POOL的BLOCK数量 */
    BYTE                TotalPools;                                 /**< 管理的POOL数量 */
    BOOL                SearchForHash:1;                            /**< 使用HASH搜索 */
#if (CONFIG_CORE_POOL_MAX > 8)
    BYTE                Reserved;
    MANA_MAP_T          ContainerBitmap;                            /**< 位图-记录空闲POOL */
#elif (CONFIG_CORE_POOL_MAX > 1)
    MANA_MAP_T          ContainerBitmap;                            /**< 位图-记录空闲POOL */
#else
    BYTE                Reserved;
#endif
}CONTAINER_ATTRIBUTE;

struct tagCORE_CONTAINER{
    CONTAINER_ATTRIBUTE Attribute;
    CORE_POOL           Pool[CONFIG_CORE_POOL_MAX];                 /**< Pools */
};

typedef unsigned long KCONTAINER_ID_T;

#define     INVALID_CONTAINER_ID                    INVALID_VALUE


#define     GetContainerID(lpM, Pid, Bid)           (GetBlocksPrePool(lpM) * (Pid) + (Bid))
#define     ContainerID2BlockID(lpM, Kid)           ((Kid) % GetBlocksPrePool(lpM))
#define     ContainerID2PoolID(lpM, Kid)            ((Kid) / GetBlocksPrePool(lpM))

#define     GetPoolMagic(lpP)                       ((lpP)->Magic)
#define     SetPoolMagic(lpP, M)                    do {(lpP)->Magic = (M);} while(0)
#define     GetPoolBuffer(lpP)                      ((lpP)->lpBuffer)
#define     SetPoolBuffer(lpP, lpTable)             do {(lpP)->lpBuffer = (lpTable);} while(0)
#define     GetPoolTotalBlocks(lpP)                 ((lpP)->TotalBlocks)
#define     SetPoolTotalBlocks(lpP, Items)          do {(lpP)->TotalBlocks = (Items);} while(0)
#define     GetPoolRemainBlocks(lpP)                ((lpP)->RemainBlocks)
#define     SetPoolRemainBlocks(lpP, Items)         do {(lpP)->RemainBlocks = (Items);} while(0)
#define     GetPoolBlockLength(lpP)                 ((lpP)->BlockLength)
#define     SetPoolBlockLength(lpP, Size)           do {(lpP)->BlockLength = (Size);} while(0)

#define     GetPoolForID(lpM, Id)                   (&(lpM)->Pool[Id])
#define     GetPoolBlockForID(lpP, Id)              ((LPVOID)&(lpP)->lpBuffer[(Id) * GetPoolBlockLength(lpP)])
#define     GetPoolBitmap(lpP)                      ((lpP)->PoolBitmap)
#define     AddPoolBitmap(lpP, Mask)                (((lpP)->PoolBitmap) |= (Mask))
#define     SubPoolBitmap(lpP, Mask)                (((lpP)->PoolBitmap) ^= (Mask))
#define     CheckBlockIsFree(lpM, Pid, Eid)                                     \
            (((lpM)->Pool[Pid].PoolBitmap & (1 << (Eid))) != 0)

#define     GetContainerName(lpM)                   ((lpM)->Attribute.lpContainerName)
#define     SetContainerName(lpM, Name)             do {(lpM)->Attribute.lpContainerName = (Name);} while(0)
#define     GetTotalPools(lpM)                      ((lpM)->Attribute.TotalPools)
#define     SetTotalPools(lpM, Pools)               do {(lpM)->Attribute.TotalPools = (Pools);} while(0)
#define     GetBlocksPrePool(lpM)                   ((lpM)->Attribute.BlocksPrePool)
#define     SetBlocksPrePool(lpM, Blocks)           do {(lpM)->Attribute.BlocksPrePool = (Blocks);} while(0)
#define     GetContainerSearchMode(lpM)             ((lpM)->Attribute.SearchForHash)
#define     SetContainerSearchMode(lpM, bMode)      do {(lpM)->Attribute.SearchForHash = (bMode);} while(0)

#if (CONFIG_CORE_POOL_MAX > 1)
#define     GetContainerBitmap(lpM)                 ((lpM)->Attribute.ContainerBitmap)
#define     SetContainerBitmap(lpM, ID, V)          SET_BIT_VALUE((lpM)->Attribute.ContainerBitmap, ID, V)
#define     AddContainerBitmap(lpM, Mask)           (((lpM)->Attribute.ContainerBitmap) |= (Mask))
#define     SubContainerBitmap(lpM, Mask)           (((lpM)->Attribute.ContainerBitmap) ^= (Mask))
#else

#define     GetContainerBitmap(lpM)                   1
#define     SetContainerBitmap(lpM, ID, V)
#define     AddContainerBitmap(lpM, Mask)
#define     SubContainerBitmap(lpM, Mask)
#endif

#define     POOL_INITIALIZE_TABLE(lpTable, Pid, Size)                                   \
            ((lpTable) ? &((lpTable)[(Pid) * (Size)]) : NULL)

#if (CONFIG_MEM_REGION_MAX == 0)
#define     POOL_INITIALIZE_MAGIC                   POL_MAGIC
#else
#define     POOL_INITIALIZE_MAGIC                   INVALID_MAGIC
#endif

#ifdef __cplusplus
extern "C" {
#endif
    typedef E_STATUS (CALLBACK *POOLHASHCHECK)(LPVOID lpResult, LPDWORD Hash);
    EXPORT VOID CORE_CreatePoolContainer(LPCORE_CONTAINER lpManager, LPCSTR lpName,
                                        LPBYTE lpTable, BYTE Pools, BYTE BlockPrePool,
                                        DWORD BytePreBlock, BOOL SearchForHash);
    EXPORT KCONTAINER_ID_T CORE_PoolMallocBlock(LPCORE_CONTAINER lpManager);
    EXPORT LPVOID CORE_PoolTakeBlock(LPCORE_CONTAINER lpManager, KCONTAINER_ID_T Kid);
    PUBLIC E_STATUS CORE_PoolFreeBlock(LPCORE_CONTAINER lpManager, KCONTAINER_ID_T Kid);
#ifdef __cplusplus
}
#endif
#endif


