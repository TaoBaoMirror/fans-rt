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
#ifndef __K_CORE_H
#define __K_CORE_H

#include <fadefs.h>
#include <faerror.h>
#include <fatypes.h>

#define     INVALID_MAGIC               INVALID_VALUE
/* The some system objects magic */
#define     POL_MAGIC                   MAKE_DWORD('P', 'O', 'L')               /**< The pool object */
#define     STK_MAGIC                   MAKE_DWORD('S', 'T', 'K')               /**< The Stack object */
#define     TSK_MAGIC                   MAKE_DWORD('T', 'S', 'K')               /**< The task object */
#define     EVT_MAGIC                   MAKE_DWORD('E', 'V', 'T')
#define     MTX_MAGIC                   MAKE_DWORD('M', 'T', 'X')
#define     SEM_MAGIC                   MAKE_DWORD('S', 'E', 'M')
#define     SET_MAGIC                   MAKE_DWORD('S', 'E', 'T')


/* The some system modules magic */
#define     APP_MAGIC                   MAKE_DWORD('A', 'P', 'P')               /**< Application */

/* The some system manager services magic */
#define     LPC_MAGIC                   MAKE_DWORD('L', 'P', 'C')               /**< System local procedure call manager */
#define     IRQ_MAGIC                   MAKE_DWORD('I', 'R', 'Q')               /**< System interrupt manager */
#define     SMM_MAGIC                   MAKE_DWORD('S', 'M', 'M')               /**< System memory manager */
#define     STM_MAGIC                   MAKE_DWORD('T', 'S', 'K')               /**< System task manager */
#define     SCM_MAGIC                   MAKE_DWORD('C', 'L', 'K')               /**< System clock manager */
#define     SDM_MAGIC                   MAKE_DWORD('S', 'D', 'M')               /**< System debug manager */
#define     SOM_MAGIC                   MAKE_DWORD('S', 'O', 'M')               /**< System object manager*/
#define     SYSTEM_CALL_OOPS()          kSystemOops()


#ifdef __cplusplus
extern "C" {
#endif
    EXPORT VOID kSystemOops(VOID);
    EXPORT E_STATUS kprintf(CONST CHAR * Format,...);
    EXPORT E_STATUS kwprintf(CONST WCHAR * Format,...);
    PUBLIC VOID CORE_Starting(VOID);
#ifdef __cplusplus
}
#endif

#endif

