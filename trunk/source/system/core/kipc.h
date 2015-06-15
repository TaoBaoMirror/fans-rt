#ifndef __KIPC_H
#define __KIPC_H

#include <fadefs.h>
#include <fatypes.h>

typedef struct tagIPC_EVENT_CREATE_PARAM IPC_EVENT_CREATE_PARAM;
typedef struct tagIPC_EVENT_CREATE_PARAM * PIPC_EVENT_CREATE_PARAM;
typedef struct tagIPC_EVENT_CREATE_PARAM FAR * LPIPC_EVENT_CREATE_PARAM;

#define         MARK_EVENT_SIGNAL_SHIFT         0x0
#define         MARK_EVENT_SIGNAL_MASK          (1 << MARK_EVENT_SIGNAL_SHIFT)
#define         MARK_EVENT_AUTO_SHIFT           0x1
#define         MARK_EVENT_AUTO_MASK            (1 << MARK_EVENT_AUTO_SHIFT)
#define         MARK_EVENT_BITS_MASK            (MARK_EVENT_SIGNAL_MASK | MARK_EVENT_AUTO_MASK)

typedef union tagIPC_EVENT_MARKS{
    struct{
        BOOL           Signal:1;
        BOOL           Automatic:1;
    }Bits;
    BYTE               Value;
}IPC_EVENT_MARKS;

struct tagIPC_EVENT_CREATE_PARAM{
    IPC_EVENT_MARKS     Marks;
};

typedef struct tagIPC_MUTEX_CREATE_PARAM IPC_MUTEX_CREATE_PARAM;
typedef struct tagIPC_MUTEX_CREATE_PARAM * PIPC_MUTEX_CREATE_PARAM;
typedef struct tagIPC_MUTEX_CREATE_PARAM FAR * LPIPC_MUTEX_CREATE_PARAM;

#define         MARK_MUTEX_SIGNAL_SHIFT         0x0
#define         MARK_MUTEX_SIGNAL_MASK          (1 << MARK_MUTEX_SIGNAL_SHIFT)
#define         MARK_MUTEX_BITS_MASK            (MARK_MUTEX_SIGNAL_MASK)

typedef union tagIPC_MUTEX_MARKS{
    struct{
        BOOL            Signal:1;           /* пе╨е */
    }Bits;
    BYTE                Value;
}IPC_MUTEX_MARKS;


struct tagIPC_MUTEX_CREATE_PARAM{
    IPC_MUTEX_MARKS     Marks;
};

#endif

