#include <string.h>
#include <fauser.h>
#include <fadefs.h>
#include <faerror.h>
#include <fatypes.h>
#include <fapi.h>

#include "kipc.h"
#include "kcore.h"
#include "libcal.h"
#include "cadebug.h"

/**
 * Take a specified object.
 * @param The name of object.
 * @return The handle of object.
 * \par
 * Call system to find specified object, if the name has been found, return
 * the handle of object, else return invalid object.
 *
 * ����ϵͳ���ܲ���ָ���Ķ���, �����������һ�µĶ��󣬷��ض�����������
 * ����Ч���
 *
 * date           author          notes
 * 2015-06-16     JiangYong       first version
 */
FANSAPI CODE_TEXT HANDLE TakeObject(LPCTSTR lpCTName)
{
    CHAR caName[OBJECT_NAME_MAX];

    memset(caName, 0, sizeof(caName));
#ifdef _UNICODE
#error "Not implemented unicode."
#else
    strncpy(caName, lpCTName, OBJECT_NAME_MAX-1);
#endif
    
    return caTakeObject(caName, NULL);
}

/**
 * Create an event object.
 * @param The name of object.
 * @param Event automatic mode.
 * @param The signal of event.
 * @return The handle of object.
 * \par
 * Call system to create event object, if the specified name has been found, 
 * the function will be return invalid handle(means create failed), else 
 * return the handle of object for want to created.
 * If the parameter 'Automatic' is TRUE, the event object can be automatic
 * reset self(no need call ResetEvent function).
 * If the parameter 'Signal' is TRUE, the first task no need wait.
 *
 * ����һ���¼�������������Ѿ����ڣ��򷵻���Ч���(��ʾ����ʧ��)�����򷵻�
 * �ľ��Ϊ�����Ķ�������
 * ������� Automatic Ϊ TRUE, �ö�����Զ���λ(����Ҫ���ú��� ResetEvent)
 * ������� Signal Ϊ TRUE, ��һ��������Ҫ�ȴ�.
 *
 * date           author          notes
 * 2015-06-16     JiangYong       first version
 */
FANSAPI CODE_TEXT HANDLE CreateEvent(LPCTSTR lpCTName, BOOL Automatic, BOOL Signal)
{
    HANDLE hEvent;
    EVENT_ATTRIBUTE Attribute;
    CHAR caName[OBJECT_NAME_MAX];
    
    memset(caName, 0, sizeof(caName));

    if (NULL == lpCTName)
    {
        caChooseName(caName, "EVT");
    }
    else
    {
#ifdef _UNICODE
#error "Not implemented unicode."
#else
        strncpy(caName, lpCTName, OBJECT_NAME_MAX-1);
#endif
    }

    Attribute.Value   = ((!!Automatic) << MARK_EVENT_AUTO_SHIFT)
                      + ((!!Signal) << MARK_EVENT_SIGNAL_SHIFT);

    hEvent = caMallocObject(caName, EVT_MAGIC, &Attribute);

    if (INVALID_HANDLE_VALUE != hEvent)
    {
        if (STATE_SUCCESS != caActiveObject(hEvent, &Attribute))
        {
            caFreeObject(hEvent);
            hEvent = INVALID_HANDLE_VALUE;
        }
    }

    return hEvent;
}

/**
 * Post signal to event.
 * @param The handle of event object.
 * @return STATE_SUCCESS successfully.
 * @return STATE_INVALID_PARAMETER the handle of object is invalid.
 * @return STATE_INVALID_STATE the state of event object is not active.
 * @return STATE_REMOVED the object has been removed.
 * \par
 * After call this function, the all tasks blocked by the object will be awakened.
 *
 * ���øú����󣬱��ö����������������񽫱����ѡ�
 *
 * date           author          notes
 * 2015-06-16     JiangYong       first version
 */
FANSAPI CODE_TEXT E_STATUS PostEvent(HANDLE handle)
{
    return caPostObject(handle, NULL);
}

/**
 * Reset the signal of event.
 * @param The handle of event object.
 * @return STATE_SUCCESS successfully.
 * @return STATE_INVALID_PARAMETER the handle of object is invalid.
 * @return STATE_INVALID_STATE the state of event object is not active.
 * @return STATE_REMOVED the object has been removed.
 * \par
 * After reset event, the signal of event object will be lost.
 * 
 * ���øú������¼�������źŽ���ʧ��
 *
 * date           author          notes
 * 2015-06-16     JiangYong       first version
 */
FANSAPI CODE_TEXT E_STATUS ResetEvent(HANDLE handle)
{
    return caResetObject(handle, NULL);
}

/**
 * Create a mutex object.
 * @param The name of object.
 * @param Lock mutex by create task.
 * @return The handle of object.
 * \par
 * Call system to create mutex object, if the specified name has been found, 
 * the function will be return invalid handle(means create failed), else 
 * return the handle of object for want to created.
 * If the parameter 'Owner' is TRUE, the mutex object will be locked, and
 * the create task is owner task.
 *
 * ����һ�����������������Ѿ����ڣ��򷵻���Ч���(��ʾ����ʧ��)�����򷵻�
 * �ľ��Ϊ�����Ķ�������
 * ������� Owner Ϊ TRUE, �ö��󽫱���ס�����������������Ǹ����ĳ����ߡ�
 *
 * date           author          notes
 * 2015-06-16     JiangYong       first version
 */
FANSAPI CODE_TEXT HANDLE CreateMutex(LPCTSTR lpCTName, BOOL Owner)
{
    HANDLE hMutex;
    MUTEX_ATTRIBUTE Attribute;
    CHAR caName[OBJECT_NAME_MAX];
    
    memset(caName, 0, sizeof(caName));

    if (NULL == lpCTName)
    {
        caChooseName(caName, "MTX");
    }
    else
    {
#ifdef _UNICODE
#error "Not implemented unicode."
#else
        strncpy(caName, lpCTName, OBJECT_NAME_MAX-1);
#endif
    }

    Attribute.MutexValue = Owner ? 0 : 1;

    hMutex = caMallocObject(caName, MTX_MAGIC, &Attribute);

    if (INVALID_HANDLE_VALUE != hMutex)
    {
        if (STATE_SUCCESS != caActiveObject(hMutex, &Attribute))
        {
            caFreeObject(hMutex);
            hMutex = INVALID_HANDLE_VALUE;
        }
    }

    return hMutex;
}

/**
 * Unlock specified mutex.
 * @param The handle of mutex object.
 * @return STATE_SUCCESS successfully.
 * @return STATE_INVALID_PARAMETER the handle of object is invalid.
 * @return STATE_INVALID_STATE the state of event object is not active.
 * @return STATE_REMOVED the object has been removed.
 * \par
 * Unlock specified mutex, the highest priority tasks blocked by the object
 * will be awakened.
 *
 * ����ָ���Ļ�����󣬱��ö���������������ȼ����񽫱����ѡ�
 *
 * date           author          notes
 * 2015-06-16     JiangYong       first version
 */
FANSAPI CODE_TEXT E_STATUS MutexUnlock(HANDLE hMutex)
{
    return caPostObject(hMutex, NULL);
}

/**
 * Lock specified mutex.
 * @param The handle of mutex object.
 * @return STATE_SUCCESS successfully.
 * @return STATE_INVALID_PARAMETER the handle of object is invalid.
 * @return STATE_INVALID_STATE the state of event object is not active.
 * @return STATE_REMOVED the object has been removed.
 * \par
 * Lock specified mutex, If the object has no signal, the current task will 
 * be blocked.
 *
 * ����ָ���Ļ�������������û���źţ���ǰ���񽫱�������
 *
 * date           author          notes
 * 2015-06-16     JiangYong       first version
 */
FANSAPI CODE_TEXT E_STATUS MutexLock(HANDLE hMutex)
{
    return caWaitObject(hMutex, WAIT_INFINITE);
}
