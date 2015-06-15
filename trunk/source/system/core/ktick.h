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
#ifndef __CORE_TICK_H
#define __CORE_TICK_H

#include <fadefs.h>
#include <faerror.h>
#include <fatypes.h>

enum{
    LPC_SCM_GET_SYSTICK        =    0,
};
#ifdef __cplusplus
extern "C" {
#endif
    EXPORT TICK CORE_GetSystemTick(VOID);
#ifdef __cplusplus
}
#endif
#endif
