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


VOID DriverUnload(IN PDRIVER_OBJECT DriverObject)
{
    PDEVICE_OBJECT NextDevice;
    PDEVICE_OBJECT ThisDevice;

    KdPrint((DRIVER_NAMEA"[%04d][%s]: Virtual SCSI Driver Leave...\n",
        __LINE__, __FUNCTION__));

    NextDevice = DriverObject->DeviceObject;
    ThisDevice = NextDevice;

    while (NextDevice) 
    {
        NextDevice = ThisDevice->NextDevice;
        
        if (DriverGetControlDevice() == ThisDevice)
        {
            PCONTROL_EXTENSION ControlExtension = ThisDevice->DeviceExtension;
            IoDeleteSymbolicLink(&ControlExtension->SymbolName);
            IoDeleteDevice(ControlExtension->ControlDevice);
        }
    }
}


NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject,
                     IN PUNICODE_STRING RegistryPath)
{
    NTSTATUS            Status = STATUS_SUCCESS;
    PMINISCSI_ADAPTER   MiniScsiAdapter  = NULL;
    PCONTROL_EXTENSION  ControlExtension = NULL;

    KdPrint((DRIVER_NAMEA"[%04d][%s]: Virtual SCSI Driver Entry...\n",
        __LINE__, __FUNCTION__));

    Status = DriverInitializeMiniScsi(DriverObject,  RegistryPath, &MiniScsiAdapter);

    if (!NT_SUCCESS(Status))
    {
        KdPrint((DRIVER_NAMEA"[%04d][%s]: Initialize mini scsi failure.\n",
            __LINE__, __FUNCTION__));
        return Status;
    }

    Status = DriverCreateControlDevice(DriverObject, MiniScsiAdapter);

    if (!NT_SUCCESS(Status))
    {
        KdPrint((DRIVER_NAMEA"[%04d][%s]: Create control device '%s' failure.\n",
            __LINE__, __FUNCTION__, DEVICE_NAME_CONTROLA));
        return Status;
    }

    DriverObject->DriverUnload  =   DriverUnload;

    KdPrint((DRIVER_NAMEA"[%04d][%s]: starting virtual SCSI adapter successfully...\n",
        __LINE__, __FUNCTION__));

    return Status;
}

