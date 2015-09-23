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


STATIC UCHAR MediaRead(PMINISCSI_LUNMEDIA LunMedia, PVOID Buffer, LONGLONG Offset, ULONG ReadBytes)
{
    MINISCSI_ASSERT(NULL != LunMedia, return SRB_STATUS_INVALID_LUN);
    MINISCSI_ASSERT(0 != LunMedia->BlockSize, return SRB_STATUS_INVALID_LUN);
    
    if (Offset + ReadBytes > LunMedia->Capacity.QuadPart)
    {
        KdPrint((DRIVER_NAMEA"[%04d][%s]: Read media %p data overrun.\n",
                 __LINE__, __FUNCTION__, LunMedia));
        return SRB_STATUS_DATA_OVERRUN;
    }

    if (0 != (ReadBytes%LunMedia->BlockSize))
    {
        KdPrint((DRIVER_NAMEA"[%04d][%s]: Read media %p, bad data buffer length.\n",
                 __LINE__, __FUNCTION__, LunMedia));

        return SRB_STATUS_BAD_SRB_BLOCK_LENGTH;
    }

    RtlMoveMemory(LunMedia->DataBuffer + Offset, Buffer, ReadBytes);

    return SRB_STATUS_SUCCESS;
}

STATIC UCHAR MediaWrite(PMINISCSI_LUNMEDIA LunMedia, PVOID Buffer, LONGLONG Offset, ULONG WriteBytes)
{
    MINISCSI_ASSERT(NULL != LunMedia, return SRB_STATUS_INVALID_LUN);
    MINISCSI_ASSERT(0 != LunMedia->BlockSize, return SRB_STATUS_INVALID_LUN);
    
    if (Offset + WriteBytes > LunMedia->Capacity.QuadPart)
    {
        KdPrint((DRIVER_NAMEA"[%04d][%s]: Read media %p data overrun.\n",
                 __LINE__, __FUNCTION__, LunMedia));
        return SRB_STATUS_DATA_OVERRUN;
    }

    if (0 != (WriteBytes%LunMedia->BlockSize))
    {
        KdPrint((DRIVER_NAMEA"[%04d][%s]: Read media %p, bad data buffer length.\n",
                 __LINE__, __FUNCTION__, LunMedia));

        return SRB_STATUS_BAD_SRB_BLOCK_LENGTH;
    }

    RtlMoveMemory(Buffer, LunMedia->DataBuffer + Offset, WriteBytes);

    return SRB_STATUS_SUCCESS;

}

STATIC NTSTATUS MediaRemove(PVOID Media)
{
    return STATUS_SUCCESS;
}

STATIC NTSTATUS MediaInitialize(IN OUT PMINISCSI_LUNMEDIA LunMedia,
                                IN MSCSI_MEDIA_TYPE MediaType,
                                IN PVOID Details)
{
    SIZE_T * DiskCapacity =  Details;

    KdPrint((DRIVER_NAMEA"[%04d][%s]: Allocate memory %u bytes ...\n",
                            __LINE__, __FUNCTION__, *DiskCapacity));

    LunMedia->Capacity.QuadPart = *DiskCapacity;
    LunMedia->BlockSize = DEFAULT_SECTOR_SIZE;
    LunMedia->BlockCount = (ULONG) (LunMedia->Capacity.QuadPart / DEFAULT_SECTOR_SIZE);
    LunMedia->DataBuffer = ExAllocatePoolWithTag(PagedPool, (SIZE_T) *DiskCapacity, MINISCSI_MAGIC);

    if (NULL == LunMedia->DataBuffer)
    {
        KdPrint((DRIVER_NAMEA"[%04d][%s]: Allocate memory %u failed.\n",
                        __LINE__, __FUNCTION__, *DiskCapacity));
        return STATUS_UNSUCCESSFUL;
    }

    wcscpy(LunMedia->MediaString, L"RAM");

    RtlInitUnicodeString(&LunMedia->MediaUnicode, LunMedia->MediaString);
    
    return STATUS_SUCCESS;
}


DEFINE_SCSIMEDIA(Ram, MSCSI_MEDIA_RAM, MediaInitialize, MediaRead, MediaWrite, MediaRemove);


