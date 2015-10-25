#ifndef __MAKE_BOOT_H
#define __MAKE_BOOT_H

#include <stdio.h>
#include "fatboot.h"
#include "mainboot.h"

#ifndef MAX_PATH
#define	MAX_PATH	260
#endif

#define     INVALID_BOOT        -1001

typedef struct tagBOOT_SECTORS{
    struct{
        PMBR_SECTOR         Sector;
        size_t              Length;
    }Mbr;
    struct{
        PFAT_PBR_SECTOR     Sector;
        size_t              Length;
    }Fat12;
    struct{
        PFAT_PBR_SECTOR     Sector;
        size_t              Length;
    }Fat16;
    struct{
        PFAT32_PBR_SECTOR   Sector;
        size_t              Length;
    }Fat32;
}BOOT_SECTORS, * PBOOT_SECTORS;


typedef struct tagTOOL_RUNTIME_OBJECT{
    PBOOT_SECTORS           BootSectors;
    union{
        MBR_SECTOR          Mbr;
        FAT_PBR_SECTOR      Fat;
        FAT32_PBR_SECTOR    Fat32;
        BYTE                Sector[SECTOR_SIZE];
    }u;
    E_FSID                  FsID;
    int                     Error;
    char                    FullPath[MAX_PATH];
}TOOL_RUNTIME_OBJECT, * PTOOL_RUNTIME_OBJECT;

#endif

