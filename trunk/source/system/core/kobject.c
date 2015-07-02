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
    KOBJECT_HEADER      Header;
    CHAR                Reserved[SIZEOF_OBJECT0 - sizeof(KOBJECT_HEADER)];
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
    KOBJECT_HEADER      Header;
    CHAR                Reserved[SIZEOF_OBJECT1 - sizeof(KOBJECT_HEADER)];
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
    KOBJECT_HEADER      Header;
    CHAR                Reserved[SIZEOF_OBJECT2 - sizeof(KOBJECT_HEADER)];
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
    KOBJECT_HEADER      Header;
    CHAR                Reserved[SIZEOF_OBJECT3 - sizeof(KOBJECT_HEADER)];
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
    KOBJECT_HEADER      Header;
    CHAR                Reserved[SIZEOF_OBJECT4 - sizeof(KOBJECT_HEADER)];
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
    KOBJECT_HEADER      Header;
    CHAR                Reserved[SIZEOF_OBJECT5 - sizeof(KOBJECT_HEADER)];
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
    KOBJECT_HEADER       Header;
    CHAR                 Reserved[SIZEOF_OBJECT6 - sizeof(KOBJECT_HEADER)];
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
    KOBJECT_HEADER      Header;
    CHAR                Reserved[SIZEOF_OBJECT7 - sizeof(KOBJECT_HEADER)];
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

#define     TID2PoolContainer(Tid)      (g_GlobalObjectPoolTable[Tid])
#define     CID2ClassDescriptor(Cid)    (g_GlobalClassTable[Cid])

STATIC CONST LPCORE_CONTAINER g_GlobalObjectPoolTable[CONFIG_OBJECT_POOL_TABLE_MAX] = 
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

STATIC RW_CORE_DATA LPKCLASS_DESCRIPTOR g_GlobalClassTable[CONFIG_SYSTEM_CLASS_MAX] = {NULL};
STATIC RW_CORE_DATA LIST_HEAD g_GlobalHashTable[CONFIG_OBJECT_HASH_TABLE_MAX] = {NULL};
STATIC RW_CORE_DATA DWORD g_SystemIDValue = 2;

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
    LPDWORD lpdName = (LPVOID) lpName;
    DWORD Hid = GetObjectHash(lpName);
    DWORD dwFlags = CORE_DisableIRQ();
    
    LIST_FOR_EACH(lpNode, &g_GlobalHashTable[Hid])
    {
        LPKOBJECT_HEADER lpHeader = GetObjecctByHashNode(lpNode);
        
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
        return lpClassArray[Id]->Magic;
    }
    
    return 0;
}
#endif

STATIC INLINE BYTE Class2Tid(SIZE_T Length)
{
    WORD Magic = (Length >> CONFIG_OBJECT_SIZE_SHIFT) & 0xffff;
    BYTE Highest = GetWordHighestBit(Magic);
   
    return (BYTE) (Highest + (!!(Magic ^ (1 << Highest))));
}


STATIC INLINE KOBJECT_STATE SetObjectStateSafe(LPKOBJECT_HEADER lpHeader, KOBJECT_STATE NewState)
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
            CORE_ERROR(TRUE, "The class id value(%d) too large.", ClassID);
            CORE_SetError(STATE_OVER_RANGE);
            return NULL;
        }
        
        return CID2ClassDescriptor(ClassID);

    }

    CORE_ERROR(TRUE, "Invalid object header pointer.");
    CORE_SetError(STATE_INVALID_PARAMETER);
    return NULL;
}

STATIC INLINE LPCORE_CONTAINER KObject2Container(LPKOBJECT_HEADER lpHeader)
{
    DWORD Tid = GetObjectTid(lpHeader);
    
    if (Tid >= SIZEOF_ARRAY(g_GlobalObjectPoolTable))
    {
        CORE_ERROR(TRUE, "Object(0x%08x), Tid 0x%x, no pool manager found.", lpHeader, Tid);
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
        CORE_ERROR(TRUE, "Invalid parameter.");
        return STATE_INVALID_PARAMETER;
    }
    
    if (NULL == lpClass->ClassName)
    {
        CORE_ERROR(TRUE, "Invalid class(0x%p) name.", lpClass);
        return STATE_INVALID_NAME;
    }
    
    if (lpClass->ObjectSize > CONFIG_OBJECT_SIZE_MAX ||
        lpClass->ObjectSize < CONFIG_OBJECT_SIZE_MIN)
    {
        CORE_ERROR(TRUE, "Invalid object size(%d) for class(%s).",
            lpClass->ObjectSize, lpClass->ClassName);
        return STATE_INVALID_SIZE;
    }
    
    if (NULL == lpClass->fnMallocObject ||
        NULL == lpClass->fnActiveObject ||
        NULL == lpClass->fnTakeObject   ||
        NULL == lpClass->fnWaitObject   ||
        NULL == lpClass->fnPostObject   ||
        NULL == lpClass->fnResetObject  ||
        NULL == lpClass->fnFreeObject)
    {
        CORE_ERROR(TRUE, "Class(%s) method is invalid.", lpClass->ClassName);
        return STATE_INVALID_CLASS;
    }
    
    ClassID = Magic2ClassID(lpClass->Magic);
    
    CORE_INFOR(TRUE, "Class(%s-%d) register to %d ...",
            lpClass->ClassName, Magic2ClassIDKey(lpClass->Magic), ClassID);
    
    dwFlags = CORE_DisableIRQ();
    
    if (NULL != g_GlobalClassTable[ClassID])
    {
#ifdef __DEBUG__
        DWORD Count = 0;
        DWORD MagicArray[CONFIG_SYSTEM_CLASS_MAX];
#endif

#ifdef __DEBUG__
        Count = CORE_HashSetArray(LoadMagic, g_GlobalClassTable,
                MagicArray, lpClass->Magic, CONFIG_SYSTEM_CLASS_MAX);

        CORE_HashDebug(ClassHashKey, MagicArray, Count, 8, 32,
                "CONFIG_SYSTEM_CLASS_MAX", "Magic2ClassID");
#endif
        CORE_RestoreIRQ(dwFlags);
        CORE_ERROR(TRUE, "Class(%s) register to %d failed.", lpClass->ClassName, ClassID);
        CORE_ERROR(TRUE, "New class(%s) key is (%d), old class(%s) key is %d.",
            lpClass->ClassName, Magic2ClassIDKey(lpClass->Magic),
            g_GlobalClassTable[ClassID]->ClassName,
            Magic2ClassIDKey(g_GlobalClassTable[ClassID]->Magic));

        return STATE_EXISTING;
    }

    g_GlobalClassTable[ClassID] = (LPVOID) lpClass;
    
    CORE_RestoreIRQ(dwFlags);
    
    CORE_INFOR(TRUE, "Register class '%s' to %d successfully.",
            lpClass->ClassName, ClassID);
    
    return STATE_SUCCESS;
}



STATIC LPKOBJECT_HEADER MallocObjectFromPool(LPKCLASS_DESCRIPTOR lpClass, LPCSTR lpName)
{
    LPKOBJECT_HEADER lpHeader = NULL;
    LPCORE_CONTAINER lpManager = NULL;
    KCONTAINER_ID_T Pid = INVALID_CONTAINER_ID;
    KOBJTABLE_ID_T Tid = Class2Tid(lpClass->ObjectSize);

    CORE_ASSERT(Tid < CONFIG_OBJECT_POOL_TABLE_MAX, SYSTEM_CALL_OOPS(),
        "BUG: Invalid tid value %u, from object size %d bytes.", Tid, lpClass->ObjectSize);

    lpManager = TID2PoolContainer(Tid);
    
    if (NULL == lpManager)
    {
        CORE_ERROR(TRUE, "Invalid pool manager for magic(0x%08x), Tid = %d.", lpClass->Magic, Tid);
        CORE_SetError(STATE_INVALID_POOL);
        return NULL;
    }

    Pid = CORE_PoolMallocBlock(lpManager);

    if (INVALID_CONTAINER_ID == Pid)
    {
        CORE_ERROR(TRUE, "Invalid container id for magic(0x%08x).", lpClass->Magic);
        //CORE_SetError(STATE_INVALID_POOL); Set by pool
        return NULL;
    }

    lpHeader = CORE_PoolTakeBlock(lpManager, Pid);
    
    if (NULL == lpHeader)
    {
        CORE_ERROR(TRUE, "Can't found header for magic(0x%08x) pid(%d).", lpClass->Magic, Pid);
        return NULL;
    }
    
    if (NULL != lpName)
    {
        DWORD dwFlags = CORE_DisableIRQ();

        Attach2HashList(GetObjectHash(lpName), lpHeader);

        initNamedObject(lpHeader, lpClass->Magic, ((LPDWORD)lpName), Pid, GetSystemObjectSid(), Tid);
        
        CORE_RestoreIRQ(dwFlags);
    }
    else
    {
        initNonameObject(lpHeader, lpClass->Magic, Pid, GetSystemObjectSid(), Tid);
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

    State = SetObjectStateSafe(lpHeader, KOBJECT_STATE_FREE);

    if (KOBJECT_STATE_DEATH != State)
    {
        CORE_ERROR(TRUE, "Free object(0x%08x) state not detach(%d).", lpHeader, State);
        SetObjectStateSafe(lpHeader, State);
        return STATE_INVALID_STATE;
    }
    
    CORE_DEBUG(TRUE, "Free object(0x%08x) state is detach.", lpHeader);

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
        CORE_ERROR(TRUE, "handle 0x%08x, Tid 0x%x, no pool manager found.", handle, Tid);
        CORE_SetError(STATE_INVALID_POOL);
        return NULL;
    }
    
    lpHeader = CORE_PoolTakeBlock(lpManager, Pid);

    if (NULL == lpHeader)
    {
        CORE_ERROR(TRUE, "handle 0x%08x, container 0x%x, no object header found.", handle, Pid);
        CORE_SetError(STATE_INVALID_OBJECT);
        return NULL;
    }

    if (TRUE == Check)
    {
        KOBJECT_STATE State = GetObjectState(lpHeader);

        if (KOBJECT_STATE_CREATE != State && KOBJECT_STATE_ACTIVE != State)
        {
            CORE_DEBUG(TRUE, "handle 0x%08x object %p state %d is invalid.",4
                handle, lpHeader, State);
            CORE_SetError(STATE_INVALID_OBJECT);
            return NULL;
        }

        if (handle != GetObjectHandle(lpHeader))
        {
            CORE_DEBUG(TRUE, "handle 0x%08x not 0x%08x.", handle, GetObjectHandle(lpHeader));
            CORE_SetError(STATE_INVALID_OBJECT);
            return NULL;
        }
    }

    return lpHeader;
}

EXPORT LPKOBJECT_HEADER CORE_MallocNoNameObject(DWORD Magic, LPVOID lpParam)
{
    E_STATUS State;
    LPKOBJECT_HEADER lpHeader = NULL;
    LPKCLASS_DESCRIPTOR lpClass = NULL;

    lpClass = CID2ClassDescriptor(Magic2ClassID(Magic));
    
    if (NULL == lpClass)
    {
        CORE_ERROR(TRUE, "No class(0x%08x) to malloc no name object.", Magic);
        CORE_SetError(STATE_INVALID_CLASS);
        return NULL;
    }
    
    if (Magic != lpClass->Magic)
    {
        CORE_ERROR(TRUE, "Magic(0x%08x) not match for class(0x%08x) to create no name object.",
            Magic, lpClass->Magic);
        CORE_SetError(STATE_NOT_MATCH);
        return NULL;
    }

    CORE_ASSERT(lpClass->fnMallocObject, SYSTEM_CALL_OOPS(), 
        "BUG: Not support function for class(%s) to create no name object.",
            lpClass->ClassName);

    lpHeader = MallocObjectFromPool(lpClass, NULL);

    if (NULL == lpHeader)
    {
        CORE_ERROR(TRUE, "Can't malloc no name object for class '%s'",
            lpClass->ClassName);
        return NULL;
    }

    State = lpClass->fnMallocObject(lpHeader, lpParam);
    
    if (STATE_SUCCESS != State)
    {
        CORE_ERROR(TRUE, "No name bject call malloc in the class '%s' failed.",
            lpClass->ClassName);
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
    LPKOBJECT_HEADER lpHeader = NULL;
    LPKCLASS_DESCRIPTOR lpClass = NULL;

    if (NULL == lpName)
    {
        CORE_ERROR(TRUE, "Invalid name to open object for class(0x%08x).", Magic);
        CORE_SetError(STATE_INVALID_NAME);
        return NULL;
    }
    
    lpClass = CID2ClassDescriptor(Magic2ClassID(Magic));
    
    if (NULL == lpClass)
    {
        CORE_ERROR(TRUE, "No class(0x%08x) to malloc object '%s'.", Magic, lpName);
        CORE_SetError(STATE_INVALID_CLASS);
        return NULL;
    }
    
    if (Magic != lpClass->Magic)
    {
        CORE_ERROR(TRUE, "Magic(0x%08x) not match for class(0x%08x) to open object '%s'",
            Magic, lpClass->Magic, lpName);
        CORE_SetError(STATE_NOT_MATCH);
        return NULL;
    }

    CORE_ASSERT(lpClass->fnMallocObject, SYSTEM_CALL_OOPS(), 
        "BUG: Not support function for class(%s) to malloc object '%s'.",
            lpClass->ClassName, lpName);

    lpHeader = FindObjectHash(lpName);

    if (NULL != lpHeader)
    {
        CORE_ERROR(TRUE, "Object '%s' was exist: 0x%P, '%s'.", lpName, lpHeader, GetObjectName(lpHeader));
        CORE_SetError(STATE_EXISTING);
        return NULL;
    }
    
    lpHeader = MallocObjectFromPool(lpClass, lpName);

    if (NULL == lpHeader)
    {
        CORE_ERROR(TRUE, "Can't malloc object '%s' for class '%s'",
            lpName, lpClass->ClassName);
        return NULL;
    }

    State = lpClass->fnMallocObject(lpHeader, lpParam);
    
    if (STATE_SUCCESS != State)
    {
        CORE_ERROR(TRUE, "Object '%s' call malloc in the class '%s' failed.",
            lpName, lpClass->ClassName);
        SetObjectState(lpHeader, KOBJECT_STATE_DEATH);
        CORE_FreeObject(lpHeader);
        lpHeader = NULL;
    }
    
    CORE_SetError(State);
    
    return lpHeader;
     
}

EXPORT E_STATUS CORE_ActiveObject(LPKOBJECT_HEADER lpHeader, LPVOID lpParam)
{
    LPKCLASS_DESCRIPTOR lpClass = KObject2KClass(lpHeader);
    
    if (NULL == lpClass)
    {
        return CORE_GetError();
    }

    if (KOBJECT_STATE_CREATE != GetObjectState(lpHeader))
    {
        CORE_ERROR(TRUE, "Object handle 0x%p state %d error.",
                lpHeader, GetObjectState(lpHeader));
        return STATE_INVALID_STATE;
    }

    CORE_ASSERT(lpClass->fnActiveObject, SYSTEM_CALL_OOPS(), 
        "BUG: Not support function for class(%s) to wait object '%s'.",
        lpClass->ClassName, GetObjectName(lpHeader));

    return lpClass->fnActiveObject(lpHeader, lpParam);
}

EXPORT LPKOBJECT_HEADER CORE_TakeObject(LPCSTR lpName, LPVOID lpParam)
{
    E_STATUS State;
    LPKOBJECT_HEADER lpHeader = NULL;
    LPKCLASS_DESCRIPTOR lpClass = NULL;
 
    if (NULL == lpName)
    {
        CORE_ERROR(TRUE, "BUG: Invalid name to take object.");
        return NULL;
    }

    lpHeader = FindObjectHash(lpName);
    lpClass = KObject2KClass(lpHeader);


    if (NULL == lpClass)
    {
        return NULL;
    }

    if (KOBJECT_STATE_ACTIVE != GetObjectState(lpHeader))
    {
        CORE_ERROR(TRUE, "Object handle 0x%p state %d error.",
                lpHeader, GetObjectState(lpHeader));
        CORE_SetError(STATE_INVALID_STATE);
        return NULL;
    }

    CORE_ASSERT(lpClass->fnTakeObject, SYSTEM_CALL_OOPS(), 
        "BUG: Not support function for class(%s) to open object '%s'.",
            lpClass->ClassName, lpName);
    
    State = lpClass->fnTakeObject(lpHeader, lpParam);
    
    CORE_SetError(State);
    
    if (STATE_SUCCESS != State)
    {
        CORE_ERROR(TRUE, "Object '%s' call open in the class '%s' failed.",
            lpName, lpClass->ClassName);
        
        return NULL;
    }

    return lpHeader;
}

EXPORT E_STATUS CORE_WaitObject(LPKOBJECT_HEADER lpHeader, DWORD WaitTime)
{
    LPKCLASS_DESCRIPTOR lpClass = KObject2KClass(lpHeader);
    
    if (NULL == lpClass)
    {
        return CORE_GetError();
    }

    if (KOBJECT_STATE_ACTIVE != GetObjectState(lpHeader))
    {
        CORE_ERROR(TRUE, "Object handle 0x%p state %d error.",
                lpHeader, GetObjectState(lpHeader));
        return STATE_INVALID_STATE;
    }

    CORE_ASSERT(lpClass->fnWaitObject, SYSTEM_CALL_OOPS(), 
        "BUG: Not support function for class(%s) to wait object '%s'.",
        lpClass->ClassName, GetObjectName(lpHeader));
    
    return lpClass->fnWaitObject(lpHeader, WaitTime);
}

EXPORT E_STATUS CORE_PostObject(LPKOBJECT_HEADER lpHeader, LPVOID lpParam)
{
    LPKCLASS_DESCRIPTOR lpClass = KObject2KClass(lpHeader);
    
    if (NULL == lpClass)
    {
        return CORE_GetError();
    }

    if (KOBJECT_STATE_ACTIVE != GetObjectState(lpHeader))
    {
        CORE_ERROR(TRUE, "Object handle 0x%p state %d error.",
                lpHeader, GetObjectState(lpHeader));
        return STATE_INVALID_STATE;
    }

    CORE_ASSERT(lpClass->fnPostObject, SYSTEM_CALL_OOPS(), 
        "BUG: Not support function for class(%s) to post object '%s'.",
        lpClass->ClassName, GetObjectName(lpHeader));
    
    return lpClass->fnPostObject(lpHeader, lpParam);
}

EXPORT E_STATUS CORE_ResetObject(LPKOBJECT_HEADER lpHeader, LPVOID lpParam)
{
    LPKCLASS_DESCRIPTOR lpClass = KObject2KClass(lpHeader);
    
    if (NULL == lpClass)
    {
        return CORE_GetError();
    }

    if (KOBJECT_STATE_ACTIVE != GetObjectState(lpHeader))
    {
        CORE_ERROR(TRUE, "Object handle 0x%p state %d error.",
                lpHeader, GetObjectState(lpHeader));
        return STATE_INVALID_STATE;
    }

    CORE_ASSERT(lpClass->fnResetObject, SYSTEM_CALL_OOPS(), 
        "BUG: Not support function for class(%s) to reset object '%s'.",
        lpClass->ClassName, GetObjectName(lpHeader));
    
    return lpClass->fnResetObject(lpHeader, lpParam);
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
    
    State = SetObjectStateSafe(lpHeader, KOBJECT_STATE_DEATH);

    if (KOBJECT_STATE_FREE == State || KOBJECT_STATE_DEATH == State)
    {
        CORE_ERROR(TRUE, "Object handle 0x%p state %d error.",
                lpHeader, GetObjectState(lpHeader));
        return STATE_INVALID_STATE;
    }

    CORE_ASSERT(lpClass->fnFreeObject, SYSTEM_CALL_OOPS(), 
        "BUG: Not support function for class(%s) to free object '%s'.",
        lpClass->ClassName, GetObjectName(lpHeader));

    if (STATE_SUCCESS == (Result = lpClass->fnFreeObject(lpHeader)))
    {   
        CORE_INFOR(TRUE, "Object '%s' call free in the class '%s' successfully.",
            GetObjectName(lpHeader), lpClass->ClassName);
        
        if (STATE_SUCCESS == (Result = FreeObjectToPool(lpHeader)))
        {
            return Result;
        }
    }

    SetObjectStateSafe(lpHeader, State);

    CORE_ERROR(TRUE, "Object '%s' call free in the class '%s' failed result %d.",
            GetObjectName(lpHeader), lpClass->ClassName, Result);

    return Result;
}

EXPORT E_STATUS CORE_DetachIPCQueue(LPKOBJECT_HEADER lpHeader, LPVOID lpParam)
{
    LPKCLASS_DESCRIPTOR lpClass = KObject2KClass(lpHeader);
    
    if (NULL == lpClass)
    {
        return CORE_GetError();
    }
    
    if (KOBJECT_STATE_ACTIVE != GetObjectState(lpHeader))
    {
        CORE_ERROR(TRUE, "Object handle 0x%p state %d error.",
                lpHeader, GetObjectState(lpHeader));
        return STATE_INVALID_STATE;
    }
    
    return STATE_SUCCESS;
    
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

    REQhParam(lpParam, u1) = GetObjectHandle(lpHeader);
    
    return STATE_SUCCESS;
}

STATIC E_STATUS SVC_ActiveObject(LPVOID lpPrivate, LPVOID lpParam)
{
    E_STATUS State = CORE_ActiveObject(CORE_Handle2Header(REQhParam(lpParam, u0)), REQpParam(lpParam, u1));
    
    CORE_SetError(State);
    
    return State;
}

STATIC E_STATUS SVC_TakeObject(LPVOID lpPrivate, LPVOID lpParam)
{
    LPKOBJECT_HEADER lpHeader = CORE_TakeObject(REQpParam(lpParam, u0), REQpParam(lpParam, u1));
    
    if (NULL == lpHeader)
    {
        REQhParam(lpParam, u1) = INVALID_HANDLE_VALUE;
        return CORE_GetError();
    }
    
    REQhParam(lpParam, u1) = GetObjectHandle(lpHeader);
    
    return STATE_SUCCESS;
}


STATIC E_STATUS SVC_WaitObject(LPVOID lpPrivate, LPVOID lpParam)
{
    E_STATUS State =  CORE_WaitObject(CORE_Handle2Header(REQhParam(lpParam, u0)), REQdParam(lpParam, u1));
    
    CORE_SetError(State);
    
    return State;
}

STATIC E_STATUS SVC_PostObject(LPVOID lpPrivate, LPVOID lpParam)
{
    E_STATUS State =  CORE_PostObject(CORE_Handle2Header(REQhParam(lpParam, u0)), REQpParam(lpParam, u1));
    
    CORE_SetError(State);
    
    return State;
}

STATIC E_STATUS SVC_ResetObject(LPVOID lpPrivate, LPVOID lpParam)
{
    E_STATUS State =  CORE_ResetObject(CORE_Handle2Header(REQhParam(lpParam, u0)), REQpParam(lpParam, u1));  
    
    CORE_SetError(State);
    
    return State;  
}

STATIC E_STATUS SVC_FreeObject(LPVOID lpPrivate, LPVOID lpParam)
{
    E_STATUS State =  CORE_FreeObject(CORE_Handle2Header(REQhParam(lpParam, u0)));  
    
    CORE_SetError(State);
    
    return State; 
}

STATIC E_STATUS SVC_GetObjectName(LPVOID lpPrivate, LPVOID lpParam)
{
    LPKOBJECT_HEADER lpHeader;

    lpHeader = TASK_SELF_HANDLE == REQhParam(lpParam, u0)
             ? (LPVOID) CORE_GetCurrentTask()
             : CORE_Handle2Header(REQhParam(lpParam, u0));
    
    if (NULL == lpHeader)
    {
        CORE_SetError(STATE_INVALID_OBJECT);
        return STATE_INVALID_OBJECT;
    }
    
    if (NULL == REQpParam(lpParam, u1) || REQdParam(lpParam, u2) < OBJECT_NAME_MAX)
    {
        CORE_SetError(STATE_INVALID_PARAMETER);
        return STATE_INVALID_PARAMETER;
    }

    DumpObjectName(lpHeader, (LPDWORD)(REQpParam(lpParam, u1)));

    CORE_SetError(STATE_SUCCESS);

    return STATE_SUCCESS;
}

STATIC CONST REQUEST_HANDLER fnHandlers[] = {
    SVC_MallocObject,                   /**< 0.LPC_SOM_OBJECT_MALLOC */
    SVC_ActiveObject,                   /**< 1.LPC_SOM_OBJECT_ACTIVE */
    SVC_TakeObject,                     /**< 2.LPC_SOM_OBJECT_TAKE */
    SVC_WaitObject,                     /**< 3.LPC_SOM_OBJECT_WAIT */
    SVC_PostObject,                     /**< 4.LPC_SOM_OBJECT_POST */
    SVC_ResetObject,                    /**< 5.LPC_SOM_OBJECT_RESET */
    SVC_FreeObject,                     /**< 6.LPC_SOM_OBJECT_FREE */
    SVC_GetObjectName,                  /**< 7.LPC_SOM_OBJECT_GETNAME */
};


DEFINE_LPC_SERVICE(LPCService, SOM_MAGIC, SIZEOF_ARRAY(fnHandlers), NULL, fnHandlers);

PUBLIC E_STATUS initCoreSystemObjectPoolManager(VOID)
{
    DWORD i = 0;
    E_STATUS Result = STATE_SUCCESS;

    CORE_INFOR(TRUE, "OBJECT0_CONTAINER: %d      OBJECT1_CONTAINER: %d.",
            sizeof(OBJECT0_CONTAINER), sizeof(OBJECT0_CONTAINER));
    CORE_INFOR(TRUE, "OBJECT2_CONTAINER: %d      OBJECT3_CONTAINER: %d.",
            sizeof(OBJECT2_CONTAINER), sizeof(OBJECT3_CONTAINER));
    CORE_INFOR(TRUE, "OBJECT4_CONTAINER: %d      OBJECT5_CONTAINER: %d.",
            sizeof(OBJECT4_CONTAINER), sizeof(OBJECT5_CONTAINER));
    CORE_INFOR(TRUE, "OBJECT6_CONTAINER: %d      OBJECT7_CONTAINER: %d.",
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

