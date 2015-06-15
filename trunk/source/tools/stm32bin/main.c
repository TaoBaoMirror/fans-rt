#include <stdio.h>
#include <windows.h>
#include <FansError.h>
#include "Language.h"
#include "Version.h"
#include "common.h"

LPSTR lpCfgFile = "ld_flags.mak";
LPSTR lpElfFile = "fans-rt.elf";
LPSTR lpBinFile = "fans-rt.bin";

ELFABI * Elfabi = NULL;

/*
* 函数：GetFileName
* 功能：判断文件名的有效性，并生成一个备份文件名
* 入口：
*       [in]        char * file                     输入文件名
*       [out]       char * out                      输出的备份文件名
* 出口：
*       如果成功，返回文件名指针，否则返回 NULL
*/
static LPSTR GetFileName(CHAR * pFileName,CHAR * pOutName, SIZE_T Length)
{
    CHAR * pTemp;
    if (*pFileName=='\0')
    {
        return NULL;
    }

    if (strchr(pFileName,'*') || strchr(pFileName,'?'))
    {
        return NULL;
    }

    strncpy(pOutName, pFileName, Length);
#if defined(__linux__)
    pTemp = strrchr(pOutName,'/');
#else
    pTemp = strrchr(pOutName,'\\');
#endif
    pTemp = pTemp ? pTemp : pOutName;
    pTemp = strrchr(pOutName,'.');

    if (pTemp) *pTemp = '\0';

    return pFileName;
}

/*
* 函数：GetParamString
* 功能：获取命令行中 lpOption 选项的 value
* 说明：该函数会在命令行数组中搜索 lpOption 指定的选项，并将选项后的参数输出 lpDest, 如果出
*       现错误， lpDest 会指出出错的位置。
* 入口：
*       [in]        INT    uiArgc               数组中参数的个数
*       [in]        LPSTR  lpArgv[]             参数数组
*       [in]        LPSTR  lpOption             输入的选项字符串(选项必须前缀 '-' 符号)
*       [out]       LPSTR *lpDest               输出的字符串指针
* 出口：
*       如果成功，返回 0 否则返回非 0 值。
*/
static E_STATUS GetParamString(INT uiArgc,LPSTR lpArgv[], LPSTR lpOption, LPSTR * lpDest)
{
    INT i = 0;

    for(i=1; i<uiArgc; i++)
    {
        if (strcmp(lpArgv[i],lpOption))
        {
            continue;
        }

        if (i == uiArgc-1)
        {
             *lpDest = lpArgv[i];
            return STATE_INVALID_PARAMETER;
        }

        if (lpArgv[i+1]==NULL)
        {
            *lpDest = lpArgv[i];
            return STATE_INVALID_PARAMETER;
        }

        *lpDest = lpArgv[i+1];

        if (*lpArgv[i+1]=='\0')
        {
            *lpDest = lpArgv[i];
            return STATE_INVALID_PARAMETER;
        }

        if (strchr(lpArgv[i+1],'*') || strchr(lpArgv[i+1],'?'))
        {
            *lpDest = lpArgv[i+1];
            return STATE_INVALID_NAME;
        }

        if (*(lpArgv[i+1])=='-')
        {
            *lpDest = lpArgv[i+1];
            return STATE_NOT_SUPPORT;
        }

        break;
    }

    return STATE_SUCCESS;
}

/*
* 函数：CheckParam
* 功能：检测参数数组的有效性
* 说明：如果在 lpArgv 中发现 lpParam 数组中没有的选项，则返回该选项的指针以指出错误
*       的位置。
* 入口：
*       [in]        INT    uiArgc                 数组中参数的个数
*       [in]        LPSTR  lpArgv[]               参数数组
*       [in]        LPSTR  lpParam[]              本代码所能支持的参数列表
*       [in]        INT    uiCount                lpParam 的成员个数
* 出口：
*       如果出错，返回导致错误的选项指针，否则返回 NULL
*/
static LPSTR CheckParam(INT uiArgc,LPSTR lpArgv[], LPSTR lpParam[],INT uiCount)
{
    INT i,j;

    for (i=1; i<uiArgc; i++)
    {
        INT uiChk = 1;
        for(j=0; j<uiCount && uiChk; j++)
        {
            uiChk = *lpArgv[i]=='-' ? strcmp(lpArgv[i],lpParam[j]) : 0;
        }

        if(uiChk) return lpArgv[i];
    }
    return NULL;
}


/*
* 函数：ShowHelp
* 功能：检查命令行参数中是否有帮助选项
* 入口：
*       [in]        int    uiArgc                 数组中参数的个数
*       [in]        char * lpArgv[]               参数数组
* 出口：
*       如果命令行中有帮助选项，返回一个非 0 值，否则返回 0
*/
static E_STATUS ShowHelp(int uiArgc,char * lpArgv[])
{
    int i,j;
    LPSTR lpHelp[] = {"-h","-H","--h","--H","/?","-?","--help", "--HELP"};

    for (i=1;i<uiArgc;i++)
    {
        for(j=0;j<sizeof(lpHelp)/sizeof(lpHelp[0]);j++)
        {
            if(!strcmp(lpArgv[i],lpHelp[j]))
            {
                printf(MSG_HELP);
                return STATE_INVALID_PARAMETER;
            }
        }
    }

    return STATE_SUCCESS;
}



static E_STATUS GetParameter(INT uiArgc,LPSTR lpArgv[])
{
    LPSTR  lpParameter[] = {"-m", "-f", "-o"};
    INT i,uiCount = sizeof(lpParameter)/sizeof(lpParameter[0]);
    LPSTR * lpDest[] = {&lpCfgFile,&lpElfFile,&lpBinFile};
    LPSTR lpErrParam = CheckParam(uiArgc,lpArgv,lpParameter,uiCount);

    if (lpErrParam)
    {
        printf(MSG_NOT_SUPPORT,lpErrParam);
        return -1;
    }

    for (i=0;i<uiCount;i++)
    {

        switch (GetParamString(uiArgc,lpArgv,lpParameter[i],lpDest[i]))
        {
        case STATE_INVALID_PARAMETER:
            printf(MSG_NOFOUND_NAME,*lpDest[i]);
            return STATE_INVALID_PARAMETER;
        case STATE_INVALID_NAME:
            printf(MSG_LAWLESS_FILE,*lpDest[i]);
            return STATE_INVALID_NAME;
        case STATE_NOT_SUPPORT:
            printf(MSG_NOT_SUPPORT,*lpDest[i]);
            return STATE_NOT_SUPPORT;
        }
    }

    return STATE_SUCCESS;
}

BOOL SaveBinFile(LPSTR lpBinFile, LPVOID lpData, SIZE_T FileSize)
{
    SIZE_T WriteCount = 0;
    FILE * fp = fopen(lpBinFile, "wb");
    
    if (NULL == fp)
    {
        printf(MSG_FAILED_OPEN, lpBinFile);
        return FALSE;
    }
    
    WriteCount = fwrite(lpData, FileSize, 1, fp);
    
    if (FileSize != WriteCount * FileSize)
    {
        printf(MSG_FAILED_WRITE, lpBinFile, FileSize, WriteCount * FileSize);
        fclose(fp);
        return FALSE;
    }
    
    fclose(fp);
    
    return TRUE;
}

BOOL CreateBinFile(LPSTR lpBinFile, ELF_SECTION * lpSections)
{
    char * lpData = NULL;
    SIZE_T HeaderSize = lpSections[1].Address - lpSections[0].Address;
    SIZE_T FileSize = HeaderSize + lpSections[1].SectLen;
    
    if (HeaderSize < lpSections[0].SectLen + lpSections[2].SectLen)
    {
        printf(MSG_DATALENGTH_FAIL, lpSections[2].SectLen);
        return FALSE;
    }
    
    lpData = malloc(FileSize);
    
    if (NULL == lpData)
    {
        printf(MSG_MEMALLOC_FAIL, FileSize);
        return FALSE;
    }
    
    memset(lpData, 0, FileSize);

    //printf("", HeaderSize);
    memcpy(lpData, lpSections[0].Buffer, lpSections[0].DataLen);
    memcpy(lpData + 0x110, lpSections[2].Buffer, lpSections[2].DataLen);
    memcpy(lpData + HeaderSize, lpSections[1].Buffer, lpSections[1].DataLen);
    
    if (TRUE != SaveBinFile(lpBinFile, lpData, FileSize))
    {
        free(lpData);
        return FALSE;
    }
    
    free(lpData);
    
    return TRUE;
}


BOOL CreateConfig(LPSTR lpCfgFile, ELF_SECTION * lpSections)
{
    return TRUE;
}

BOOL AnalysisElfFile(LPSTR lpElfFile, LPSTR lpBinFile, LPSTR lpCfgFile)
{
    int i = 0;
    BOOL State = FALSE;
    ELFABI * Elfabi = NULL;
    ELF_SECTION * lpSections;

    Elfabi = ELF_OpenFile(lpElfFile);

    if (NULL == Elfabi)
    {
        return FALSE;
    }
    
    if (Elfabi->PhdrCount < 4)
    {
        printf(MSG_SECTION_FAIL, Elfabi->PhdrCount);
        return FALSE;
    }

    lpSections = malloc(sizeof(ELF_SECTION) * Elfabi->PhdrCount);
    
    if (NULL == lpSections)
    {
        return FALSE;
    }
    
    memset(lpSections, 0, sizeof(ELF_SECTION) * Elfabi->PhdrCount);

    printf(" 段类型      文件偏移  内存地址  文件长度  内存长度  特殊标志  对齐方式\n");

    for (i=0; i<Elfabi->PhdrCount; i++)
    {
        if (TRUE != ELF_LoadSection(Elfabi, &lpSections[i], i))
        {
            free(lpSections);
            ELF_CloseFile(Elfabi);
            return FALSE;
        }
    }

    if (TRUE != CreateBinFile(lpBinFile, lpSections))
    {
        free(lpSections);
        ELF_CloseFile(Elfabi);
        return FALSE;
    }
    
    if (TRUE != CreateConfig(lpCfgFile, lpSections))
    {
        free(lpSections);
        ELF_CloseFile(Elfabi);
        return FALSE;
    }
    
    ELF_CloseFile(Elfabi);

    return TRUE;
}



int main(INT uiArgc, LPSTR lpArgv[])
{
    show_version();

    if (ShowHelp(uiArgc,lpArgv)!=0)
    {
        return 0;
    }

    if(GetParameter(uiArgc,lpArgv)!=0)
    {
        getchar();
        return -1;
    }

    if (TRUE != AnalysisElfFile(lpElfFile, lpBinFile, lpCfgFile))
    {
        return -1;
    }

    return 0;
}