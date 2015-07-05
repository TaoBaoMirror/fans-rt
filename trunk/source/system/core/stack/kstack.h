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
 *    2015-06-27     JiangYong       new file
 */
#ifndef __K_STACK_H
#define __K_STACK_H

#include "config.h"
#include <fadefs.h>
#include <fatypes.h>

enum{
    LPC_STK_STACK_MALLOC,
    LPC_STK_STACK_INIT,
    LPC_STK_STACK_FREE,
};

typedef struct tagSTACK_DESCRIPTOR STACK_DESCRIPTOR;
typedef struct tagSTACK_DESCRIPTOR * PSTACK_DESCRIPTOR;
typedef struct tagSTACK_DESCRIPTOR FAR * LPSTACK_DESCRIPTOR;

struct tagSTACK_DESCRIPTOR{
    SIZE_T                  StackCapacity;                      /* ¶ÑÕ»ÈÝÁ¿ */
    LPVOID                  lpStackBuffer;                      /* ¶ÑÕ»Ö¸Õë */
    LPVOID                  lpStackPosition;                    /* Õ»¶¥Ö¸Õë */
#if (FALSE == CONFIG_DYNAMIC_STACK_ENABLE)
    HANDLE                  StackHandle;
#else
    DWORD                   Reserved;
#endif
};

#define     SetStackCapacity(lpSD, Capacity)            do { (lpSD)->StackCapacity = (Capacity); } while(0)
#define     GetStackCapacity(lpSD)                      ((lpSD)->StackCapacity)
#define     SetStackPosition(lpSD, lpPosition)          do { (lpSD)->lpStackPosition = (lpPosition); } while(0)
#define     GetStackPosition(lpSD)                      ((lpSD)->lpStackPosition)
#define     SetStackBuffer(lpSD, lpBuffer)              do { (lpSD)->lpStackBuffer = (lpBuffer); } while(0)
#define     GetStackBuffer(lpSD)                        ((lpSD)->lpStackBuffer)
#if (FALSE == CONFIG_DYNAMIC_STACK_ENABLE)
#define     SetStackHandle(lpSD, hStack)                do { (lpSD)->StackHandle = (hStack); } while(0)
#define     GetStackHandle(lpSD)                        ((lpSD)->StackHandle)
#else
#define     SetStackHandle(lpSD, hStack)
#endif


#ifdef __cplusplus
extern "C" {
#endif
    PUBLIC E_STATUS CORE_StackMalloc(LPVOID lpTaskContext, LPVOID lpTaskParam, E_TASK_PERMISSION Permission);
    PUBLIC E_STATUS CORE_StackInit(LPVOID lpTaskContext, LPVOID lpParam, E_TASK_PERMISSION Permission);
    PUBLIC E_STATUS CORE_StackFree(LPVOID lpTaskContext, E_TASK_PERMISSION Permission);
#ifdef __cplusplus
}
#endif
#endif

