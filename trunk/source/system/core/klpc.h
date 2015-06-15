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
#ifndef __K_LPC_H
#define __K_LPC_H

#include <fadefs.h>
#include <fatypes.h>

/* ϵͳ�������� */
typedef struct tagLPC_SERVICE LPC_SERVICE;
typedef struct tagLPC_SERVICE * PLPC_SERVICE;
typedef struct tagLPC_SERVICE FAR * LPLPC_SERVICE;

#define DEFINE_LPC_SERVICE(Symbol, SvcID, MaxFuncs, PrivData, FuncArray)                \
        STATIC CONST LPC_SERVICE Symbol = {                                             \
            LPC_MAGIC,                                                                  \
            { SvcID },                                                                  \
            MaxFuncs,                                                                   \
            PrivData,                                                                   \
            FuncArray                                                                   \
        }

#define DEFINE_LPC_MANAGER(Symbol, SvcID, MaxFuncs, PrivData, FuncArray)                \
        STATIC CONST LPC_SERVICE Symbol = {                                             \
            LPC_MAGIC,                                                                  \
            { SvcID },                                                                  \
            MaxFuncs,                                                                   \
            PrivData,                                                                   \
            FuncArray                                                                   \
        }

#define LPC_INSTALL(SVC, Msg)                                                           \
        do{                                                                             \
            LPTSTR lpMessage = Msg;                                                     \
            E_STATUS State = CORE_InstallLPC(SVC);                                     \
                                                                                        \
            if (STATE_SUCCESS != State)                                                 \
            {                                                                           \
                kprintf("%s return %d... [FAILED]\n", lpMessage, State);            \
                SYSTEM_CALL_OOPS();                                                     \
            }                                                                           \
                                                                                        \
            CORE_INFOR(TRUE, "%s ... [OK]", lpMessage);                                 \
                                                                                        \
        }while(0)

typedef E_STATUS (* REQUEST_HANDLER)(LPVOID lpPrivate, LPVOID lpPacket);

struct tagLPC_SERVICE
{
    DWORD                   Magic;                          /**< ħ�� */
    union{
        DWORD               ServiceID;                      /**< ����ID */
        CHAR                SvcName[4];                     /**< ������ */
    }u;
    WORD                    MaxFuncs;                       /**< ֧�ֵĹ����� */
    LPVOID                  lpPrivate;                      /**< ����˽������*/
    CONST REQUEST_HANDLER * fnHandlers;                     /**< ���������� */
};


typedef struct tagLPC_REQUEST_PACKET LPC_REQUEST_PACKET;
typedef struct tagLPC_REQUEST_PACKET * PLPC_REQUEST_PACKET;
typedef struct tagLPC_REQUEST_PACKET FAR * LPLPC_REQUEST_PACKET;

struct tagLPC_REQUEST_PACKET{
    union{
        VOLATILE DWORD                  ServiceID;
        VOLATILE CHAR                   SvcName[4];
    }un;
    VOLATILE E_STATUS               StateCode;
    VOLATILE BYTE                   Reserved;
    VOLATILE BYTE                   FunctionID;
    union{
        VOLATILE LONG               lParam;
        VOLATILE DWORD              dParam;
        VOLATILE LPVOID             pParam;
        VOLATILE HANDLE             hParam;
    }u0;
    union{
        VOLATILE LONG               lParam;
        VOLATILE LPVOID             pParam;
        VOLATILE DWORD              dParam;
        VOLATILE HANDLE             hParam;
    }u1;
    union{
        VOLATILE LONG               lParam;
        VOLATILE LPVOID             pParam;
        VOLATILE DWORD              dParam;
        VOLATILE HANDLE             hParam;
    }u2;
};


enum{
    LPC_SERVICE_INSTALL     =       0,
    LPC_SERVICE_UNSTALL     =       1,
    LPC_MANAGER_MAX
};

#ifdef __cplusplus
extern "C" {
#endif
    EXPORT E_STATUS CORE_InstallLPC(CONST LPC_SERVICE * lpService);
    EXPORT E_STATUS ntInstallLPC(CONST LPC_SERVICE * lpService);
    EXPORT E_STATUS ntUnstallLPC(CONST LPC_SERVICE * lpService);
    EXPORT VOID CORE_HandlerLPC(LPLPC_REQUEST_PACKET lpPacket, DWORD ServiceID, BYTE FunctionID);
#ifdef __cplusplus
}
#endif
#endif
