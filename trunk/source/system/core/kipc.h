#ifndef __KIPC_H
#define __KIPC_H

#include <fadefs.h>
#include <fatypes.h>

#include "kobject.h"

#define         MARK_EVENT_SIGNAL_SHIFT         0x0
#define         MARK_EVENT_SIGNAL_MASK          (1 << MARK_EVENT_SIGNAL_SHIFT)
#define         MARK_EVENT_AUTO_SHIFT           0x1
#define         MARK_EVENT_AUTO_MASK            (1 << MARK_EVENT_AUTO_SHIFT)
#define         MARK_EVENT_BITS_MASK            (MARK_EVENT_SIGNAL_MASK | MARK_EVENT_AUTO_MASK)

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

#endif

