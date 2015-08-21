#ifndef __WS_COMMON_H
#define __WS_COMMON_H

#include <windows.h>

typedef unsigned int ADDR32;

typedef struct tagELFABI{
    int             fd;
    void *          Elf;
    unsigned int    PhdrCount;
    size_t          Length;
    char *          Buffer;
}ELFABI;

typedef struct tagELF_SECTION{
    size_t          SectLen;
    ADDR32          Address;
    size_t          DataLen;
    char *          Buffer;
}ELF_SECTION;

//  BOOL Elf_SendFile(LPSTR lpFileName, HANDLE hCOM);
    ELFABI * ELF_OpenFile(LPSTR lpFileName);
    BOOL ELF_LoadSection(ELFABI * Elfabi, ELF_SECTION * lpSection, UINT index);
    //ELFABI * ELF_OpenFile(LPSTR lpFileName);
    //BOOL ELF_GetPhdr(ELFABI * Elfabi, ELFPHDR * Elfphdr, unsigned int Index);

#endif