#ifndef __FAT_BOOT_H
#define __FAT_BOOT_H

#define     FAT_JUMP_SIZE       3
#define     FAT_OEM_SIZE        8
#define     FAT_LABLE_SIZE      11
#define     FAT_FSNAME_SIZE     8
#define     FAT_CODE_SIZE       436
#define     FAT32_CODE_SIZE     414
#ifndef     SECTOR_SIZE
#define     SECTOR_SIZE         512
#endif

#define     FAT12_SMALL_MAX_SECTORS   32680
#define     FAT16_SMALL_MAX_SECTORS   65535

#define     FAT12_NAME          "FAT12   "
#define     FAT16_NAME          "FAT16   "
#define     FAT32_NAME          "FAT32   "

#ifdef _MSC_VER
#pragma pack(push)
#pragma pack(1)
#endif

typedef struct tagFAT_BPB_TABLE{
    BYTE                OemString[FAT_OEM_SIZE];        /**< OEM字符串*/
    WORD                BytePreSector;                  /**< 每个扇区的字节数*/
    BYTE                SectorPreCluster;               /**< 每个簇的扇区数*/
    WORD                ReservedSectors;                /**< 保留扇区数*/
    BYTE                NumberOfFATs;                   /**< FAT表的副本数量*/
    WORD                RootFileEntrys;                 /**< 根目录文件项数*/
    WORD                TotalSectors16;                 /**< 小容量磁盘的总扇区数*/
    BYTE                TypeOfMedia;                    /**< 介质类型*/
    WORD                SectorPreFAT;                   /**< 每FAT 扇区数*/
    WORD                SectorPreTrack;                 /**< 每磁道扇区数*/
    WORD                NumberHeads;                    /**< 磁头数*/
    DWORD               HiddenSectors;                  /**< 隐藏扇区数*/
    DWORD               TotalSectors;                   /**< 总扇区数 */
    BYTE                DriverNumber;                   /**< 驱动器号*/
    BYTE                ReservedByte;                   /**< 保留- 未使用*/
    BYTE                ExtenFeature;                   /**< 扩展引导特征*/
    DWORD               SerialNumber;                   /**< 磁盘序列号*/
    BYTE                VolumeLable[FAT_LABLE_SIZE];    /**< 磁盘卷标*/
    BYTE                Name[FAT_FSNAME_SIZE];          /**< 文件系统名称*/
}ATTRIBUTE_PACKED FAT_BPB_TABLE;

typedef FAT_BPB_TABLE * PFAT_BPB_TABLE;
typedef FAT_BPB_TABLE FAR * LPFAT_BPB_TABLE;


typedef struct tagFAT_PBR_SECTOR{
    BYTE                JumpCode[FAT_JUMP_SIZE];        /**< Jmp xxxx */
    FAT_BPB_TABLE       BPBTable;                       /**< BPB表*/
    BYTE                BootCode[FAT_CODE_SIZE];        /**< 引导代码*/
    DWORD               SecondSector;                   /**< 第二扇区*/
    WORD                FansFeatures;                   /**< Fans boot 特征码0x46 0x41 */
    WORD                BootFeatures;                   /**< 特征码0x55 0xaa */
}ATTRIBUTE_PACKED FAT_PBR_SECTOR;

typedef FAT_PBR_SECTOR * PFAT_PBR_SECTOR;
typedef FAT_PBR_SECTOR FAR * LPFAT_PBR_SECTOR;


#define     FAT32_REV_SIZE      12

typedef struct tagFAT32_BPB_TABLE{
    BYTE                OemString[FAT_OEM_SIZE];     /**< OEM字符串*/
    WORD                BytePreSector;               /**< 每个扇区的字节数*/
    BYTE                SectorPreCluster;            /**< 每个簇的扇区数*/
    WORD                ReservedSectors;             /**< 保留扇区数*/
    BYTE                NumberOfFATs;                /**< FAT表的副本数量*/
    WORD                RootFileEntrys;              /**< 根目录文件项数*/
    WORD                TotalSectors16;              /**< 小容量磁盘的总扇区数*/
    BYTE                TypeOfMedia;                 /**< 介质类型*/
    WORD                SectorPreFAT16;              /**< 每FAT 扇区数*/
    WORD                SectorPreTrack;              /**< 每磁道扇区数*/
    WORD                NumberHeads;                 /**< 磁头数*/
    DWORD               HiddenSectors;               /**< 隐藏扇区数*/
    DWORD               TotalSectors;                /**< 总扇区数 */
    /* Begin FAT 32 BPB Section */
    DWORD               SectorPreFAT32;              /**< 每个FAT 表占用扇区数*/
    WORD                FAT32Features;               /**< FAT32 特征码*/
    WORD                FAT32Version;                /**< FAT32 版本号*/
    DWORD               FAT32RootStart;              /**< 根目录首簇号*/
    WORD                FAT32Infor;                  /**< 文件系统信息扇区*/
    WORD                FAT32BootBackup;             /**< 引导扇区备份*/
    BYTE                FAT32Rev[FAT32_REV_SIZE];    /**< 保留12字节*/
    /* End FAT 32 BPB Section */
    BYTE                DriverNumber;                /**< 驱动器号*/
    BYTE                ReservedByte;                /**< 保留- 未使用*/
    BYTE                ExtenFeature;                /**< 扩展引导特征*/
    DWORD               SerialNumber;                /**< 磁盘序列号*/
    BYTE                VolumeLable[FAT_LABLE_SIZE]; /**< 磁盘卷标*/
    BYTE                Name[FAT_FSNAME_SIZE];       /**< 文件系统名称*/
}ATTRIBUTE_PACKED FAT32_BPB_TABLE;

typedef FAT32_BPB_TABLE * PFAT32_BPB_TABLE;
typedef FAT32_BPB_TABLE FAR * LPFAT32_BPB_TABLE;


typedef struct tagFAT32_PBR_SECTOR{
    BYTE                JumpCode[FAT_JUMP_SIZE];        /**< Jmp xxxx */
    FAT32_BPB_TABLE     BPBTable;                       /**< FAT32 BPB表*/
    BYTE                BootCode[FAT32_CODE_SIZE];      /**< 引导代码*/
    DWORD               SecondSector;                   /**< 第二扇区*/
    WORD                FansFeatures;                   /**< Fans boot 特征码0x46 0x41 */
    WORD                BootFeatures;                   /**< 特征码0x55 0xaa */
}ATTRIBUTE_PACKED FAT32_PBR_SECTOR;

typedef FAT32_PBR_SECTOR * PFAT32_PBR_SECTOR;
typedef FAT32_PBR_SECTOR FAR * LPFAT32_PBR_SECTOR;

#ifdef _MSC_VER
#pragma pack(pop)
#endif


#endif

