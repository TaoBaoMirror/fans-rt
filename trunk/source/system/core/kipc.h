#ifndef __KIPC_H
#define __KIPC_H

#include <fadefs.h>
#include <fatypes.h>

#include "ktask.h"
#include "karch.h"
#include "kobject.h"

enum{
    KIPC_METHOD_WAIT        =       0,
    KIPC_METHOD_POST        =       1,
    KIPC_METHOD_RESET       =       2,
    KIPC_METHOD_DETACH      =       3,
    KIPC_CLASS_METHODS,
};

typedef struct tagKIPC_WAIT_PARAM KIPC_WAIT_PARAM;
typedef struct tagKIPC_WAIT_PARAM * PKIPC_WAIT_PARAM;
typedef struct tagKIPC_WAIT_PARAM FAR * LPKIPC_WAIT_PARAM;

struct tagKIPC_WAIT_PARAM{
    SHORT                       SignalID;
    LONG                        WaitTime;
};

#define     GetWaitTime4mParam(P)       (((LPKIPC_WAIT_PARAM)(P))->WaitTime)
#define     SetWaitTime2Param(P, Tm)    do { (((LPKIPC_WAIT_PARAM)(P))->WaitTime) = (Tm); } while(0)
#define     GetSignalID4mParam(P)       (((LPKIPC_WAIT_PARAM)(P))->SignalID)
#define     SetSignalID2Param(P, Id)    do { (((LPKIPC_WAIT_PARAM)(P))->SignalID) = (Id); } while(0)


typedef struct tagKIPC_CLASS_DESCRIPTOR KIPC_CLASS_DESCRIPTOR;
typedef struct tagKIPC_CLASS_DESCRIPTOR * PKIPC_CLASS_DESCRIPTOR;
typedef struct tagKIPC_CLASS_DESCRIPTOR FAR * LPKIPC_CLASS_DESCRIPTOR;

struct tagKIPC_CLASS_DESCRIPTOR{
    KCLASS_HEADER               Header;
    FNCLASSMETHOD               fnClassMethods[KIPC_CLASS_METHODS];
};

typedef struct tagKIPC_CLASS_HEADER KIPC_CLASS_HEADER;
typedef struct tagKIPC_CLASS_HEADER * PKIPC_CLASS_HEADER;
typedef struct tagKIPC_CLASS_HEADER FAR * LPKIPC_CLASS_HEADER;

struct tagKIPC_CLASS_HEADER{
    KOBJECT_HEADER      Header;
    LIST_HEAD           WaitQueue;
};

typedef union tagKIPC_ATTRIBUTE KIPC_ATTRIBUTE;
typedef union tagKIPC_ATTRIBUTE * PKIPC_ATTRIBUTE;
typedef union tagKIPC_ATTRIBUTE FAR * LPKIPC_ATTRIBUTE;

union tagKIPC_ATTRIBUTE{
    struct {
        DWORD           Lock:1;
        DWORD           Info:31;
    }Bits;
    struct {
        SPIN_LOCK_T     Lock:1;
        BYTE            Reserved:7;
        BYTE            Info[3];
    }Byte;
    DWORD               Value;
};

typedef struct tagKIPC_CLASS_BASE KIPC_CLASS_BASE;
typedef struct tagKIPC_CLASS_BASE * PKIPC_CLASS_BASE;
typedef struct tagKIPC_CLASS_BASE FAR * LPKIPC_CLASS_BASE;

struct tagKIPC_CLASS_BASE{
    KIPC_CLASS_HEADER           Base;
    VOLATILE KIPC_ATTRIBUTE     Attribute;
};

#define     GetIPCSpinLock(lpHeader)                                                                \
                (((LPKIPC_CLASS_BASE)(lpHeader))->Attribute.Byte.Lock)

#define     GetIPCAttribute(lpHeader)                                                               \
                (((LPKIPC_CLASS_BASE)(lpHeader))->Attribute.Value)

#define     SetIPCAttribute(lpHeader, lpAttribute)                                                  \
                do { (((LPKIPC_CLASS_BASE)(lpHeader))->Attribute.Value) =                           \
                        ((lpAttribute)->Value & (~0x1)); } while(0)
#define     IPC_SPIN_LOCK_IRQ(lpHeader, dwFlags)                                                    \
                CORE_SPIN_LOCK_IRQ((((LPKIPC_CLASS_BASE)(lpHeader))->Attribute.Byte.Lock), dwFlags)
#define     IPC_SPIN_UNLOCK_IRQ(lpHeader, dwFlags)                                                  \
                CORE_SPIN_UNLOCK_IRQ((((LPKIPC_CLASS_BASE)(lpHeader))->Attribute.Byte.Lock), dwFlags)
#define     IPC_ENTRY(Ptr, Member)                  ((LPKIPC_CLASS_HEADER)CONTAINER_OF(Ptr, KIPC_CLASS_HEADER, Member))
#define     GetHeaderByWaitQueue(Ptr)               IPC_ENTRY(Ptr, WaitQueue)
#define     GetIPCWaitQueue(lpObject)               (&(((LPKIPC_CLASS_HEADER)(lpObject))->WaitQueue))
#define     GetFirstWaitNode(lpObject)              LIST_NEXT_NODE(GetIPCWaitQueue(lpObject))
#define     GetFirstWaitTask(lpObject)              GetContextByIPCNode(GetFirstWaitNode(lpObject))

#define     EVENT_SIGNAL_SHIFT                      (0x1)
#define     EVENT_SIGNAL_MASK                       (1 << EVENT_SIGNAL_SHIFT)
#define     EVENT_AUTOMATIC_SHIFT                   (0x2)
#define     EVENT_AUTOMATIC_MASK                    (1 << EVENT_AUTOMATIC_SHIFT)
#define     EVENT_BITS_MASK                         (EVENT_SIGNAL_MASK | EVENT_AUTOMATIC_MASK)

typedef union tagEVENT_ATTRIBUTE EVENT_ATTRIBUTE;
typedef union tagEVENT_ATTRIBUTE * PEVENT_ATTRIBUTE;
typedef union tagEVENT_ATTRIBUTE FAR * LPEVENT_ATTRIBUTE;

union tagEVENT_ATTRIBUTE{
    struct{
        DWORD           Lock:1;
        DWORD           Signal:1;
        DWORD           Automatic:1;
        DWORD           Reserved:13;
        LONG            Blocked:16;
    }Bits;
    struct{
        SPIN_LOCK_T     Lock:1;
        BOOL            Signal:1;
        BOOL            Automatic:1;
        BYTE            Reserved:5;
        BYTE            ReservedW[3];
    }Byte;
    DWORD               Value;
};

#define     MUTEX_ONWER_MASK        (~HANDLE_OBJECT_SID_MASK)
#define     MUTEX_VALUE_BITS        OBJECT_SID_BITS

typedef union tagMUTEX_ATTRIBUTE MUTEX_ATTRIBUTE;
typedef union tagMUTEX_ATTRIBUTE * PMUTEX_ATTRIBUTE;
typedef union tagMUTEX_ATTRIBUTE FAR * LPMUTEX_ATTRIBUTE;

union tagMUTEX_ATTRIBUTE{
    struct {
        DWORD           Lock:1;
        LONG            MutexValue:10;
        HANDLE          hOnwerTask:32-MUTEX_VALUE_BITS;
    }Bits;
    struct {
        SPIN_LOCK_T     Lock:1;
        BYTE            MutexValueL:7;
        BYTE            MutexValueH:3;
        BYTE            hOnwerTaskL:5;
        BYTE            hOnwerTaskH[2];
    }Bytes;
    HANDLE              Handle;
    DWORD               Value;
};

typedef union tagSEMAPHORE_ATTRIBUTE SEMAPHORE_ATTRIBUTE;
typedef union tagSEMAPHORE_ATTRIBUTE * PSEMAPHORE_ATTRIBUTE;
typedef union tagSEMAPHORE_ATTRIBUTE FAR * LPSEMAPHORE_ATTRIBUTE;


union tagSEMAPHORE_ATTRIBUTE{
    struct {
        DWORD           Lock:1;
        DWORD           Reserved:1;
        LONG            Signal:15;
        LONG            MaxCount:15;
    }Bits;
    struct {
        SPIN_LOCK_T     Lock:1;
        BYTE            InforL:7;
        BYTE            InforH[3];
    }Byte;
    DWORD               Value;
};

typedef union tagSEMSET_ATTRIBUTE SEMSET_ATTRIBUTE;
typedef union tagSEMSET_ATTRIBUTE * PSEMSET_ATTRIBUTE;
typedef union tagSEMSET_ATTRIBUTE FAR * LPSEMSET_ATTRIBUTE;

#define     SEMSET_LOCK_SHIFT           (0)
#define     SEMSET_LOCK_MASK            (1<<SEMSET_LOCK_SHIFT)
#define     SEMSET_FULL_SHIFT           (1)
#define     SEMSET_FULL_MASK            (1<<SEMSET_FULL_SHIFT)
#define     SEMSET_BLOCKS_SHIFT         (2)
#define     SEMSET_BLOCKS_MASK          (0x1f << SEMSET_BLOCKS_SHIFT)
#define     SEMSET_LIGHTS_SHIFT         (7)
#define     SEMSET_LIGHTS_MASK          (0x1f << SEMSET_LIGHTS_SHIFT)
#define     SEMSET_SIGNAL_NULL          (0x00000000u)
#define     SEMSET_SIGNAL_FULL          (0x000fffffu)
#define     SEMSET_SIGNAL_SHIFT         (12)
#define     SEMSET_SIGNAL_MASK          (SEMSET_SIGNAL_FULL << SEMSET_SIGNAL_SHIFT)

union tagSEMSET_ATTRIBUTE{
    struct {
        DWORD           Lock:1;
        DWORD           Full:1;
        DWORD           Blocked:5;
        DWORD           Lights:5;
        DWORD           LightMask:20;
    }Bits;
    struct {
        SPIN_LOCK_T     Lock:1;
        BOOL            Full:1;
        BYTE            Blocked:5;
        BYTE            Reserved:1;
        BYTE            LightMask[3];
    }Byte;
    DWORD               Value;
};

#ifdef __cplusplus
extern "C" {
#endif
    EXPORT E_STATUS CORE_DetachIPCObject(LPTASK_CONTEXT lpTaskContext);
#ifdef __cplusplus
}
#endif
#endif

