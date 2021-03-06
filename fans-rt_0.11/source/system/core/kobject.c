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
#include <stdio.h>
#include <string.h>
#include <fadefs.h>
#include <faerror.h>
#include <fatypes.h>

#include "klpc.h"
#include "karch.h"
#include "kcore.h"
#include "kpool.h"
#include "klist.h"
#include "khash.h"
#include "ktask.h"
#include "kdebug.h"
#include "kobject.h"

//#define KOBJECT_DEBUG_ENABLE TRUE

#if (KOBJECT_DEBUG_ENABLE == TRUE)
#define     KOBJECT_DEBUG(Enter, ...)                            CORE_DEBUG(Enter, __VA_ARGS__)
#define     KOBJECT_INFOR(Enter, ...)                            CORE_INFOR(Enter, __VA_ARGS__)
#define     KOBJECT_ERROR(Enter, ...)                            CORE_ERROR(Enter, __VA_ARGS__)
#ifndef _MSC_VER
#define     KOBJECT_ASSERT(condition, code, ...)                 CORE_ASSERT(condition, code, __VA_ARGS__)
#else
#define     KOBJECT_ASSERT(condition, code, ...)
#endif
#else
#define     KOBJECT_DEBUG(...)
#define     KOBJECT_INFOR(...)
#define     KOBJECT_ERROR(...)
#define     KOBJECT_ASSERT(condition, code, ...)
#endif

#define     SIZEOF_OBJECT0                 (CONFIG_OBJECT_SIZE_MAX>>7)          /**< 32 bytes object */
#define     SIZEOF_OBJECT1                 (CONFIG_OBJECT_SIZE_MAX>>6)          /**< 64 bytes object */
#define     SIZEOF_OBJECT2                 (CONFIG_OBJECT_SIZE_MAX>>5)          /**< 128 bytes object */
#define     SIZEOF_OBJECT3                 (CONFIG_OBJECT_SIZE_MAX>>4)          /**< 256 bytes object */
#define     SIZEOF_OBJECT4                 (CONFIG_OBJECT_SIZE_MAX>>3)          /**< 512 bytes object */
#define     SIZEOF_OBJECT5                 (CONFIG_OBJECT_SIZE_MAX>>2)          /**< 1024 bytes object */
#define     SIZEOF_OBJECT6                 (CONFIG_OBJECT_SIZE_MAX>>1)          /**< 2048 bytes object */
#define     SIZEOF_OBJECT7                 (CONFIG_OBJECT_SIZE_MAX>>0)          /**< 4096 bytes object */

#define     initNamedObject(lpHeader, mg, lpdName, Pid, Sid, Tid)                                   \
            do{                                                                                     \
                KOBJCLASS_ID_T Cid = Magic2ClassID(mg);                                             \
                HANDLE handle = MakeHandle(Cid, Pid, KOBJECT_STATE_CREATE, Sid, Tid);               \
                                                                                                    \
                SetObjectHandle(lpHeader, handle);                                                  \
                SetObjectName(lpHeader, lpdName);                                                   \
            }while(0)

#define     initNonameObject(lpHeader, mg, Pid, Sid, Tid)                                           \
            do{                                                                                     \
                KOBJCLASS_ID_T Cid = Magic2ClassID(mg);                                             \
                HANDLE handle = MakeHandle(Cid, Pid, KOBJECT_STATE_CREATE, Sid, Tid);               \
                                                                                                    \
                ZeroObjectName(lpHeader);                                                           \
                SetObjectHandle(lpHeader, handle);                                                  \
            }while(0)
            
#define     ObjectRepair(lpHeader, Cid, Pid, Tid)                                                   \
            do{                                                                                     \
                HANDLE handle = MakeHandle(Cid, Pid, KOBJECT_STATE_CREATE, 0, Tid);                 \
                                                                                                    \
                SetObjectHandle(lpHeader, handle);                                                  \
            }while(0)
/****************************** Define some symbol for OBJECT0 *******************************/

typedef struct tagOBJECT0_CONTAINER OBJECT0_CONTAINER;
typedef struct tagOBJECT0_CONTAINER * POBJECT0_CONTAINER;
typedef struct tagOBJECT0_CONTAINER FAR * LPOBJECT0_CONTAINER;

typedef struct tagOBJECT0_HEADER OBJECT0_HEADER;
typedef struct tagOBJECT0_HEADER * POBJECT0_HEADER;
typedef struct tagOBJECT0_HEADER FAR * LPOBJECT0_HEADER;

struct tagOBJECT0_HEADER{
    QWORD               Data[SIZEOF_OBJECT0/sizeof(QWORD)];
};

struct tagOBJECT0_CONTAINER{
    CONTAINER_ATTRIBUTE Attribute;
    CORE_POOL           Pool[CONFIG_OBJECT0_POOL_MAX + !CONFIG_OBJECT0_POOL_MAX];
                                                                    /**< Pools */
};

#if ((0 != CONFIG_OBJECT0_POOL_MAX) && (0 != CONFIG_OBJECT0_BLOCK_MAX))
STATIC      OBJECT0_CONTAINER       RW_CORE_DATA    g_Object0PoolContainer;
#define     OBJECT0_POOL_TABLE      ((LPVOID)(&g_Object0PoolContainer))
#if (CONFIG_MEM_REGION_MAX == 0)
STATIC      OBJECT0_HEADER          RW_CORE_DATA    g_Object0Table[CONFIG_OBJECT0_BLOCK_MAX];
#define     OBJECT0_HEADER_TABLE    ((LPVOID)(&g_Object0Table))
#else
#define     OBJECT0_HEADER_TABLE    NULL
#endif

#else // OBJECT 0
#define     OBJECT0_POOL_TABLE      NULL
#define     OBJECT0_HEADER_TABLE    NULL
#endif

/****************************** Define some symbol for OBJECT1 *******************************/

typedef struct tagOBJECT1_CONTAINER OBJECT1_CONTAINER;
typedef struct tagOBJECT1_CONTAINER * POBJECT1_CONTAINER;
typedef struct tagOBJECT1_CONTAINER FAR * LPOBJECT1_CONTAINER;

typedef struct tagOBJECT1_HEADER OBJECT1_HEADER;
typedef struct tagOBJECT1_HEADER * POBJECT1_HEADER;
typedef struct tagOBJECT1_HEADER FAR * LPOBJECT1_HEADER;

struct tagOBJECT1_HEADER{
    QWORD               Data[SIZEOF_OBJECT1/sizeof(QWORD)];
};

struct tagOBJECT1_CONTAINER{
    CONTAINER_ATTRIBUTE Attribute;
    CORE_POOL           Pool[CONFIG_OBJECT1_POOL_MAX + !CONFIG_OBJECT1_POOL_MAX];
                                                                    /**< Pools */
};

#if ((0 != CONFIG_OBJECT1_POOL_MAX) && (0 != CONFIG_OBJECT1_BLOCK_MAX))
STATIC      OBJECT1_CONTAINER       RW_CORE_DATA    g_Object1PoolContainer;
#define     OBJECT1_POOL_TABLE      ((LPVOID)(&g_Object1PoolContainer))
#if (CONFIG_MEM_REGION_MAX == 0)
STATIC      OBJECT1_HEADER          RW_CORE_DATA    g_Object1Table[CONFIG_OBJECT1_BLOCK_MAX];
#define     OBJECT1_HEADER_TABLE    ((LPVOID)(&g_Object1Table))
#else
#define     OBJECT1_HEADER_TABLE    NULL
#endif

#else
#define     OBJECT1_POOL_TABLE      NULL
#define     OBJECT1_HEADER_TABLE    NULL
#endif

/****************************** Define some symbol for OBJECT2 *******************************/

typedef struct tagOBJECT2_CONTAINER OBJECT2_CONTAINER;
typedef struct tagOBJECT2_CONTAINER * POBJECT2_CONTAINER;
typedef struct tagOBJECT2_CONTAINER FAR * LPOBJECT2_CONTAINER;

typedef struct tagOBJECT2_HEADER OBJECT2_HEADER;
typedef struct tagOBJECT2_HEADER * POBJECT2_HEADER;
typedef struct tagOBJECT2_HEADER FAR * LPOBJECT2_HEADER;

struct tagOBJECT2_HEADER{
    QWORD               Data[SIZEOF_OBJECT2/sizeof(QWORD)];
};

struct tagOBJECT2_CONTAINER{
    CONTAINER_ATTRIBUTE Attribute;
    CORE_POOL           Pool[CONFIG_OBJECT2_POOL_MAX + !CONFIG_OBJECT2_POOL_MAX];
                                                                    /**< Pools */
};

#if ((0 != CONFIG_OBJECT2_POOL_MAX) && (0 != CONFIG_OBJECT2_BLOCK_MAX))
STATIC      OBJECT2_CONTAINER       RW_CORE_DATA    g_Object2PoolContainer;
#define     OBJECT2_POOL_TABLE      ((LPVOID)(&g_Object2PoolContainer))
#if (CONFIG_MEM_REGION_MAX == 0)
STATIC      OBJECT2_HEADER          RW_CORE_DATA    g_Object2Table[CONFIG_OBJECT2_BLOCK_MAX];
#define     OBJECT2_HEADER_TABLE    ((LPVOID)(&g_Object2Table))
#else
#define     OBJECT2_HEADER_TABLE    NULL
#endif

#else
#define     OBJECT2_POOL_TABLE      NULL
#define     OBJECT2_HEADER_TABLE    NULL
#endif


typedef struct tagOBJECT3_CONTAINER OBJECT3_CONTAINER;
typedef struct tagOBJECT3_CONTAINER * POBJECT3_CONTAINER;
typedef struct tagOBJECT3_CONTAINER FAR * LPOBJECT3_CONTAINER;

typedef struct tagOBJECT3_HEADER OBJECT3_HEADER;
typedef struct tagOBJECT3_HEADER * POBJECT3_HEADER;
typedef struct tagOBJECT3_HEADER FAR * LPOBJECT3_HEADER;

struct tagOBJECT3_HEADER{
    QWORD               Data[SIZEOF_OBJECT3/sizeof(QWORD)];
};

struct tagOBJECT3_CONTAINER{
    CONTAINER_ATTRIBUTE Attribute;
    CORE_POOL           Pool[CONFIG_OBJECT3_POOL_MAX + !CONFIG_OBJECT3_POOL_MAX];
                                                                    /**< Pools */
};

/****************************** Define some symbol for OBJECT3 *******************************/
#if ((0 != CONFIG_OBJECT3_POOL_MAX) && (0 != CONFIG_OBJECT3_BLOCK_MAX))

STATIC      OBJECT3_CONTAINER       RW_CORE_DATA    g_Object3PoolContainer;
#define     OBJECT3_POOL_TABLE      ((LPVOID)(&g_Object3PoolContainer))
#if (CONFIG_MEM_REGION_MAX == 0)
STATIC      OBJECT3_HEADER          RW_CORE_DATA    g_Object3Table[CONFIG_OBJECT3_BLOCK_MAX];
#define     OBJECT3_HEADER_TABLE    ((LPVOID)(&g_Object3Table))
#else
#define     OBJECT3_HEADER_TABLE    NULL
#endif

#else
#define     OBJECT3_POOL_TABLE      NULL
#define     OBJECT3_HEADER_TABLE    NULL
#endif

/****************************** Define some symbol for OBJECT4 *******************************/
typedef struct tagOBJECT4_CONTAINER OBJECT4_CONTAINER;
typedef struct tagOBJECT4_CONTAINER * POBJECT4_CONTAINER;
typedef struct tagOBJECT4_CONTAINER FAR * LPOBJECT4_CONTAINER;

typedef struct tagOBJECT4_HEADER OBJECT4_HEADER;
typedef struct tagOBJECT4_HEADER * POBJECT4_HEADER;
typedef struct tagOBJECT4_HEADER FAR * LPOBJECT4_HEADER;

struct tagOBJECT4_HEADER{
    QWORD               Data[SIZEOF_OBJECT4/sizeof(QWORD)];
};

struct tagOBJECT4_CONTAINER{
    CONTAINER_ATTRIBUTE Attribute;
    CORE_POOL           Pool[CONFIG_OBJECT4_POOL_MAX + !CONFIG_OBJECT4_POOL_MAX];
                                                                    /**< Pools */
};

#if ((0 != CONFIG_OBJECT4_POOL_MAX) && (0 != CONFIG_OBJECT4_BLOCK_MAX))
STATIC      OBJECT4_CONTAINER       RW_CORE_DATA    g_Object4PoolContainer;
#define     OBJECT4_POOL_TABLE      ((LPVOID)(&g_Object4PoolContainer))
#if (CONFIG_MEM_REGION_MAX == 0)
STATIC      OBJECT4_HEADER          RW_CORE_DATA    g_Object4Table[CONFIG_OBJECT4_BLOCK_MAX];
#define     OBJECT4_HEADER_TABLE    ((LPVOID)(&g_Object4Table))
#else
#define     OBJECT4_HEADER_TABLE    NULL
#endif

#else
#define     OBJECT4_POOL_TABLE      NULL
#define     OBJECT4_HEADER_TABLE    NULL
#endif


typedef struct tagOBJECT5_CONTAINER OBJECT5_CONTAINER;
typedef struct tagOBJECT5_CONTAINER * POBJECT5_CONTAINER;
typedef struct tagOBJECT5_CONTAINER FAR * LPOBJECT5_CONTAINER;

typedef struct tagOBJECT5_HEADER OBJECT5_HEADER;
typedef struct tagOBJECT5_HEADER * POBJECT5_HEADER;
typedef struct tagOBJECT5_HEADER FAR * LPOBJECT5_HEADER;

struct tagOBJECT5_HEADER{
    QWORD               Data[SIZEOF_OBJECT5/sizeof(QWORD)];
};

struct tagOBJECT5_CONTAINER{
    CONTAINER_ATTRIBUTE Attribute;
    CORE_POOL           Pool[CONFIG_OBJECT5_POOL_MAX + !CONFIG_OBJECT5_POOL_MAX];
                                                                    /**< Pools */
};

/****************************** Define some symbol for OBJECT5 *******************************/
#if ((0 != CONFIG_OBJECT5_POOL_MAX) && (0 != CONFIG_OBJECT5_BLOCK_MAX))
STATIC      OBJECT5_CONTAINER       RW_CORE_DATA    g_Object5PoolContainer;
#define     OBJECT5_POOL_TABLE      ((LPVOID)(&g_Object5PoolContainer))
#if (CONFIG_MEM_REGION_MAX == 0)
STATIC      OBJECT5_HEADER          RW_CORE_DATA    g_Object5Table[CONFIG_OBJECT5_BLOCK_MAX];
#define     OBJECT5_HEADER_TABLE    ((LPVOID)(&g_Object5Table))
#else
#define     OBJECT5_HEADER_TABLE    NULL
#endif

#else
#define     OBJECT5_POOL_TABLE      NULL
#define     OBJECT5_HEADER_TABLE    NULL
#endif

/****************************** Define some symbol for OBJECT6 *******************************/
typedef struct tagOBJECT6_CONTAINER OBJECT6_CONTAINER;
typedef struct tagOBJECT6_CONTAINER * POBJECT6_CONTAINER;
typedef struct tagOBJECT6_CONTAINER FAR * LPOBJECT6_CONTAINER;

typedef struct tagOBJECT6_HEADER OBJECT6_HEADER;
typedef struct tagOBJECT6_HEADER * POBJECT6_HEADER;
typedef struct tagOBJECT6_HEADER FAR * LPOBJECT6_HEADER;

struct tagOBJECT6_HEADER{
    QWORD               Data[SIZEOF_OBJECT6/sizeof(QWORD)];
};

struct tagOBJECT6_CONTAINER{
    CONTAINER_ATTRIBUTE Attribute;
    CORE_POOL           Pool[CONFIG_OBJECT6_POOL_MAX + !CONFIG_OBJECT6_POOL_MAX];
                                                                    /**< Pools */
};

#if ((0 != CONFIG_OBJECT6_POOL_MAX) && (0 != CONFIG_OBJECT6_BLOCK_MAX))
STATIC      OBJECT6_CONTAINER       RW_CORE_DATA    g_Object6PoolContainer;
#define     OBJECT6_POOL_TABLE      ((LPVOID)(&g_Object6PoolContainer))
#if (CONFIG_MEM_REGION_MAX == 0)
STATIC      OBJECT6_HEADER          RW_CORE_DATA    g_Object6Table[CONFIG_OBJECT6_BLOCK_MAX];
#define     OBJECT6_HEADER_TABLE    ((LPVOID)(&g_Object6Table))
#else
#define     OBJECT6_HEADER_TABLE    NULL
#endif

#else
#define     OBJECT6_POOL_TABLE      NULL
#define     OBJECT6_HEADER_TABLE    NULL
#endif

/****************************** Define some symbol for OBJECT7 *******************************/
typedef struct tagOBJECT7_CONTAINER OBJECT7_CONTAINER;
typedef struct tagOBJECT7_CONTAINER * POBJECT7_CONTAINER;
typedef struct tagOBJECT7_CONTAINER FAR * LPOBJECT7_CONTAINER;

typedef struct tagOBJECT7_HEADER OBJECT7_HEADER;
typedef struct tagOBJECT7_HEADER * POBJECT7_HEADER;
typedef struct tagOBJECT7_HEADER FAR * LPOBJECT7_HEADER;

struct tagOBJECT7_HEADER{
    QWORD               Data[SIZEOF_OBJECT7/sizeof(QWORD)];
};

struct tagOBJECT7_CONTAINER{
    CONTAINER_ATTRIBUTE Attribute;
    CORE_POOL           Pool[CONFIG_OBJECT7_POOL_MAX + !CONFIG_OBJECT7_POOL_MAX];
                                                                    /**< Pools */
};

#if ((0 != CONFIG_OBJECT7_POOL_MAX) && (0 != CONFIG_OBJECT7_BLOCK_MAX))
STATIC      OBJECT7_CONTAINER       RW_CORE_DATA    g_Object7PoolContainer;
#define     OBJECT7_POOL_TABLE      ((LPVOID)(&g_Object7PoolContainer))
#if (CONFIG_MEM_REGION_MAX == 0)
STATIC      OBJECT7_HEADER          RW_CORE_DATA    g_Object7Table[CONFIG_OBJECT7_BLOCK_MAX];
#define     OBJECT7_HEADER_TABLE    ((LPVOID)(&g_Object7Table))
#else
#define     OBJECT7_HEADER_TABLE    NULL
#endif

#else
#define     OBJECT7_POOL_TABLE      NULL
#define     OBJECT7_HEADER_TABLE    NULL
#endif

#define     TID2PoolContainer(Tid)      ((LPVOID)(g_GlobalObjectPoolTable[Tid]))
#define     CID2ClassDescriptor(Cid)    (g_GlobalClassTable[Cid])

STATIC RO_CORE_DATA CONST CORE_CONTAINER * CONST RO_CORE_DATA g_GlobalObjectPoolTable[CONFIG_OBJECT_POOL_TABLE_MAX] = 
{
    OBJECT0_POOL_TABLE,
#if (CONFIG_OBJECT_POOL_TABLE_MAX > 1)
    OBJECT1_POOL_TABLE,
#endif
#if (CONFIG_OBJECT_POOL_TABLE_MAX > 2)
    OBJECT2_POOL_TABLE,
#endif
#if (CONFIG_OBJECT_POOL_TABLE_MAX > 3)
    OBJECT3_POOL_TABLE,
#endif
#if (CONFIG_OBJECT_POOL_TABLE_MAX > 4)
    OBJECT4_POOL_TABLE,
#endif
#if (CONFIG_OBJECT_POOL_TABLE_MAX > 5)
    OBJECT5_POOL_TABLE,
#endif
#if (CONFIG_OBJECT_POOL_TABLE_MAX > 6)
    OBJECT6_POOL_TABLE,
#endif
#if (CONFIG_OBJECT_POOL_TABLE_MAX > 7)
    OBJECT7_POOL_TABLE,
#endif
};

STATIC RW_CORE_DATA LIST_HEAD g_GlobalHashTable[CONFIG_OBJECT_HASH_TABLE_MAX];
STATIC RW_CORE_DATA LPKCLASS_DESCRIPTOR g_GlobalClassTable[CONFIG_SYSTEM_CLASS_MAX];
STATIC RW_CORE_DATA WORD g_SystemIDValue = 2;


#define     InitHashNode(lpHeader)                                          \
            LIST_HEAD_INIT(&(lpHeader)->HashNode)
#define     Attach2HashList(HashValue, lpHeader)                            \
            LIST_INSERT_TAIL(&g_GlobalHashTable[HashValue], &(lpHeader)->HashNode)
#define     DetachHashList(lpHeader)                                        \
            LIST_REMOVE_NODE(&(lpHeader)->HashNode)


STATIC DWORD GetObjectHash(LPCSTR lpName)
{
    DWORD Value0 = (((DWORD)lpName[0]) << 24)
                 + (((DWORD)lpName[1]) << 16)
                 + (((DWORD)lpName[2]) << 8);
    DWORD Value1 = (((DWORD)lpName[3]) << 24)
                 + (((DWORD)lpName[4]) << 16)
                 + (((DWORD)lpName[5]) << 8)
                 + (((DWORD)lpName[6]) << 0);
    
    return ((Value0 + Value1) % CONFIG_OBJECT_HASH_TABLE_MAX);
}

STATIC LPKOBJECT_HEADER FindObjectHash(LPCSTR lpName)
{
    LPLIST_HEAD lpNode = NULL;
    DWORD_PTR lpdName = (LPVOID) lpName;
    DWORD Hid = GetObjectHash(lpName);
    DWORD dwFlags = CORE_DisableIRQ();
    
    KOBJECT_DEBUG(TRUE, "Hid = 0x%08X,  Table = 0x%P.", Hid, &g_GlobalHashTable[Hid]);
    
    
    LIST_FOR_EACH(lpNode, &g_GlobalHashTable[Hid])
    {
        LPKOBJECT_HEADER lpHeader = GetObjecctByHashNode(lpNode);
        
        KOBJECT_DEBUG(TRUE, "lpNode = 0x%P,  lpHeader = 0x%P.", lpNode, lpHeader);
        
        if (TRUE == ObjectNameMatch(lpHeader, lpdName))
        {
            CORE_RestoreIRQ(dwFlags);
            return lpHeader;
        }
    }
    
    CORE_RestoreIRQ(dwFlags);
    
    return NULL;
}

STATIC DWORD GetSystemObjectSid(VOID)
{
    return g_SystemIDValue ++;
}

#ifdef __DEBUG__
STATIC DWORD ClassHashKey(DWORD Magic)
{
    return Magic2ClassIDKey(Magic);
}

STATIC DWORD LoadMagic(LPVOID lpPrivate, DWORD Id)
{
    LPKCLASS_DESCRIPTOR * lpClassArray = lpPrivate;
    
    if (lpClassArray[Id])
    {
        return GetClassMagic(lpClassArray[Id]);
    }
    
    return 0;
}
#endif

STATIC INLINE KOBJTABLE_ID_T Size2Tid(SIZE_T Length)
{
    KOBJTABLE_ID_T Tid = GetWordHighestBit(Length);
    DWORD Adjust = (!!(Length ^ (1 << Tid)));

    return (Tid - CONFIG_OBJECT_SIZE_SHIFT) + Adjust;
#if 0
    WORD Magic = (Length >> CONFIG_OBJECT_SIZE_SHIFT) & 0xffff;
    BYTE Highest = GetWordHighestBit(Magic);
   
    return (BYTE) (Highest + (!!(Magic ^ (1 << Highest))));
#endif
}


STATIC INLINE KOBJECT_STATE KObjectSetStateSafe(LPKOBJECT_HEADER lpHeader, KOBJECT_STATE NewState)
{
    DWORD dwFlags = CORE_DisableIRQ();
    KOBJECT_STATE State = GetObjectState(lpHeader);

    SetObjectState(lpHeader, NewState);

    CORE_RestoreIRQ(dwFlags);
    
    return State;
}

STATIC LPKCLASS_DESCRIPTOR KObject2KClass(LPKOBJECT_HEADER lpHeader)
{
    if (NULL != lpHeader)
    {
        KOBJCLASS_ID_T ClassID = GetObjectCid(lpHeader);
        
        if (ClassID >= SIZEOF_ARRAY(g_GlobalClassTable))
        {
            KOBJECT_ERROR(TRUE, "The class id value(%d) too large.", ClassID);
            CORE_SetError(STATE_OVER_RANGE);
            return NULL;
        }
        
        return CID2ClassDescriptor(ClassID);

    }

    KOBJECT_ERROR(TRUE, "Invalid object header pointer.");
    CORE_SetError(STATE_INVALID_PARAMETER);

    return NULL;
}

PUBLIC DWORD CORE_Header2Magic(LPKOBJECT_HEADER lpHeader)
{
    return CID2ClassDescriptor(GetObjectCid(lpHeader))->Header.un.Bits.Magic;
}

STATIC INLINE LPCORE_CONTAINER KObject2Container(LPKOBJECT_HEADER lpHeader)
{
    DWORD Tid = GetObjectTid(lpHeader);
    
    if (Tid >= SIZEOF_ARRAY(g_GlobalObjectPoolTable))
    {
        KOBJECT_ERROR(TRUE, "Object(0x%08x), Tid 0x%x, no pool manager found.", lpHeader, Tid);
        return NULL;
    }

    return TID2PoolContainer(Tid);
}


PUBLIC E_STATUS CORE_RegisterClass(CONST KCLASS_DESCRIPTOR * lpClass)
{
    DWORD dwFlags;
    KOBJCLASS_ID_T ClassID;
    
    if (NULL == lpClass)
    {
        KOBJECT_ERROR(TRUE, "Invalid parameter.");
        return STATE_INVALID_PARAMETER;
    }
    
    if (NULL == GetClassName(lpClass))
    {
        KOBJECT_ERROR(TRUE, "Invalid class(0x%p) name.", lpClass);
        return STATE_INVALID_NAME;
    }
    
    if (NULL == lpClass->Header.fnSizeofObject ||
        NULL == lpClass->Header.fnMallocObject ||
        NULL == lpClass->Header.fnActiveObject ||
        NULL == lpClass->Header.fnFreeObject)
    {
        KOBJECT_ERROR(TRUE, "Class(%s) method is invalid.", GetClassName(lpClass));
        return STATE_INVALID_CLASS;
    }
    
    ClassID = Magic2ClassID(GetClassMagic(lpClass));
    
    KOBJECT_INFOR(TRUE, "Class(%s-%d) register to %d ...",
            GetClassName(lpClass), Magic2ClassIDKey(GetClassMagic(lpClass)), ClassID);
    
    dwFlags = CORE_DisableIRQ();
    
    if (NULL != g_GlobalClassTable[ClassID])
    {
#ifdef __DEBUG__
        DWORD Count = 0;
        DWORD MagicArray[CONFIG_SYSTEM_CLASS_MAX];

        Count = CORE_HashSetArray(LoadMagic, g_GlobalClassTable,
                MagicArray, GetClassMagic(lpClass), CONFIG_SYSTEM_CLASS_MAX);

        CORE_HashDebug(ClassHashKey, MagicArray, Count, 8, 32,
                "CONFIG_SYSTEM_CLASS_MAX", "Magic2ClassID");
#endif
        CORE_RestoreIRQ(dwFlags);
        KOBJECT_ERROR(TRUE, "Class(%s) register to %d failed.", GetClassName(lpClass), ClassID);
        KOBJECT_ERROR(TRUE, "New class(%s) key is (%d), old class(%s) key is %d.",
            GetClassName(lpClass), Magic2ClassIDKey(GetClassMagic(lpClass)),
            GetClassName(g_GlobalClassTable[ClassID]),
            Magic2ClassIDKey(GetClassMagic(g_GlobalClassTable[ClassID])));

        return STATE_EXISTING;
    }

    g_GlobalClassTable[ClassID] = (LPVOID) lpClass;
    
    CORE_RestoreIRQ(dwFlags);
    
    KOBJECT_INFOR(TRUE, "Register class '%s' to %d successfully.",
            GetClassName(lpClass), ClassID);
    
    return STATE_SUCCESS;
}



STATIC LPKOBJECT_HEADER MallocObjectFromPool(LPKCLASS_DESCRIPTOR lpClass, LPCSTR lpName, SIZE_T ObjectSize)
{
    LPKOBJECT_HEADER lpHeader = NULL;
    LPCORE_CONTAINER lpManager = NULL;
    KCONTAINER_ID_T Pid = INVALID_CONTAINER_ID;
    KOBJTABLE_ID_T Tid = Size2Tid(ObjectSize);

    CORE_ASSERT(Tid < CONFIG_OBJECT_POOL_TABLE_MAX, SYSTEM_CALL_OOPS(),
        "BUG: Invalid tid value %u, from object size %d bytes.", Tid, ObjectSize);

    lpManager = TID2PoolContainer(Tid);
    
    KOBJECT_DEBUG(TRUE, "Malloc pool from '%s', Tid(%d), ObjectSize(%u).",
        GetClassName(lpClass), Tid, ObjectSize);
    
    if (NULL == lpManager)
    {
        KOBJECT_ERROR(TRUE, "Container not found, magic(0x%08x), ObjectSize(%u) ==> Tid(%d), table(0x%P).",
            GetClassMagic(lpClass), ObjectSize, Tid, g_GlobalObjectPoolTable);
        CORE_SetError(STATE_INVALID_POOL);
        return NULL;
    }

    Pid = CORE_PoolMallocBlock(lpManager);

    if (INVALID_CONTAINER_ID == Pid)
    {
        KOBJECT_ERROR(TRUE, "Invalid container id for magic(0x%08x).",
            GetClassMagic(lpClass));
        //CORE_SetError(STATE_INVALID_POOL); Set by pool
        return NULL;
    }

    lpHeader = CORE_PoolTakeBlock(lpManager, Pid);
    
    if (NULL == lpHeader)
    {
        KOBJECT_ERROR(TRUE, "Can't found header for magic(0x%08x) pid(%d).", 
            GetClassMagic(lpClass), Pid);
        return NULL;
    }
    
    if (NULL != lpName)
    {
        DWORD dwFlags = CORE_DisableIRQ();

        Attach2HashList(GetObjectHash(lpName), lpHeader);

        initNamedObject(lpHeader, GetClassMagic(lpClass),
                        ((DWORD_PTR)lpName), Pid, GetSystemObjectSid(), Tid);
        
        CORE_RestoreIRQ(dwFlags);
    }
    else
    {
        InitHashNode(lpHeader);
        initNonameObject(lpHeader, GetClassMagic(lpClass), Pid, GetSystemObjectSid(), Tid);
    }
   
    CORE_SetError(STATE_SUCCESS); 

    return lpHeader;
}

STATIC INLINE VOID DetachObject(LPKOBJECT_HEADER lpHeader)
{
    DWORD dwFlags = CORE_DisableIRQ();
    DetachHashList(lpHeader);
    CORE_RestoreIRQ(dwFlags);
}

STATIC E_STATUS FreeObjectToPool(LPKOBJECT_HEADER lpHeader)
{
    KOBJECT_STATE State;
    KCONTAINER_ID_T Pid;
    LPCORE_CONTAINER lpManager = KObject2Container(lpHeader);

    if (NULL == lpManager)
    {
        return STATE_INVALID_OBJECT;
    }

    State = KObjectSetStateSafe(lpHeader, KOBJECT_STATE_FREE);

    if (KOBJECT_STATE_DEATH != State)
    {
        KOBJECT_ERROR(TRUE, "Free object(0x%08x) state not detach(%d).", lpHeader, State);
        KObjectSetStateSafe(lpHeader, State);
        return STATE_INVALID_STATE;
    }
    
    KOBJECT_DEBUG(TRUE, "Free object(0x%08x) state is detach.", lpHeader);

    Pid = GetObjectPid(lpHeader);
    SetObjectHandle(lpHeader, INVALID_HANDLE_VALUE);
    DetachObject(lpHeader);

    return CORE_PoolFreeBlock(lpManager, Pid);
}

/***************************************************************************************
                             The some export core functions
***************************************************************************************/

EXPORT LPKOBJECT_HEADER CORE_Handle2HeaderCheck(HANDLE handle, BOOL Check)
{
    LPCORE_CONTAINER lpManager;
    LPKOBJECT_HEADER lpHeader;
    DWORD Tid = GetHandleObjectTid(handle);
    KCONTAINER_ID_T Pid = GetHandleObjectPid(handle);
    
    lpManager = TID2PoolContainer(Tid);
    
    if (NULL == lpManager)
    {
        KOBJECT_ERROR(TRUE, "handle 0x%08x, Tid 0x%x, no pool manager found.", handle, Tid);
        CORE_SetError(STATE_INVALID_POOL);
        return NULL;
    }
    
    lpHeader = CORE_PoolTakeBlock(lpManager, Pid);

    if (NULL == lpHeader)
    {
        KOBJECT_ERROR(TRUE, "handle 0x%08x, container 0x%x, no object header found.", handle, Pid);
        CORE_SetError(STATE_INVALID_OBJECT);
        return NULL;
    }

    if (TRUE == Check)
    {
        KOBJECT_STATE State = GetObjectState(lpHeader);

        if (KOBJECT_STATE_CREATE != State && KOBJECT_STATE_ACTIVE != State)
        {
            KOBJECT_DEBUG(TRUE, "handle 0x%08x object %p state %d is invalid.",4
                handle, lpHeader, State);
            CORE_SetError(STATE_INVALID_OBJECT);
            return NULL;
        }

        if (handle != GetObjectHandle(lpHeader))
        {
            KOBJECT_DEBUG(TRUE, "handle 0x%08x not 0x%08x.", handle, GetObjectHandle(lpHeader));
            CORE_SetError(STATE_INVALID_OBJECT);
            return NULL;
        }
    }

    return lpHeader;
}

EXPORT HANDLE CORE_RebuildHandle(DWORD Magic, SIZE_T Length, KOBJECT_STATE State, DWORD Pid)
{
    return MakeHandle(Magic2ClassID(Magic), Pid, State, 0, Size2Tid(Length));
}

EXPORT LPKOBJECT_HEADER CORE_MallocNoNameObject(DWORD Magic, LPVOID lpParam)
{
    E_STATUS State;
    SIZE_T Length = 0;
    LPKOBJECT_HEADER lpHeader = NULL;
    LPKCLASS_DESCRIPTOR lpClass = NULL;

    lpClass = CID2ClassDescriptor(Magic2ClassID(Magic));
    
    if (NULL == lpClass)
    {
        KOBJECT_ERROR(TRUE, "No class(0x%08x) to malloc no name object.", Magic);
        CORE_SetError(STATE_INVALID_CLASS);
        return NULL;
    }
    
    if (Magic != GetClassMagic(lpClass))
    {
        KOBJECT_ERROR(TRUE, "Magic(0x%08x) not match for class(0x%08x) to create no name object.",
            Magic, GetClassMagic(lpClass));
        CORE_SetError(STATE_NOT_MATCH);
        return NULL;
    }

    CORE_ASSERT(lpClass->Header.fnMallocObject, SYSTEM_CALL_OOPS(), 
        "BUG: Not support malloc for class(%s) to create no name object.",
            GetClassName(lpClass));

    CORE_ASSERT(lpClass->Header.fnSizeofObject, SYSTEM_CALL_OOPS(), 
        "BUG: Not support get size for class(%s) to create no name object.",
            GetClassName(lpClass));

    Length = lpClass->Header.fnSizeofObject(lpClass, lpParam);
    
    if (Length > CONFIG_OBJECT_SIZE_MAX ||
        Length < CONFIG_OBJECT_SIZE_MIN)
    {
        KOBJECT_ERROR(TRUE, "Invalid object size(%d) for class(%s).",
            Length, GetClassName(lpClass));
        CORE_SetError(STATE_OVER_RANGE);
        return NULL;
    }
    
    lpHeader = MallocObjectFromPool(lpClass, NULL, Length);

    if (NULL == lpHeader)
    {
        KOBJECT_ERROR(TRUE, "Can't malloc no name object for class '%s'",
            GetClassName(lpClass));
        return NULL;
    }

    State = lpClass->Header.fnMallocObject(lpHeader, lpParam);
    
    if (STATE_SUCCESS != State)
    {
        KOBJECT_ERROR(TRUE, "No name object call malloc in the class '%s' failed.",
            GetClassName(lpClass));
        SetObjectState(lpHeader, KOBJECT_STATE_DEATH);
        CORE_FreeObject(lpHeader);
        lpHeader = NULL;
    }
    
    CORE_SetError(State);
    
    return lpHeader;
     
}


EXPORT LPKOBJECT_HEADER CORE_MallocObject(DWORD Magic, LPCSTR lpName, LPVOID lpParam)
{
    E_STATUS State;
    SIZE_T Length = 0;
    LPKOBJECT_HEADER lpHeader = NULL;
    LPKCLASS_DESCRIPTOR lpClass = NULL;

    if (NULL == lpName)
    {
        KOBJECT_ERROR(TRUE, "Invalid name to open object for class(0x%08x).", Magic);
        CORE_SetError(STATE_INVALID_NAME);
        return NULL;
    }
    
    lpClass = CID2ClassDescriptor(Magic2ClassID(Magic));
    
    if (NULL == lpClass)
    {
        KOBJECT_ERROR(TRUE, "No class(0x%08x) to malloc object '%s'.", Magic, lpName);
        CORE_SetError(STATE_INVALID_CLASS);
        return NULL;
    }
    
    if (Magic != GetClassMagic(lpClass))
    {
        KOBJECT_ERROR(TRUE, "Magic(0x%08x) not match for class(0x%08x) to open object '%s'",
            Magic, GetClassMagic(lpClass), lpName);
        CORE_SetError(STATE_NOT_MATCH);
        return NULL;
    }

    CORE_ASSERT(lpClass->Header.fnMallocObject, SYSTEM_CALL_OOPS(), 
        "BUG: Not support function for class(%s) to malloc object '%s'.",
            GetClassName(lpClass), lpName);

    CORE_ASSERT(lpClass->Header.fnSizeofObject, SYSTEM_CALL_OOPS(), 
        "BUG: Not support get size for class(%s) to create no name object.",
            GetClassName(lpClass));

    Length = lpClass->Header.fnSizeofObject(lpClass, lpParam);
    
    if (Length > CONFIG_OBJECT_SIZE_MAX ||
        Length < CONFIG_OBJECT_SIZE_MIN)
    {
        KOBJECT_ERROR(TRUE, "Invalid object size(%d) for class(%s).",
            Length, GetClassName(lpClass));
        CORE_SetError(STATE_OVER_RANGE);
        return NULL;
    }
    
    lpHeader = FindObjectHash(lpName);

    if (NULL != lpHeader)
    {
        KOBJECT_ERROR(TRUE, "Object '%s' was exist: 0x%P, '%s'.", lpName, lpHeader, GetObjectName(lpHeader));
        CORE_SetError(STATE_EXISTING);
        return NULL;
    }

    lpHeader = MallocObjectFromPool(lpClass, lpName, Length);

    if (NULL == lpHeader)
    {
        KOBJECT_ERROR(TRUE, "Can't malloc object '%s' for class '%s'",
            lpName, GetClassName(lpClass));
        return NULL;
    }

    State = lpClass->Header.fnMallocObject(lpHeader, lpParam);

    if (STATE_SUCCESS != State)
    {
        KOBJECT_ERROR(TRUE, "Object '%s' call malloc in the class '%s' failed.",
            lpName, GetClassName(lpClass));
        SetObjectState(lpHeader, KOBJECT_STATE_DEATH);
        CORE_FreeObject(lpHeader);
        lpHeader = NULL;
    }

    CORE_SetError(State);

    return lpHeader;
}


EXPORT LPKOBJECT_HEADER CORE_TakeObject(LPCSTR lpName, LPVOID lpParam)
{
    E_STATUS State;
    LPKOBJECT_HEADER lpHeader = NULL;
    LPKCLASS_DESCRIPTOR lpClass = NULL;
 
    if (NULL == lpName)
    {
        KOBJECT_ERROR(TRUE, "BUG: Invalid name to take object.");
        return NULL;
    }

    lpHeader = FindObjectHash(lpName);

    if (NULL == lpHeader)
    {
        CORE_SetError(STATE_NOT_FOUND);
        return NULL;
    }
    
    lpClass = KObject2KClass(lpHeader);


    if (NULL == lpClass)
    {
        CORE_SetError(STATE_INVALID_CLASS);
        return NULL;
    }

    if (KOBJECT_STATE_ACTIVE != GetObjectState(lpHeader))
    {
        KOBJECT_ERROR(TRUE, "Object handle 0x%p state %d error.",
                lpHeader, GetObjectState(lpHeader));
        CORE_SetError(STATE_INVALID_STATE);
        return NULL;
    }

    CORE_ASSERT(lpClass->Header.fnTakeObject, SYSTEM_CALL_OOPS(), 
        "BUG: Not support function for class(%s) to open object '%s'.",
            GetClassName(lpClass), lpName);
    
    State = lpClass->Header.fnTakeObject(lpHeader, lpParam);
    
    CORE_SetError(State);
    
    if (STATE_SUCCESS != State)
    {
        KOBJECT_ERROR(TRUE, "Object '%s' call open in the class '%s' failed.",
            lpName, GetClassName(lpClass));
        
        return NULL;
    }

    return lpHeader;
}

EXPORT E_STATUS CORE_ActiveObject(LPKOBJECT_HEADER lpHeader, LPVOID lpParam)
{
    E_STATUS Result;
    LPKCLASS_DESCRIPTOR lpClass = KObject2KClass(lpHeader);
    
    if (NULL == lpClass)
    {
        return CORE_GetError();
    }

    if (KOBJECT_STATE_CREATE != GetObjectState(lpHeader))
    {
        KOBJECT_ERROR(TRUE, "Object handle 0x%p state %d error.",
                lpHeader, GetObjectState(lpHeader));
        return STATE_INVALID_STATE;
    }

    CORE_ASSERT(lpClass->Header.fnActiveObject, SYSTEM_CALL_OOPS(), 
        "BUG: Not support function for class(%s) to wait object '%s'.",
        GetClassName(lpClass), GetObjectName(lpHeader));

    Result = lpClass->Header.fnActiveObject(lpHeader, lpParam);
    
    if (STATE_SUCCESS == Result)
    {
        SetObjectState(lpHeader, KOBJECT_STATE_ACTIVE);
    }
    
    return Result;
}


EXPORT E_STATUS CORE_FreeObject(LPKOBJECT_HEADER lpHeader)
{
    E_STATUS Result;
    KOBJECT_STATE State;
    LPKCLASS_DESCRIPTOR lpClass = KObject2KClass(lpHeader);

    if (NULL == lpClass)
    {
        return CORE_GetError();
    }

    /* After setting state to death, the all operations */
    /* for this object will be return a failure */
    /* Please see active object, take object and request method. */
    State = KObjectSetStateSafe(lpHeader, KOBJECT_STATE_DEATH);

    /* If the state of this object is already dead, */
    /* which means another task already to destroy it. */
    /* So the current task should be return a failure */
    /* to finish this operation. */
    if (KOBJECT_STATE_FREE == State || KOBJECT_STATE_DEATH == State)
    {
        KOBJECT_ERROR(TRUE, "Object handle 0x%p state %d error.",
                lpHeader, GetObjectState(lpHeader));
        KObjectSetStateSafe(lpHeader, State);

        return STATE_INVALID_STATE;
    }

    CORE_ASSERT(lpClass->Header.fnFreeObject, SYSTEM_CALL_OOPS(), 
        "BUG: Not support function for class(%s) to free object '%s'.",
        GetClassName(lpClass), GetObjectName(lpHeader));

    if (STATE_SUCCESS == (Result = lpClass->Header.fnFreeObject(lpHeader)))
    {   
        KOBJECT_DEBUG(TRUE, "Object '%s' call free in the class '%s' successfully.",
            GetObjectName(lpHeader), GetClassName(lpClass));
        
        if (STATE_SUCCESS == (Result = FreeObjectToPool(lpHeader)))
        {
            return Result;
        }
    }

    KObjectSetStateSafe(lpHeader, State);

    KOBJECT_ERROR(TRUE, "Object '%s' call free in the class '%s' failed result %d.",
            GetObjectName(lpHeader), GetClassName(lpClass), Result);

    return Result;
}

EXPORT E_STATUS CORE_RequestMethod(LPKOBJECT_HEADER lpHeader, LPVOID lpParam, DWORD Method)
{
    FNCLASSMETHOD fnClassMethod = NULL;
    LPKCLASS_DESCRIPTOR lpClass = KObject2KClass(lpHeader);
    
    if (NULL == lpClass)
    {
        KOBJECT_ERROR(TRUE, "Request object(0x%P) method(%u) failed.",
                lpHeader, Method);
        return CORE_GetError();
    }
    
    if (KOBJECT_STATE_ACTIVE != GetObjectState(lpHeader))
    {
        KOBJECT_ERROR(TRUE, "Object handle 0x%p state %d error.",
                lpHeader, GetObjectState(lpHeader));
        return STATE_INVALID_STATE;
    }
    
    if (Method >= GetClassMethods(lpClass))
    {
        KOBJECT_ERROR(TRUE, "Request object(%s) method(%u) is greater than %u.",
                GetObjectName(lpHeader), Method, GetClassMethods(lpClass));
        return STATE_OVER_RANGE;
    }

    fnClassMethod = GetClassMethodFn(lpClass, Method);

    if (NULL == fnClassMethod)
    {
        KOBJECT_ERROR(TRUE, "Request object(%s) method(%u) is invalid.",
                GetObjectName(lpHeader), Method, GetClassMethods(lpClass));
        return STATE_INVALID_METHOD;
    }
    
    return fnClassMethod(lpHeader, lpParam);
}



STATIC E_STATUS SVC_MallocObject(LPVOID lpPrivate, LPVOID lpParam)
{
    LPKOBJECT_HEADER lpHeader = CORE_MallocObject(REQdParam(lpParam, u0),
                                                  REQpParam(lpParam, u1),
                                                  REQpParam(lpParam, u2));

    if (NULL == lpHeader)
    {
        REQhParam(lpParam, u1) = INVALID_HANDLE_VALUE;
        return CORE_GetError();
    }

    SetREQhParam(lpParam, u1, GetObjectHandle(lpHeader));
    
    return STATE_SUCCESS;
}

STATIC E_STATUS SVC_ActiveObject(LPVOID lpPrivate, LPVOID lpParam)
{
    return CORE_ActiveObject(CORE_Handle2Header(REQhParam(lpParam, u0)), REQpParam(lpParam, u1));
}

STATIC E_STATUS SVC_TakeObject(LPVOID lpPrivate, LPVOID lpParam)
{
    LPKOBJECT_HEADER lpHeader = CORE_TakeObject(REQpParam(lpParam, u0), REQpParam(lpParam, u1));
    
    if (NULL == lpHeader)
    {
        REQhParam(lpParam, u1) = INVALID_HANDLE_VALUE;
        return CORE_GetError();
    }
    
    SetREQhParam(lpParam, u1, GetObjectHandle(lpHeader));
    
    return STATE_SUCCESS;
}

STATIC E_STATUS SVC_FreeObject(LPVOID lpPrivate, LPVOID lpParam)
{
    return CORE_FreeObject(CORE_Handle2Header(REQhParam(lpParam, u0)));  
}

STATIC E_STATUS SVC_GetObjectName(LPVOID lpPrivate, LPVOID lpParam)
{
    LPKOBJECT_HEADER lpHeader;

    lpHeader = TASK_SELF_HANDLE == REQhParam(lpParam, u0)
             ? (LPVOID) CORE_GetCurrentTask()
             : CORE_Handle2Header(REQhParam(lpParam, u0));
    
    if (NULL == lpHeader)
    {
        return STATE_INVALID_OBJECT;
    }
    
    if (NULL == REQpParam(lpParam, u1) || REQdParam(lpParam, u2) < OBJECT_NAME_MAX)
    {
        return STATE_INVALID_PARAMETER;
    }

    DumpObjectName(lpHeader, (DWORD_PTR)(REQpParam(lpParam, u1)));

    return STATE_SUCCESS;
}


STATIC E_STATUS SVC_RequestMethod(LPVOID lpPrivate, LPVOID lpParam)
{
    return CORE_RequestMethod(CORE_Handle2Header(REQhParam(lpParam, u0)),
                                         REQpParam(lpParam, u1),
                                         REQdParam(lpParam, u2));
}

STATIC E_STATUS SVC_MallocNmObject(LPVOID lpPrivate, LPVOID lpParam)
{
    LPKOBJECT_HEADER lpHeader = CORE_MallocNoNameObject(REQdParam(lpParam, u0),
                                                        REQpParam(lpParam, u1));

    if (NULL == lpHeader)
    {
        SetREQdParam(lpParam, u2, INVALID_HANDLE_VALUE);
        return CORE_GetError();
    }

    SetREQhParam(lpParam, u2, GetObjectHandle(lpHeader));
    
    return STATE_SUCCESS;
}

STATIC CONST REQUEST_HANDLER fnHandlers[] = {
    SVC_MallocObject,                   /**< 0.LPC_SOM_OBJECT_MALLOC */
    SVC_ActiveObject,                   /**< 1.LPC_SOM_OBJECT_ACTIVE */
    SVC_TakeObject,                     /**< 2.LPC_SOM_OBJECT_TAKE */
    SVC_FreeObject,                     /**< 3.LPC_SOM_OBJECT_FREE */
    SVC_GetObjectName,                  /**< 4.LPC_SOM_OBJECT_GETNAME */
    SVC_RequestMethod,                  /**< 5.LPC_SOM_OBJECT_METHOD */
    SVC_MallocNmObject,                 /**< 6.LPC_SOM_OBJECT_MALLOCNM */
};


DEFINE_LPC_SERVICE(LPCService, SOM_MAGIC, SIZEOF_ARRAY(fnHandlers), NULL, fnHandlers);

PUBLIC E_STATUS initCoreSystemObjectPoolManager(VOID)
{
    DWORD i = 0;
    E_STATUS Result = STATE_SUCCESS;

    KOBJECT_INFOR(TRUE, "OBJECT0_CONTAINER: %d      OBJECT1_CONTAINER: %d.",
            sizeof(OBJECT0_CONTAINER), sizeof(OBJECT0_CONTAINER));
    KOBJECT_INFOR(TRUE, "OBJECT2_CONTAINER: %d      OBJECT3_CONTAINER: %d.",
            sizeof(OBJECT2_CONTAINER), sizeof(OBJECT3_CONTAINER));
    KOBJECT_INFOR(TRUE, "OBJECT4_CONTAINER: %d      OBJECT5_CONTAINER: %d.",
            sizeof(OBJECT4_CONTAINER), sizeof(OBJECT5_CONTAINER));
    KOBJECT_INFOR(TRUE, "OBJECT6_CONTAINER: %d      OBJECT7_CONTAINER: %d.",
            sizeof(OBJECT6_CONTAINER), sizeof(OBJECT7_CONTAINER));

    for (i = 0; i < CONFIG_OBJECT_HASH_TABLE_MAX; i ++)
    {
        LIST_HEAD_INIT(&g_GlobalHashTable[i]);
    }
    
#if ((0 != CONFIG_OBJECT0_POOL_MAX) && (0 != CONFIG_OBJECT0_BLOCK_MAX))
    memset(OBJECT0_POOL_TABLE, 0, sizeof(OBJECT0_POOL_TABLE));
    Result = CORE_CreatePoolContainer(OBJECT0_POOL_TABLE,
            "Object0", OBJECT0_HEADER_TABLE,
            CONFIG_OBJECT0_POOL_MAX,
            CONFIG_OBJECT0_BLOCK_MAX,
            sizeof(OBJECT0_HEADER), TRUE);
    
    CORE_ASSERT(STATE_SUCCESS == Result, SYSTEM_CALL_OOPS(),"Create pool failed !");
#endif
#if ((0 != CONFIG_OBJECT1_POOL_MAX) && (0 != CONFIG_OBJECT1_BLOCK_MAX))
    memset(OBJECT1_POOL_TABLE, 0, sizeof(OBJECT1_POOL_TABLE));
    Result = CORE_CreatePoolContainer(OBJECT1_POOL_TABLE,
            "Object1", OBJECT1_HEADER_TABLE,
            CONFIG_OBJECT1_POOL_MAX,
            CONFIG_OBJECT1_BLOCK_MAX,
            sizeof(OBJECT1_HEADER), TRUE);
    CORE_ASSERT(STATE_SUCCESS == Result, SYSTEM_CALL_OOPS(),"Create pool failed !");
#endif
#if ((0 != CONFIG_OBJECT2_POOL_MAX) && (0 != CONFIG_OBJECT2_BLOCK_MAX))
    memset(OBJECT2_POOL_TABLE, 0, sizeof(OBJECT2_POOL_TABLE));
    Result = CORE_CreatePoolContainer(OBJECT2_POOL_TABLE,
            "Object2", OBJECT2_HEADER_TABLE,
            CONFIG_OBJECT2_POOL_MAX,
            CONFIG_OBJECT2_BLOCK_MAX,
            sizeof(OBJECT2_HEADER), TRUE);
    CORE_ASSERT(STATE_SUCCESS == Result, SYSTEM_CALL_OOPS(),"Create pool failed !");
#endif
#if ((0 != CONFIG_OBJECT3_POOL_MAX) && (0 != CONFIG_OBJECT3_BLOCK_MAX))
    memset(OBJECT3_POOL_TABLE, 0, sizeof(OBJECT3_POOL_TABLE));
    Result = CORE_CreatePoolContainer(OBJECT3_POOL_TABLE,
            "Object3", OBJECT3_HEADER_TABLE,
            CONFIG_OBJECT3_POOL_MAX,
            CONFIG_OBJECT3_BLOCK_MAX,
            sizeof(OBJECT3_HEADER), TRUE);
    CORE_ASSERT(STATE_SUCCESS == Result, SYSTEM_CALL_OOPS(),"Create pool failed !");
#endif
#if ((0 != CONFIG_OBJECT4_POOL_MAX) && (0 != CONFIG_OBJECT4_BLOCK_MAX))
    memset(OBJECT4_POOL_TABLE, 0, sizeof(OBJECT4_POOL_TABLE));
    Result = CORE_CreatePoolContainer(OBJECT4_POOL_TABLE,
            "Object4", OBJECT4_HEADER_TABLE,
            CONFIG_OBJECT4_POOL_MAX,
            CONFIG_OBJECT4_BLOCK_MAX,
            sizeof(OBJECT4_HEADER), TRUE);
    CORE_ASSERT(STATE_SUCCESS == Result, SYSTEM_CALL_OOPS(),"Create pool failed !");
#endif
#if ((0 != CONFIG_OBJECT5_POOL_MAX) && (0 != CONFIG_OBJECT5_BLOCK_MAX))
    memset(OBJECT5_POOL_TABLE, 0, sizeof(OBJECT5_POOL_TABLE));
    Result = CORE_CreatePoolContainer(OBJECT5_POOL_TABLE,
            "Object5", OBJECT5_HEADER_TABLE,
            CONFIG_OBJECT5_POOL_MAX,
            CONFIG_OBJECT5_BLOCK_MAX,
            sizeof(OBJECT5_HEADER), TRUE);
    CORE_ASSERT(STATE_SUCCESS == Result, SYSTEM_CALL_OOPS(),"Create pool failed !");
#endif
#if ((0 != CONFIG_OBJECT6_POOL_MAX) && (0 != CONFIG_OBJECT6_BLOCK_MAX))
    memset(OBJECT6_POOL_TABLE, 0, sizeof(OBJECT6_POOL_TABLE));
    Result = CORE_CreatePoolContainer(OBJECT6_POOL_TABLE,
            "Object6", OBJECT6_HEADER_TABLE,
            CONFIG_OBJECT6_POOL_MAX,
            CONFIG_OBJECT6_BLOCK_MAX,
            sizeof(OBJECT6_HEADER), TRUE);
    CORE_ASSERT(STATE_SUCCESS == Result, SYSTEM_CALL_OOPS(),"Create pool failed !");
#endif
#if ((0 != CONFIG_OBJECT7_POOL_MAX) && (0 != CONFIG_OBJECT7_BLOCK_MAX))
    memset(OBJECT7_POOL_TABLE, 0, sizeof(OBJECT7_POOL_TABLE));
    Result = CORE_CreatePoolContainer(OBJECT7_POOL_TABLE,
            "Object7", OBJECT7_HEADER_TABLE,
            CONFIG_OBJECT7_POOL_MAX,
            CONFIG_OBJECT7_BLOCK_MAX,
            sizeof(OBJECT7_HEADER), TRUE);
    CORE_ASSERT(STATE_SUCCESS == Result, SYSTEM_CALL_OOPS(),"Create pool failed !");
#endif
    LPC_INSTALL(&LPCService, "System object management(SOM) service starting");
    
    return STATE_SUCCESS;
}

