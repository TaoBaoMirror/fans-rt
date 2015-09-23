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


STATIC PMINISCSI_ADAPTER  g_ScsiAdapter = NULL;
STATIC PCONTROL_EXTENSION g_ControlExtension = NULL;
STATIC PDEVICE_OBJECT   g_ControlDevice = NULL;
STATIC PDRIVER_DISPATCH g_HookFunctions[IRP_MJ_MAXIMUM_FUNCTION + 1];



STATIC NTSTATUS fnIoControlLoadImage(IN PCONTROL_EXTENSION ControlExtension, IN PIRP Irp)
{
    PVOID                       Descriptor;
    PVSCSI_IO_REQUEST_PACKET    Packet;
    MSCSI_MEDIA_TYPE            MediaType = MSCSI_MEDIA_NULL;

    Packet = (PVSCSI_IO_REQUEST_PACKET) Irp->AssociatedIrp.SystemBuffer;

    if (sizeof(Packet->LoadImage) != Packet->LoadImage.Length)
    {
        KdPrint((DRIVER_NAMEA"[%04d][%s]: Packet failed.\n",
            __LINE__, __FUNCTION__));

        return STATUS_INVALID_PARAMETER;
    }

    if (0 != Packet->LoadImage.DiskCapacity)
    {
        KdPrint((DRIVER_NAMEA"[%04d][%s]: DiskCapacity is %u.\n",
            __LINE__, __FUNCTION__, Packet->LoadImage.DiskCapacity));
        Descriptor  =   &Packet->LoadImage.DiskCapacity;
        MediaType   =   MSCSI_MEDIA_RAM;
        
    }
    else
    {
        Descriptor  =   &Packet->LoadImage.FileName;
        MediaType   =   MSCSI_MEDIA_FILE;
    }

    Irp->IoStatus.Information = 0;

    return CreateLun(ControlExtension->ScsiAdapter,
                     Packet->LoadImage.DiskName,
                     MediaType,
                     Descriptor);
}

STATIC NTSTATUS fnIoControlUnloadImage(IN PCONTROL_EXTENSION ControlExtension, IN PIRP Irp)
{
    PVSCSI_IO_REQUEST_PACKET    Packet;

    Packet = (PVSCSI_IO_REQUEST_PACKET) Irp->AssociatedIrp.SystemBuffer;

    if (sizeof(Packet->UnloadImage) != Packet->UnloadImage.Length)
    {
        KdPrint((DRIVER_NAMEA"[%04d][%s]: Packet failed.\n",
            __LINE__, __FUNCTION__));

        return STATUS_INVALID_PARAMETER;
    }

    KdPrint((DRIVER_NAMEA"[%04d][%s]: Unload file from '%s', adapter is 0x%p.\n",
        __LINE__, __FUNCTION__,
        Packet->UnloadImage.DiskName,
        ControlExtension->ScsiAdapter));

    Irp->IoStatus.Information = 0;

    return STATUS_SUCCESS;
}

typedef NTSTATUS (*FNCONTROL)(IN PCONTROL_EXTENSION ControlExtension, IN PIRP Irp);

typedef struct tagIOCTROL_DESCRIPTOR{
    ULONG               IoControlCode;
    FNCONTROL           IoControlFunc;
}IOCTROL_DESCRIPTOR;


static IOCTROL_DESCRIPTOR g_IoControlGroup[] =
{
    {   IOCTL_VSCSI_LOAD_IMAGE,             fnIoControlLoadImage        },
    {   IOCTL_VSCSI_UNLOAD_IMAGE,           fnIoControlUnloadImage      },
};

STATIC NTSTATUS ThreadIoControl(IN PCONTROL_EXTENSION ControlExtension,
                                  IN PIRP Irp)
{
    ULONG i = 0;
    NTSTATUS Status;
    PIO_STACK_LOCATION  IOStack;

    IOStack = IoGetCurrentIrpStackLocation(Irp);

    Irp->IoStatus.Status = STATUS_SUCCESS;

    KdPrint((DRIVER_NAMEA"[%04d][%s]: IO Control request id 0x%X.\n",
        __LINE__, __FUNCTION__, IOStack->Parameters.DeviceIoControl.IoControlCode));

    for (i = 0; i < SIZEOF_ARRAY(g_IoControlGroup); i++)
    {
        if (IOStack->Parameters.DeviceIoControl.IoControlCode
            == g_IoControlGroup[i].IoControlCode)
        {
            Irp->IoStatus.Status = g_IoControlGroup[i].IoControlFunc(ControlExtension, Irp);
            break;
        }
    }

    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return Irp->IoStatus.Status;

}


STATIC NTSTATUS DriverIoControl(IN PDEVICE_OBJECT DeviceObject,
                                 IN PIRP Irp) 
{
    ULONG i = 0;
    PIO_STACK_LOCATION  IOStack;
    PCONTROL_EXTENSION  ControlExtension;

    if (DeviceObject != g_ControlDevice)
    {
        if (g_HookFunctions[IRP_MJ_DEVICE_CONTROL])
        {
            return g_HookFunctions[IRP_MJ_DEVICE_CONTROL](DeviceObject, Irp);
        }

        KdPrint((DRIVER_NAMEA"[%04d][%s]: Operating not supported.\n",
            __LINE__, __FUNCTION__));

        return STATUS_NOT_SUPPORTED;
    }

    ControlExtension = (PCONTROL_EXTENSION) DeviceObject->DeviceExtension;
    IOStack = IoGetCurrentIrpStackLocation(Irp);
#if 0
    KdPrint((DRIVER_NAMEA"[%04d][%s]: IO Control request id 0x%X.\n",
        __LINE__, __FUNCTION__, IOStack->Parameters.DeviceIoControl.IoControlCode));

    for (i = 0; i < SIZEOF_ARRAY(g_IoControlGroup); i++)
    {
        if (IOStack->Parameters.DeviceIoControl.IoControlCode
            == g_IoControlGroup[i].IoControlCode)
        {
            Irp->IoStatus.Status = g_IoControlGroup[i].IoControlFunc(ControlExtension, Irp);
            break;
        }
    }

    Irp->IoStatus.Information = 0;
    ScsiPortNotification(BusChangeDetected, ControlExtension->ScsiAdapter, 0);
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return Irp->IoStatus.Status;
#else
    ExInterlockedInsertTailList(&ControlExtension->IrpListHead,
                                &Irp->Tail.Overlay.ListEntry,
                                &ControlExtension->IrpSpinLock);

    KeSetEvent(&ControlExtension->RequestEvent,
               (KPRIORITY) 0,
               FALSE);

    Irp->IoStatus.Status = STATUS_PENDING;
    IoMarkIrpPending(Irp);

    return STATUS_PENDING;
#endif

}

STATIC NTSTATUS DriverCreateFile(IN PDEVICE_OBJECT DeviceObject,
                          IN PIRP Irp) 
{
    if (DeviceObject != g_ControlDevice)
    {
        if (g_HookFunctions[IRP_MJ_CREATE])
        {
            return g_HookFunctions[IRP_MJ_CREATE](DeviceObject, Irp);
        }

        KdPrint((DRIVER_NAMEA"[%04d][%s]: Operating not supported.\n",
            __LINE__, __FUNCTION__));

        return STATUS_NOT_SUPPORTED;
    }

    KdPrint((DRIVER_NAMEA"[%04d][%s]: Open file '%s' successfully\n",
        __LINE__, __FUNCTION__, DEVICE_LINK_CONTROLA));

    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;

    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return STATUS_SUCCESS;
}


STATIC NTSTATUS DriverCloseFile(IN PDEVICE_OBJECT DeviceObject,
                          IN PIRP Irp) 
{
    if (DeviceObject != g_ControlDevice)
    {
        if (g_HookFunctions[IRP_MJ_READ])
        {
            return g_HookFunctions[IRP_MJ_READ](DeviceObject, Irp);
        }

        KdPrint((DRIVER_NAMEA"[%04d][%s]: Operating not supported.\n",
        __LINE__, __FUNCTION__));

        return STATUS_NOT_SUPPORTED;
    }

    KdPrint((DRIVER_NAMEA"[%04d][%s]: Close file '%s' successfully\n",
        __LINE__, __FUNCTION__, DEVICE_LINK_CONTROLA));

    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;

    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return STATUS_SUCCESS;
}

STATIC NTSTATUS DriverWriteFile(IN PDEVICE_OBJECT DeviceObject,
                              IN PIRP Irp) 
{
    if (DeviceObject != g_ControlDevice)
    {
        if (g_HookFunctions[IRP_MJ_WRITE])
        {
            return g_HookFunctions[IRP_MJ_WRITE](DeviceObject, Irp);
        }

        KdPrint((DRIVER_NAMEA"[%04d][%s]: Operating not supported.\n",
        __LINE__, __FUNCTION__));

        return STATUS_NOT_SUPPORTED;
    }

    KdPrint((DRIVER_NAMEA"[%04d][%s]: Write file '%s' successfully\n",
        __LINE__, __FUNCTION__, DEVICE_LINK_CONTROLA));

    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;

    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return STATUS_SUCCESS;
}

STATIC NTSTATUS DriverReadFile(IN PDEVICE_OBJECT DeviceObject,
                              IN PIRP Irp) 
{
    if (DeviceObject != g_ControlDevice)
    {
        if (g_HookFunctions[IRP_MJ_READ])
        {
            return g_HookFunctions[IRP_MJ_READ](DeviceObject, Irp);
        }

        KdPrint((DRIVER_NAMEA"[%04d][%s]: Operating not supported.\n",
        __LINE__, __FUNCTION__));

        return STATUS_NOT_SUPPORTED;
    }

    KdPrint((DRIVER_NAMEA"[%04d][%s]: Read file '%s' successfully\n",
        __LINE__, __FUNCTION__, DEVICE_LINK_CONTROLA));

    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;

    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return STATUS_SUCCESS;
}

/**************************************************************************
 * The control device request process thread.
 * param: The context of the thread.
 * @return none.
 *
 * date           author          notes
 * 2015-09-14     JiangYong       new function
**************************************************************************/
STATIC VOID ControlThread(VOID * Context)
{
    PCONTROL_EXTENSION ControlExtension;
    PDEVICE_OBJECT ControlDevice = Context;

    KdPrint((DRIVER_NAMEA"[%04d][%s]: Check parameters.\n",
            __LINE__, __FUNCTION__));
    
    MINISCSI_ASSERT(NULL != Context, PsTerminateSystemThread(STATUS_SUCCESS));

    KdPrint((DRIVER_NAMEA"[%04d][%s]: Starting control thread ...\n",
        __LINE__, __FUNCTION__));

    ControlExtension = ControlDevice->DeviceExtension;

    KeSetPriorityThread(KeGetCurrentThread(), LOW_REALTIME_PRIORITY);

    for(;;)
    {
        PLIST_ENTRY RequestNode = NULL;
        
        KeWaitForSingleObject(&ControlExtension->RequestEvent,
                              Executive,
                              KernelMode,
                              FALSE,
                              NULL);
 
        if (ControlExtension->TerminateThread)
        {
            ControlExtension->FinishedThread = TRUE;
            break;
        }

        while(NULL != (RequestNode = ExInterlockedRemoveHeadList(
              &ControlExtension->IrpListHead,
              &ControlExtension->IrpSpinLock)))
        {
            PIRP CurrentIRP = CONTAINING_RECORD(RequestNode, IRP, Tail.Overlay.ListEntry);
            ThreadIoControl(ControlExtension, CurrentIRP);
        }
        
    }

    KdPrint((DRIVER_NAMEA"[%04d][%s]: Terminate control thread ...\n",
        __LINE__, __FUNCTION__));

    PsTerminateSystemThread(STATUS_SUCCESS);
    
    return;
}

STATIC VOID HookMajorFunction(IN OUT PDRIVER_OBJECT DriverObject, 
                              IN PDRIVER_DISPATCH Routine,
                              IN ULONG FunctionID)
{
    g_HookFunctions[FunctionID]  =  DriverObject->MajorFunction[FunctionID];
    DriverObject->MajorFunction[FunctionID] = Routine;
}

PUBLIC PDEVICE_OBJECT DriverGetControlDevice(VOID)
{
    return g_ControlDevice;
}

PUBLIC PCONTROL_EXTENSION DriverGetControlExtension(VOID)
{
    return g_ControlExtension;
}

PUBLIC PMINISCSI_ADAPTER DriverGetSCSIAdapter(VOID)
{
    return g_ScsiAdapter;
}

PUBLIC NTSTATUS DriverCreateControlDevice(IN PDRIVER_OBJECT DriverObject,
                                          IN OUT PMINISCSI_ADAPTER ScsiAdapter)
{
    NTSTATUS            Status;
    UNICODE_STRING      DeviceName;
    UNICODE_STRING      SymbolName;
    PDEVICE_OBJECT      ControlDevice = NULL;
    PCONTROL_EXTENSION  ControlExtension = NULL;

    KdPrint((DRIVER_NAMEA"[%04d][%s]: Check parameters.\n",
            __LINE__, __FUNCTION__));

    MINISCSI_ASSERT(NULL != ScsiAdapter, return STATUS_INVALID_PARAMETER);

    KdPrint((DRIVER_NAMEA"[%04d][%s]: Starting create control device.\n",
        __LINE__, __FUNCTION__));

    RtlZeroMemory(g_HookFunctions, sizeof(g_HookFunctions));
    RtlInitUnicodeString(&DeviceName, DEVICE_NAME_CONTROLW);
    RtlInitUnicodeString(&SymbolName, DEVICE_LINK_CONTROLW);

    Status = IoCreateDevice(DriverObject,
                            sizeof(CONTROL_EXTENSION),
                            &DeviceName,
                            FILE_DEVICE_UNKNOWN,
                            0, TRUE,
                            &ControlDevice);

    if (!NT_SUCCESS(Status))
    {
        KdPrint((DRIVER_NAMEA"[%04d][%s]: Create device '%s' failure.\n",
            __LINE__, __FUNCTION__, DEVICE_NAME_CONTROLA));
        return Status;
    }

    ControlDevice->Flags |= DO_DIRECT_IO;
    ControlExtension = (PCONTROL_EXTENSION) ControlDevice->DeviceExtension;
    ScsiAdapter->ControlDevice  =   ControlExtension;

    RtlZeroMemory(ControlExtension, sizeof(CONTROL_EXTENSION));
    
    ControlExtension->Magic      =  CTRL_MAGIC;
    ControlExtension->DeviceName =  DeviceName;
    ControlExtension->SymbolName =  SymbolName;
    ControlExtension->ControlDevice = ControlDevice;
    ControlExtension->ScsiAdapter   = ScsiAdapter;
    ControlExtension->TerminateThread = FALSE;
    ControlExtension->FinishedThread  = FALSE;

    InitializeListHead(&ControlExtension->IrpListHead);
    KeInitializeSpinLock(&ControlExtension->IrpSpinLock);
    KeInitializeEvent(&ControlExtension->RequestEvent,
                      SynchronizationEvent, FALSE);

    g_ScsiAdapter = ScsiAdapter;
    g_ControlDevice = ControlDevice;
    g_ControlExtension = ControlExtension;

    HookMajorFunction(DriverObject, DriverCreateFile, IRP_MJ_CREATE);
    HookMajorFunction(DriverObject, DriverCloseFile, IRP_MJ_CLOSE);
    HookMajorFunction(DriverObject, DriverWriteFile, IRP_MJ_WRITE);
    HookMajorFunction(DriverObject, DriverReadFile, IRP_MJ_READ);
    HookMajorFunction(DriverObject, DriverIoControl, IRP_MJ_DEVICE_CONTROL);
  
    Status = IoCreateSymbolicLink(&SymbolName, &DeviceName);

    if (!NT_SUCCESS(Status))
    {
        KdPrint((DRIVER_NAMEA"[%04d][%s]: Create symbol link '%s' failure.\n",
            __LINE__, __FUNCTION__, DEVICE_LINK_CONTROLA));
        IoDeleteDevice(ControlDevice);
        return Status;
    }

    Status = PsCreateSystemThread(&ControlExtension->ThreadHandle,
                                  (ACCESS_MASK) 0L,
                                  NULL,
                                  NULL,
                                  NULL,
                                  ControlThread,
                                  ControlDevice);

    if (!NT_SUCCESS(Status))
    {
        KdPrint((DRIVER_NAMEA"[%04d][%s]: Create control thread failure.\n",
            __LINE__, __FUNCTION__, DEVICE_LINK_CONTROLA));
        IoDeleteSymbolicLink(&ControlExtension->SymbolName);
        IoDeleteDevice(ControlDevice);
        return Status;
    }

    return Status;
}
