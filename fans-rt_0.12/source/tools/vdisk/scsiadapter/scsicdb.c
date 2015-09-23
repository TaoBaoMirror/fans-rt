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

typedef UCHAR (* CDB_PROCESSES)(IN PMINISCSI_ADAPTER Adapter, IN PSCSI_REQUEST_BLOCK Srb);

#pragma SECTION_DATA_BASE
STATIC CDB_PROCESSES g_fnProcessesCdbRequest[256];

#pragma SECTION_CODE_BASE

STATIC UCHAR ExecuteCDBInquiry(IN PMINISCSI_ADAPTER Adapter,
                                  IN PSCSI_REQUEST_BLOCK Srb)
{
    PMINISCSI_LOGICAL ScsiLun;
    PINQUIRYDATA InquiryData = (PINQUIRYDATA)Srb->DataBuffer;

    KdPrint((DRIVER_NAMEA"[%04d][%s]: Inquiry SCSI %02X:%02X:%02X. Adapter is %p\n",
        __LINE__, __FUNCTION__, Srb->PathId, Srb->TargetId, Srb->Lun, Adapter));

    ScsiLun = AdapterGetLogicUnit(Adapter, Srb->TargetId, Srb->Lun);

    if (NULL == ScsiLun)
    {
        KdPrint((DRIVER_NAMEA"[%04d][%s]: SCSI %02X:%02X:%02X object not found.\n",
            __LINE__, __FUNCTION__, Srb->PathId, Srb->TargetId, Srb->Lun));
        return SRB_STATUS_INVALID_LUN;
    }

    if (!SCSI_LUN_CONNECTED(ScsiLun))
    {
        KdPrint((DRIVER_NAMEA"[%04d][%s]: SCSI %02X:%02X:%02X not connected.\n",
            __LINE__, __FUNCTION__, Srb->PathId, Srb->TargetId, Srb->Lun));
        AdapterPutLogicUnit(Adapter, Srb->TargetId, Srb->Lun);
        return SRB_STATUS_INVALID_LUN;
    }

    InquiryData->DeviceType = SCSI_LUN_ISREADONLY(ScsiLun)
                            ? READ_ONLY_DIRECT_ACCESS_DEVICE
                            : DIRECT_ACCESS_DEVICE;
    InquiryData->DeviceTypeModifier = SCSI_LUN_TYPE(ScsiLun);
    InquiryData->DeviceTypeQualifier= SCSI_LUN_CONNECTED(ScsiLun)
                                    ? DEVICE_CONNECTED
                                    : DEVICE_QUALIFIER_NOT_ACTIVE;
    InquiryData->RemovableMedia = TRUE;
    InquiryData->Versions = 0;
    InquiryData->ResponseDataFormat = 2;
    InquiryData->HiSupport = 0;
    InquiryData->MultiPort = 0;
//    InquiryData->MediumChanger = TRUE;
    InquiryData->AdditionalLength = sizeof(INQUIRYDATA) - 5;

//  RtlMoveMemory(InquiryData->VendorId, MINISCSI_VENDOR_ID, 6);
//  RtlMoveMemory(InquiryData->ProductId, MINISCSI_DEVICE_ID, 16);
//  RtlMoveMemory(InquiryData->ProductRevisionLevel, "1010", 4);

    AdapterPutLogicUnit(Adapter, Srb->TargetId, Srb->Lun);

    KdPrint((DRIVER_NAMEA"[%04d][%s]: Inquiry SCSI %02X:%02X:%02X successfully, LUN is %p\n",
        __LINE__, __FUNCTION__, Srb->PathId, Srb->TargetId, Srb->Lun, ScsiLun));

    return SRB_STATUS_SUCCESS;

}


STATIC UCHAR ExecuteCDBReadCaps(IN PMINISCSI_ADAPTER Adapter,
                                    IN PSCSI_REQUEST_BLOCK Srb)
{
    ULONG Capacity, BlockSize;
    PMINISCSI_LOGICAL ScsiLun;
    PREAD_CAPACITY_DATA CapacityData = (PREAD_CAPACITY_DATA)Srb->DataBuffer;

    KdPrint((DRIVER_NAMEA"[%04d][%s]: Readcaps SCSI %02X:%02X:%02X \n",
        __LINE__, __FUNCTION__, Srb->PathId, Srb->TargetId, Srb->Lun));

    ScsiLun = AdapterGetLogicUnit(Adapter, Srb->TargetId, Srb->Lun);

    if (NULL == ScsiLun)
    {
        KdPrint((DRIVER_NAMEA"[%04d][%s]: SCSI %02X:%02X:%02X object not found.\n",
            __LINE__, __FUNCTION__, Srb->PathId, Srb->TargetId, Srb->Lun));
        return SRB_STATUS_INVALID_LUN;
    }
    
    BlockSize = (ULONG) SCSI_LUN_BLOCKSIZE(ScsiLun);
    
    if (0 == BlockSize)
    {
        AdapterPutLogicUnit(Adapter, Srb->TargetId, Srb->Lun);
        KdPrint((DRIVER_NAMEA"[%04d][%s]: SCSI %02X:%02X:%02X invalid block size.\n",
            __LINE__, __FUNCTION__, Srb->PathId, Srb->TargetId, Srb->Lun));

        return SRB_STATUS_INVALID_LUN;
    }

    Capacity = (ULONG)(SCSI_LUN_CAPACITY(ScsiLun) / BlockSize);

    KdPrint((DRIVER_NAMEA"[%04d][%s]: SCSI %02X:%02X:%02X => Capacity : %u,  BlockSize : %u.\n",
            __LINE__, __FUNCTION__, Srb->PathId, Srb->TargetId, Srb->Lun, Capacity, BlockSize));

    REVERSE_LONG(&Capacity);
    REVERSE_LONG(&BlockSize);

    KdPrint((DRIVER_NAMEA"[%04d][%s]: SCSI %02X:%02X:%02X => Capacity : %u,  BlockSize : %u.\n",
            __LINE__, __FUNCTION__, Srb->PathId, Srb->TargetId, Srb->Lun, Capacity, BlockSize));

    CapacityData->LogicalBlockAddress = Capacity;
    CapacityData->BytesPerBlock = BlockSize;

    AdapterPutLogicUnit(Adapter, Srb->TargetId, Srb->Lun);

    return SRB_STATUS_SUCCESS;
}

#define     SHIFT(b, shift)             (((ULONG)(b)) << (shift))

#define     MAKE_ADDRESS(b0, b1, b2, b3)                                    \
            (SHIFT(b0, 0) + SHIFT(b1, 8) + SHIFT(b2, 16) + SHIFT(b3, 24))
#define     MAKE_CDB_LBA(Cdb)                                               \
            MAKE_ADDRESS(Cdb.LogicalBlockByte0, Cdb.LogicalBlockByte1, Cdb.LogicalBlockByte2, Cdb.LogicalBlockByte3)
#define     MAKE_CDB_CNT(Cdb)                                               \
            (SHIFT(Cdb.TransferBlocksMsb, 0) + SHIFT(Cdb.TransferBlocksLsb, 0))

STATIC UCHAR ExecuteCDBReadWrite(IN PMINISCSI_ADAPTER Adapter,
                                    IN PSCSI_REQUEST_BLOCK Srb, 
                                    IN SCSI_OP_E ReadWrite)
{
    UCHAR Status;
    PMINISCSI_LOGICAL ScsiLun;
    PCDB Cdb = (PCDB) Srb->Cdb;
    ULONG LBA = MAKE_CDB_LBA(Cdb->CDB10);
    ULONG Count = MAKE_CDB_CNT(Cdb->CDB10);

    KdPrint((DRIVER_NAMEA"[%04d][%s]: %s SCSI %02X:%02X:%02X => LBA  %u, Count %u.\n",
        __LINE__, __FUNCTION__,
        ReadWrite == OP_MEDIA_READ ? "Read" : "Write",
        Srb->PathId, Srb->TargetId, Srb->Lun, LBA, Count));

    ScsiLun = AdapterGetLogicUnit(Adapter, Srb->TargetId, Srb->Lun);

    if (NULL == ScsiLun)
    {
        KdPrint((DRIVER_NAMEA"[%04d][%s]: Read/Write SCSI %02X:%02X:%02X invalid lun.\n",
                 __LINE__, __FUNCTION__,  Srb->PathId, Srb->TargetId, Srb->Lun));
        return SRB_STATUS_INVALID_LUN;
    }

    Status = MediaReadWrite(SCSI_LUN_MEDIA(ScsiLun), Srb->DataBuffer, LBA, Count, ReadWrite);

    AdapterPutLogicUnit(Adapter, Srb->TargetId, Srb->Lun);

    return Status;
}

STATIC UCHAR ExecuteCDBRead(IN PMINISCSI_ADAPTER Adapter,
                                    IN PSCSI_REQUEST_BLOCK Srb)
{
    return ExecuteCDBReadWrite(Adapter, Srb, OP_MEDIA_READ);
}

STATIC UCHAR ExecuteCDBWrite(IN PMINISCSI_ADAPTER Adapter,
                                    IN PSCSI_REQUEST_BLOCK Srb)
{
    return ExecuteCDBReadWrite(Adapter, Srb, OP_MEDIA_WRITE);
}


#pragma SECTION_CODE_BASE
UCHAR ExecuteScsiCDB(IN PMINISCSI_ADAPTER Adapter,
                     IN PSCSI_REQUEST_BLOCK Srb)
{
    CDB_PROCESSES fnProcess;
    PCDB Cdb = (PCDB) Srb->Cdb;
    KdPrint((DRIVER_NAMEA"[%04d][%s]: %p  CDB[0] is 0x%02x.\n",
        __LINE__, __FUNCTION__, Srb, Srb->Cdb[0] & 0xff));

    fnProcess = g_fnProcessesCdbRequest[Srb->Cdb[0] & 0xff];

    if (NULL != fnProcess)
    {
        return fnProcess(Adapter, Srb);
    }

    return SRB_STATUS_BAD_FUNCTION;
}

#pragma SECTION_CODE_INIT
VOID CDBProcessInitialize(VOID)
{
    RtlZeroMemory(g_fnProcessesCdbRequest, sizeof(g_fnProcessesCdbRequest));

    g_fnProcessesCdbRequest[SCSIOP_INQUIRY]         =   ExecuteCDBInquiry;
    g_fnProcessesCdbRequest[SCSIOP_READ]            =   ExecuteCDBRead;
    g_fnProcessesCdbRequest[SCSIOP_WRITE]           =   ExecuteCDBWrite;
    g_fnProcessesCdbRequest[SCSIOP_READ_CAPACITY]   =   ExecuteCDBReadCaps;
}

