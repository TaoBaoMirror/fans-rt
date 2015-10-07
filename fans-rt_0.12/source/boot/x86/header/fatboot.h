#ifndef __FAT_BOOT_H
#define __FAT_BOOT_H

typedef struct tagFAT_PBR_SECTOR FAT_PBR_SECTOR;
typedef struct tagFAT_PBR_SECTOR * PFAT_PBR_SECTOR;
typedef struct tagFAT_PBR_SECTOR FAR * LPFAT_PBR_SECTOR;

#define     FAT_JUMP_SIZE       3
#define     FAT_OEM_SIZE        8
#define     FAT_LABLE_SIZE      11
#define     FAT_FSNAME_SIZE     8
#define     FAT_CODE_SIZE       450

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
}FAT_BPB_TABLE;

struct tagFAT_PBR_SECTOR{
    BYTE                JumpCode[FAT_JUMP_SIZE];        /**< Jmp xxxx */
    FAT_BPB_TABLE       BPBTable;                       /**< BPB��*/
    BYTE                BootCode[FAT_CODE_SIZE];        /**< ��������*/
    WORD                BootFeatures;                   /**< ������0x55 0xaa */
};

typedef struct tagFAT32_PBR_SECTOR FAT32_PBR_SECTOR;
typedef struct tagFAT32_PBR_SECTOR * PFAT32_PBR_SECTOR;
typedef struct tagFAT32_PBR_SECTOR FAR * LPFAT32_PBR_SECTOR;

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
       WORD                SectorPreFAT;                /**< ÿFAT ������*/
       WORD                SectorPreTrack;              /**< ÿ�ŵ�������*/
       WORD                NumberHeads;                 /**< ��ͷ��*/
       DWORD               HiddenSectors;               /**< ����������*/
       DWORD               TotalSectors;                /**< �������� */
    /* Begin FAT 32 BPB Section */
       DWORD               SectorPreFAT;                /**< ÿ��FAT ��ռ��������*/
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

}FAT32_BPB_TABLE;


struct tagFAT32_PBR_SECTOR{
    BYTE                JumpCode[FAT_JUMP_SIZE];        /**< Jmp xxxx */
    FAT32_BPB_TABLE     BPBTable;                       /**< FAT32 BPB��*/
    BYTE                BootCode[FAT_CODE_SIZE];        /**< ��������*/
    WORD                BootFeatures;                   /**< ������0x55 0xaa */
};

#endif

