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
#ifndef __VDISK_COMMON_H
#define __VDISK_COMMON_H

#ifndef     STATIC
#define     STATIC      static
#endif

#ifndef     PUBLIC
#define     PUBLIC
#endif

#ifndef     EXTERN
#define     EXTERN      extern
#endif

#ifdef      _NTDDK_
#define     _TEXT(x)    L##x
#endif

#ifndef     MAX_PATH
#define     MAX_PATH    260
#endif

#ifndef     MAX_NAME
#define     MAX_NAME    16
#endif

#define     DRIVER_NAMEA                        "SCSIAdapter"
#define     DRIVER_NAMEW                        L"SCSIAdapter"

#define     DEVICE_NAME_CONTROLA                "\\Device\\SCSIControl"
#define     DEVICE_NAME_CONTROLW                L"\\Device\\SCSIControl"

#define     DEFAULT_LUNA                        "LUN0"

#ifdef      _NTDDK_
#define     DEVICE_LINK_CONTROLA                "\\??\\SCSIControl"
#define     DEVICE_LINK_CONTROLW                L"\\??\\SCSIControl"
#else
#define     DEVICE_LINK_CONTROLA                "\\\\.\\SCSIControl"
#define     DEVICE_LINK_CONTROLW                L"\\\\.\\SCSIControl"
#endif

#ifdef      _NTDDK_
#define     DRIVER_NAME                         DRIVER_NAMEW
#define     DEVICE_LINK_CONTROL                 DEVICE_NAME_CONTROLW
#define     DEVICE_NAME_CONTROL                 DEVICE_NAME_CONTROLW
#else
#define     DRIVER_NAME                         DRIVER_NAMEA
#define     DEVICE_LINK_CONTROL                 DEVICE_NAME_CONTROLA
#define     DEVICE_NAME_CONTROL                 DEVICE_NAME_CONTROLA
#endif


#define     FILE_DEVICE_VSCSI                   0x8000
#define     IOCTL_VSCSI_LOAD_IMAGE              CTL_CODE(FILE_DEVICE_VSCSI, 0x100, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define     IOCTL_VSCSI_UNLOAD_IMAGE            CTL_CODE(FILE_DEVICE_VSCSI, 0x101, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

typedef union tagVSCSI_IO_REQUEST_PACKET{
    struct _load{
        SIZE_T      Length;
        SIZE_T      DiskNameLength;
        CHAR        DiskName[MAX_NAME];
        SIZE_T      FileNameLength;
        CHAR        FileName[MAX_PATH];
        SIZE_T      DiskCapacity;
    }LoadImage;
    struct _uload{
        SIZE_T      Length;
        SIZE_T      DiskNameLength;
        CHAR        DiskName[MAX_NAME];
    }UnloadImage;
}VSCSI_IO_REQUEST_PACKET, * PVSCSI_IO_REQUEST_PACKET;


#endif