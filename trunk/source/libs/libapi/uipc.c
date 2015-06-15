#include <string.h>
#include <fadefs.h>
#include <faerror.h>
#include <fatypes.h>
#include <fapi.h>

#include "kipc.h"
#include "kcore.h"
#include "libcal.h"
#include "cadebug.h"


FANSAPI HANDLE TakeObject(LPCTSTR lpCTName)
{
    CHAR caName[OBJECT_NAME_MAX];

    memset(caName, 0, sizeof(caName));
#ifdef _UNICODE
#error "Not implemented unicode."
#else
    strncpy(caName, lpCTName, OBJECT_NAME_MAX-1);
#endif
    
    return ntTakeObject(caName, NULL);
}


FANSAPI HANDLE CreateEvent(LPCTSTR lpCTName, BOOL Automatic, BOOL Signal)
{
    HANDLE hEvent;
    IPC_EVENT_CREATE_PARAM Param;
    CHAR caName[OBJECT_NAME_MAX];
    
    memset(caName, 0, sizeof(caName));

    if (NULL == lpCTName)
    {
        ntChooseName(caName, "EVT");
    }
    else
    {
#ifdef _UNICODE
#error "Not implemented unicode."
#else
        strncpy(caName, lpCTName, OBJECT_NAME_MAX-1);
#endif
    }

    Param.Marks.Value = ((!!Automatic) << MARK_EVENT_AUTO_SHIFT)
                      + ((!!Signal) << MARK_EVENT_SIGNAL_SHIFT);

    hEvent = ntMallocObject(caName, EVT_MAGIC, &Param);

    if (INVALID_HANDLE_VALUE != hEvent)
    {
        if (STATE_SUCCESS != ntActiveObject(hEvent, &Param))
        {
            ntFreeObject(hEvent);
            hEvent = INVALID_HANDLE_VALUE;
        }
    }

    return hEvent;
}

FANSAPI E_STATUS PostEvent(HANDLE handle)
{
    return ntPostObject(handle, NULL);
}

FANSAPI E_STATUS ResetEvent(HANDLE handle)
{
    return ntResetObject(handle, NULL);
}


FANSAPI HANDLE CreateMutex(LPCTSTR lpCTName, BOOL Owner)
{
    HANDLE hMutex;
    IPC_MUTEX_CREATE_PARAM Param;
    CHAR caName[OBJECT_NAME_MAX];
    
    memset(caName, 0, sizeof(caName));

    if (NULL == lpCTName)
    {
        ntChooseName(caName, "EVT");
    }
    else
    {
#ifdef _UNICODE
#error "Not implemented unicode."
#else
        strncpy(caName, lpCTName, OBJECT_NAME_MAX-1);
#endif
    }

    Param.Marks.Value = ((!Owner) << MARK_MUTEX_SIGNAL_SHIFT);

    hMutex = ntMallocObject(caName, MTX_MAGIC, &Param);

    if (INVALID_HANDLE_VALUE != hMutex)
    {
        if (STATE_SUCCESS != ntActiveObject(hMutex, &Param))
        {
            ntFreeObject(hMutex);
            hMutex = INVALID_HANDLE_VALUE;
        }
    }

    return hMutex;
}

FANSAPI E_STATUS MutexUnlock(HANDLE hMutex)
{
    return ntPostObject(hMutex, NULL);
}

FANSAPI E_STATUS MutexLock(HANDLE hMutex)
{
    return ntWaitObject(hMutex, WAIT_INFINITE);
}

FANSAPI E_STATUS WaitObject(HANDLE handle, LONG WaitTime)
{
    return ntWaitObject(handle, WaitTime);
}

FANSAPI E_STATUS CloseHandle(HANDLE handle)
{
    return ntFreeObject(handle);
}
