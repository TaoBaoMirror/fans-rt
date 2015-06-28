/*
 *    Copyright(C) 2013-2015, Fans-rt development team.
 *
 *    All rights reserved.
 *
 *    This is open source software.
 *    Learning and research can be unrestricted to  modification, use and dissemination.
 *    If you need for commercial purposes, you should get the author's permission.
 *
 *    date           author          notes
 *    2015-06-28     JiangYong       new file
 */
#include <stdio.h>
#include <string.h>
#include <fadefs.h>
#include <faerror.h>
#include <fatypes.h>


#include "cadebug.h"

#include "libfw.h"
#include "libcal.h"


/**
 * ������ں���
 * @param fnMain ����������ָ��
 * @param lpArgument ������ڲ���
 * @param HANDLE hTask ������
 * @return VOID
 * \par
 *     ��������ں�����Ҫ����������������ARCH��ʵ�ֵ������ջ��亯����Ҫ����
 * ������������ǡ���Ķ�ջλ�á�
 */
EXPORT CODE_TEXT VOID USER_TaskEntry(FNTASKMAIN fnMain, LPVOID lpArgument, HANDLE hTask)
{
    FANS_ASSERT(fnMain, , "The task main function is NULL.");
    FANS_ASSERT(INVALID_HANDLE_VALUE != hTask, , "The task object handle is invalid.");

    LOG_DEBUG(TRUE, "Task 0x%08X main 0x%P permission %u starting(%lld) now ...",
            hTask, fnMain, lpArgument, caGetTaskStartTick(hTask));

    fnMain(lpArgument);
    
    caCloseTask(hTask);

    LOG_ERROR(TRUE, "BUG: ****The task(handle: 0x%08X) has been died ****", hTask);
}

EXPORT VOID USER_TaskLeave(VOID)
{
    caCloseTask(TASK_SELF_HANDLE);
    while(1) caScheduleTimeout(1000);
}
