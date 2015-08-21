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
* ������GetFileName
* ���ܣ��ж��ļ�������Ч�ԣ�������һ�������ļ���
* ��ڣ�
*       [in]        char * file                     �����ļ���
*       [out]       char * out                      ����ı����ļ���
* ���ڣ�
*       ����ɹ��������ļ���ָ�룬���򷵻� NULL
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
* ������GetParamString
* ���ܣ���ȡ�������� lpOption ѡ��� value
* ˵�����ú����������������������� lpOption ָ����ѡ�����ѡ���Ĳ������ lpDest, �����
*       �ִ��� lpDest ��ָ�������λ�á�
* ��ڣ�
*       [in]        INT    uiArgc               �����в����ĸ���
*       [in]        LPSTR  lpArgv[]             ��������
*       [in]        LPSTR  lpOption             �����ѡ���ַ���(ѡ�����ǰ׺ '-' ����)
*       [out]       LPSTR *lpDest               ������ַ���ָ��
* ���ڣ�
*       ����ɹ������� 0 ���򷵻ط� 0 ֵ��
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
* ������CheckParam
* ���ܣ��������������Ч��
* ˵��������� lpArgv �з��� lpParam ������û�е�ѡ��򷵻ظ�ѡ���ָ����ָ������
*       ��λ�á�
* ��ڣ�
*       [in]        INT    uiArgc                 �����в����ĸ���
*       [in]        LPSTR  lpArgv[]               ��������
*       [in]        LPSTR  lpParam[]              ����������֧�ֵĲ����б�
*       [in]        INT    uiCount                lpParam �ĳ�Ա����
* ���ڣ�
*       ����������ص��´����ѡ��ָ�룬���򷵻� NULL
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
* ������ShowHelp
* ���ܣ���������в������Ƿ��а���ѡ��
* ��ڣ�
*       [in]        int    uiArgc                 �����в����ĸ���
*       [in]        char * lpArgv[]               ��������
* ���ڣ�
*       ������������а���ѡ�����һ���� 0 ֵ�����򷵻� 0
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

    printf(" ������      �ļ�ƫ��  �ڴ��ַ  �ļ�����  �ڴ泤��  �����־  ���뷽ʽ\n");

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