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
#ifndef __K_OBJECT_H
#define __K_OBJECT_H

#include <string.h>
#include <fadefs.h>
#include <faerror.h>
#include <fatypes.h>

#include "klist.h"
#include "kpool.h"

enum{
    LPC_SOM_OBJECT_MALLOC   =       0,
    LPC_SOM_OBJECT_ACTIVE   =       1,
    LPC_SOM_OBJECT_TAKE     =       2,
    LPC_SOM_OBJECT_FREE     =       3,
    LPC_SOM_OBJECT_GETNAME  =       4,
    LPC_SOM_OBJECT_METHOD   =       5,
    LPC_SOM_OBJECT_MALLOCNM =       6,
    LPC_SOM_REQUEST_MAX
};

typedef struct tagKOBJECT_HEADER KOBJECT_HEADER;
typedef struct tagKOBJECT_HEADER * PKOBJECT_HEADER;
typedef struct tagKOBJECT_HEADER FAR * LPKOBJECT_HEADER;

typedef struct tagKCLASS_HEADER KCLASS_HEADER;
typedef struct tagKCLASS_HEADER * PKCLASS_HEADER;
typedef struct tagKCLASS_HEADER FAR * LPKCLASS_HEADER;

typedef struct tagKCLASS_DESCRIPTOR KCLASS_DESCRIPTOR;
typedef struct tagKCLASS_DESCRIPTOR * PKCLASS_DESCRIPTOR;
typedef struct tagKCLASS_DESCRIPTOR FAR * LPKCLASS_DESCRIPTOR;

typedef E_STATUS (*FNCLASSMETHOD)(LPKOBJECT_HEADER Header, LPVOID lpParam);
#define     KCLASS_METHODS_MAX      8

struct tagKCLASS_HEADER{
    union{
        struct{
            DWORD       Magic:24;
            DWORD       Methods:8;
        }Bits;
        DWORD           Magic;
    }un;
    LPCSTR              lpClassName;
    SIZE_T              (* fnSizeofObject)(LPKCLASS_DESCRIPTOR lpClass, LPVOID lpParam);
    E_STATUS            (* fnMallocObject)(LPKOBJECT_HEADER Header, LPVOID lpParam);
    E_STATUS            (* fnActiveObject)(LPKOBJECT_HEADER Header, LPVOID lpParam);
    E_STATUS            (* fnTakeObject)(LPKOBJECT_HEADER Header, LPVOID lpParam);
    E_STATUS            (* fnFreeObject)(LPKOBJECT_HEADER Header);
};



struct tagKCLASS_DESCRIPTOR{
    KCLASS_HEADER           Header;
    FNCLASSMETHOD           fnClassMethods[KCLASS_METHODS_MAX];
    
#if 0
    E_STATUS            (* fnActiveObject)(LPKOBJECT_HEADER Header, LPVOID lpParam);
    E_STATUS            (* fnTakeObject)(LPKOBJECT_HEADER Header, LPVOID lpParam);
    E_STATUS            (* fnWaitObject)(LPKOBJECT_HEADER Header, LONG WaitTime);
    E_STATUS            (* fnPostObject)(LPKOBJECT_HEADER Header, LPVOID lpParam);
    E_STATUS            (* fnResetObject)(LPKOBJECT_HEADER Header, LPVOID lpParam);
    E_STATUS            (* fnDetachTask)(LPKOBJECT_HEADER Header, LPVOID lpParam);
#endif
};

#define     CLASS_MAGIC(M, Ops)         ((M) + (((Ops)<<(24))))

#define     DEFINE_KCLASS(KCLASS_NAME, CaseName, M, MaxOP, fnSize, fnMalloc, fnActive, fnTake, fnFree, ...) \
                                                                                                            \
            STATIC CONST KCLASS_NAME g_##CaseName = {                                                       \
                .Header.un.Bits.Magic       =       M,                                                      \
                .Header.un.Bits.Methods     =       MaxOP,                                                  \
                .Header.lpClassName         =       #CaseName,                                              \
                .Header.fnSizeofObject      =       fnSize,                                                 \
                .Header.fnMallocObject      =       fnMalloc,                                               \
                .Header.fnActiveObject      =       fnActive,                                               \
                .Header.fnTakeObject        =       fnTake,                                                 \
                .Header.fnFreeObject        =       fnFree,                                                 \
                {__VA_ARGS__},                                                                              \
            };

#define     REGISTER_KCLASS(CaseName)       CORE_RegisterClass((LPVOID)&(g_##CaseName))

#define     GetClassName(lpClass)           ((lpClass)->Header.lpClassName)
#define     GetClassMagic(lpClass)          ((lpClass)->Header.un.Bits.Magic)
#define     GetClassMethods(lpClass)        ((lpClass)->Header.un.Bits.Methods)
#define     GetClassMethodFn(lpClass, Id)   ((lpClass)->fnClassMethods[Id])
typedef enum{
    KOBJECT_STATE_FREE          =       0,                          /**< ¿ÕÏÐ×´Ì¬ */
    KOBJECT_STATE_CREATE        =       1,                          /**< ´´½¨×´Ì¬ */
    KOBJECT_STATE_ACTIVE        =       2,                          /**< »î¶¯×´Ì¬ */
    KOBJECT_STATE_DEATH         =       3,                          /**< ËÀÍö×´Ì¬ */
    KOBJECT_STATE_MAX           =       4,
}KOBJECT_STATE, * PKOBJECT_STATE, FAR * LPKOBJECT_STATE;

typedef unsigned long KOBJTABLE_ID_T;
typedef unsigned long KOBJCLASS_ID_T;
typedef unsigned long KOBJTABLE_ID_T;

#define     OBJECT_SID_BITS         11
#define     OBJECT_PID_BITS         10
#define     OBJECT_TID_BITS         3
#define     OBJECT_CID_BITS         5
#define     OBJECT_STS_BITS         2
#define     OBJECT_USR_BITS         1

typedef struct tagHANDLE_BITS{
    HANDLE              ObjectSid: OBJECT_SID_BITS;                /**< Object sn */
    HANDLE              ObjectPid: OBJECT_PID_BITS;                /**< Pool id */
    HANDLE              ObjectTid: OBJECT_TID_BITS;                /**< Table id */
    HANDLE              ObjectCid: OBJECT_CID_BITS;                /**< Class id */
    HANDLE              ObjectState: OBJECT_STS_BITS;              /**< Object state*/
    HANDLE              ObjectUser: OBJECT_USR_BITS;
}HANDLE_BITS;


#define     OBJECT_PID_MASK                         (0x3ffUL)
#define     OBJECT_SID_MASK                         (0x7ffUL)
#define     OBJECT_TID_MASK                         (CONFIG_OBJECT_POOL_TABLE_MASK)
#define     OBJECT_CID_MASK                         (CONFIG_SYSTEM_CLASS_MASK)
#define     OBJECT_STS_MASK                         (0x03UL)
#define     OBJECT_UFG_MASK                         (0x1UL)

#define     HANDLE_OBJECT_SID_SHIFT                 0
#define     HANDLE_OBJECT_SID_MASK                  (OBJECT_SID_MASK << HANDLE_OBJECT_SID_SHIFT)

#define     HANDLE_OBJECT_PID_SHIFT                 11
#define     HANDLE_OBJECT_PID_MASK                  (OBJECT_PID_MASK << HANDLE_OBJECT_PID_SHIFT)

#define     HANDLE_OBJECT_TID_SHIFT                 21
#define     HANDLE_OBJECT_TID_MASK                  (OBJECT_TID_MASK << HANDLE_OBJECT_TID_SHIFT)

#define     HANDLE_OBJECT_CID_SHIFT                 24
#define     HANDLE_OBJECT_CID_MASK                  (OBJECT_CID_MASK << HANDLE_OBJECT_CID_SHIFT)

#define     HANDLE_OBJECT_STATE_SHIFT               29
#define     HANDLE_OBJECT_STATE_MASK                (OBJECT_STS_MASK << HANDLE_OBJECT_STATE_SHIFT)

#define     HANDLE_OBJECT_USER_SHIFT                31
#define     HANDLE_OBJECT_USER_MASK                 (OBJECT_UFG_MASK << HANDLE_OBJECT_USER_SHIFT)

struct tagKOBJECT_HEADER{
    union{
        HANDLE_BITS     Bits;
        HANDLE          Handle;
    }uh;
    LIST_HEAD           HashNode;
    union{
        CHAR            caName[OBJECT_NAME_MAX];
        DWORD           daName[OBJECT_NAME_MAX/sizeof(DWORD)];
        DWORD           daMagic[OBJECT_NAME_MAX/sizeof(DWORD)];
        DWORD           daValue[OBJECT_NAME_MAX/sizeof(DWORD)];
        struct{
            DWORD               Total:5;
            DWORD               Marks:27;
            LPVOID              lpTask;
        }lsot;
    }un;
};


#define     OBJECT_ENTRY(Ptr, Member)                                                               \
            ((LPKOBJECT_HEADER)CONTAINER_OF(Ptr, KOBJECT_HEADER, Member))

#define     GetHandleObjectSid(handle)                                                              \
            (((handle) & HANDLE_OBJECT_SID_MASK) >> HANDLE_OBJECT_SID_SHIFT)
#define     SetHandleObjectSid(handle, sid)                                                         \
            do {                                                                                    \
                (handle) |= (((sid) << HANDLE_OBJECT_SID_SHIFT) & HANDLE_OBJECT_SID_MASK);          \
            } while(0)
#define     GetHandleObjectTid(handle)                                                              \
            (((handle) & HANDLE_OBJECT_TID_MASK) >> HANDLE_OBJECT_TID_SHIFT)
#define     SetHandleObjectTid(handle, Tid)                                                         \
            do {                                                                                    \
                (handle) |= (((Tid) << HANDLE_OBJECT_TID_SHIFT) & HANDLE_OBJECT_TID_MASK);          \
            } while(0)
#define     GetHandleObjectPid(handle)                                                              \
            (((handle) & HANDLE_OBJECT_PID_MASK) >> HANDLE_OBJECT_PID_SHIFT)
#define     SetHandleObjectPid(handle, pid)                                                         \
            do {                                                                                    \
                (handle) |= (((pid) << HANDLE_OBJECT_PID_SHIFT) & HANDLE_OBJECT_PID_MASK);          \
            } while(0)
#define     GetHandleObjectCid(handle)                                                              \
            (((handle) & HANDLE_OBJECT_CID_MASK) >> HANDLE_OBJECT_CID_SHIFT)
#define     SetHandleObjectCid(handle, cid)                                                         \
            do {                                                                                    \
                (handle) |= (((cid) << HANDLE_OBJECT_CID_SHIFT) & HANDLE_OBJECT_CID_MASK);          \
            }while(0)
#define     MakeHandle(cid, pid, state, sid, tid)                                                   \
            ((HANDLE)((((cid) << HANDLE_OBJECT_CID_SHIFT) & HANDLE_OBJECT_CID_MASK) |               \
            (((pid) << HANDLE_OBJECT_PID_SHIFT) & HANDLE_OBJECT_PID_MASK) |                         \
            (((sid) << HANDLE_OBJECT_SID_SHIFT) & HANDLE_OBJECT_SID_MASK) |                         \
            (((tid) << HANDLE_OBJECT_TID_SHIFT) & HANDLE_OBJECT_TID_MASK) |                         \
            (((state) << HANDLE_OBJECT_STATE_SHIFT) & HANDLE_OBJECT_STATE_MASK) |                   \
            HANDLE_OBJECT_USER_MASK))

#define     GetObjectHandle(lpHeader)                                                               \
            ((lpHeader)->uh.Handle & (~(HANDLE_OBJECT_STATE_MASK)))
#define     SetObjectHandle(lpHeader, H)            do { (lpHeader)->uh.Handle = (H);} while(0)

#define     GetObjectPid(lpHeader)                  ((lpHeader)->uh.Bits.ObjectPid)
#define     SetObjectPid(lpHeader, Pid)                                                             \
            do { (lpHeader)->uh.Bits.ObjectPid = (Pid);} while(0)

#define     GetObjectCid(lpHeader)                  ((KOBJCLASS_ID_T)((lpHeader)->uh.Bits.ObjectCid))
#define     SetObjectCid(lpHeader, Cid)                                                             \
            do { (lpHeader)->uh.Bits.ObjectCid = (Cid);} while(0)

#define     GetObjectSid(lpHeader)                  ((lpHeader)->uh.Bits.ObjectSid)
#define     SetObjectSid(lpHeader, Sid)                                                             \
            do { (lpHeader)->uh.Bits.ObjectSid = (Sid);} while(0)

#define     GetObjectTid(lpHeader)                  ((lpHeader)->uh.Bits.ObjectTid)
#define     SetObjectTid(lpHeader, Tid)                                                             \
            do { (lpHeader)->uh.Bits.ObjectTid = (Tid);} while(0)
            
#define     GetObjectState(lpHeader)                ((KOBJECT_STATE)((lpHeader)->uh.Bits.ObjectState))
#define     SetObjectState(lpHeader, State)                                                         \
            do { (lpHeader)->uh.Bits.ObjectState = (State);} while(0)
            
#define     GetObjectName(lpHeader)                 ((lpHeader)->un.caName)
#define     ObjectNameMatch(lpHeader, lpdName)                                                      \
            ((lpdName[0] == (lpHeader)->un.daName[0]) && (lpdName[1] == (lpHeader)->un.daName[1]))
#define     DumpObjectName(lpHeader, lpdName)                                                       \
            do {                                                                                    \
                (lpdName)[0] = (lpHeader)->un.daName[0];                                            \
                (lpdName)[1] = (lpHeader)->un.daName[1];                                            \
            } while(0)
#define     SetObjectName(lpHeader, lpdName)                                                        \
            do {                                                                                    \
                (lpHeader)->un.daName[0] = lpdName[0];                                              \
                (lpHeader)->un.daName[1] = lpdName[1];                                              \
            } while(0)

#define     ZeroObjectName(lpHeader)                                                                \
            do {                                                                                    \
                (lpHeader)->un.daName[0] = 0;                                                       \
                (lpHeader)->un.daName[1] = 0;                                                       \
            } while(0)

#define     SetObjectMagic(lpHeader, M0, M1)                                                        \
            do {                                                                                    \
                (lpHeader)->un.daMagic[0] = M0;                                                     \
                (lpHeader)->un.daMagic[1] = M0;                                                     \
            } while(0)

#define     GetObjecctByHashNode(lpNode)    OBJECT_ENTRY(lpNode, HashNode)
#define     initSelfHashNode(lpHeader)      LIST_HEAD_INIT(&lpHeader->HashNode)

            
#ifdef __cplusplus
extern "C" {
#endif
#define     CORE_Handle2Header(handle)          CORE_Handle2HeaderCheck(handle, TRUE)
    EXPORT LPKOBJECT_HEADER CORE_Handle2HeaderCheck(HANDLE handle, BOOL Check);
    PUBLIC E_STATUS CORE_RegisterClass(CONST KCLASS_DESCRIPTOR * Class);

    EXPORT LPKOBJECT_HEADER CORE_MallocObject(DWORD Magic, LPCSTR lpName, LPVOID lpParam);
    EXPORT LPKOBJECT_HEADER CORE_MallocNoNameObject(DWORD Magic, LPVOID lpParam);
    EXPORT E_STATUS CORE_ActiveObject(LPKOBJECT_HEADER lpHeader, LPVOID lpParam);
    EXPORT LPKOBJECT_HEADER CORE_TakeObject(LPCSTR lpName, LPVOID lpParam);
    EXPORT E_STATUS CORE_FreeObject(LPKOBJECT_HEADER lpHeader);
    EXPORT E_STATUS CORE_RequestMethod(LPKOBJECT_HEADER lpHeader, LPVOID lpParam, DWORD Method);

#ifdef __cplusplus
}
#endif
#endif

