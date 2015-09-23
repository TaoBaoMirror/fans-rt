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

#include <stdio.h>
#include <ntddk.h>
#include "common.h"
#include "vport.h"



PUBLIC PMINISCSI_LOGICAL AcquireLun(PMINISCSI_LOGICAL ScsiLun)
{
    KIRQL Flags = 0;
 
    MINISCSI_ASSERT(NULL != ScsiLun, return NULL);
    
    if (LOGICAL_MAGIC != ScsiLun->Magic)
    {
        KdPrint((DRIVER_NAMEA"[%04d][%s]: ***BUG:  SCSI lun %p invalid magic.\n",
              __LINE__, __FUNCTION__, ScsiLun));
        return NULL;
    }

    KeAcquireSpinLock(&ScsiLun->LunLock, &Flags);

    if (ScsiLun->Removed)
    {
        KeReleaseSpinLock(&ScsiLun->LunLock, Flags);
        KdPrint((DRIVER_NAMEA"[%04d][%s]: ***BUG:  SCSI lun %p is removed.\n",
              __LINE__, __FUNCTION__, ScsiLun));
        
        return NULL;
    }

    ScsiLun->RefCount ++;

    KeReleaseSpinLock(&ScsiLun->LunLock, Flags);

    return ScsiLun;
}

PUBLIC BOOLEAN ReleaseLun(PMINISCSI_LOGICAL ScsiLun)
{
    KIRQL Flags = 0;
    MINISCSI_ASSERT(NULL != ScsiLun, return TRUE);
    
    if (LOGICAL_MAGIC != ScsiLun->Magic)
    {
        KdPrint((DRIVER_NAMEA"[%04d][%s]: ***BUG:  SCSI lun %p invalid magic.\n",
              __LINE__, __FUNCTION__, ScsiLun));
        return TRUE;
    }

    KeAcquireSpinLock(&ScsiLun->LunLock, &Flags);

    if (ScsiLun->Removed)
    {
        KeReleaseSpinLock(&ScsiLun->LunLock, Flags);
        KdPrint((DRIVER_NAMEA"[%04d][%s]: ***BUG:  SCSI lun %p is removed.\n",
              __LINE__, __FUNCTION__, ScsiLun));
        
        return TRUE;
    }

    if (0 == (--ScsiLun->RefCount))
    {
        ScsiLun->Removed = TRUE;
        ScsiLun->Magic = 0;
    }

    KeReleaseSpinLock(&ScsiLun->LunLock, Flags);

    if (ScsiLun->Removed)
    {
        KdPrint((DRIVER_NAMEA"[%04d][%s]: Now remove this scsi lun.\n",
              __LINE__, __FUNCTION__, ScsiLun));
        ExFreePool(ScsiLun);
        return TRUE;
    }

    return FALSE;
}



PUBLIC PMINISCSI_LOGICAL AllocatScsiLun(IN PMINISCSI_ADAPTER Adapter,
                                         IN UCHAR TargetId,
                                         IN UCHAR LunId)
{
    PMINISCSI_LOGICAL ScsiLun;

    ScsiLun = ExAllocatePoolWithTag(PagedPool, sizeof(MINISCSI_LOGICAL), MINISCSI_MAGIC);

    if (NULL == ScsiLun)
    {
        KdPrint((DRIVER_NAMEA"[%04d][%s]: ***BUG:  No memory to create SCSI lun 00:%02X:%02X.\n",
                __LINE__, __FUNCTION__, TargetId, LunId));
        return NULL;
    }

    RtlZeroMemory(ScsiLun, sizeof(MINISCSI_LOGICAL));
    KeInitializeSpinLock(&ScsiLun->LunLock);
    ScsiLun->Magic     = LOGICAL_MAGIC;
    ScsiLun->RefCount  = 1;
    ScsiLun->Removed   = FALSE;
    ScsiLun->Allocated = FALSE;
    ScsiLun->Connected = FALSE;
    ScsiLun->ReadOnly  = FALSE;
    ScsiLun->TargetID  = TargetId;
    ScsiLun->LunID     = LunId;
    ScsiLun->DiskType  = DRIVE_UNKNOWN;
    ScsiLun->Adapter   = Adapter;

    _snprintf(ScsiLun->DiskName, MAX_NAME-1, "00:%02X:%02X", TargetId, LunId);

    return ScsiLun;
    
}

PUBLIC PMINISCSI_LOGICAL 
AcquireFreeScsiLun(IN PMINISCSI_ADAPTER ScsiAdapter)
{
    UCHAR TargetId, LunId;
    PMINISCSI_LOGICAL ScsiLun = NULL;

    for (TargetId = 0; TargetId < SCSI_MAXIMUM_TARGETS; TargetId ++)
    {
        for (LunId = 0; LunId < SCSI_MAXIMUM_LOGICAL_UNITS; LunId ++)
        {
            KIRQL Flags = 0;
            
            ScsiLun = AdapterGetLogicUnit(ScsiAdapter, TargetId, LunId);

            if (NULL == ScsiLun)
            {
                KdPrint((DRIVER_NAMEA"[%04d][%s]: ***BUG:  SCSI lun 00:%02X:%02X not found.\n",
                        __LINE__, __FUNCTION__, TargetId, LunId));
                continue;
            }

            KeAcquireSpinLock(&ScsiLun->LunLock, &Flags);
            
            if (!ScsiLun->Allocated)
            {
                ScsiLun->Allocated = TRUE;
                KeReleaseSpinLock(&ScsiLun->LunLock, Flags);
                return ScsiLun;
            }

            KeReleaseSpinLock(&ScsiLun->LunLock, Flags);

            AdapterPutLogicUnit(ScsiAdapter, TargetId, LunId);
        }
    }

    return NULL;

}

PUBLIC PMINISCSI_LOGICAL
SearchScsiLun(IN PMINISCSI_ADAPTER ScsiAdapter,
               IN CHAR * DiskName)
{
    UCHAR TargetId, LunId;
    PMINISCSI_LOGICAL ScsiLun = NULL;

    for (TargetId = 0; TargetId < SCSI_MAXIMUM_TARGETS; TargetId ++)
    {
        for (LunId = 0; LunId < SCSI_MAXIMUM_LOGICAL_UNITS; LunId ++)
        {
            ScsiLun = AdapterGetLogicUnit(ScsiAdapter, TargetId, LunId);

            if (NULL == ScsiLun)
            {
                KdPrint((DRIVER_NAMEA"[%04d][%s]: ***BUG:  SCSI lun 00:%02X:%02X not found.\n",
                        __LINE__, __FUNCTION__, TargetId, LunId));
                continue;
            }

            if (0 == strcmp(ScsiLun->DiskName, DiskName))
            {
                return ScsiLun;
            }

            AdapterPutLogicUnit(ScsiAdapter, TargetId, LunId);
        }
    }

    return NULL;
}

PUBLIC NTSTATUS CreateLun(IN PMINISCSI_ADAPTER Adapter,
                           IN CHAR * DiskName,
                           IN MSCSI_MEDIA_TYPE MediaType,
                           IN PVOID MediaDetails)
{
    KIRQL                   Flags   =   0;
    CHAR                    Type    =   DRIVE_UNKNOWN;
    PVOID                   Buffer  =   NULL;
    NTSTATUS                Status  =   STATUS_UNSUCCESSFUL;
    PMINISCSI_LOGICAL       ScsiLun =   NULL;

    KdPrint((DRIVER_NAMEA"[%04d][%s]: Create ramdisk '%s' for adapter is 0x%p.\n",
                    __LINE__, __FUNCTION__, DiskName, Adapter));

    ScsiLun = SearchScsiLun(Adapter, DiskName);

    if (NULL == ScsiLun)
    {
        ScsiLun = AcquireFreeScsiLun(Adapter);

        if (NULL == ScsiLun)
        {
            KdPrint((DRIVER_NAMEA"[%04d][%s]: Can not found any free SCSI LUN.\n",
                        __LINE__, __FUNCTION__));
            return STATUS_DEVICE_DOES_NOT_EXIST;
        }
    }
    else
    {
        KeAcquireSpinLock(&ScsiLun->LunLock, &Flags);

        if (ScsiLun->Connected)
        {
            KeReleaseSpinLock(&ScsiLun->LunLock, Flags);
            ReleaseLun(ScsiLun);

            KdPrint((DRIVER_NAMEA"[%04d][%s]: The lun '%s' already exist.\n",
                        __LINE__, __FUNCTION__, DiskName));
            
            return STATUS_DEVICE_ALREADY_ATTACHED;
        }

        KeReleaseSpinLock(&ScsiLun->LunLock, Flags);
    }

    Status = InitializeMedia(&ScsiLun->LunMedia, MediaType, MediaDetails);

    if (!NT_SUCCESS(Status))
    {
        ReleaseLun(ScsiLun);
        KdPrint((DRIVER_NAMEA"[%04d][%s]: Initialize lun '%s' media failed.\n",
                        __LINE__, __FUNCTION__, DiskName));
        return Status;
    }

    Type = MediaGetDiskType(&ScsiLun->LunMedia);

    KeAcquireSpinLock(&ScsiLun->LunLock, &Flags);

    SCSI_LUN_INITIALIZE(ScsiLun,  /* LUN Pointer*/
                        Adapter,  /* Adapter pointer */
                        Type,     /* Disk type */
                        (Type == DRIVE_CDROM ? TRUE : FALSE),   /* Is read only */
                        DiskName);
    
    KeReleaseSpinLock(&ScsiLun->LunLock, Flags);

    ScsiPortNotification(BusChangeDetected, Adapter, 0);

    return STATUS_SUCCESS;
}

