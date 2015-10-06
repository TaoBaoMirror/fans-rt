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

#include <stdarg.h>
#include <fadefs.h>
#include <faerror.h>
#include <fatypes.h>

#define     INVALID_MAGIC               INVALID_VALUE
/* The some system objects magic */
#define     POL_MAGIC                   MAKE_DWORD('P', 'O', 'L')               /**< The pool object */
#define     SCC_MAGIC                   MAKE_DWORD('S', 'C', 'C')               /**< Core stack for core task */
#define     SCU_MAGIC                   MAKE_DWORD('S', 'C', 'U')               /**< Core stack for user task */
#define     SUU_MAGIC                   MAKE_DWORD('S', 'U', 'U')               /**< User stack for user task */
#define     TSK_MAGIC                   MAKE_DWORD('T', 'S', 'K')               /**< The task object */
#define     EVT_MAGIC                   MAKE_DWORD('E', 'V', 'T')               /**< The event object */
#define     MTX_MAGIC                   MAKE_DWORD('M', 'T', 'X')               /**< The mutex object */
#define     SEM_MAGIC                   MAKE_DWORD('S', 'E', 'M')               /**< The semaphore object */
#define     SET_MAGIC                   MAKE_DWORD('S', 'E', 'T')               /**< The semset object */
#define     PPE_MAGIC                   MAKE_DWORD('P', 'P', 'E')               /**< The pipe object */
#define     SLT_MAGIC                   MAKE_DWORD('S', 'L', 'T')               /**< System local storage of task */


/* The some system modules magic */
#define     APP_MAGIC                   MAKE_DWORD('A', 'P', 'P')               /**< Application */

/* The some system manager services magic */
#define     LPC_MAGIC                   MAKE_DWORD('L', 'P', 'C')               /**< System local procedure call manager */
#define     IRQ_MAGIC                   MAKE_DWORD('I', 'R', 'Q')               /**< System interrupt manager */
#define     SMM_MAGIC                   MAKE_DWORD('S', 'M', 'M')               /**< System memory manager */
#define     STM_MAGIC                   MAKE_DWORD('T', 'S', 'K')               /**< System task manager */
#define     SCM_MAGIC                   MAKE_DWORD('S', 'C', 'M')               /**< System clock manager */
#define     SDM_MAGIC                   MAKE_DWORD('S', 'D', 'M')               /**< System debug manager */
#define     SOM_MAGIC                   MAKE_DWORD('S', 'O', 'M')               /**< System object manager*/
#define     STK_MAGIC                   MAKE_DWORD('S', 'T', 'K')               /**< System stack manager */

#define     SYSTEM_CALL_OOPS()          kSystemOops()


#ifdef __cplusplus
extern "C" {
#endif
    EXPORT VOID kSystemOops(VOID);
    EXPORT int kprintf(CONST CHAR * Format,...);
    EXPORT int kvprintf(CONST CHAR * Format, va_list vargs);
    EXPORT int kwprintf(CONST WCHAR * Format,...);
    EXPORT TICK CORE_GetSystemTick(VOID);
    PUBLIC VOID CORE_Starting(VOID);
#ifdef __cplusplus
}
#endif

#endif

