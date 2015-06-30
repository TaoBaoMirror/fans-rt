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
 *    2015-06-22     JiangYong       new file
 */

#include <string.h>
#include <fauser.h>
#include <fadefs.h>
#include <faerror.h>
#include <fatypes.h>
#include <fapi.h>

#include "libcal.h"
#include "cadebug.h"

#define     SetObjectNameEnd(Name)            do { Name[OBJECT_NAME_MAX-1] = '\0'; } while(0)

/**
 * Get the name of the specified object.
 * @param The handle of the specified object.
 * @param The name buffer.
 * @param The size of the name buffer.
 * @return The state of current operating result.
 *
 * date           author          notes
 * 2014-11-16     JiangYong       new function
 */
FANSAPI RO_CODE E_STATUS GetObjectName(HANDLE hObject, LPTSTR lpName, SIZE_T SizeofBuffer)
{
    E_STATUS State;
    TCHAR Name[OBJECT_NAME_MAX];
    
    if (NULL == lpName || SizeofBuffer < OBJECT_NAME_MAX || INVALID_HANDLE_VALUE == hObject)
    {
        return STATE_INVALID_PARAMETER;
    }

    if (STATE_SUCCESS == (State = caGetObjectName(hObject, Name, SizeofBuffer)))
    {
#ifdef _UNICODE
#error "not support unicode charset."
#else
        strncpy(lpName, Name, SizeofBuffer-1);
        SetObjectNameEnd(lpName);
#endif
    }

    return State;
}
EXPORT_SYMBOL(GetObjectName);


/**
 * Wait for specified object.
 * @param The handle of object.
 * @return STATE_SUCCESS successfully.
 * @return STATE_INVALID_PARAMETER the handle of object is invalid.
 * @return STATE_INVALID_STATE the state of event object is not active.
 * @return STATE_REMOVED the object has been removed.
 * \par
 * Wait for specified object, If the object has no signal, the current task will 
 * be blocked.
 *
 * �ȴ�ָ���Ķ����������û���źţ���ǰ���񽫱�������
 *
 * date           author          notes
 * 2015-06-16     JiangYong       first version
 */
FANSAPI RO_CODE E_STATUS WaitObject(HANDLE handle, LONG WaitTime)
{
    return caWaitObject(handle, WaitTime);
}

/**
 * Close the specified object.
 * @param The handle of object.
 * @return STATE_SUCCESS successfully.
 * @return STATE_INVALID_PARAMETER the handle of object is invalid.
 * @return STATE_INVALID_STATE the state of event object is not active.
 * @return STATE_REMOVED the object has been removed.
 * \par
 * Close the specified object, the all tasks blocked by the object will be
 * awakened and giving STATE_REMOVE as the blocked task to result value.
 *
 * �ر�ָ���Ķ������б��ö������������񽫱����ѣ������������񽫻᷵�� STATE_REMOVE��
 *
 * date           author          notes
 * 2015-06-16     JiangYong       first version
 */
FANSAPI RO_CODE E_STATUS CloseHandle(HANDLE handle)
{
    return caFreeObject(handle);
}



