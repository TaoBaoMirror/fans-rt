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
 *    2014-09-07     JiangYong       new file
 */
#ifndef __K_MEM_H
#define __K_MEM_H

#include <fadefs.h>
#include <fatypes.h>
#include <faerror.h>

enum{
    LPC_MMS_PAGE_ALLOC      =       0,
    LPC_MMS_PAGE_FREE       =       1,
    LPC_MMS_NEW_REGION      =       2,
    LPC_MMS_FREE_PAGES      =       3,
    LPC_MMS_TAKE_INFOR      =       4,
    LPC_MMS_SHOW_INFOR      =       5,
    LPC_MMS_SHOW_SECTION    =       6,
    LPC_MMS_REQUEST_MAX
};

#ifdef __cplusplus
extern "C" {
#endif
    EXPORT LPVOID CORE_PageAlloc(SIZE_T Length);
    EXPORT E_STATUS CORE_PageFree(LPVOID lpAddress);
#ifdef __cplusplus
}
#endif
#endif
