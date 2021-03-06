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
#include <fauser.h>
#include <fadefs.h>
#include <faerror.h>
#include <fatypes.h>
#include <fapi.h>

#include "kcore.h"
#include "ktask.h"
#include "klsot.h"
#include "libcal.h"
#include "libapi.h"
#include "cadebug.h"

/**
 * Create new task.
 * @param The name of new task.
 * @param The some parameters of new task.
 * @return The handle of new task.
 *
 * date           author          notes
 * 2014-11-16     JiangYong       new function
 */
FANSAPI RO_USER_CODE HANDLE CreateTaskEx(LPCTSTR lpTaskName, LPTASK_CREATE_PARAM lpParam)
{
    HANDLE hTask;
    
    TASK_CREATE_PARAM TaskParam;
    CHAR TaskName[OBJECT_NAME_MAX];

    memset(TaskName, 0, sizeof(TaskName));

    if (NULL == lpTaskName)
    {
        lpTaskName = caChooseName(TaskName, "TSK");
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
        caSetError(STATE_INVALID_PARAMETER);
        return INVALID_HANDLE_VALUE;
    }

    TaskParam = *lpParam;

    if (NULL == TaskParam.fnTaskMain)
    {
        LOG_ERROR(TRUE, "Invalid entry to create task '%s' !", TaskName);
        caSetError(STATE_INVALID_POINTER);
        return INVALID_HANDLE_VALUE;
    }

#if (CONFIG_TASK_PRIORITY_MAX != 256)
    if (TaskParam.Priority >= CONFIG_TASK_PRIORITY_MAX)
    {
        LOG_ERROR(TRUE, "Invalid priority(%d) to create task '%s'!",
                        TaskParam.Priority, TaskName);
        caSetError(STATE_INVALID_PRIORITY);
        return INVALID_HANDLE_VALUE;
    }
#endif

    if (TASK_PRIORITY_IDLE == TaskParam.Priority)
    {
        LOG_ERROR(TRUE, "Invalid priority(%d) to create task '%s'!",
                        TaskParam.Priority, TaskName);
        caSetError(STATE_INVALID_PRIORITY);
        return INVALID_HANDLE_VALUE;
    }

    if (TaskParam.SliceLength < CONFIG_TIME_SLICE_NORMAL)
    {
        LOG_ERROR(TRUE, "Invalid slice time to create task '%s' !", TaskName);
        caSetError(STATE_INVALID_TIME);
        return INVALID_HANDLE_VALUE;
    }

    TaskParam.StackSize   =   0 == TaskParam.StackSize
                          ?   CONFIG_DEFAULT_STACK_SIZE
                          :   TaskParam.StackSize;
    TaskParam.lpTaskPath  =   NULL == TaskParam.lpTaskPath
                          ?   CONFIG_DEFAULT_PATH
                          :   TaskParam.lpTaskPath;
    
    if (INVALID_HANDLE_VALUE == (hTask = caMallocObject(TaskName, TSK_MAGIC, &TaskParam)))
    {
        LOG_ERROR(TRUE, "No memory to create task '%s' !", TaskName);
        return INVALID_HANDLE_VALUE;
    }

    if (STATE_SUCCESS != caStackMalloc(hTask, &TaskParam, TASK_PERMISSION_USER))
    {
        LOG_ERROR(TRUE, "Malloc user stack for task '%s' failed.", TaskName);
        goto lable0;
    }
    
    if (STATE_SUCCESS != caStackFill(hTask, &TaskParam, TASK_PERMISSION_USER))
    {
        LOG_ERROR(TRUE, "Fill user stack for task '%s' failed.", TaskName);
        goto lable1;
    }

    if (STATE_SUCCESS != caStackMalloc(hTask, &TaskParam, TASK_PERMISSION_CORE))
    {
        LOG_ERROR(TRUE, "Malloc core stack for task '%s' failed.", TaskName);
        goto lable1;
    }

    if (STATE_SUCCESS != caStackFill(hTask, &TaskParam, TASK_PERMISSION_CORE))
    {
        LOG_ERROR(TRUE, "Fill user stack for task '%s' failed.", TaskName);
        goto lable2;
    }

#if (defined(CONFIG_BUILD_LOCAL_STORE) && (TRUE == CONFIG_BUILD_LOCAL_STORE))
    if (0 != TaskParam.LsotKeys)
    {
        if (STATE_SUCCESS != uCreateLsot(hTask, TaskParam.LsotKeys))
        {
            LOG_ERROR(TRUE, "Task %s create lsot object failed.", TaskName);
            goto lable2;
        }
    }
#endif

    if (STATE_SUCCESS == caActiveObject(hTask, &TaskParam))
    {
        return hTask;
    }

    LOG_ERROR(TRUE, "Active object failed to create task '%s' !", TaskName);
#if (defined(CONFIG_BUILD_LOCAL_STORE) && (TRUE == CONFIG_BUILD_LOCAL_STORE))
    if (0 != TaskParam.LsotKeys)
    {
        uCloseLsotObject(hTask);
    }
#endif
lable2:
    caStackFree(hTask, TASK_PERMISSION_CORE);
lable1:
    caStackFree(hTask, TASK_PERMISSION_USER);
lable0:
    caCloseTask(hTask);

    return INVALID_HANDLE_VALUE;
}
EXPORT_SYMBOL(CreateTaskEx);

/**
 * Create new task.
 * @param The name of new task.
 * @param The entry function of new task.
 * @param The argument of new task.
 * @param The priority of new task.
 * @return The handle of new task.
 *
 * date           author          notes
 * 2014-11-16     JiangYong       new function
 */
FANSAPI RO_USER_CODE HANDLE CreatePriorityTask(LPCSTR __IN lpTaskName, FNTASKMAIN fnMain,
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
#if (defined(CONFIG_BUILD_LOCAL_STORE) && (TRUE == CONFIG_BUILD_LOCAL_STORE))
    TaskParam.LsotKeys      =   0;
#endif
    return CreateTaskEx(lpTaskName, &TaskParam);
}
EXPORT_SYMBOL(CreatePriorityTask);

/**
 * Kill the specified task.
 * @param The handle of the specified task.
 * @return The state of current operating result.
 *
 * date           author          notes
 * 2014-11-16     JiangYong       new function
 */
FANSAPI RO_USER_CODE E_STATUS KillTask(HANDLE hTask)
{
    if (INVALID_HANDLE_VALUE == hTask)
    {
        return STATE_INVALID_PARAMETER;
    }
    
    return caCloseTask(hTask);
}
EXPORT_SYMBOL(KillTask);

/**
 * Kill current task.
 * @return The state of current operating result.
 *
 * date           author          notes
 * 2014-11-16     JiangYong       new function
 */
FANSAPI RO_USER_CODE E_STATUS TaskExit(VOID)
{
    return caCloseTask(TASK_SELF_HANDLE);
}

/**
 * Current task will be sleep.
 * @param how many milliseconds as sleep.
 * @return The state of current operating result.
 * \par
 * If the member AutoStartup of the second argument is FALSE to call the function CreateTaskEx,
 * an infinite sleep task will be created, you may call this function to wakeup the new task.
 *
 * date           author          notes
 * 2014-11-16     JiangYong       new function
 */
FANSAPI RO_USER_CODE E_STATUS Sleep(LONG Timeout)
{
    if (Timeout <= 0)
    {
        return STATE_INVALID_PARAMETER;
    }
    
    return caScheduleTimeout(Timeout);
}
EXPORT_SYMBOL(Sleep);

/**
 * Post start signal to the specified task.
 * @param The handle of the specified task.
 * @return The state of current operating result.
 * \par
 * If the member AutoStartup of the second argument is FALSE to call the function CreateTaskEx,
 * an infinite sleep task will be created, you may call this function to wakeup the new task.
 *
 * date           author          notes
 * 2014-11-16     JiangYong       new function
 */
FANSAPI RO_USER_CODE E_STATUS TaskStartup(HANDLE hTask)
{
    return caTaskWakeup(hTask);
}
EXPORT_SYMBOL(TaskStartup);


/**
 * Set the last error of the current task.
 * @param The last error code.
 * @return The state of current operating result.
 *
 * date           author          notes
 * 2014-11-16     JiangYong       new function
 */
FANSAPI RO_USER_CODE E_STATUS SetError(E_STATUS State)
{
    return caSetError(State);
}
EXPORT_SYMBOL(SetError)

/**
 * Get the last error of the current task.
 * @return The last error code.
 *
 * date           author          notes
 * 2014-11-16     JiangYong       new function
 */
FANSAPI RO_USER_CODE E_STATUS GetError(VOID)
{
    return caGetError();
}
EXPORT_SYMBOL(GetError)

/**
 * Get the handle of current task.
 * @return The handle of current task.
 *
 * date           author          notes
 * 2014-11-16     JiangYong       new function
 */
FANSAPI RO_USER_CODE HANDLE GetCurrentTask(VOID)
{
    return caGetCurrentTask();
}
EXPORT_SYMBOL(GetCurrentTask);

/**
 * Get the start time for the specified task.
 * @param The handle of the specified task.
 * @return The start tick of time.
 *
 * date           author          notes
 * 2015-06-17     JiangYong       new function
 */
FANSAPI RO_USER_CODE TICK GetTaskStartTick(HANDLE hTask)
{
    if (INVALID_HANDLE_VALUE == hTask)
    {
        SetError(STATE_INVALID_PARAMETER);
        return INVALID_TICK;
    }

    return GetTaskStartTick(hTask);
}

/**
 * Post cancel signal to the specified task.
 * @param The handle of the specified task.
 * @return The state of current operating result.
 *
 * date           author          notes
 * 2014-11-16     JiangYong       new function
 */
FANSAPI RO_USER_CODE E_STATUS PostCancel(HANDLE hTask)
{
    if (INVALID_HANDLE_VALUE == hTask)
    {
        return STATE_INVALID_PARAMETER;
    }

    return caPostCancel(hTask);
}
EXPORT_SYMBOL(PostCancel);

/**
 * Testing cancel signal of the current task.
 * @return TRUE The cancel signal has been set.
 * @return FALSE The cancel signal is not detected.
 *
 * date           author          notes
 * 2014-11-16     JiangYong       new function
 */
FANSAPI RO_USER_CODE BOOL TestCancel(VOID)
{
    return caTestCancel();
}
EXPORT_SYMBOL(TestCancel);

/**
 * Get the priority of the specified task.
 * @param The handle of the specified task.
 * @param The new priority will be setting.
 * @return The priority of the task.
 *
 * date           author          notes
 * 2014-11-16     JiangYong       new function
 */
FANSAPI RO_USER_CODE E_STATUS SetPriority(HANDLE hTask, TASK_PRIORITY Priority)
{
    if (INVALID_HANDLE_VALUE == hTask)
    {
        return STATE_INVALID_PARAMETER;
    }

    return caSetPriority(hTask, Priority);
}
EXPORT_SYMBOL(SetPriority);

/**
 * Get the priority of the specified task.
 * @param The handle of the specified task.
 * @return The priority of the task.
 *
 * date           author          notes
 * 2014-11-16     JiangYong       new function
 */
FANSAPI RO_USER_CODE TASK_PRIORITY GetPriority(HANDLE hTask)
{
    if (INVALID_HANDLE_VALUE == hTask)
    {
        SetError(STATE_INVALID_PARAMETER);

        return TASK_PRIORITY_INVALID;
    }
    
    return caGetPriority(hTask);
}
EXPORT_SYMBOL(GetPriority);

/**
 * Get the state of the specified task.
 * @param The handle of the specified task.
 * @return The state of the task.
 *
 * date           author          notes
 * 2014-11-16     JiangYong       new function
 */
FANSAPI RO_USER_CODE TASK_STATUS GetTaskState(HANDLE hTask)
{
    if (INVALID_HANDLE_VALUE == hTask)
    {
        SetError(STATE_INVALID_PARAMETER);

        return TASK_STATE_MAX;
    }
    
    return caGetTaskState(hTask);
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
FANSAPI RO_USER_CODE E_STATUS GetTaskInformation(HANDLE hTask, LPTASK_INFOR lpTaskInfor)
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
FANSAPI RO_USER_CODE HANDLE EnumerationNextTask(HANDLE hTask)
{
    return INVALID_HANDLE_VALUE;
}
EXPORT_SYMBOL(EnumerationNextTask);

