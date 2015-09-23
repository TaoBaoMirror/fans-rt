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


PUBLIC PMINISCSI_LOGICAL
AdapterGetLogicUnit(IN PMINISCSI_ADAPTER ScsiAdapter,
                       IN UCHAR TargetID,
                       IN UCHAR LunID)
{
    if (TargetID >= SCSI_MAXIMUM_TARGETS || LunID >= SCSI_MAXIMUM_LOGICAL_UNITS)
    {
        return NULL;
    }

    return AcquireLun(ScsiAdapter->ScsiLunArray[TargetID][LunID]);
}

PUBLIC BOOLEAN
AdapterPutLogicUnit(IN PMINISCSI_ADAPTER ScsiAdapter,
                       IN UCHAR TargetID,
                       IN UCHAR LunID)
{    
    if (TargetID >= SCSI_MAXIMUM_TARGETS || LunID >= SCSI_MAXIMUM_LOGICAL_UNITS)
    {
        return TRUE;
    }

    return ReleaseLun(ScsiAdapter->ScsiLunArray[TargetID][LunID]);
}

BOOLEAN Thread_StartIO (IN PMINISCSI_ADAPTER Adapter,
                          IN PSCSI_REQUEST_BLOCK Srb);

STATIC VOID AdapterSrbTimer(PVOID DeviceExtension)
{
    PLIST_ENTRY         Request = NULL;
    PMINISCSI_ADAPTER   Adapter = DeviceExtension;

    while(NULL != (Request = ExInterlockedRemoveHeadList(
              &Adapter->Finished.SrbListHead,
              &Adapter->Finished.SrbListLock)))
    {
        PMINISCSI_SRBEXTN SrbExtn = 
            CONTAINING_RECORD(Request,
                    MINISCSI_SRBEXTN,
                    SrbList);
        if (SRBEXTN_MAGIC != SrbExtn->Magic ||  NULL == SrbExtn->SrbThis)
        {
            KdPrint((DRIVER_NAMEA"[%04d][%s]: *** BUG: Invalid SRB(%p) magic 0x%08x.\n",
                    __LINE__, __FUNCTION__, SrbExtn, SrbExtn->Magic));
            continue;
        }

        ScsiPortNotification(RequestComplete, DeviceExtension,SrbExtn->SrbThis);
    }

    ScsiPortNotification(RequestTimerCall, DeviceExtension, AdapterSrbTimer, 10);
}

#pragma SECTION_CODE_BASE

STATIC VOID AdapterThread(VOID * Context)
{
    PMINISCSI_ADAPTER   Adapter = Context;

    KdPrint((DRIVER_NAMEA"[%04d][%s]: Check parameters.\n",
            __LINE__, __FUNCTION__));

    MINISCSI_ASSERT(NULL != Context, PsTerminateSystemThread(STATUS_SUCCESS));

    KeSetPriorityThread(KeGetCurrentThread(), LOW_REALTIME_PRIORITY);

    for(;;)
    {
        PLIST_ENTRY Request = NULL;
        
        KeWaitForSingleObject(&Adapter->RequestEvent,
                              Executive,
                              KernelMode,
                              FALSE,
                              NULL);

        if (Adapter->TerminateThread)
        {
            Adapter->FinishedThread = TRUE;
            break;
        }

        while(NULL != (Request = ExInterlockedRemoveHeadList(
              &Adapter->Pending.SrbListHead,
              &Adapter->Pending.SrbListLock)))
        {
            PMINISCSI_SRBEXTN SrbExtn = 
                    CONTAINING_RECORD(Request,
                                      MINISCSI_SRBEXTN,
                                      SrbList);

            if (SRBEXTN_MAGIC != SrbExtn->Magic ||  NULL == SrbExtn->SrbThis)
            {
                KdPrint((DRIVER_NAMEA"[%04d][%s]: *** BUG: Invalid SRB(%p) magic 0x%08x.\n",
                        __LINE__, __FUNCTION__, SrbExtn, SrbExtn->Magic));
                continue;
            }
            
            Thread_StartIO(Adapter, SrbExtn->SrbThis);

            ExInterlockedInsertTailList(&Adapter->Finished.SrbListHead,
                                        Request,
                                        &Adapter->Finished.SrbListLock);
        }
    }

    KdPrint((DRIVER_NAMEA"[%04d][%s]: Terminate adapter thread ...\n",
        __LINE__, __FUNCTION__));

    PsTerminateSystemThread(STATUS_SUCCESS);

    return;

}



#pragma SECTION_CODE_BASE

STATIC BOOLEAN StartIO (IN PVOID DeviceExtension,
                 IN PSCSI_REQUEST_BLOCK Srb)
{
    PMINISCSI_ADAPTER   Adapter = DeviceExtension;
    PMINISCSI_SRBEXTN   SrbExtn = Srb->SrbExtension;

    Srb->ScsiStatus = SCSISTAT_GOOD;
    Srb->SrbStatus = SRB_STATUS_PENDING;

    SrbExtn->SrbThis = Srb;
    SrbExtn->Magic = SRBEXTN_MAGIC;

    if (0 != Srb->PathId)
    {
        KdPrint((DRIVER_NAMEA"[%04d][%s]: Not support path %u \n",
            __LINE__, __FUNCTION__, Srb->PathId));

        Srb->SrbStatus = SRB_STATUS_INVALID_PATH_ID;
        ScsiPortNotification(RequestComplete, Adapter,Srb);
        ScsiPortNotification(NextRequest, Adapter);
    
        return TRUE;
    }

    if (Srb->TargetId >= SCSI_MAXIMUM_TARGETS)
    {
        KdPrint((DRIVER_NAMEA"[%04d][%s]: Not support target %u \n",
            __LINE__, __FUNCTION__, Srb->TargetId));

        Srb->SrbStatus = SRB_STATUS_INVALID_TARGET_ID;
        ScsiPortNotification(RequestComplete, Adapter,Srb);
        ScsiPortNotification(NextRequest, Adapter);
    
        return TRUE;
    }

    if (Srb->Lun >= SCSI_MAXIMUM_LOGICAL_UNITS)
    {
        KdPrint((DRIVER_NAMEA"[%04d][%s]: Not support lun %u \n",
            __LINE__, __FUNCTION__, Srb->Lun));

        Srb->SrbStatus = SRB_STATUS_INVALID_LUN;
        ScsiPortNotification(RequestComplete, Adapter,Srb);
        ScsiPortNotification(NextRequest, Adapter);
    
        return TRUE;
    }

    ExInterlockedInsertTailList(&Adapter->Pending.SrbListHead,
                                &SrbExtn->SrbList,
                                &Adapter->Pending.SrbListLock);

    KeSetEvent(&Adapter->RequestEvent,
               IO_NO_INCREMENT,
               FALSE);

    ScsiPortNotification(RequestTimerCall, DeviceExtension, AdapterSrbTimer, 10);
    ScsiPortNotification(NextRequest, Adapter);

    return TRUE;
}


STATIC ULONG FindAdapter(IN PVOID DeviceExtension,
                  IN PVOID HwContext,
                  IN PVOID BusInformation,
                  IN PCHAR ArgumentString,
                  IN OUT PPORT_CONFIGURATION_INFORMATION ConfigInfo,
                  OUT PBOOLEAN Again)
{
    PMINISCSI_ADAPTER   Adapter = DeviceExtension;
    PMINISCSI_ADAPTER * ReturnAdapter = HwContext;

    KdPrint((DRIVER_NAMEA"[%04d][%s]: Check parameters.\n",
        __LINE__, __FUNCTION__));

    MINISCSI_ASSERT(NULL != Adapter, return SP_RETURN_NOT_FOUND);
    MINISCSI_ASSERT(NULL != HwContext, return SP_RETURN_NOT_FOUND);

    *ReturnAdapter = Adapter;

    KdPrint((DRIVER_NAMEA"[%04d][%s]: Starting find adapter.\n",
        __LINE__, __FUNCTION__));

    ConfigInfo->Length  =   sizeof(PORT_CONFIGURATION_INFORMATION);
    ConfigInfo->AdapterInterfaceType    =   Isa;
    ConfigInfo->AlignmentMask           =   0x00000003;
    ConfigInfo->AutoRequestSense        =   TRUE;
    ConfigInfo->BufferAccessScsiPortControlled =    FALSE;
    ConfigInfo->BusInterruptLevel       =   0;
    ConfigInfo->BusInterruptVector      =   0;
    ConfigInfo->Dma32BitAddresses       =   TRUE;
    ConfigInfo->Master          =   TRUE;
    ConfigInfo->CachesData      =   TRUE;
    ConfigInfo->NumberOfBuses   =   1;
    ConfigInfo->MaximumNumberOfTargets  =   SCSI_MAXIMUM_TARGETS;
    ConfigInfo->MaximumTransferLength   =   DEFAULT_SECTOR_SIZE * 128;
    ConfigInfo->MultipleRequestPerLu    =   FALSE;
    ConfigInfo->NumberOfPhysicalBreaks  =   0x00F8;
    ConfigInfo->ScatterGather   =   FALSE;
    ConfigInfo->TaggedQueuing   =   FALSE;
    ConfigInfo->WmiDataProvider =   FALSE;

    *Again = FALSE;

    return SP_RETURN_FOUND; 
}


STATIC BOOLEAN Initialize(IN PVOID DeviceExtension)
{
    NTSTATUS            Status;
    UCHAR               TargetId, LunId;
    PMINISCSI_ADAPTER   Adapter = DeviceExtension;
    
    KdPrint((DRIVER_NAMEA"[%04d][%s]: Check parameters.\n",
        __LINE__, __FUNCTION__));

    MINISCSI_ASSERT(NULL != Adapter, return FALSE);
    
    KdPrint((DRIVER_NAMEA"[%04d][%s]: Check parameters.\n",
        __LINE__, __FUNCTION__));

    
    RtlZeroMemory(Adapter, sizeof(PMINISCSI_ADAPTER)); 

    KeInitializeSpinLock(&Adapter->AdapterLock);
    InitializeListHead(&Adapter->Pending.SrbListHead);
    KeInitializeSpinLock(&Adapter->Pending.SrbListLock);
    InitializeListHead(&Adapter->Finished.SrbListHead);
    KeInitializeSpinLock(&Adapter->Finished.SrbListLock);
    KeInitializeEvent(&Adapter->RequestEvent,
                      SynchronizationEvent, FALSE);

    Adapter->Magic =   ADAPTER_MAGIC;

    for (TargetId = 0; TargetId < SCSI_MAXIMUM_TARGETS; TargetId ++)
    {
        for (LunId = 0; LunId < SCSI_MAXIMUM_LOGICAL_UNITS; LunId ++)
        {
            Adapter->ScsiLunArray[TargetId][LunId]
                = AllocatScsiLun(Adapter, TargetId, LunId);

            if (NULL == Adapter->ScsiLunArray[TargetId][LunId])
            {
                KdPrint((DRIVER_NAMEA"[%04d][%s]: Allocate scsi lun 00:%02X:%02X failed.\n",
                    __LINE__, __FUNCTION__, TargetId, LunId));

                return FALSE;
            }
        }
    }

    Status = PsCreateSystemThread(&Adapter->AdapterThread,
                                  (ACCESS_MASK) 0L,
                                  NULL,
                                  NULL,
                                  NULL,
                                  AdapterThread,
                                  Adapter);

    if (!NT_SUCCESS(Status))
    {
        KdPrint((DRIVER_NAMEA"[%04d][%s]: Create adapter thread failed.\n",
            __LINE__, __FUNCTION__));
        return FALSE;
    }

    return TRUE;
}

STATIC BOOLEAN ResetBus(IN PVOID DeviceExtension, IN ULONG PathId)
{
    KdPrint((DRIVER_NAMEA"[%04d][%s]: %p  PathId is %u.\n",
        __LINE__, __FUNCTION__, DeviceExtension, PathId));
    return TRUE;
}


STATIC SCSI_ADAPTER_CONTROL_STATUS 
AdapterControl(PVOID DeviceExtension,
               SCSI_ADAPTER_CONTROL_TYPE CtlType,
               PVOID Parameters)
{
    PSCSI_SUPPORTED_CONTROL_TYPE_LIST ScsiList=NULL;
    SCSI_ADAPTER_CONTROL_STATUS status = ScsiAdapterControlSuccess;

    KdPrint((DRIVER_NAMEA"[%04d][%s]: %p.\n",
        __LINE__, __FUNCTION__, DeviceExtension));

    switch (CtlType)
    {
    case ScsiQuerySupportedControlTypes:
        KdPrint(("AdapterControl: ScsiQuerySupportedControlTypes\n"));
        ScsiList = (PSCSI_SUPPORTED_CONTROL_TYPE_LIST)Parameters;
        ScsiList->SupportedTypeList[ScsiStopAdapter] = TRUE;
        ScsiList->SupportedTypeList[ScsiRestartAdapter] = TRUE;
        ScsiList->SupportedTypeList[ScsiQuerySupportedControlTypes] = TRUE;
        break;

    case ScsiStopAdapter:
        KdPrint(("AdapterControl: ScsiStopAdapter\n"));
        break;

    case ScsiRestartAdapter:
        KdPrint(("AdapterControl: ScsiRestartAdapter\n"));
        break;

    default:
        status = ScsiAdapterControlUnsuccessful;
        break;
    }

    return status;
}


STATIC BOOLEAN AdapterState(IN PVOID DeviceExtension,
                            IN PVOID Context,
                            IN BOOLEAN SaveState)
{
    KdPrint((DRIVER_NAMEA"[%04d][%s]: %p.\n",
        __LINE__, __FUNCTION__, DeviceExtension));
    return TRUE;
}

STATIC BOOLEAN Interrput(IN PVOID DeviceExtension)
{
    KdPrint((DRIVER_NAMEA"[%04d][%s]: %p.\n",
        __LINE__, __FUNCTION__, DeviceExtension));
    return TRUE;
}

NTSTATUS DriverInitializeMiniScsi(IN PDRIVER_OBJECT DriverObject,
                                  IN PUNICODE_STRING RegistryPath,
                                  IN PMINISCSI_ADAPTER * ReturnAdapter)
{
    NTSTATUS Status;
    HW_INITIALIZATION_DATA HwInitData;

    CDBProcessInitialize();
    SRBProcessInitialize();

    KdPrint((DRIVER_NAMEA"[%04d][%s]: Virtual SCSI Driver Entry...\n",
        __LINE__, __FUNCTION__));

    RtlZeroMemory(&HwInitData, sizeof(HW_INITIALIZATION_DATA));
    HwInitData.HwInitializationDataSize = sizeof(HW_INITIALIZATION_DATA);
    HwInitData.AdapterInterfaceType = Isa;

    HwInitData.HwInitialize     =   Initialize;
    HwInitData.HwStartIo        =   StartIO;

    HwInitData.HwInterrupt      =   Interrput;
    HwInitData.HwFindAdapter    =   FindAdapter;

    HwInitData.HwResetBus       =   ResetBus; 
    HwInitData.HwDmaStarted     =   NULL;
    HwInitData.HwAdapterState   =   AdapterState;
    HwInitData.HwAdapterControl =   AdapterControl;

    HwInitData.DeviceExtensionSize      =   sizeof(MINISCSI_ADAPTER); 
    HwInitData.SpecificLuExtensionSize  =   sizeof(MINISCSI_LOGICAL);
    HwInitData.SrbExtensionSize         =   sizeof(MINISCSI_SRBEXTN); 
    HwInitData.NumberOfAccessRanges     =   0;
    HwInitData.MapBuffers               =   TRUE;
    HwInitData.NeedPhysicalAddresses    =   FALSE;
    HwInitData.TaggedQueuing            =   FALSE;
    HwInitData.AutoRequestSense         =   TRUE;
    HwInitData.MultipleRequestPerLu     =   TRUE;
    HwInitData.ReceiveEvent             =   FALSE;
    HwInitData.VendorIdLength           =   0;
    HwInitData.VendorId                 =   0;
    HwInitData.DeviceIdLength           =   0;
    HwInitData.DeviceId                 =   0;

    KdPrint((DRIVER_NAMEA"[%04d][%s]: Starting scsiport driver ....\n",
        __LINE__, __FUNCTION__));

    Status = ScsiPortInitialize(DriverObject, RegistryPath, &HwInitData, ReturnAdapter);

    KdPrint((DRIVER_NAMEA"[%04d][%s]: Storport initialize result 0x%08X.\n",
        __LINE__, __FUNCTION__, Status));

    return Status;

}


