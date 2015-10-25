#ifndef __MAIN_BOOT_H
#define __MAIN_BOOT_H

#define     BOOT_FEATURES       0xAA55
#define     BOOT_ACTIVE         0x80
#ifndef     SECTOR_SIZE
#define     SECTOR_SIZE         512
#endif

typedef enum{
    FSID_NULL       =       0,
    FSID_FAT12      =       1,
    FSID_FAT16      =       2,
    FSID_FAT32      =       3,
    FSID_MAX,
}E_FSID;


#define     FST_FAT12_SMALL         0x01                /**< less than 32680 sectors */
#define     FST_FAT16_SMALL         0x04                /**< 32680 < sectors < 65535 */
#define     FST_FAT16_MIDLE         0x06                /**< Logic drive */
#define     FST_FAT16_LARGE         0x0E                /**< Logic drive for entend int 13h */
#define     FST_FAT32_SMALL         0x0B                /**< Logic drive */
#define     FST_FAT32_LARGE         0x0C                /**< Logic drive for entend int 13h */
#define     FST_EXTEN_SMALL         0x05
#define     FST_EXTEN_LARGE         0x0F

#ifdef _MSC_VER
#pragma pack(push)
#pragma pack(1)
#endif

typedef struct tagPARTITION_TABLE{
    BYTE                BootActive;                     /**< ������־0x80 = �����*/
    BYTE                StartHeader;                    /**< ��ʼ��ͷ��*/
    BYTE                StartSector;                    /**< ��ʼ������*/
    BYTE                StartCylinder;                  /**< ��ʼ�����*/
    BYTE                FileSystemType;                 /**< �ļ�ϵͳ����*/
    BYTE                FinalTrack;                     /**< ������ͷ��*/
    BYTE                FinalSector;                    /**< ����������*/
    BYTE                FinalCylinder;                  /**< ���������*/
    DWORD               StartLBA;                       /**< LBAѰַģʽ�µ��׿��*/
    DWORD               TotalLBA;                       /**< LBAѰַģʽ�µ��ܿ���*/
}ATTRIBUTE_PACKED PARTITION_TABLE;

typedef PARTITION_TABLE * PPARTITION_TABLE;
typedef PARTITION_TABLE FAR * LPPARTITION_TABLE;

#define     MBR_CODE_SIZE       446                     /**< MBR ����������*/
#define     MBR_TABLES          4                       /**< MBR ����������*/

typedef struct tagMBR_SECTOR{
    BYTE                BootCode[MBR_CODE_SIZE];        /**< ��������*/
    PARTITION_TABLE     MbrTable[MBR_TABLES];           /**< �ĸ���������*/
    WORD                BootFeatures;                   /**< ������0x55 0xaa */
}ATTRIBUTE_PACKED MBR_SECTOR;

typedef MBR_SECTOR * PMBR_SECTOR;
typedef MBR_SECTOR * LPMBR_SECTOR;


#ifdef _MSC_VER
#pragma pack(pop)
#endif

#endif


