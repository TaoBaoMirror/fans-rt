/*
 *    Copyright(C) 2013-2015, Fans-rt development team.
 *
 *    All rights reserved.
 *
 *    This is open source software.
 *    Learning and research can be unrestricted to modification, use and dissemination.
 *    If you need for commercial purposes, you should get the author's permission.
 *
 *    date           author          notes
 *    2015-06-16     JiangYong       new file
 *    2015-07-27     JiangYong       Add semphore
 */
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
FANSAPI RO_USER_CODE HANDLE CreateEvent(LPCTSTR lpCTName, BOOL Automatic, BOOL Signal)
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
 * After calling this function, the all tasks blocked by the object will be awakening.
 *
 * ���øú����󣬱��ö����������������񽫱����ѡ�
 *
 * date           author          notes
 * 2015-06-16     JiangYong       first version
 */
FANSAPI RO_USER_CODE E_STATUS PostEvent(HANDLE handle)
{
    return caRequestMethod(handle, NULL, KIPC_METHOD_POST);
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
FANSAPI RO_USER_CODE E_STATUS ResetEvent(HANDLE handle)
{
    return caRequestMethod(handle, NULL, KIPC_METHOD_RESET);
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
FANSAPI RO_USER_CODE HANDLE CreateMutex(LPCTSTR lpCTName, BOOL Owner)
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
 * Unlock specified mutex, the highest priority task blocked by the object
 * will be awakening.
 *
 * ����ָ���Ļ�����󣬱��ö���������������ȼ����񽫱����ѡ�
 *
 * date           author          notes
 * 2015-06-16     JiangYong       first version
 */
FANSAPI RO_USER_CODE E_STATUS MutexUnlock(HANDLE hMutex)
{
    return caRequestMethod(hMutex, NULL, KIPC_METHOD_POST);
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
 * be blocking.
 *
 * ����ָ���Ļ�������������û���źţ���ǰ���񽫱�������
 *
 * date           author          notes
 * 2015-06-16     JiangYong       first version
 */
FANSAPI RO_USER_CODE E_STATUS MutexLock(HANDLE hMutex)
{
    E_STATUS Result;
    KIPC_WAIT_PARAM Param;

    SetObjectID2Param(&Param, WAIT_INVALID_OBJECT_ID);
    SetWaitTime2Param(&Param, WAIT_INFINITE);
    
    if (STATE_SUCCESS != (Result = caRequestMethod(hMutex, &Param, KIPC_METHOD_WAIT)))
    {
        return Result;
    }
    
    if (WAIT_FIRST_OBJECT_ID != Param.ObjectID)
    {
        Result = STATE_INVALID_OBJECT;
    }
    
    return Result;
}


/**
 * Create a semaphore object.
 * @param The name of object.
 * @param The light numbers of semaphore.
 * @return The handle of object.
 * \par
 * Call system to create semaphore object, if the specified name has been 
 * found, the function will be return invalid handle(means create failed),
 * else return the handle of object for want to created.
 *
 * ����һ���ź���������������Ѿ����ڣ��򷵻���Ч���(��ʾ����ʧ��)�����򷵻�
 * �ľ��Ϊ�����Ķ�������
 *
 * date           author          notes
 * 2015-07-27     JiangYong       first version
 */
FANSAPI RO_USER_CODE HANDLE CreateSemaphore(LPCTSTR lpCTName, SHORT Lights, SHORT MaxLights)
{
    HANDLE hSemaphore;
    SEMAPHORE_ATTRIBUTE Attribute;
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

    Attribute.Signal    = Lights;
    Attribute.MaxCount  = MaxLights;

    hSemaphore = caMallocObject(caName, SEM_MAGIC, &Attribute);

    if (INVALID_HANDLE_VALUE != hSemaphore)
    {
        if (STATE_SUCCESS != caActiveObject(hSemaphore, &Attribute))
        {
            caFreeObject(hSemaphore);
            hSemaphore = INVALID_HANDLE_VALUE;
        }
    }

    return hSemaphore;
}


/**
 * Post signal to semaphore.
 * @param The handle of semaphore object.
 * @param Will increase the count of signal lights.
 * @return STATE_SUCCESS successfully.
 * @return STATE_INVALID_PARAMETER the handle of object is invalid.
 * @return STATE_INVALID_STATE the state of event object is not active.
 * @return STATE_REMOVED the object has been removed.
 * \par
 * After calling this function, the highest priority tasks blocked by this
 * object will be awakening.
 *
 * ���øú����󣬱��ö��������ĸ����ȼ���һЩ���񽫱����ѡ�
 *
 * date           author          notes
 * 2015-07-27     JiangYong       first version
 */
FANSAPI RO_USER_CODE E_STATUS PostSemaphore(HANDLE hSemaphore, SHORT Lights)
{
    return caRequestMethod(hSemaphore, &Lights, KIPC_METHOD_POST);
}

/**
 * Create a semset object.
 * @param The name of object.
 * @param The light mask of semset.
 * @return The handle of object.
 * \par
 * Call system to create semset object, if the specified name has been 
 * found, the function will be return invalid handle(means create failed),
 * else return the handle of object for want to created.
 *
 * ����һ���ź�����������������Ѿ����ڣ��򷵻���Ч���(��ʾ����ʧ��)�����򷵻�
 * �ľ��Ϊ�����Ķ�������
 *
 * date           author          notes
 * 2015-07-29     JiangYong       first version
 */
FANSAPI RO_USER_CODE HANDLE CreateSemset(LPCTSTR lpCTName, DWORD Mask)
{
    HANDLE hSemset;
    SEMSET_ATTRIBUTE Attribute;
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

    Attribute.Mask    = Mask;

    hSemset = caMallocObject(caName, SEM_MAGIC, &Attribute);

    if (INVALID_HANDLE_VALUE != hSemset)
    {
        if (STATE_SUCCESS != caActiveObject(hSemset, &Attribute))
        {
            caFreeObject(hSemset);
            hSemset = INVALID_HANDLE_VALUE;
        }
    }

    return hSemset;
}

/**
 * Post signal to semset.
 * @param The handle of semset object.
 * @param The light ID.
 * @return STATE_SUCCESS successfully.
 * @return STATE_INVALID_PARAMETER the handle of object is invalid.
 * @return STATE_INVALID_STATE the state of event object is not active.
 * @return STATE_REMOVED the object has been removed.
 * \par
 * After calling this function, the highest priority tasks blocked by this
 * object will be awakening.
 *
 * ���øú����󣬱��ö����������񽫱����ѡ�
 *
 * date           author          notes
 * 2015-07-29     JiangYong       first version
 */
FANSAPI RO_USER_CODE E_STATUS PostSemset(HANDLE hSemset, BYTE SemID)
{
    return caRequestMethod(hSemset, &SemID, KIPC_METHOD_POST);
}

