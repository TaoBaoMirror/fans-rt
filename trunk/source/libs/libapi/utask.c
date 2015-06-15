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
 *    2014-09-07     JiangYong       new file
 */

#include <stdio.h>
#include <string.h>
#include <fadefs.h>
#include <faerror.h>
#include <fatypes.h>
#include <fapi.h>

#include "kcore.h"
#include "ktask.h"
#include "libcal.h"
#include "cadebug.h"

#define     SetStackObjectName(Name, Value)            do { Name[0] = Value; } while(0)

FANSAPI HANDLE CreateTaskEx(LPCTSTR lpTaskName, LPTASK_CREATE_PARAM lpParam)
{
    KTASK_CREATE_PARAM TaskParam;
    CHAR TaskName[OBJECT_NAME_MAX];
#if (CONFIG_DYNAMIC_STACK_ENABLE == FALSE)
    CHAR StackName[OBJECT_NAME_MAX];
#endif
    memset(TaskName, 0, sizeof(TaskName));

    if (NULL == lpTaskName)
    {
        lpTaskName = ntChooseName(TaskName, "TSK");
    }
    else
    {
#ifdef _UNICODE
#error "Can not support unicode charset."
#else
        strncpy(TaskName, lpTaskName, OBJECT_NAME_MAX-1);
#endif
    }

    if (NULL == lpParam)
    {
        LOG_ERROR(TRUE, "Invalid parameter to create task '%s' !", TaskName);
        ntSetError(STATE_INVALID_PARAMETER);
        return INVALID_HANDLE_VALUE;
    }

    TaskParam.Param = *lpParam;
    TaskParam.hTask = INVALID_HANDLE_VALUE;

    if (NULL == TaskParam.Param.fnTaskMain)
    {
        LOG_ERROR(TRUE, "Invalid entry to create task '%s' !", TaskName);
        ntSetError(STATE_INVALID_METHOD);
        return INVALID_HANDLE_VALUE;
    }

#if (CONFIG_TASK_PRIORITY_MAX != 256)
    if (TaskParam.Param.Priority >= CONFIG_TASK_PRIORITY_MAX)
    {
        LOG_ERROR(TRUE, "Invalid priority(%d) to create task '%s'!", TaskParam.Param.Priority, TaskName);
        ntSetError(STATE_INVALID_PRIORITY);
        return INVALID_HANDLE_VALUE;
    }
#endif

    if (TASK_PRIORITY_IDLE == TaskParam.Param.Priority)
    {
        LOG_ERROR(TRUE, "Invalid priority(%d) to create task '%s'!", TaskParam.Param.Priority, TaskName);
        ntSetError(STATE_INVALID_PRIORITY);
        return INVALID_HANDLE_VALUE;
    }

    if (TASK_SLICE_INFINITE == TaskParam.Param.SliceLength)
    {
        LOG_ERROR(TRUE, "Invalid slice time to create task '%s' !", TaskName);
        ntSetError(STATE_INVALID_TIME);
        return INVALID_HANDLE_VALUE;
    }

    TaskParam.Param.StackSize   =   0 == TaskParam.Param.StackSize
                                ?   CONFIG_DEFAULT_STACK_SIZE
                                :   TaskParam.Param.StackSize;
    TaskParam.Param.lpTaskPath  =   NULL == TaskParam.Param.lpTaskPath
                                ?   CONFIG_DEFAULT_PATH
                                :   TaskParam.Param.lpTaskPath;
    TaskParam.Param.SliceLength =   TaskParam.Param.SliceLength < CONFIG_TIME_SLICE_NORMAL
                                ?   CONFIG_TIME_SLICE_NORMAL
                                :   TaskParam.Param.SliceLength;
    
    if (INVALID_HANDLE_VALUE == (TaskParam.hTask = ntMallocObject(TaskName, TSK_MAGIC, &TaskParam)))
    {
        LOG_ERROR(TRUE, "No memory to create task '%s' !", TaskName);
        return INVALID_HANDLE_VALUE;
    }
    
#if (CONFIG_DYNAMIC_STACK_ENABLE == FALSE)

    memcpy(StackName, TaskName, sizeof(TaskName));
    SetStackObjectName(StackName, '*');
    
    if (INVALID_HANDLE_VALUE == ntMallocObject(StackName, STK_MAGIC, &TaskParam))
#else
    if (STATE_SUCCESS != ntStackMalloc(TaskParam.hTask, &TaskParam))
#endif
    {
        LOG_ERROR(TRUE, "No memory to create task '%s' !", TaskName);
        ntCloseTask(TaskParam.hTask);
        return INVALID_HANDLE_VALUE;
    }

    if (STATE_SUCCESS != ntActiveObject(TaskParam.hTask, &TaskParam))
    {
        LOG_ERROR(TRUE, "Active object failed to create task '%s' !", TaskName);
        ntCloseTask(TaskParam.hTask);
        return INVALID_HANDLE_VALUE;
    }

    return TaskParam.hTask;
}
EXPORT_SYMBOL(CreateTaskEx);

/*******************************************************************************************
 *  �� �� ����CreatePriorityTask
 *  ��������: ϵͳAPI�ӿں���
 *  ��    �ܣ���ָ�����ȼ�����һ������
 *  ��    ����LPCSTR __IN    lpTaskName        ������
 *            FNTASKMAIN     fnMain            ����������
 *            LPVOID         lpArgument        �������
 *            TASK_PRIORITY  Priority          �������ȼ�
 *                                             TASK_PRIORITY_IDLE Ϊ������ȼ������ܴ��� IDLE ����
 *  �� �� ֵ��HANDLE         ʧ��              INVALID_HANDLE_VALUE
 *            HANDLE         �ɹ�              ������
 *  �޸���ʷ��
 *      1.�޸�ʱ�䣺2014-11-16
 *        ��    ��: ����
 *        �޸�����: ��������
 *******************************************************************************************/
FANSAPI HANDLE CreatePriorityTask(LPCSTR __IN lpTaskName, FNTASKMAIN fnMain,
                                  LPVOID lpArgument, TASK_PRIORITY Priority)
{
    TASK_CREATE_PARAM TaskParam;

    TaskParam.AutoStartup   =   TRUE;
    TaskParam.lpTaskPath    =   NULL;
    TaskParam.fnTaskMain    =   fnMain;
    TaskParam.lpArgument    =   lpArgument;
    TaskParam.Priority      =   Priority;
    TaskParam.SliceLength   =   CONFIG_TIME_SLICE_NORMAL;
    TaskParam.StackSize     =   CONFIG_DEFAULT_STACK_SIZE;

    return CreateTaskEx(lpTaskName, &TaskParam);
}
EXPORT_SYMBOL(CreatePriorityTask);

/*******************************************************************************************
 *  �� �� ����CreateTask
 *  ��������: ϵͳAPI�ӿں���
 *  ��    �ܣ�����һ��Ĭ�����ȼ�����ͨ����
 *  ��    ����LPCSTR __IN    lpTaskName        ������
 *            FNTASKMAIN     fnMain           �������ں���
 *            LPVOID         lpArgument        �������
 *  �� �� ֵ��HANDLE         ʧ��              INVALID_HANDLE_VALUE
 *            HANDLE         �ɹ�              ������
 *  �޸���ʷ��
 *      1.�޸�ʱ�䣺2014-11-16
 *        ��    ��: ����
 *        �޸�����: ��������
 *******************************************************************************************/

FANSAPI HANDLE CreateTask(LPCTSTR __IN lpTaskName, FNTASKMAIN fnMain, LPVOID lpArgument)
{
    TASK_CREATE_PARAM TaskParam;
    
    TaskParam.AutoStartup   =   TRUE;
    TaskParam.lpTaskPath    =   NULL;
    TaskParam.fnTaskMain    =   fnMain;
    TaskParam.lpArgument    =   lpArgument;
    TaskParam.Priority      =   TASK_PRIORITY_NORMAL;
    TaskParam.SliceLength   =   CONFIG_TIME_SLICE_NORMAL;
    TaskParam.StackSize     =   CONFIG_DEFAULT_STACK_SIZE;

    return CreateTaskEx(lpTaskName, &TaskParam);
}
EXPORT_SYMBOL(CreateTask);

/*******************************************************************************************
 *  �� �� ����KillTask
 *  ��    �ܣ�ɱ�����񣬽������ϵͳ������ɾ��
 *  ��    ����HANDLE         hTask             ������
 *  �� �� ֵ��E_STATUS       ʧ��              ʧ��ԭ��
 *            E_STATUS       �ɹ�              STATE_SUCCESS
 *  �޸���ʷ��
 *      1.�޸�ʱ�䣺2014-11-16
 *        ��    ��: ����
 *        �޸�����: ��������
 *******************************************************************************************/
FANSAPI E_STATUS KillTask(HANDLE hTask)
{
    LOG_FIX(TRUE, "Fix !!! Kill self to call close task.");
    return ntCloseTask(hTask);
}
EXPORT_SYMBOL(KillTask);


/*******************************************************************************************
 *  �� �� ����Sleep
 *  ��    �ܣ���ǰ��������
 *  ��    ��: LONG Timeout
 *  �� �� ֵ��E_STATUS       �ɹ�              STATUS_SUCCESS
 *            E_STATUS       ʧ��              ʧ��ԭ��
 *  �޸���ʷ��
 *      1.�޸�ʱ�䣺2014-11-16
 *        ��    ��: ����
 *        �޸�����: ��������
 *******************************************************************************************/
FANSAPI E_STATUS Sleep(LONG Timeout)
{
    return ntScheduleTimeout(Timeout);
}
EXPORT_SYMBOL(Sleep);


/*******************************************************************************************
 *  �� �� ����TaskStartup
 *  ��    �ܣ���������(�´������Զ�ִ��������Ҫ����)
 *  ��    ��: HANDLE hTask
 *  �� �� ֵ��E_STATUS       �ɹ�              STATUS_SUCCESS
 *            E_STATUS       ʧ��              ʧ��ԭ��
 *  �޸���ʷ��
 *      1.�޸�ʱ�䣺2014-11-16
 *        ��    ��: ����
 *        �޸�����: ��������
 *******************************************************************************************/
FANSAPI E_STATUS TaskStartup(HANDLE hTask)
{
    return ntTaskWakeup(hTask);
}
EXPORT_SYMBOL(TaskStartup);



/*******************************************************************************************
 *  �� �� ����SetError
 *  ��    �ܣ����ô�����
 *  ��    ��: E_STATUS       emCode            ������
 *  �� �� ֵ��E_STATUS       �ɹ�              STATUS_SUCCESS
 *            E_STATUS       ʧ��              ʧ��ԭ��
 *  �޸���ʷ��
 *      1.�޸�ʱ�䣺2014-11-16
 *        ��    ��: ����
 *        �޸�����: ��������
 *******************************************************************************************/
FANSAPI E_STATUS SetError(E_STATUS emCode)
{
    return ntSetError(emCode);
}
EXPORT_SYMBOL(SetError)

/*******************************************************************************************
 *  �� �� ����GetError
 *  ��    �ܣ���ȡ��ǰ������
 *  ��    ���LNONE
 *  �� �� ֵ��E_STATUS       ������
 *  �޸���ʷ��
 *      1.�޸�ʱ�䣺2014-11-16
 *        ��    ��: ����
 *        �޸�����: ��������
 *******************************************************************************************/
FANSAPI E_STATUS GetError(VOID)
{
    return ntGetError();
}
EXPORT_SYMBOL(GetError)

/*******************************************************************************************
 *  �� �� ����PostCancel
 *  ��    �ܣ�ȡ������
 *  ��    ����HANDLE         hTask             ������
 *  �� �� ֵ��E_STATUS       ʧ��              ʧ��ԭ��
 *            E_STATUS       �ɹ�              STATE_SUCCESS
 *  �޸���ʷ��
 *      1.�޸�ʱ�䣺2014-11-16
 *        ��    ��: ����
 *        �޸�����: ��������
 *******************************************************************************************/
FANSAPI E_STATUS PostCancel(HANDLE hTask)
{
    return ntPostCancel(hTask);
}
EXPORT_SYMBOL(PostCancel);

/*******************************************************************************************
 *  �� �� ����TestCancel
 *  ��    �ܣ���������Ƿ�ȡ��
 *  ��    ��: NONE
 *  �� �� ֵ��E_STATUS       ȡ��ԭ��          ����ֵ
 *            E_STATUS       δ��ȡ��          STATE_SUCCESS
 *  �޸���ʷ��
 *      1.�޸�ʱ�䣺2014-11-16
 *        ��    ��: ����
 *        �޸�����: ��������
 *******************************************************************************************/
FANSAPI E_STATUS TestCancel(VOID)
{
    return ntTestCancel();
}
EXPORT_SYMBOL(TestCancel);


FANSAPI E_STATUS GetTaskName(HANDLE hTask, LPTSTR lpName, SIZE_T SizeofBuffer)
{
    E_STATUS State;
    TCHAR Name[OBJECT_NAME_MAX];
    
    if (NULL == lpName || 0 == SizeofBuffer)
    {
        return STATE_INVALID_PARAMETER;
    }

    if (STATE_SUCCESS == (State = ntGetTaskName(hTask, Name)))
    {
#ifdef _UNICODE
#error "not support unicode charset."
#else
        strncpy(lpName, Name, SizeofBuffer);
#endif
    }

    return State;
}
EXPORT_SYMBOL(GetTaskName);

/*******************************************************************************************
 *  �� �� ����GetCurrentTask
 *  ��    �ܣ���õ�ǰ������
 *  ��    ����NONE
 *  �� �� ֵ��HANDLE         ʧ��              INVALID_HANDLE_VALUE
 *            HANDLE         �ɹ�              ��ǰ������
 *  �޸���ʷ��
 *      1.�޸�ʱ�䣺2014-11-16
 *        ��    ��: ����
 *        �޸�����: ��������
 *******************************************************************************************/
FANSAPI HANDLE GetCurrentTask(VOID)
{
    return INVALID_HANDLE_VALUE;
}
EXPORT_SYMBOL(GetCurrentTask);

/*******************************************************************************************
 *  �� �� ����SetPriority
 *  ��    �ܣ������������ȼ�
 *  ��    ��: HANDLE hTask
 *  �� �� ֵ��E_STATUS       �ɹ�              STATUS_SUCCESS
 *            E_STATUS       ʧ��              ʧ��ԭ��
 *  �޸���ʷ��
 *      1.�޸�ʱ�䣺2014-11-16
 *        ��    ��: ����
 *        �޸�����: ��������
 *******************************************************************************************/
FANSAPI E_STATUS SetPriority(HANDLE hTask, TASK_PRIORITY Priority)
{
    return ntSetPriority(hTask, Priority);
}
EXPORT_SYMBOL(SetPriority);

/*******************************************************************************************
 *  �� �� ����GetPriority
 *  ��    �ܣ���ѯ�������ȼ�
 *  ��    ��: HANDLE hTask
 *  �� �� ֵ��DWORD          �������ȼ�
 *  �޸���ʷ��
 *      1.�޸�ʱ�䣺2014-11-16
 *        ��    ��: ����
 *        �޸�����: ��������
 *******************************************************************************************/
FANSAPI TASK_PRIORITY GetPriority(HANDLE hTask)
{
    return ntGetPriority(hTask);
}
EXPORT_SYMBOL(GetPriority);


/*******************************************************************************************
 *  �� �� ����GetTaskLocalData
 *  ��    �ܣ���ѯ�������ȼ�
 *  ��    ��: BYTE LocalID
 *  �� �� ֵ��LPVOID
 *  �޸���ʷ��
 *      1.�޸�ʱ�䣺2014-11-16
 *        ��    ��: ����
 *        �޸�����: ��������
 *******************************************************************************************/
FANSAPI LPVOID GetTaskLocalData(VOID)
{
    return NULL;
}
EXPORT_SYMBOL(GetTaskLocalData);

/*******************************************************************************************
 *  �� �� ����SetTaskLocalData
 *  ��    �ܣ��������񱾵ؾ�̬����
 *  ��    ��: LPVOID lpPrivate
 *  �� �� ֵ��LPVOID
 *  �޸���ʷ��
 *      1.�޸�ʱ�䣺2014-11-16
 *        ��    ��: ����
 *        �޸�����: ��������
 *******************************************************************************************/
FANSAPI E_STATUS SetTaskLocalData(LPVOID lpPrivate)
{
    return STATE_NOT_IMPLEMENTED;
}
EXPORT_SYMBOL(SetTaskLocalData);


/*******************************************************************************************
 *  �� �� ����GetTaskState
 *  ��    �ܣ���ѯ����״̬
 *  ��    ��: HANDLE hTask
 *  �� �� ֵ��TASK_STATUS       �ɹ�  < TASK_STATE_MAX
 *            TASK_STATUS       ʧ��  >= TASK_STATE_MAX
 *  �޸���ʷ��
 *      1.�޸�ʱ�䣺2014-11-16
 *        ��    ��: ����
 *        �޸�����: ��������
 *******************************************************************************************/
FANSAPI TASK_STATUS GetTaskState(HANDLE hTask)
{
    return TASK_STATE_MAX;
}
EXPORT_SYMBOL(GetTaskState);

/*******************************************************************************************
 *  �� �� ����GetTaskInformation
 *  ��    �ܣ���ѯ�����ۺ���Ϣ
 *  ��    ��: HANDLE        hTask                   ������
 *            LPTASK_INFOR  lpTaskInfor             ������Ϣ�ṹ��ָ��
 *  �� �� ֵ��E_STATUS          �ɹ�                STATE_SUCCESS
 *            E_STATUS          ʧ��                ʧ��ԭ��
 *  �޸���ʷ��
 *      1.�޸�ʱ�䣺2014-11-27
 *        ��    ��: ����
 *        �޸�����: ��������
 *******************************************************************************************/
FANSAPI E_STATUS GetTaskInformation(HANDLE hTask, LPTASK_INFOR lpTaskInfor)
{
    return STATE_NOT_IMPLEMENTED;
}
EXPORT_SYMBOL(GetTaskInformation);


/*******************************************************************************************
 *  �� �� ����EnumerationNextTask
 *  ��    �ܣ���ѯ�����ۺ���Ϣ
 *  ��    ��: HANDLE        hTask                   ������
 *  �� �� ֵ��HANDLE        �ɹ�                    ��һ��������
 *            HANDLE        ������ʧ��              INVALID_HANDLE_VALUE
 *  �޸���ʷ��
 *      1.�޸�ʱ�䣺2014-11-27
 *        ��    ��: ����
 *        �޸�����: ��������
 *******************************************************************************************/
FANSAPI HANDLE EnumerationNextTask(HANDLE hTask)
{
    return INVALID_HANDLE_VALUE;
}
EXPORT_SYMBOL(EnumerationNextTask);

