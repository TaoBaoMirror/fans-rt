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


STATIC UCHAR MediaRead(PMINISCSI_LUNMEDIA Media, PVOID Buffer, LONGLONG Offset, ULONG ReadBytes)
{
    return SRB_STATUS_SUCCESS;
}

STATIC UCHAR MediaWrite(PMINISCSI_LUNMEDIA Media, PVOID Buffer, LONGLONG Offset, ULONG WriteBytes)
{
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
    return STATUS_UNSUCCESSFUL;
}


DEFINE_SCSIMEDIA(File, MSCSI_MEDIA_FILE, MediaInitialize, MediaRead, MediaWrite, MediaRemove);


