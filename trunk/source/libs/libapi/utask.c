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
 *  函 数 名：CreatePriorityTask
 *  函数类型: 系统API接口函数
 *  功    能：以指定优先级创建一个任务
 *  参    数：LPCSTR __IN    lpTaskName        任务名
 *            FNTASKMAIN     fnMain            任务主函数
 *            LPVOID         lpArgument        任务参数
 *            TASK_PRIORITY  Priority          任务优先级
 *                                             TASK_PRIORITY_IDLE 为最低优先级，不能创建 IDLE 任务
 *  返 回 值：HANDLE         失败              INVALID_HANDLE_VALUE
 *            HANDLE         成功              任务句柄
 *  修改历史：
 *      1.修改时间：2014-11-16
 *        作    者: 姜勇
 *        修改内容: 创建函数
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
 *  函 数 名：CreateTask
 *  函数类型: 系统API接口函数
 *  功    能：创建一个默认优先级的普通任务
 *  参    数：LPCSTR __IN    lpTaskName        任务名
 *            FNTASKMAIN     fnMain           任务的入口函数
 *            LPVOID         lpArgument        任务参数
 *  返 回 值：HANDLE         失败              INVALID_HANDLE_VALUE
 *            HANDLE         成功              任务句柄
 *  修改历史：
 *      1.修改时间：2014-11-16
 *        作    者: 姜勇
 *        修改内容: 创建函数
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
 *  函 数 名：KillTask
 *  功    能：杀死任务，将任务从系统队列中删除
 *  参    数：HANDLE         hTask             任务句柄
 *  返 回 值：E_STATUS       失败              失败原因
 *            E_STATUS       成功              STATE_SUCCESS
 *  修改历史：
 *      1.修改时间：2014-11-16
 *        作    者: 姜勇
 *        修改内容: 创建函数
 *******************************************************************************************/
FANSAPI E_STATUS KillTask(HANDLE hTask)
{
    LOG_FIX(TRUE, "Fix !!! Kill self to call close task.");
    return ntCloseTask(hTask);
}
EXPORT_SYMBOL(KillTask);


/*******************************************************************************************
 *  函 数 名：Sleep
 *  功    能：当前任务休眠
 *  参    数: LONG Timeout
 *  返 回 值：E_STATUS       成功              STATUS_SUCCESS
 *            E_STATUS       失败              失败原因
 *  修改历史：
 *      1.修改时间：2014-11-16
 *        作    者: 姜勇
 *        修改内容: 创建函数
 *******************************************************************************************/
FANSAPI E_STATUS Sleep(LONG Timeout)
{
    return ntScheduleTimeout(Timeout);
}
EXPORT_SYMBOL(Sleep);


/*******************************************************************************************
 *  函 数 名：TaskStartup
 *  功    能：唤醒任务(新创建非自动执行任务需要唤醒)
 *  参    数: HANDLE hTask
 *  返 回 值：E_STATUS       成功              STATUS_SUCCESS
 *            E_STATUS       失败              失败原因
 *  修改历史：
 *      1.修改时间：2014-11-16
 *        作    者: 姜勇
 *        修改内容: 创建函数
 *******************************************************************************************/
FANSAPI E_STATUS TaskStartup(HANDLE hTask)
{
    return ntTaskWakeup(hTask);
}
EXPORT_SYMBOL(TaskStartup);



/*******************************************************************************************
 *  函 数 名：SetError
 *  功    能：设置错误码
 *  参    数: E_STATUS       emCode            错误码
 *  返 回 值：E_STATUS       成功              STATUS_SUCCESS
 *            E_STATUS       失败              失败原因
 *  修改历史：
 *      1.修改时间：2014-11-16
 *        作    者: 姜勇
 *        修改内容: 创建函数
 *******************************************************************************************/
FANSAPI E_STATUS SetError(E_STATUS emCode)
{
    return ntSetError(emCode);
}
EXPORT_SYMBOL(SetError)

/*******************************************************************************************
 *  函 数 名：GetError
 *  功    能：获取当前错误码
 *  参    数NONE
 *  返 回 值：E_STATUS       错误码
 *  修改历史：
 *      1.修改时间：2014-11-16
 *        作    者: 姜勇
 *        修改内容: 创建函数
 *******************************************************************************************/
FANSAPI E_STATUS GetError(VOID)
{
    return ntGetError();
}
EXPORT_SYMBOL(GetError)

/*******************************************************************************************
 *  函 数 名：PostCancel
 *  功    能：取消任务
 *  参    数：HANDLE         hTask             任务句柄
 *  返 回 值：E_STATUS       失败              失败原因
 *            E_STATUS       成功              STATE_SUCCESS
 *  修改历史：
 *      1.修改时间：2014-11-16
 *        作    者: 姜勇
 *        修改内容: 创建函数
 *******************************************************************************************/
FANSAPI E_STATUS PostCancel(HANDLE hTask)
{
    return ntPostCancel(hTask);
}
EXPORT_SYMBOL(PostCancel);

/*******************************************************************************************
 *  函 数 名：TestCancel
 *  功    能：检测任务是否被取消
 *  参    数: NONE
 *  返 回 值：E_STATUS       取消原因          其他值
 *            E_STATUS       未被取消          STATE_SUCCESS
 *  修改历史：
 *      1.修改时间：2014-11-16
 *        作    者: 姜勇
 *        修改内容: 创建函数
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
 *  函 数 名：GetCurrentTask
 *  功    能：获得当前任务句柄
 *  参    数：NONE
 *  返 回 值：HANDLE         失败              INVALID_HANDLE_VALUE
 *            HANDLE         成功              当前任务句柄
 *  修改历史：
 *      1.修改时间：2014-11-16
 *        作    者: 姜勇
 *        修改内容: 创建函数
 *******************************************************************************************/
FANSAPI HANDLE GetCurrentTask(VOID)
{
    return INVALID_HANDLE_VALUE;
}
EXPORT_SYMBOL(GetCurrentTask);

/*******************************************************************************************
 *  函 数 名：SetPriority
 *  功    能：设置任务优先级
 *  参    数: HANDLE hTask
 *  返 回 值：E_STATUS       成功              STATUS_SUCCESS
 *            E_STATUS       失败              失败原因
 *  修改历史：
 *      1.修改时间：2014-11-16
 *        作    者: 姜勇
 *        修改内容: 创建函数
 *******************************************************************************************/
FANSAPI E_STATUS SetPriority(HANDLE hTask, TASK_PRIORITY Priority)
{
    return ntSetPriority(hTask, Priority);
}
EXPORT_SYMBOL(SetPriority);

/*******************************************************************************************
 *  函 数 名：GetPriority
 *  功    能：查询任务优先级
 *  参    数: HANDLE hTask
 *  返 回 值：DWORD          任务优先级
 *  修改历史：
 *      1.修改时间：2014-11-16
 *        作    者: 姜勇
 *        修改内容: 创建函数
 *******************************************************************************************/
FANSAPI TASK_PRIORITY GetPriority(HANDLE hTask)
{
    return ntGetPriority(hTask);
}
EXPORT_SYMBOL(GetPriority);


/*******************************************************************************************
 *  函 数 名：GetTaskLocalData
 *  功    能：查询任务优先级
 *  参    数: BYTE LocalID
 *  返 回 值：LPVOID
 *  修改历史：
 *      1.修改时间：2014-11-16
 *        作    者: 姜勇
 *        修改内容: 创建函数
 *******************************************************************************************/
FANSAPI LPVOID GetTaskLocalData(VOID)
{
    return NULL;
}
EXPORT_SYMBOL(GetTaskLocalData);

/*******************************************************************************************
 *  函 数 名：SetTaskLocalData
 *  功    能：设置任务本地静态变量
 *  参    数: LPVOID lpPrivate
 *  返 回 值：LPVOID
 *  修改历史：
 *      1.修改时间：2014-11-16
 *        作    者: 姜勇
 *        修改内容: 创建函数
 *******************************************************************************************/
FANSAPI E_STATUS SetTaskLocalData(LPVOID lpPrivate)
{
    return STATE_NOT_IMPLEMENTED;
}
EXPORT_SYMBOL(SetTaskLocalData);


/*******************************************************************************************
 *  函 数 名：GetTaskState
 *  功    能：查询任务状态
 *  参    数: HANDLE hTask
 *  返 回 值：TASK_STATUS       成功  < TASK_STATE_MAX
 *            TASK_STATUS       失败  >= TASK_STATE_MAX
 *  修改历史：
 *      1.修改时间：2014-11-16
 *        作    者: 姜勇
 *        修改内容: 创建函数
 *******************************************************************************************/
FANSAPI TASK_STATUS GetTaskState(HANDLE hTask)
{
    return TASK_STATE_MAX;
}
EXPORT_SYMBOL(GetTaskState);

/*******************************************************************************************
 *  函 数 名：GetTaskInformation
 *  功    能：查询任务综合信息
 *  参    数: HANDLE        hTask                   任务句柄
 *            LPTASK_INFOR  lpTaskInfor             任务信息结构体指针
 *  返 回 值：E_STATUS          成功                STATE_SUCCESS
 *            E_STATUS          失败                失败原因
 *  修改历史：
 *      1.修改时间：2014-11-27
 *        作    者: 姜勇
 *        修改内容: 创建函数
 *******************************************************************************************/
FANSAPI E_STATUS GetTaskInformation(HANDLE hTask, LPTASK_INFOR lpTaskInfor)
{
    return STATE_NOT_IMPLEMENTED;
}
EXPORT_SYMBOL(GetTaskInformation);


/*******************************************************************************************
 *  函 数 名：EnumerationNextTask
 *  功    能：查询任务综合信息
 *  参    数: HANDLE        hTask                   任务句柄
 *  返 回 值：HANDLE        成功                    下一个任务句柄
 *            HANDLE        结束或失败              INVALID_HANDLE_VALUE
 *  修改历史：
 *      1.修改时间：2014-11-27
 *        作    者: 姜勇
 *        修改内容: 创建函数
 *******************************************************************************************/
FANSAPI HANDLE EnumerationNextTask(HANDLE hTask)
{
    return INVALID_HANDLE_VALUE;
}
EXPORT_SYMBOL(EnumerationNextTask);

