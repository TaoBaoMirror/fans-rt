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

#ifndef __RT_VIRTUAL_PORT_H
#define __RT_VIRTUAL_PORT_H

#include <ntddk.h>
#include <ntddscsi.h>
#include <srb.h>
#include <scsi.h>

#define     SECTION_CODE_BASE                       code_seg()
#define     SECTION_CODE_PAGE                       code_seg("PAGE")
#define     SECTION_CODE_INIT                       code_seg("INIT")

#define     SECTION_DATA_BASE                       data_seg()
#define     SECTION_DATA_PAGE                       data_seg("PAGE")
#define     SECTION_DATA_INIT                       data_seg("INIT")


#if DBG
#define     MINISCSI_ASSERT(Condition, execute)                                     \
            do {                                                                    \
                if (!(Condition)){                                                  \
                    KdPrint(("[%04d][%s]: Check %s failed.\n",                      \
                            __LINE__, __FUNCTION__, #Condition));                   \
                    execute;                                                        \
                }                                                                   \
            } while(0)
#else
#define     MINISCSI_ASSERT(Condition, execute)
#endif



#define     MINISCSI_VENDOR_ID  "RTSCSI"
#define     MINISCSI_DEVICE_ID  "Virtual SCSI"
#define     DEFAULT_SECTOR_SIZE 512      //ÉÈÇø´óÐ¡
#define     MAXIMUM_DISK_SIZE   (32 * 1024 * 1024)
#define     MINISCSI_MAGIC      'SCSI'

typedef struct tagCONTROL_EXTENSION CONTROL_EXTENSION, * PCONTROL_EXTENSION;
typedef struct tagMINISCSI_ADAPTER MINISCSI_ADAPTER, * PMINISCSI_ADAPTER;
typedef struct tagMINISCSI_LOGICAL MINISCSI_LOGICAL, * PMINISCSI_LOGICAL;
typedef struct tagMINISCSI_SRBEXTN MINISCSI_SRBEXTN, * PMINISCSI_SRBEXTN;
typedef struct tagMINISCSI_LUNMEDIA MINISCSI_LUNMEDIA, * PMINISCSI_LUNMEDIA;
typedef struct tagSCSIMEDIA_DESCRIPTOR SCSIMEDIA_DESCRIPTOR, * PSCSIMEDIA_DESCRIPTOR;


#define     CTRL_MAGIC          'CTRL'
struct tagCONTROL_EXTENSION{
    ULONG                       Magic;
    UNICODE_STRING              DeviceName;
    UNICODE_STRING              SymbolName;
    PDEVICE_OBJECT              ControlDevice;
    PDRIVER_OBJECT              ControlDriver;
    PMINISCSI_ADAPTER           ScsiAdapter;
    HANDLE                      ThreadHandle;
    KEVENT                      RequestEvent;
    BOOLEAN                     TerminateThread;
    BOOLEAN                     FinishedThread;
    KSPIN_LOCK                  IrpSpinLock;
    LIST_ENTRY                  IrpListHead;
};


#define     ADAPTER_MAGIC       'ADPR'
struct tagMINISCSI_ADAPTER{
    ULONG                       Magic;
    UNICODE_STRING              DeviceName;
    UNICODE_STRING              SymbolName;
    KSPIN_LOCK                  AdapterLock;
    struct {
        KSPIN_LOCK              SrbListLock;
        LIST_ENTRY              SrbListHead;
    }Pending;
    struct {
        KSPIN_LOCK              SrbListLock;
        LIST_ENTRY              SrbListHead;
    }Finished;
    HANDLE                      AdapterThread;
    KEVENT                      RequestEvent;
    BOOLEAN                     FinishedThread;
    BOOLEAN                     TerminateThread;
    PCONTROL_EXTENSION          ControlDevice;
    PMINISCSI_LOGICAL           ScsiLunArray[SCSI_MAXIMUM_TARGETS][SCSI_MAXIMUM_LOGICAL_UNITS];
};

#define     ADAPTER_LUN(Ad, t, l)       ((Ad)->ScsiLunArray[t][l])

typedef enum{
    MSCSI_MEDIA_NULL,
    MSCSI_MEDIA_RAM,
    MSCSI_MEDIA_FILE,
    MSCSI_MEDIA_NET,
}MSCSI_MEDIA_TYPE;

typedef NTSTATUS (*MEDIAINITIALIZE)(IN OUT PMINISCSI_LUNMEDIA LunMedia,
                                IN MSCSI_MEDIA_TYPE MediaType,
                                IN PVOID Details);
typedef UCHAR (* MEDIAREAD)(IN PMINISCSI_LUNMEDIA LunMedia, 
                              IN OUT PVOID Buffer,
                              IN LONGLONG Offset,
                              IN ULONG ReadBytes);
typedef UCHAR (* MEDIAWRITE)(IN PMINISCSI_LUNMEDIA LunMedia,
                               IN PVOID Buffer,
                               IN LONGLONG Offset,
                               IN ULONG WriteBytes);
typedef NTSTATUS (* MEDIAREMOVE)(IN PVOID Media);

typedef enum{
    OP_MEDIA_NULL   =   0,
    OP_MEDIA_READ   =   1,
    OP_MEDIA_WRITE  =   2,
}SCSI_OP_E;

struct tagMINISCSI_LUNMEDIA{
    BOOLEAN                 Removed;    /* This is removed mark - initialize for manager */
    MSCSI_MEDIA_TYPE        MediaType;  /* Media type - setting for media manager */
    LARGE_INTEGER           Capacity;   /* The size of this media - setting for instances */
    ULONG                   BlockSize;  /* The size of every sector - setting for instances*/
    ULONG                   BlockCount; /* How many sectors - setting for instances*/
    KSPIN_LOCK              MediaLock;  /* Lock media reference - initialize for manager */
    ULONG                   RefCount;   /* Reference count - initialize for manager */
    union{
        PCHAR               DataBuffer; /* The data buffer for ram disk - setting for instances */
        HANDLE              FileHandle; /* The file handle for file disk - setting for instances */
    };
    PSCSIMEDIA_DESCRIPTOR   Descriptor; /* The media descriptor - setting for manager */
    UNICODE_STRING          MediaUnicode; /* The media describe string - setting for instances */
    WCHAR                   MediaString[MAX_PATH]; /* The media describe buffer - setting for instances */
};

#define SCSI_MEDIA_CAPACITY(Media)          ((Media)->Capacity.QuadPart)
#define SCSI_MEDIA_BLOCKCOUNT(Media)        ((Media)->BlockCount)
#define SCSI_MEDIA_BLOCKSIZE(Media)        ((Media)->BlockSize)


#define POINTER_SCSIMEDIA(Name)                                                     \
        &g_Media##Name
#define DECLARE_SCSIMEDIA(Name)                                                     \
        EXTERN SCSIMEDIA_DESCRIPTOR g_Media##Name
#define DEFINE_SCSIMEDIA(Name, Type, Init, Read, Write, Remove)                     \
        PUBLIC SCSIMEDIA_DESCRIPTOR g_Media##Name = {                               \
            #Type,                                                                  \
            Type,                                                                   \
            Init,                                                                   \
            Read,                                                                   \
            Write,                                                                  \
            Remove,                                                                 \
        }

struct tagSCSIMEDIA_DESCRIPTOR{
    CONST CHAR *            MediaName;
    MSCSI_MEDIA_TYPE        MediaType;
    MEDIAINITIALIZE         MediaInitialize;
    MEDIAREAD               MediaRead;
    MEDIAWRITE              MediaWrite;
    MEDIAREMOVE             MediaRemove;
};

#define DRIVE_UNKNOWN     0
#define DRIVE_NO_ROOT_DIR 1
#define DRIVE_REMOVABLE   2
#define DRIVE_FIXED       3
#define DRIVE_REMOTE      4
#define DRIVE_CDROM       5
#define DRIVE_RAMDISK     6


#define     LOGICAL_MAGIC       'LUN\0'
struct tagMINISCSI_LOGICAL{
    ULONG                   Magic;
    ULONG                   RefCount;               /* Reference count */
    KSPIN_LOCK              LunLock;                /* Lock the lun */
    BOOLEAN                 Removed;
    BOOLEAN                 Allocated;
    BOOLEAN                 Connected;              /* Used for install or unstall image */
    BOOLEAN                 ReadOnly;               /* Is read only */
    UCHAR                   TargetID;               /* The target id */
    UCHAR                   LunID;                  /* The LUN id */
    CHAR                    DiskType;               /* CD-ROM or Floppy or HardDisk */
    PMINISCSI_ADAPTER       Adapter;                /* Pointer to scsi addapter */
    MINISCSI_LUNMEDIA       LunMedia;               /* The lun media */
    CHAR                    DiskName[MAX_NAME];     /* Allow the application to identify this disk */
};

#define     SCSI_LUN_TYPE(lun)          ((lun)->DiskType)
#define     SCSI_LUN_CAPACITY(lun)      SCSI_MEDIA_CAPACITY(&(lun)->LunMedia)
#define     SCSI_LUN_BLOCKSIZE(lun)     SCSI_MEDIA_BLOCKSIZE(&(lun)->LunMedia)
#define     SCSI_LUN_BLOCKCOUNT(lun)    SCSI_MEDIA_BLOCKCOUNT(&(lun)->LunMedia)
#define     SCSI_LUN_ISREADONLY(lun)    ((lun)->ReadOnly)
#define     SCSI_LUN_CONNECTED(lun)     ((lun)->Connected)
#define     SCSI_LUN_MEDIA(lun)         (&(lun)->LunMedia)



#define     SCSI_LUN_INITIALIZE(Lun, Adp, Type, RO, Name)                           \
            do{                                                                     \
                (Lun)->Magic      =   LOGICAL_MAGIC;                                \
                (Lun)->RefCount   =   1;                                            \
                (Lun)->Removed    =   FALSE;                                        \
                (Lun)->Allocated  =   TRUE;                                         \
                (Lun)->Connected  =   TRUE;                                         \
                (Lun)->ReadOnly   =   RO;                                           \
                (Lun)->DiskType   =   Type;                                         \
                (Lun)->Adapter    =   Adp;                                          \
                strcpy((Lun)->DiskName, Name);                                      \
            }while(0)

#define     SRBEXTN_MAGIC       'SRB\0'
struct tagMINISCSI_SRBEXTN{
    ULONG                       Magic;
    LIST_ENTRY                  SrbList;
    PSCSI_REQUEST_BLOCK         SrbThis;
};


#define     SIZEOF_ARRAY(x)         (sizeof(x)/sizeof(x[0]))

#ifdef __cplusplus
    extern 'C' {
#endif
    VOID CDBProcessInitialize(VOID);
    VOID SRBProcessInitialize(VOID);
    PDEVICE_OBJECT DriverGetControlDevice(VOID);
    PMINISCSI_ADAPTER DriverGetSCSIAdapter(VOID);
    PCONTROL_EXTENSION DriverGetControlExtension(VOID);
    NTSTATUS DriverCreateControlDevice(IN PDRIVER_OBJECT DriverObject,
                                       IN OUT PMINISCSI_ADAPTER Adapter);
    NTSTATUS DriverInitializeMiniScsi(IN PDRIVER_OBJECT DriverObject,
                                      IN PUNICODE_STRING RegistryPath,
                                      IN PMINISCSI_ADAPTER * ReturnAdapter);
    UCHAR ExecuteScsiCDB(IN PMINISCSI_ADAPTER Adapter,
                         IN PSCSI_REQUEST_BLOCK Srb);
    PMINISCSI_LOGICAL SearchScsiLun(IN PMINISCSI_ADAPTER Adapter,
                                    IN CHAR * DiskName);
    NTSTATUS CreateLun(IN PMINISCSI_ADAPTER Adapter,
                       IN CHAR * DiskName,
                       IN MSCSI_MEDIA_TYPE MediaType,
                       IN PVOID MediaDetails);
    BOOLEAN AdapterPutLogicUnit(
                       IN PMINISCSI_ADAPTER ScsiAdapter,
                       IN UCHAR TargetID,
                       IN UCHAR LunID);
    PMINISCSI_LOGICAL AdapterGetLogicUnit(
                       IN PMINISCSI_ADAPTER ScsiAdapter,
                       IN UCHAR TargetID,
                       IN UCHAR LunID);
    PMINISCSI_LOGICAL AcquireLun(IN OUT PMINISCSI_LOGICAL ScsiLun);
    BOOLEAN ReleaseLun(IN OUT PMINISCSI_LOGICAL ScsiLun);
    PMINISCSI_LOGICAL AllocatScsiLun(IN PMINISCSI_ADAPTER Adapter, 
                                     IN UCHAR TargetId,
                                     IN UCHAR LunId);
    PMINISCSI_LOGICAL AcquireFreeScsiLun(PMINISCSI_ADAPTER IN Adapter);
    NTSTATUS InitializeMedia(IN OUT PMINISCSI_LUNMEDIA LunMedia,
                             IN MSCSI_MEDIA_TYPE MediaType,
                             IN PVOID Details);
    PUBLIC PMINISCSI_LUNMEDIA AcquireMedia(IN OUT PMINISCSI_LUNMEDIA LunMedia);
    PUBLIC BOOLEAN ReleaseMedia(IN OUT PMINISCSI_LUNMEDIA LunMedia);
    PUBLIC CHAR MediaGetDiskType(IN PMINISCSI_LUNMEDIA LunMedia);
    PUBLIC UCHAR MediaReadWrite(IN PMINISCSI_LUNMEDIA Media,
                                IN OUT PVOID Buffer,
                                IN ULONG LBA,
                                IN ULONG Count,
                                IN SCSI_OP_E ReadWrite);
#ifdef __cplusplus
    };
#endif
#endif


