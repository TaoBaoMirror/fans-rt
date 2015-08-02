#ifndef __KIPC_H
#define __KIPC_H

#include <fadefs.h>
#include <fatypes.h>

#include "ktask.h"
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
    LONG                        ObjectID;
    LONG                        WaitTime;
};

#define     GetWaitTime4mParam(P)       (((LPKIPC_WAIT_PARAM)(P))->WaitTime)
#define     SetWaitTime2Param(P, Tm)    do { (((LPKIPC_WAIT_PARAM)(P))->WaitTime) = (Tm); } while(0)
#define     GetObjectID4mParam(P)       (((LPKIPC_WAIT_PARAM)(P))->ObjectID)
#define     SetObjectID2Param(P, Id)    do { (((LPKIPC_WAIT_PARAM)(P))->ObjectID) = (Id); } while(0)


typedef struct tagKIPC_CLASS_DESCRIPTOR KIPC_CLASS_DESCRIPTOR;
typedef struct tagKIPC_CLASS_DESCRIPTOR * PKIPC_CLASS_DESCRIPTOR;
typedef struct tagKIPC_CLASS_DESCRIPTOR FAR * LPKIPC_CLASS_DESCRIPTOR;

struct tagKIPC_CLASS_DESCRIPTOR{
    KCLASS_HEADER               Header;
    FNCLASSMETHOD               fnClassMethods[KIPC_CLASS_METHODS];
};

typedef struct tagIPC_BASE_OBJECT IPC_BASE_OBJECT;
typedef struct tagIPC_BASE_OBJECT * PIPC_BASE_OBJECT;
typedef struct tagIPC_BASE_OBJECT FAR * LPIPC_BASE_OBJECT;

struct tagIPC_BASE_OBJECT{
    KOBJECT_HEADER      Header;
    LIST_HEAD           WaitQueue;
};
#define     IPC_ENTRY(Ptr, Member)                  ((LPIPC_BASE_OBJECT)CONTAINER_OF(Ptr, IPC_BASE_OBJECT, Member))
#define     GetHeaderByWaitQueue(Ptr)               IPC_ENTRY(Ptr, WaitQueue)
#define     GetIPCWaitQueue(lpObject)               (&(((LPIPC_BASE_OBJECT)(lpObject))->WaitQueue))
#define     GetFirstWaitNode(lpObject)              LIST_NEXT_NODE(GetIPCWaitQueue(lpObject))
#define     GetFirstWaitTask(lpObject)              GetContextByIPCNode(GetFirstWaitNode(lpObject))

#define     MARK_EVENT_SIGNAL_SHIFT                 (0x0)
#define     MARK_EVENT_SIGNAL_MASK                  (1 << MARK_EVENT_SIGNAL_SHIFT)
#define     MARK_EVENT_AUTO_SHIFT                   (0x1)
#define     MARK_EVENT_AUTO_MASK                    (1 << MARK_EVENT_AUTO_SHIFT)
#define     MARK_EVENT_BITS_MASK                    (MARK_EVENT_SIGNAL_MASK | MARK_EVENT_AUTO_MASK)

typedef union tagEVENT_ATTRIBUTE EVENT_ATTRIBUTE;
typedef union tagEVENT_ATTRIBUTE * PEVENT_ATTRIBUTE;
typedef union tagEVENT_ATTRIBUTE FAR * LPEVENT_ATTRIBUTE;

union tagEVENT_ATTRIBUTE{
    struct{
        BOOL           Signal:1;
        BOOL           Automatic:1;                 
    }Bits;
    BYTE               Value;
};

#define     MUTEX_VALUE_BITS        OBJECT_SID_BITS
#define     MUTEX_ONWER_MASK        (~HANDLE_OBJECT_SID_MASK)

typedef struct tagMUTEX_ATTRIBUTE MUTEX_ATTRIBUTE;
typedef struct tagMUTEX_ATTRIBUTE * PMUTEX_ATTRIBUTE;
typedef struct tagMUTEX_ATTRIBUTE FAR * LPMUTEX_ATTRIBUTE;

struct tagMUTEX_ATTRIBUTE{
    S32                 MutexValue: MUTEX_VALUE_BITS;              /**< Î¨Ò»Ê¶±ðÂë */
    HANDLE              hOnwerTask: 32 - MUTEX_VALUE_BITS;
};

typedef struct tagSEMAPHORE_ATTRIBUTE SEMAPHORE_ATTRIBUTE;
typedef struct tagSEMAPHORE_ATTRIBUTE * PSEMAPHORE_ATTRIBUTE;
typedef struct tagSEMAPHORE_ATTRIBUTE FAR * LPSEMAPHORE_ATTRIBUTE;

struct tagSEMAPHORE_ATTRIBUTE{
    SHORT               Signal;
    SHORT               MaxCount;
};

typedef struct tagSEMSET_ATTRIBUTE SEMSET_ATTRIBUTE;
typedef struct tagSEMSET_ATTRIBUTE * PSEMSET_ATTRIBUTE;
typedef struct tagSEMSET_ATTRIBUTE FAR * LPSEMSET_ATTRIBUTE;

struct tagSEMSET_ATTRIBUTE{
    DWORD               Mask:32;
};

#ifdef __cplusplus
extern "C" {
#endif
    EXPORT E_STATUS CORE_DetachIPCObject(LPTASK_CONTEXT lpTaskContext);
#ifdef __cplusplus
}
#endif
#endif

