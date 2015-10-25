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
    BYTE                OemString[FAT_OEM_SIZE];        /**< OEM�ַ���*/
    WORD                BytePreSector;                  /**< ÿ���������ֽ���*/
    BYTE                SectorPreCluster;               /**< ÿ���ص�������*/
    WORD                ReservedSectors;                /**< ����������*/
    BYTE                NumberOfFATs;                   /**< FAT��ĸ�������*/
    WORD                RootFileEntrys;                 /**< ��Ŀ¼�ļ�����*/
    WORD                TotalSectors16;                 /**< С�������̵���������*/
    BYTE                TypeOfMedia;                    /**< ��������*/
    WORD                SectorPreFAT;                   /**< ÿFAT ������*/
    WORD                SectorPreTrack;                 /**< ÿ�ŵ�������*/
    WORD                NumberHeads;                    /**< ��ͷ��*/
    DWORD               HiddenSectors;                  /**< ����������*/
    DWORD               TotalSectors;                   /**< �������� */
    BYTE                DriverNumber;                   /**< ��������*/
    BYTE                ReservedByte;                   /**< ����- δʹ��*/
    BYTE                ExtenFeature;                   /**< ��չ��������*/
    DWORD               SerialNumber;                   /**< �������к�*/
    BYTE                VolumeLable[FAT_LABLE_SIZE];    /**< ���̾��*/
    BYTE                Name[FAT_FSNAME_SIZE];          /**< �ļ�ϵͳ����*/
}ATTRIBUTE_PACKED FAT_BPB_TABLE;

typedef FAT_BPB_TABLE * PFAT_BPB_TABLE;
typedef FAT_BPB_TABLE FAR * LPFAT_BPB_TABLE;


typedef struct tagFAT_PBR_SECTOR{
    BYTE                JumpCode[FAT_JUMP_SIZE];        /**< Jmp xxxx */
    FAT_BPB_TABLE       BPBTable;                       /**< BPB��*/
    BYTE                BootCode[FAT_CODE_SIZE];        /**< ��������*/
    DWORD               SecondSector;                   /**< �ڶ�����*/
    WORD                FansFeatures;                   /**< Fans boot ������0x46 0x41 */
    WORD                BootFeatures;                   /**< ������0x55 0xaa */
}ATTRIBUTE_PACKED FAT_PBR_SECTOR;

typedef FAT_PBR_SECTOR * PFAT_PBR_SECTOR;
typedef FAT_PBR_SECTOR FAR * LPFAT_PBR_SECTOR;


#define     FAT32_REV_SIZE      12

typedef struct tagFAT32_BPB_TABLE{
    BYTE                OemString[FAT_OEM_SIZE];     /**< OEM�ַ���*/
    WORD                BytePreSector;               /**< ÿ���������ֽ���*/
    BYTE                SectorPreCluster;            /**< ÿ���ص�������*/
    WORD                ReservedSectors;             /**< ����������*/
    BYTE                NumberOfFATs;                /**< FAT��ĸ�������*/
    WORD                RootFileEntrys;              /**< ��Ŀ¼�ļ�����*/
    WORD                TotalSectors16;              /**< С�������̵���������*/
    BYTE                TypeOfMedia;                 /**< ��������*/
    WORD                SectorPreFAT16;              /**< ÿFAT ������*/
    WORD                SectorPreTrack;              /**< ÿ�ŵ�������*/
    WORD                NumberHeads;                 /**< ��ͷ��*/
    DWORD               HiddenSectors;               /**< ����������*/
    DWORD               TotalSectors;                /**< �������� */
    /* Begin FAT 32 BPB Section */
    DWORD               SectorPreFAT32;              /**< ÿ��FAT ��ռ��������*/
    WORD                FAT32Features;               /**< FAT32 ������*/
    WORD                FAT32Version;                /**< FAT32 �汾��*/
    DWORD               FAT32RootStart;              /**< ��Ŀ¼�״غ�*/
    WORD                FAT32Infor;                  /**< �ļ�ϵͳ��Ϣ����*/
    WORD                FAT32BootBackup;             /**< ������������*/
    BYTE                FAT32Rev[FAT32_REV_SIZE];    /**< ����12�ֽ�*/
    /* End FAT 32 BPB Section */
    BYTE                DriverNumber;                /**< ��������*/
    BYTE                ReservedByte;                /**< ����- δʹ��*/
    BYTE                ExtenFeature;                /**< ��չ��������*/
    DWORD               SerialNumber;                /**< �������к�*/
    BYTE                VolumeLable[FAT_LABLE_SIZE]; /**< ���̾��*/
    BYTE                Name[FAT_FSNAME_SIZE];       /**< �ļ�ϵͳ����*/
}ATTRIBUTE_PACKED FAT32_BPB_TABLE;

typedef FAT32_BPB_TABLE * PFAT32_BPB_TABLE;
typedef FAT32_BPB_TABLE FAR * LPFAT32_BPB_TABLE;


typedef struct tagFAT32_PBR_SECTOR{
    BYTE                JumpCode[FAT_JUMP_SIZE];        /**< Jmp xxxx */
    FAT32_BPB_TABLE     BPBTable;                       /**< FAT32 BPB��*/
    BYTE                BootCode[FAT32_CODE_SIZE];      /**< ��������*/
    DWORD               SecondSector;                   /**< �ڶ�����*/
    WORD                FansFeatures;                   /**< Fans boot ������0x46 0x41 */
    WORD                BootFeatures;                   /**< ������0x55 0xaa */
}ATTRIBUTE_PACKED FAT32_PBR_SECTOR;

typedef FAT32_PBR_SECTOR * PFAT32_PBR_SECTOR;
typedef FAT32_PBR_SECTOR FAR * LPFAT32_PBR_SECTOR;

#ifdef _MSC_VER
#pragma pack(pop)
#endif


#endif

