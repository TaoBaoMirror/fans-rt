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

DECLARE_SCSIMEDIA(File);
DECLARE_SCSIMEDIA(Ram);

STATIC PSCSIMEDIA_DESCRIPTOR g_MediaGroup[] = 
{
    POINTER_SCSIMEDIA(File),
    POINTER_SCSIMEDIA(Ram),
};

PUBLIC NTSTATUS InitializeMedia(IN OUT PMINISCSI_LUNMEDIA LunMedia,
                                IN MSCSI_MEDIA_TYPE MediaType,
                                IN PVOID Details)
{
    ULONG i;

    KeInitializeSpinLock(&LunMedia->MediaLock);
    
    LunMedia->Removed       =   FALSE;
    LunMedia->MediaType     =   MediaType;
    LunMedia->Capacity.QuadPart      =   (LONGLONG) 0;  
    LunMedia->BlockSize     =   DEFAULT_SECTOR_SIZE;
    LunMedia->BlockCount    =   0;
    LunMedia->RefCount      =   1;
    LunMedia->DataBuffer    =   NULL;
    LunMedia->Descriptor    =   NULL;

    RtlZeroMemory(&LunMedia->MediaUnicode, sizeof(LunMedia->MediaUnicode));
    RtlZeroMemory(LunMedia->MediaString, sizeof(LunMedia->MediaString));

    for (i = 0; i < SIZEOF_ARRAY(g_MediaGroup); i ++)
    {
        if (NULL == g_MediaGroup[i])
        {
            continue;
        }

        KdPrint((DRIVER_NAMEA"[%04d][%s]: Found media %s, type %u ...\n",
                        __LINE__, __FUNCTION__,
                        g_MediaGroup[i]->MediaName,
                        g_MediaGroup[i]->MediaType));
        
        if (g_MediaGroup[i]->MediaType == MediaType)
        {
            LunMedia->Descriptor = g_MediaGroup[i];
            return g_MediaGroup[i]->MediaInitialize(LunMedia, MediaType, Details);
        }
    }

    KdPrint((DRIVER_NAMEA"[%04d][%s]: The type (%u) of the media not found.\n",
                        __LINE__, __FUNCTION__, MediaType));
    
    return STATUS_UNSUCCESSFUL;
}

PUBLIC PMINISCSI_LUNMEDIA AcquireMedia(PMINISCSI_LUNMEDIA LunMedia)
{
    KIRQL Flags = 0;
    
    MINISCSI_ASSERT(NULL != LunMedia, return NULL);

    KeAcquireSpinLock(&LunMedia->MediaLock, &Flags);

    if (LunMedia->Removed)
    {
        KeReleaseSpinLock(&LunMedia->MediaLock, Flags);
        return NULL;
    }

    LunMedia->RefCount ++;

    KeReleaseSpinLock(&LunMedia->MediaLock, Flags);

    return LunMedia;
}

PUBLIC BOOLEAN ReleaseMedia(PMINISCSI_LUNMEDIA LunMedia)
{
    KIRQL Flags = 0;
    
    MINISCSI_ASSERT(NULL != LunMedia, return TRUE);

    KeAcquireSpinLock(&LunMedia->MediaLock, &Flags);

    if (LunMedia->Removed)
    {
        KeReleaseSpinLock(&LunMedia->MediaLock, Flags);
        return TRUE;
    }

    if (0 == (--LunMedia->RefCount))
    {
        LunMedia->Removed = TRUE;
    }

    KeReleaseSpinLock(&LunMedia->MediaLock, Flags);

    return LunMedia->Removed;
}



PUBLIC CHAR MediaGetDiskType(PMINISCSI_LUNMEDIA LunMedia)
{
    WCHAR * Suffix = NULL;

    LunMedia = AcquireMedia(LunMedia);

    if (NULL == LunMedia)
    {
        KdPrint((DRIVER_NAMEA"[%04d][%s]: Invalid lun media %p.\n",
                        __LINE__, __FUNCTION__, LunMedia));
        return DRIVE_UNKNOWN;
    }

    Suffix = wcsrchr(LunMedia->MediaString, '.');

    if (NULL == Suffix)
    {
        ReleaseMedia(LunMedia);
        return DRIVE_RAMDISK;
    }
    
    if (0 == wcscmp(Suffix, L".ISO"))
    {
        ReleaseMedia(LunMedia);
        return DRIVE_CDROM;
    }
    
    ReleaseMedia(LunMedia);

    return DRIVE_REMOVABLE;
}


PUBLIC UCHAR MediaReadWrite(IN PMINISCSI_LUNMEDIA Media,
                              IN OUT PVOID Buffer,
                              IN ULONG LBA,
                              IN ULONG Count,
                              IN SCSI_OP_E ReadWrite)
{
    UCHAR Status = SRB_STATUS_ERROR;
    ULONG Length = 0;
    LONGLONG Offset = (LONGLONG) 0;
    PMINISCSI_LUNMEDIA LunMedia;
    PSCSIMEDIA_DESCRIPTOR Descriptor;
    
    LunMedia = AcquireMedia(Media);

    if (NULL == LunMedia)
    {
        KdPrint((DRIVER_NAMEA"[%04d][%s]: Invalid lun media %p.\n",
                 __LINE__, __FUNCTION__, Media));
        return SRB_STATUS_INVALID_LUN;
    }
    
    Descriptor = LunMedia->Descriptor;
    
    MINISCSI_ASSERT(0 != LunMedia->BlockSize,
        ReleaseMedia(LunMedia); return SRB_STATUS_INVALID_LUN);
    MINISCSI_ASSERT(NULL != LunMedia->Descriptor,
        ReleaseMedia(LunMedia); return SRB_STATUS_INVALID_LUN);
    MINISCSI_ASSERT(NULL != Descriptor->MediaRead,
        ReleaseMedia(LunMedia); return SRB_STATUS_INVALID_LUN);
    MINISCSI_ASSERT(NULL != Descriptor->MediaWrite,
        ReleaseMedia(LunMedia); return SRB_STATUS_INVALID_LUN);

    if ((((ULONGLONG)LBA / LunMedia->BlockSize) + Count) > (ULONGLONG)LunMedia->BlockCount)
    {
        KdPrint((DRIVER_NAMEA"[%04d][%s]: Read sectors(%u) out of range.\n",
                 __LINE__, __FUNCTION__, Count));
        ReleaseMedia(LunMedia);
        
        return SRB_STATUS_DATA_OVERRUN;
    }

    Offset = ((LONGLONG)LunMedia->BlockSize) * LBA;
    Length = LunMedia->BlockSize * Count;

    if (OP_MEDIA_READ == ReadWrite)
    {
        Status = Descriptor->MediaRead(Media, Buffer, Offset, Length);
    }
    else
    {
        Status = Descriptor->MediaWrite(Media, Buffer, Offset, Length);
    }

    ReleaseMedia(LunMedia);
    
    return Status;
}


