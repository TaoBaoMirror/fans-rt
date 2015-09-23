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
 *    2015-08-28     JiangYong       new file
 */

#include <ntddk.h>
#include "common.h"
#include "vport.h"


typedef UCHAR (* SRB_PROCESSES)(IN PMINISCSI_ADAPTER Adapter, IN PSCSI_REQUEST_BLOCK Srb);
#define     ProcessessExecuteScsi       ExecuteScsiCDB
#pragma SECTION_DATA_BASE
static SRB_PROCESSES g_fnProcessesSrbRequest[256];

#pragma SECTION_CODE_BASE
UCHAR ProcessessShutdown(IN PMINISCSI_ADAPTER Adapter,
                         IN PSCSI_REQUEST_BLOCK Srb)
{
    KdPrint((DRIVER_NAMEA"[%04d][%s]: %p.\n",
        __LINE__, __FUNCTION__, Adapter));

    return SRB_STATUS_SUCCESS;
}

#pragma SECTION_CODE_BASE
UCHAR ProcessessFlush(IN PMINISCSI_ADAPTER Adapter,
                      IN PSCSI_REQUEST_BLOCK Srb)
{
    KdPrint((DRIVER_NAMEA"[%04d][%s]: %p.\n",
        __LINE__, __FUNCTION__, Adapter));

    return SRB_STATUS_SUCCESS;
}


#pragma SECTION_CODE_BASE
UCHAR ProcessessIOControl(IN PMINISCSI_ADAPTER Adapter,
                          IN PSCSI_REQUEST_BLOCK Srb)
{
    KdPrint((DRIVER_NAMEA"[%04d][%s]: %p.\n",
        __LINE__, __FUNCTION__, Adapter));

    return SRB_STATUS_SUCCESS;
}

#pragma SECTION_CODE_BASE
UCHAR ProcessessPower(IN PMINISCSI_ADAPTER Adapter,
                      IN PSCSI_REQUEST_BLOCK Srb)
{
    KdPrint((DRIVER_NAMEA"[%04d][%s]: %p.\n",
        __LINE__, __FUNCTION__, Adapter));

    return SRB_STATUS_SUCCESS;
}

#pragma SECTION_CODE_BASE
UCHAR ProcessessPnp(IN PMINISCSI_ADAPTER Adapter,
                    IN PSCSI_REQUEST_BLOCK Srb)
{
    KdPrint((DRIVER_NAMEA"[%04d][%s]: %p.\n",
        __LINE__, __FUNCTION__, Adapter));

    return SRB_STATUS_SUCCESS;
}


#pragma SECTION_CODE_BASE
BOOLEAN Thread_StartIO (IN PMINISCSI_ADAPTER Adapter,
                          IN PSCSI_REQUEST_BLOCK Srb)
{
    SRB_PROCESSES fnProcess;

    KdPrint((DRIVER_NAMEA"[%04d][%s]: Call function %02X.\n",
        __LINE__, __FUNCTION__, Srb->Function));

    Srb->ScsiStatus = SCSISTAT_GOOD;
    Srb->SrbStatus  = SRB_STATUS_INVALID_REQUEST;
    fnProcess = g_fnProcessesSrbRequest[Srb->Function & 0xff];

    if (NULL != fnProcess)
    {
        Srb->SrbStatus = fnProcess((PMINISCSI_ADAPTER)Adapter, Srb);
        KdPrint((DRIVER_NAMEA"[%04d][%s]: Call function %02X. result 0x%02X\n",
            __LINE__, __FUNCTION__, Srb->Function, Srb->SrbStatus));
    }

    return TRUE;
}


#pragma SECTION_CODE_INIT
VOID SRBProcessInitialize(VOID)
{
    RtlZeroMemory(g_fnProcessesSrbRequest, sizeof(g_fnProcessesSrbRequest));

    g_fnProcessesSrbRequest[SRB_FUNCTION_EXECUTE_SCSI]  =   ProcessessExecuteScsi;
    g_fnProcessesSrbRequest[SRB_FUNCTION_SHUTDOWN]      =   ProcessessShutdown;
    g_fnProcessesSrbRequest[SRB_FUNCTION_FLUSH]         =   ProcessessFlush;
    g_fnProcessesSrbRequest[SRB_FUNCTION_IO_CONTROL]    =   ProcessessIOControl;
    //  g_fnProcessesSrbRequest[SRB_FUNCTION_POWER]         =   ProcessessPower;
    //  g_fnProcessesSrbRequest[SRB_FUNCTION_PNP]           =   ProcessessPnp;
}
