;=========================================================================================================================
;文 件 名:  boot.inc
;作    者:  姜勇
;依 赖 项:  无
;被依赖项:  fat32.s/fat16.s/fat12.s
;修    订:
;           2005-11-05            姜勇              创建文件
%ifndef __KERNEL_BOOT_INC
%define __KERNEL_BOOT_INC
;==========================================================================================================================
; boot sector 地址和杂项定义
SECTOR_SIZE             EQU             0x0200                  ; 每扇区字节数
SECTOR_READER           EQU             0x0201                  ; 读磁盘的请求号
LOADER_SEGBASE          EQU             0x5000                  ; LOADER 加载地址
PROTAL_SEGBASE          EQU             0x40000                 ; 入口段地址
OFFSET_SECTOR           EQU             0x7c00                  ; 引导扇区的偏移地址
OFFSET_BSFLAG           EQU             0x01fc                  ; 引导扇区标志偏移地址
DVALUE_BSFLAG           EQU             0xaa55                  ; 引导扇区标志字
DVALUE_FAFLAG           EQU             0x4146                  ; 'FA' 引导标志
FITEM_LENGTH            EQU             0x0020                  ; 文件登记项长度
FAT_OFFSET_BLOCKID_L    EQU             0x001a                  ; 文件簇号相对文件登记项的偏移地址
FAT_OFFSET_BLOCKID_H    EQU             0x0012                  ; 文件簇号相对文件登记项的偏移地址
FAT_OFFSET_FILESIZE     EQU             0x001c                  ; 文件的长度
ERETRY_COUNT            EQU             0x0005                  ; 发生错误的重试次数
;==========================================================================================================================
; ELF 文件相关定义
ELFHDR_OFFSET_SHOFF     EQU             0x20                    ; Elf32_Ehdr->shoff 的偏移地址
ELFHDR_OFFSET_SHNUM     EQU             0x30                    ; Elf32_Ehdr->shnum 的偏移地址
SECHDR_OFFSET_MMADDR    EQU             0x0c                    ; Elf32_Shdr->sh_addr 的偏移地址
SECHDR_OFFSET_OFFSET    EQU             0x10                    ; Elf32_Shdr->offset 的偏移地址
SIZEOF_SECHDR           EQU             0X28                    ; Elf32_Shdr 的长度
;==========================================================================================================================
; FAT 簇号定义
FAT32_TAIL              EQU             0x0fffffff              ; FAT32 文件结束块标志
FAT16_TAIL              EQU             0xffff                  ; FAT16 文件结束块标志
FAT12_TAIL              EQU             0x0fff                  ; FAT12 文件结束块标志
;==========================================================================================================================
; LOADER 文件格式定义
BINLOADER               EQU             0                       ; 装入的 loader 文件为 bin 文件
ELFLOADER               EQU             1                       ; 装入的 loader 文件为 elf 文件
MZLOADER                EQU             2                       ; 装入的 loader 文件为 mz 文件

%define         LOADER_FNAME                    'LOADER     '   ; 需要加载的文件名
%strlen         FNAME_LENGTH                    LOADER_FNAME

%endif
