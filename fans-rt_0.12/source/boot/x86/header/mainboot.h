#ifndef __MAIN_BOOT_H
#define __MAIN_BOOT_H

typedef tagPARTITION_TABLE PARTITION_TABLE;
typedef tagPARTITION_TABLE * PPARTITION_TABLE;
typedef tagPARTITION_TABLE FAR * LPPARTITION_TABLE;

struct tagPARTITION_TABLE{
    BYTE                BootActive;                     /**< 引导标志0x80 = 活动分区*/
    BYTE                StartHeader;                    /**< 起始磁头号*/
    BYTE                StartSector;                    /**< 起始扇区号*/
    BYTE                StartCylinder;                  /**< 起始柱面号*/
    BYTE                FileSystemType;                 /**< 文件系统类型*/
    BYTE                FinalTrack;                     /**< 结束磁头号*/
    BYTE                FinalSector;                    /**< 结束扇区号*/
    BYTE                FinalCylinder;                  /**< 结束柱面号*/
    DWORD               StartLBA;                       /**< LBA寻址模式下的首块号*/
    DWORD               TotalLBA;                       /**< LBA寻址模式下的总块数*/
};

#define     MBR_CODE_SIZE       442                     /**< MBR 代码区长度*/
#define     MBR_TABLES          4                       /**< MBR 分区表数量*/

typedef struct tagMBR_SECTOR MBR_SECTOR;
typedef struct tagMBR_SECTOR * PMBR_SECTOR;
typedef struct tagMBR_SECTOR FAR * LPMBR_SECTOR;

struct tagMBR_SECTOR{
    BYTE                BootCode[MBR_CODE_SIZE];        /**< 引导代码*/
    PARTITION_TABLE     MbrTable[MBR_TABLES];           /**< 四个分区表项*/
    WORD                BootFeatures;                   /**< 特征码0x55 0xaa */
};

#endif


