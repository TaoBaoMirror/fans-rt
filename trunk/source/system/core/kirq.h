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

#ifndef __K_IRQ_H
#define __K_IRQ_H

#include <fadefs.h>
#include <fatypes.h>
#include <faerror.h>

#include "birq.h"
#include "klist.h"
#include "kcore.h"

/* 硬件中断服务 */
typedef struct tagIRQ_SERVICE IRQ_SERVICE;
typedef struct tagIRQ_SERVICE * PIRQ_SERVICE;
typedef struct tagIRQ_SERVICE FAR * LPIRQ_SERVICE;

#define DEFINE_IRQ_SERVICE(Symbol, SvcID, IRQNumber, PrvData, fnHandler)                        \
        STATIC IRQ_SERVICE Symbol = { IRQ_MAGIC, SvcID, NULL, NULL, IRQNumber, PrvData, fnHandler }

typedef E_STATUS (FAR * IRQHANDLER)(LPVOID lpPacket, LPVOID lpPrivate);

struct tagIRQ_SERVICE
{
    DWORD               Magic;                          /**< 魔数 */
    union{
        DWORD           ServiceID;                      /**< 服务ID */
        CHAR            SvcName[4];                     /**< 服务名 */
    }u;
    LIST_HEAD           ISRList;                        /**< ISR链表 */
    E_IRQ_ID            IRQNumber;                      /**< 中断类型 */
    LPVOID              lpPrivate;                      /**< 私有数据*/
    IRQHANDLER          fnHandler;                      /**< 处理函数 */
};


enum{
    IRQ_SERVICE_INSTALL         =       0,
    IRQ_SERVICE_UNSTALL         =       1,
    IRQ_MANAGER_MAX
};

#define IRQ_INSTALL(SVC, Msg)                                                           \
        do{                                                                             \
            LPTSTR lpMessage = Msg;                                                     \
            if (STATE_SUCCESS != ntInstallIRQ(SVC))                                 \
            {                                                                           \
                kprintf("%s ... [FAILED]\n", lpMessage);                            \
                SYSTEM_CALL_OOPS();                                                     \
            }                                                                           \
            CORE_INFOR(TRUE, _TEXT"%s ... [OK]", lpMessage);                                    \
        }while(0)
        
#ifdef __cplusplus
extern "C" {
#endif
    EXPORT E_STATUS ntInstallIRQ(LPIRQ_SERVICE lpHandler);
    EXPORT E_STATUS ntUnstallIRQ(LPIRQ_SERVICE lpHandler);
    EXPORT VOID CORE_HandlerIRQ(LPVOID lpPacket, E_IRQ_ID IRQNumber);
#ifdef __cplusplus
}
#endif
#endif
