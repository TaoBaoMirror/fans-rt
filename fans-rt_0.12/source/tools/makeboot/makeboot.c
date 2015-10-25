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
 *    2015-10-11     JiangYong       new file
 */

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>

#include "libtools.h"
#include "makeboot.h"
#include "language.h"

static void fat_sector_debug(PFAT_PBR_SECTOR Sector)
{
    char Buffer[FAT_LABLE_SIZE+1] = {0};
    printf("*****************Sector: %p*********************\n", Sector);
    printf("OemString       : %02d,  %02d,  '%s'\n",
        offsetof(FAT_PBR_SECTOR, BPBTable.OemString[0]),
        sizeof(Sector->BPBTable.OemString), Sector->BPBTable.OemString);
    printf("BytePreSector   : %02d,  %02d,  0x%x\n",
        offsetof(FAT_PBR_SECTOR, BPBTable.BytePreSector),
        sizeof(Sector->BPBTable.BytePreSector), Sector->BPBTable.BytePreSector);
    printf("SectorPreCluster: %02d,  %02d,  0x%X\n",
        offsetof(FAT_PBR_SECTOR, BPBTable.SectorPreCluster),
        sizeof(Sector->BPBTable.SectorPreCluster), Sector->BPBTable.SectorPreCluster);
    printf("ReservedSectors : %02d,  %02d,  0x%X\n",
        offsetof(FAT_PBR_SECTOR, BPBTable.ReservedSectors),
        sizeof(Sector->BPBTable.ReservedSectors), Sector->BPBTable.ReservedSectors);
    printf("NumberOfFATs    : %02d,  %02d,  0x%X\n",
        offsetof(FAT_PBR_SECTOR, BPBTable.NumberOfFATs),
        sizeof(Sector->BPBTable.NumberOfFATs), Sector->BPBTable.NumberOfFATs);
    printf("RootFileEntrys  : %02d,  %02d,  0x%X\n",
        offsetof(FAT_PBR_SECTOR, BPBTable.RootFileEntrys),
        sizeof(Sector->BPBTable.RootFileEntrys), Sector->BPBTable.RootFileEntrys);
    printf("TotalSectors16  : %02d,  %02d,  0x%X\n",
        offsetof(FAT_PBR_SECTOR, BPBTable.TotalSectors16),
        sizeof(Sector->BPBTable.TotalSectors16), Sector->BPBTable.TotalSectors16);
    printf("TypeOfMedia     : %02d,  %02d,  0x%X\n",
        offsetof(FAT_PBR_SECTOR, BPBTable.TypeOfMedia),
        sizeof(Sector->BPBTable.TypeOfMedia), Sector->BPBTable.TypeOfMedia);
    printf("SectorPreFAT    : %02d,  %02d,  0x%X\n",
        offsetof(FAT_PBR_SECTOR, BPBTable.SectorPreFAT),
        sizeof(Sector->BPBTable.SectorPreFAT), Sector->BPBTable.SectorPreFAT);
    printf("SectorPreTrack  : %02d,  %02d,  0x%X\n",
        offsetof(FAT_PBR_SECTOR, BPBTable.SectorPreTrack),
        sizeof(Sector->BPBTable.SectorPreTrack), Sector->BPBTable.SectorPreTrack);
    printf("NumberHeads     : %02d,  %02d,  0x%X\n",
        offsetof(FAT_PBR_SECTOR, BPBTable.NumberHeads),
        sizeof(Sector->BPBTable.NumberHeads), Sector->BPBTable.NumberHeads);
    printf("HiddenSectors   : %02d,  %02d,  0x%X\n",
        offsetof(FAT_PBR_SECTOR, BPBTable.HiddenSectors),
        sizeof(Sector->BPBTable.HiddenSectors), Sector->BPBTable.HiddenSectors);
    printf("TotalSectors    : %02d,  %02d,  0x%X\n",
        offsetof(FAT_PBR_SECTOR, BPBTable.TotalSectors),
        sizeof(Sector->BPBTable.TotalSectors), Sector->BPBTable.TotalSectors);
    printf("DriverNumber    : %02d,  %02d,  0x%X\n",
        offsetof(FAT_PBR_SECTOR, BPBTable.DriverNumber),
        sizeof(Sector->BPBTable.DriverNumber), Sector->BPBTable.DriverNumber);
    printf("ReservedByte    : %02d,  %02d,  0x%X\n",
        offsetof(FAT_PBR_SECTOR, BPBTable.ReservedByte),
        sizeof(Sector->BPBTable.ReservedByte), Sector->BPBTable.ReservedByte);
    printf("ExtenFeature    : %02d,  %02d,  0x%X\n",
        offsetof(FAT_PBR_SECTOR, BPBTable.ExtenFeature),
        sizeof(Sector->BPBTable.ExtenFeature), Sector->BPBTable.ExtenFeature);
    printf("SerialNumber    : %02d,  %02d,  0x%X\n",
        offsetof(FAT_PBR_SECTOR, BPBTable.SerialNumber),
        sizeof(Sector->BPBTable.SerialNumber), Sector->BPBTable.SerialNumber);
    printf("VolumeLable     : %02d,  %02d,  '%s'\n",
        offsetof(FAT_PBR_SECTOR, BPBTable.VolumeLable[0]),
        sizeof(Sector->BPBTable.VolumeLable), Sector->BPBTable.VolumeLable);
    printf("Name            : %02d,  %02d,  '%s'\n",
        offsetof(FAT_PBR_SECTOR, BPBTable.Name[0]),
        sizeof(Sector->BPBTable.Name), Sector->BPBTable.Name);
    
}

static void fat32_sector_debug(PFAT32_PBR_SECTOR Sector)
{
    char Buffer[FAT_LABLE_SIZE+1] = {0};
    
    printf("*****************Sector: %p  size %d *********************\n", Sector, sizeof(FAT32_PBR_SECTOR));
    printf("OemString       : %02d,  %02d,  '%s'\n",
        offsetof(FAT32_PBR_SECTOR, BPBTable.OemString[0]),
        sizeof(Sector->BPBTable.OemString), Sector->BPBTable.OemString);
    printf("BytePreSector   : %02d,  %02d,  0x%x\n",
        offsetof(FAT32_PBR_SECTOR, BPBTable.BytePreSector),
        sizeof(Sector->BPBTable.BytePreSector), Sector->BPBTable.BytePreSector);
    printf("SectorPreCluster: %02d,  %02d,  0x%X\n",
        offsetof(FAT32_PBR_SECTOR, BPBTable.SectorPreCluster),
        sizeof(Sector->BPBTable.SectorPreCluster), Sector->BPBTable.SectorPreCluster);
    printf("ReservedSectors : %02d,  %02d,  0x%X\n",
        offsetof(FAT32_PBR_SECTOR, BPBTable.ReservedSectors),
        sizeof(Sector->BPBTable.ReservedSectors), Sector->BPBTable.ReservedSectors);
    printf("NumberOfFATs    : %02d,  %02d,  0x%X\n",
        offsetof(FAT32_PBR_SECTOR, BPBTable.NumberOfFATs),
        sizeof(Sector->BPBTable.NumberOfFATs), Sector->BPBTable.NumberOfFATs);
    printf("RootFileEntrys  : %02d,  %02d,  0x%X\n",
        offsetof(FAT32_PBR_SECTOR, BPBTable.RootFileEntrys),
        sizeof(Sector->BPBTable.RootFileEntrys), Sector->BPBTable.RootFileEntrys);
    printf("TotalSectors16  : %02d,  %02d,  0x%X\n",
        offsetof(FAT32_PBR_SECTOR, BPBTable.TotalSectors16),
        sizeof(Sector->BPBTable.TotalSectors16), Sector->BPBTable.TotalSectors16);
    printf("TypeOfMedia     : %02d,  %02d,  0x%X\n",
        offsetof(FAT32_PBR_SECTOR, BPBTable.TypeOfMedia),
        sizeof(Sector->BPBTable.TypeOfMedia), Sector->BPBTable.TypeOfMedia);
    printf("SectorPreFAT    : %02d,  %02d,  0x%X\n",
        offsetof(FAT32_PBR_SECTOR, BPBTable.SectorPreFAT16),
        sizeof(Sector->BPBTable.SectorPreFAT16), Sector->BPBTable.SectorPreFAT16);
    printf("SectorPreTrack  : %02d,  %02d,  0x%X\n",
        offsetof(FAT32_PBR_SECTOR, BPBTable.SectorPreTrack),
        sizeof(Sector->BPBTable.SectorPreTrack), Sector->BPBTable.SectorPreTrack);
    printf("NumberHeads     : %02d,  %02d,  0x%X\n",
        offsetof(FAT32_PBR_SECTOR, BPBTable.NumberHeads),
        sizeof(Sector->BPBTable.NumberHeads), Sector->BPBTable.NumberHeads);
    printf("HiddenSectors   : %02d,  %02d,  0x%X\n",
        offsetof(FAT32_PBR_SECTOR, BPBTable.HiddenSectors),
        sizeof(Sector->BPBTable.HiddenSectors), Sector->BPBTable.HiddenSectors);
    printf("TotalSectors    : %02d,  %02d,  0x%X\n",
        offsetof(FAT32_PBR_SECTOR, BPBTable.TotalSectors),
        sizeof(Sector->BPBTable.TotalSectors), Sector->BPBTable.TotalSectors);

    printf("SectorPreFAT32  : %02d,  %02d,  0x%X\n",
        offsetof(FAT32_PBR_SECTOR, BPBTable.SectorPreFAT32),
        sizeof(Sector->BPBTable.SectorPreFAT32), Sector->BPBTable.SectorPreFAT32);
    printf("FAT32Features   : %02d,  %02d,  0x%X\n",
        offsetof(FAT32_PBR_SECTOR, BPBTable.FAT32Features),
        sizeof(Sector->BPBTable.FAT32Features), Sector->BPBTable.FAT32Features);
    printf("FAT32Version    : %02d,  %02d,  0x%X\n",
        offsetof(FAT32_PBR_SECTOR, BPBTable.FAT32Version),
        sizeof(Sector->BPBTable.FAT32Version), Sector->BPBTable.FAT32Version);
    printf("FAT32RootStart  : %02d,  %02d,  0x%X\n",
        offsetof(FAT32_PBR_SECTOR, BPBTable.FAT32RootStart),
        sizeof(Sector->BPBTable.FAT32RootStart), Sector->BPBTable.FAT32RootStart);
    printf("FAT32Infor      : %02d,  %02d,  0x%X\n",
        offsetof(FAT32_PBR_SECTOR, BPBTable.FAT32Infor),
        sizeof(Sector->BPBTable.FAT32Infor), Sector->BPBTable.FAT32Infor);
    printf("FAT32BootBackup : %02d,  %02d,  0x%X\n",
        offsetof(FAT32_PBR_SECTOR, BPBTable.FAT32BootBackup),
        sizeof(Sector->BPBTable.FAT32BootBackup), Sector->BPBTable.FAT32BootBackup);
    
    printf("DriverNumber    : %02d,  %02d,  0x%X\n",
        offsetof(FAT32_PBR_SECTOR, BPBTable.DriverNumber),
        sizeof(Sector->BPBTable.DriverNumber), Sector->BPBTable.DriverNumber);
    printf("ReservedByte    : %02d,  %02d,  0x%X\n",
        offsetof(FAT32_PBR_SECTOR, BPBTable.ReservedByte),
        sizeof(Sector->BPBTable.ReservedByte), Sector->BPBTable.ReservedByte);
    printf("ExtenFeature    : %02d,  %02d,  0x%X\n",
        offsetof(FAT32_PBR_SECTOR, BPBTable.ExtenFeature),
        sizeof(Sector->BPBTable.ExtenFeature), Sector->BPBTable.ExtenFeature);
    printf("SerialNumber    : %02d,  %02d,  0x%X\n",
        offsetof(FAT32_PBR_SECTOR, BPBTable.SerialNumber),
        sizeof(Sector->BPBTable.SerialNumber), Sector->BPBTable.SerialNumber);

    memset(Buffer, 0, sizeof(Buffer));
    memcpy(Buffer, Sector->BPBTable.VolumeLable, FAT_LABLE_SIZE);
    
    printf("VolumeLable     : %02d,  %02d,  '%s'\n",
        offsetof(FAT32_PBR_SECTOR, BPBTable.VolumeLable[0]),
        sizeof(Sector->BPBTable.VolumeLable), Buffer);

    memset(Buffer, 0, sizeof(Buffer));
    memcpy(Buffer, Sector->BPBTable.Name, FAT_FSNAME_SIZE);
    
    printf("Name            : %02d,  %02d,  '%s'\n",
        offsetof(FAT32_PBR_SECTOR, BPBTable.Name[0]),
        sizeof(Sector->BPBTable.Name), Buffer);

    printf("SecondSector    : %d,  %02d,  0x%08X\n",
        offsetof(FAT32_PBR_SECTOR, SecondSector),
        sizeof(Sector->SecondSector), Sector->SecondSector);
}

static void show_data(char * buffer, size_t length)
{
    size_t i;

    for (i = 0; i < length; i ++)
    {
        unsigned char data = ((unsigned char *)buffer)[i];

        if (0 == (i % SECTOR_SIZE))
        {
            printf("\n*******************sector %d*******************\n",
                i / SECTOR_SIZE);
        }
        
        if (0 == (i % 16))
        {
            printf("\n %08X: ", i);
        }
        else if (0 == (i % 8))
        {
            printf("-");
        }
        else
        {
            printf(" ");
        }

        printf("%02X", data);
    }

}


/**************************************************************************
 * Read data to buffer from the specified file.
 * param: The file discriptor id.
 * param: The data offset of this file.
 * param: The data buffer pointer.
 * param: The data buffer length.
 * return: If return 0 mean successfully, the other value means failure.
 *   
 * date           author          notes
 * 2015-10-25     JiangYong       new function
**************************************************************************/
static int read_file(int fd, long offset, void * buffer, size_t length)
{
    if (offset != lseek(fd, offset, SEEK_SET))
    {
        printf(NOTE_FILE_SIZE_ERROR);
        return INVALID_SIZE;
    }

    if (length != read(fd, buffer, length))
    {
        printf(NOTE_FILE_READ_ERROR);
        return INVALID_READ;
    }

    return 0;
}
/**************************************************************************
 * Write data from buffer to the specified file.
 * param: The file discriptor id.
 * param: The data offset of this file.
 * param: The data buffer pointer.
 * param: The data buffer length.
 * return: If return 0 mean successfully, the other value means failure.
 *   
 * date           author          notes
 * 2015-10-25     JiangYong       new function
**************************************************************************/
static int write_file(int fd, off_t offset, void * buffer, size_t length)
{
    printf("\n\nWrite offset 0x%08X, buffer 0x%p, length %d.", offset, buffer, length);
    
    show_data(buffer, length);

    if (offset != lseek(fd, offset, SEEK_SET))
    {
        printf(NOTE_FILE_SIZE_ERROR);
        return INVALID_SIZE;
    }

    if (length != write(fd, buffer, length))
    {
        printf(NOTE_FILE_READ_ERROR);
        return INVALID_READ;
    }

    return 0;
}


/**************************************************************************
 * Save FAT16/FAT12 boot sector to virtual disk.
 * param: The data pointer of the new boot sector.
 * param: The size of the boot sector data.
 * param: Path of this virtual disk file.
 * return: If return 0 mean successfully, the other value means failure.
 *   
 * date           author          notes
 * 2015-10-25     JiangYong       new function
**************************************************************************/
static int save_fat(PFAT_PBR_SECTOR Sector, size_t Length, char * FullPath)
{
    int result;
    DWORD Current = 0;
    DWORD * SectorNext = &Sector->SecondSector;
    DWORD Total = Length / SECTOR_SIZE;
    DWORD SectorLBA = Sector->BPBTable.HiddenSectors + 1;
    off_t offset = Sector->BPBTable.HiddenSectors * SECTOR_SIZE;
    char * buffer = (char *)Sector;
    int fd = open(FullPath, O_RDWR | O_BINARY);

    fat_sector_debug(Sector);

    if (fd < 0)
    {
        printf(NOTE_FILE_OPEN_ERROR, FullPath);
        return INVALID_FILE;
    }

    if (Length/SECTOR_SIZE >= Sector->BPBTable.ReservedSectors)
    {
        printf(NOTE_BOOT_SECTOR_LOW, Sector->BPBTable.ReservedSectors);
        close(fd);
        return INVALID_SIZE;
    }
    /* 修改引导扇区列表(从第二扇区开始)*/
    /* 引导记录的首扇区不保存在改表中*/
    for (Current = 0; Current < Total - 1; Current ++)
    {
        *SectorNext = SectorLBA;
        SectorNext --;
        SectorLBA ++;
    }

    result = write_file(fd, offset, buffer, SECTOR_SIZE * Total);

    close(fd);

    return result;
}

/**************************************************************************
 * Make the FAT12 boot sector.
 * param: The run time object pointer.
 * return: If return 0 mean successfully, the other value means failure.
 *   
 * date           author          notes
 * 2015-10-25     JiangYong       new function
**************************************************************************/
static int make_fat12(PTOOL_RUNTIME_OBJECT Object)
{
    PBOOT_SECTORS Sectors = Object->BootSectors;
    char fsname[FAT_FSNAME_SIZE + 1] = {0};

    if (FSID_NULL != Object->FsID && FSID_FAT12 != Object->FsID)
    {
        TOOLS_LOG("This partition(0x%02X) is not FAT12.", Object->FsID);
        return INVALID_BOOT;
    }

    strncpy(fsname, Object->u.Fat.BPBTable.Name, FAT_FSNAME_SIZE);
    TOOLS_LOG("The file system name is '%s'.", fsname);

    if (0 != strcmp(FAT12_NAME, fsname))
    {
        return INVALID_BOOT;
    }

    Object->FsID = FSID_FAT12;

    memcpy(&Sectors->Fat12.Sector->BPBTable,
           &Object->u.Fat.BPBTable,
           sizeof(FAT_BPB_TABLE));
    
    return save_fat(Sectors->Fat12.Sector, Sectors->Fat12.Length, Object->FullPath);
}

/**************************************************************************
 * Make the FAT16 boot sector.
 * param: The run time object pointer.
 * return: If return 0 mean successfully, the other value means failure.
 *   
 * date           author          notes
 * 2015-10-25     JiangYong       new function
**************************************************************************/
static int make_fat16(PTOOL_RUNTIME_OBJECT Object)
{
    PBOOT_SECTORS Sectors = Object->BootSectors;
    char fsname[FAT_FSNAME_SIZE + 1] = {0};

    if (FSID_NULL != Object->FsID && FSID_FAT16 != Object->FsID)
    {
        TOOLS_LOG("This partition(0x%02X) is not FAT16.", Object->FsID);
        return INVALID_BOOT;
    }

    strncpy(fsname, Object->u.Fat.BPBTable.Name, FAT_FSNAME_SIZE);
    TOOLS_LOG("The file system name is '%s'.", fsname);

    if (0 != strcmp(FAT16_NAME, fsname))
    {
        return INVALID_BOOT;
    }

    Object->FsID = FSID_FAT16;

    memcpy(&Sectors->Fat16.Sector->BPBTable,
           &Object->u.Fat.BPBTable,
           sizeof(FAT_BPB_TABLE));

    return save_fat(Sectors->Fat16.Sector, Sectors->Fat16.Length, Object->FullPath);
}


/**************************************************************************
 * Save FAT32 boot sector to virtual disk.
 * param: The data pointer of the new boot sector.
 * param: The size of the boot sector data.
 * param: Path of this virtual disk file.
 * return: If return 0 mean successfully, the other value means failure.
 *   
 * date           author          notes
 * 2015-10-25     JiangYong       new function
**************************************************************************/
static int save_fat32(PFAT32_PBR_SECTOR Sector, size_t Length, char * FullPath)
{
    int result;
    DWORD Current = 0;
    DWORD * SectorNext = &Sector->SecondSector;
    DWORD Total = Length / SECTOR_SIZE;
    DWORD SectorLBA = Sector->BPBTable.HiddenSectors + 1;
    off_t offset = Sector->BPBTable.HiddenSectors * SECTOR_SIZE;
    char * buffer = (char *)Sector;
    int fd = open(FullPath, O_RDWR | O_BINARY);

    fat32_sector_debug(Sector);

    if (fd < 0)
    {
        printf(NOTE_FILE_OPEN_ERROR, FullPath);
        return INVALID_FILE;
    }

    if (Length/SECTOR_SIZE >= Sector->BPBTable.ReservedSectors)
    {
        printf(NOTE_BOOT_SECTOR_LOW, Sector->BPBTable.ReservedSectors);
        close(fd);
        return INVALID_SIZE;
    }
    /* 修改引导扇区列表(从第二扇区开始)*/
    /* 引导记录的首扇区不保存在改表中*/
    for (Current = 0; Current < Total - 1; Current ++)
    {
        *SectorNext = SectorLBA;
        SectorNext --;
        SectorLBA ++;
    }

    result = write_file(fd, offset, buffer, SECTOR_SIZE * Total);

    close(fd);

    return result;
}

/**************************************************************************
 * Make the FAT32 boot sector.
 * param: The run time object pointer.
 * return: If return 0 mean successfully, the other value means failure.
 *   
 * date           author          notes
 * 2015-10-25     JiangYong       new function
**************************************************************************/
static int make_fat32(PTOOL_RUNTIME_OBJECT Object)
{
    PBOOT_SECTORS Sectors = Object->BootSectors;
    char fsname[FAT_FSNAME_SIZE + 1] = {0};

    if (FSID_NULL != Object->FsID && FSID_FAT32 != Object->FsID)
    {
        TOOLS_LOG("This partition(0x%02X) is not FAT32.", Object->FsID);
        return INVALID_BOOT;
    }

    strncpy(fsname, Object->u.Fat32.BPBTable.Name, FAT_FSNAME_SIZE);
    TOOLS_LOG("The file system name is '%s'.", fsname);

    if (0 != strcmp(FAT32_NAME, fsname))
    {
        return INVALID_BOOT;
    }

    Object->FsID = FSID_FAT32;

    memcpy(&Sectors->Fat32.Sector->BPBTable,
           &Object->u.Fat32.BPBTable,
           sizeof(FAT32_BPB_TABLE));

    return save_fat32(Sectors->Fat32.Sector, Sectors->Fat32.Length, Object->FullPath);
}

/**************************************************************************
 * Check the MBR of this disk,  If this is a MBR virtual disk will be loading
 * the activity partition boot sector.
 * param: The run time object pointer.
 * param: The virtual disk file descriptor.
 * return: If return 0 mean successfully, the other value means failure.
 *   
 * date           author          notes
 * 2015-10-25     JiangYong       new function
**************************************************************************/

static BOOL check_mbr(PTOOL_RUNTIME_OBJECT Object, int fd)
{
    int i;
    int boot = 0;
    int count = 0;
    off_t offset = 0;
    PMBR_SECTOR Mbr = &Object->u.Mbr;

    for (i = 0; i < MBR_TABLES; i++)
    {
        if (BOOT_ACTIVE == Mbr->MbrTable[i].BootActive)
        {
            boot = i;
            count ++;
        }
    }

    if (1 != count)
    {
        TOOLS_LOG("Not found active partition, count %d.", count);
        return TRUE;
    }

    TOOLS_LOG("File system type is 0x%02X.",
        Mbr->MbrTable[boot].FileSystemType);

    switch(Mbr->MbrTable[boot].FileSystemType)
    {
    case FST_FAT12_SMALL:
        if (Mbr->MbrTable[boot].TotalLBA >= FAT12_SMALL_MAX_SECTORS)
        {
            return TRUE;
        }

        Object->FsID = FSID_FAT12;
        
        break;
    case FST_FAT16_SMALL:
        if (Mbr->MbrTable[boot].TotalLBA >= FAT16_SMALL_MAX_SECTORS)
        {
            return TRUE;
        }

        Object->FsID = FSID_FAT16;
        
        break;
    case FST_FAT16_MIDLE:
        Object->FsID = FSID_FAT16;
        break;
    case FST_FAT16_LARGE:
        Object->FsID = FSID_FAT16;
        break;
    case FST_FAT32_SMALL:
        Object->FsID = FSID_FAT32;
        break;
    case FST_FAT32_LARGE:
        Object->FsID = FSID_FAT32;
        break;
    default:
        
        return TRUE;
    }

    TOOLS_LOG("The LBA start sector is 0x%08X.",
        Mbr->MbrTable[boot].StartLBA);
    
    offset = Mbr->MbrTable[boot].StartLBA * SECTOR_SIZE;

    if (0 == read_file(fd, offset, Object->u.Sector, SECTOR_SIZE))
    {
        printf(NOTE_FILE_READ_ERROR, Object->FullPath);
        return FALSE;
    }

    return TRUE;
}


typedef int (*fncheck)(PTOOL_RUNTIME_OBJECT Object);

static fncheck g_Fncheck[] =
{
    make_fat12,
    make_fat16,
    make_fat32,
};

/**************************************************************************
 * Check the file system type and make the boot sectors.
 * param: The run time object pointer.
 * return: If return 0 mean successfully, the other value means failure.
 *
 * date           author          notes
 * 2015-10-25     JiangYong       new function
**************************************************************************/
static int Check_file_system(PTOOL_RUNTIME_OBJECT Object)
{
    int i;
    BOOL NeedCheck = FALSE;
    int fd = open(Object->FullPath, O_RDONLY | O_BINARY);

    TOOLS_LOG("The MBR struct size is %d, - %p.",
        sizeof(MBR_SECTOR), Object->FullPath);

    if (fd < 0)
    {
        printf(NOTE_FILE_OPEN_ERROR, Object->FullPath);
        return INVALID_FILE;
    }

    if (0 == read_file(fd, 0L, Object->u.Sector, SECTOR_SIZE))
    {
        printf(NOTE_FILE_READ_ERROR, Object->FullPath);
        close(fd);
        return INVALID_READ;
    }

    if (BOOT_FEATURES != Object->u.Mbr.BootFeatures)
    {
        printf(NOTE_DISK_BOOT_ERROR, Object->FullPath);
        show_data((char *)&Object->u.Mbr, SECTOR_SIZE);
        close(fd);
        return INVALID_BOOT;
    }

    NeedCheck = check_mbr(Object, fd);

    close(fd);

    if (TRUE == NeedCheck)
    {
        for (i = 0; i < SIZEOF_ARRAY(g_Fncheck); i++)
        {
            if (0 == g_Fncheck[i](Object))
            {
                return 0;
            }
        }

    }

    printf(NOTE_UNKNOW_SYSTEM);
     
    return INVALID_BOOT;

}

int tool_run(tool_descriptor * descriptor)
{
    PTOOL_RUNTIME_OBJECT Object = descriptor->tprivate;
    TOOLS_LOG("The MBR struct size is %d, - %p.",
        sizeof(MBR_SECTOR), Object->FullPath);
    return Check_file_system(descriptor->tprivate);
}

