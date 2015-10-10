#ifndef __MAIN_BOOT_H
#define __MAIN_BOOT_H

typedef tagPARTITION_TABLE PARTITION_TABLE;
typedef tagPARTITION_TABLE * PPARTITION_TABLE;
typedef tagPARTITION_TABLE FAR * LPPARTITION_TABLE;

struct tagPARTITION_TABLE{
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
};

#define     MBR_CODE_SIZE       442                     /**< MBR ����������*/
#define     MBR_TABLES          4                       /**< MBR ����������*/

typedef struct tagMBR_SECTOR MBR_SECTOR;
typedef struct tagMBR_SECTOR * PMBR_SECTOR;
typedef struct tagMBR_SECTOR FAR * LPMBR_SECTOR;

struct tagMBR_SECTOR{
    BYTE                BootCode[MBR_CODE_SIZE];        /**< ��������*/
    PARTITION_TABLE     MbrTable[MBR_TABLES];           /**< �ĸ���������*/
    WORD                BootFeatures;                   /**< ������0x55 0xaa */
};

#endif


