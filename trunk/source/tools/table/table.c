/*****************************************************************************
*�� �� ��:  table.c
*��    ��:  ����
*��    ��:
*            2011-10-15            ����              �����ļ�
*��    ��:  beta 0.11.10.15
*��    ��:  ���Ҳ��滻�ļ��е� TAB ��Ϊ�ո�
*��    ��:  gcc -o table table.c
*****************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "language.h"

#define BAK_FILE            ".bak"                      /* �����ļ�����չ�� */
#define NO_FILE             -1                          
#define INVALID_FILE        -2
#define INVALID_PARAM       -3

static char o_file[1024];                               /* ����ı����ļ��� */
static char * i_file=NULL;                              /* ��Ҫ�滻���ļ��� */
static char * t_size="4";                               /* �������Ĭ�ϳ��� */
static unsigned int table_size = 4;

/*
* ������get_filename
* ���ܣ��ж��ļ�������Ч�ԣ�������һ�������ļ���
* ��ڣ�
*       [in]        char * file                     �����ļ���
*       [out]       char * out                      ����ı����ļ���
* ���ڣ�
*       ����ɹ��������ļ���ָ�룬���򷵻� NULL
*/
static char * get_filename(char * file,char * out)
{
    char * temp;
    if (*file=='\0')
    {
        return NULL;
    }

    if (strchr(file,'*') || strchr(file,'?'))
    {
        return NULL;
    }

    strcpy(out,file);
#if defined(__linux__)
    temp = strrchr(out,'/');
#else
    temp = strrchr(out,'\\');
#endif
    temp = temp ? temp : out;
    temp = strrchr(out,'.');
    if (temp) * temp = '\0';

    return file;
}

/*
* ������get_paramstr
* ���ܣ���ȡ�������� text ѡ��� value
* ˵�����ú����������������������� test ָ����ѡ�����ѡ���Ĳ������ dest, �����
*       �ִ��� dest ��ָ�������λ�á�
* ��ڣ�
*       [in]        int    argc                 �����в����ĸ���
*       [in]        char * argv[]               ��������
*       [in]        char * test                 �����ѡ���ַ���(ѡ�����ǰ׺ '-' ����)
*       [out]       char **dest                 ������ַ���ָ��
* ���ڣ�
*       ����ɹ������� 0 ���򷵻ط� 0 ֵ��
*/
static int get_paramstr(int argc,char * argv[], char * text, char * * dest)
{
    int i;
    for(i=1;i<argc-1;i++)
    {
        if (strcmp(argv[i],text))
        {
            continue;
        }

        if (argv[i+1]==NULL)
        {
            *dest = argv[i];
            return NO_FILE;
        }

        *dest = argv[i+1];

        if (*argv[i+1]=='\0')
        {
            *dest = argv[i];
            return NO_FILE;
        }

        if (strchr(argv[i+1],'*') || strchr(argv[i+1],'?'))
        {
            *dest = argv[i+1];
            return INVALID_FILE;
        }

        if (*(argv[i+1])=='-')
        {
            *dest = argv[i+1];
            return INVALID_PARAM;
        }
        break;
    }
    return 0;
}

/*
* ������check_param
* ���ܣ��������������Ч��
* ˵��������� argv �з��� param ������û�е�ѡ��򷵻ظ�ѡ���ָ����ָ������
*       ��λ�á�
* ��ڣ�
*       [in]        int    argc                 �����в����ĸ���
*       [in]        char * argv[]               ��������
*       [in]        char * param[]              ����������֧�ֵĲ����б�
*       [in]        int    count                param �ĳ�Ա����
* ���ڣ�
*       ����������ص��´����ѡ��ָ�룬���򷵻� NULL
*/
static char * check_param(int argc,char * argv[], char * param[],int count)
{
    int i,j;

    for (i=1;i<argc;i++)
    {
        int chk = 1;
        for(j=0;j<count && chk;j++)
        {
            chk = *argv[i]=='-' ? strcmp(argv[i],param[j]) : 0;
        }

        if(chk) return argv[i];
    }
    return NULL;
}

/*
* ������get_help
* ���ܣ���������в������Ƿ��а���ѡ��
* ��ڣ�
*       [in]        int    argc                 �����в����ĸ���
*       [in]        char * argv[]               ��������
* ���ڣ�
*       ������������а���ѡ�����һ���� 0 ֵ�����򷵻� 0
*/
static int get_help(int argc,char * argv[])
{
    int i,j;
    char * help[] = {"-h","-H","--h","--H","/?","-?","--help"};

    for (i=1;i<argc;i++)
    {
        for(j=0;j<sizeof(help)/sizeof(help[0]);j++)
        {
            if(!strcmp(argv[i],help[j])) return -1;
        }
    }

    return 0;
}

/*
* ������open_files
* ���ܣ����ļ�
* ��ڣ�
*       [in]        const char * dir            Ŀ���ļ����ڵ�Ŀ¼
*       [in]        const char * file           Ŀ���ļ����ļ���
*       [in]        const char * file           Ŀ���ļ�����չ��
*       [in]        const char * ext_name       Ŀ���ļ��Ĵ򿪷�ʽ
*                                               �򿪷�ʽ����Ϊ "rb" "wb" ��
* ���ڣ�
*       ����һ���ļ��������Ϊ NULL ��ʧ�ܣ�����ɹ�
*       
*/
static FILE * open_files(const char * dir, const char * file,const char * ext_name,const char * mode)
{
    FILE * stream;
    char buffer[1024] = {0};

    if (dir)
    {
        strncpy(buffer,dir,1023);
        if( buffer[strlen(buffer)]!='/' &&
            buffer[strlen(buffer)]!='\\' )
        {
#if defined(__linux__)
            strcat(buffer,"/");
#else
            strcat(buffer,"\\");
#endif
        }
    }


    if (NULL==file)
    {
        return NULL;
    }

    strcat(buffer,file);

    if (ext_name)
    {
        strcat(buffer,ext_name);
    }

    if ((stream=fopen(buffer,mode))==NULL)
    {
        printf(MSG_FAILED_OPEN,buffer);
    }

    return stream;
}

/*
* ������load_buffer
* ���ܣ����ļ�װ�뵽һ��������
* ��ڣ�
*       [in]        const char * file           Ŀ���ļ����ļ���
* ���ڣ�
*       ����ɹ������ػ�����ָ�룬���ʧ�ܣ��򷵻� NULL
*       
*/
static char * load_buffer(const char * file)
{
    FILE * temp;
    long length;
    char * buffer;

    if ((temp=open_files(NULL,file,NULL,"rb"))==NULL)
    {
        return NULL;
    }

    fseek(temp,0,SEEK_END);
    length = ftell(temp) + 2;
    fseek(temp,0,SEEK_SET);
    if (length==0)
    {
        printf(MSG_BUFFER_FAILE,i_file);
        return NULL;
    }

    if ((buffer=calloc(1,length))==NULL)
    {
        printf(MSG_BUFFER_FAILE,i_file);
        return NULL;
    }
    fread(buffer,length,1,temp);
    fclose(temp);

    return buffer;
}

static void free_buffer(char * buffer)
{
    free(buffer);
}

/*
* ������file_backup
* ���ܣ����������е�����д�뵽һ�������ļ���
* ��ڣ�
*       [in]        const char * file           Ŀ���ļ����ļ���
*       [in]        const char * buffer         ������ָ��
* ���ڣ�
*       ����ɹ����� 0�����򷵻ط� 0 ֵ��
*       
*/
static int file_backup(const char * buffer, const char * file)
{
    FILE * stream;

    if ((stream = open_files(NULL,file,BAK_FILE,"wb"))==NULL)
    {
        return 1;
    }

    while (*buffer)
    {
        fputc(*buffer++,stream);
    }

    fclose(stream);

    return 0;
}

/*
* ������put_space
* ���ܣ����ļ�д��ո��
* ��ڣ�
*       [in]        FILE * stream               Ŀ���ļ���ָ��
*       [in]        unsigned int scale          ���ڵĹ��λ��
*       [in]        unsigned int table          һ������ĳ���
* ���ڣ�
*       ��������˶��ٸ��ո�
*/
static unsigned int put_space(FILE * stream, unsigned int scale,unsigned int table)
{
    unsigned int i;
    unsigned int scmod = (scale%table);
    unsigned int count = table - scmod;

    for (i=0;i<count;i++)
    {
        fputc(' ',stream);
    }
    
    return count;
}

/*
* ������convert_table
* ���ܣ�ת���������е� TAB ��
* ��ڣ�
*       [in]        const char * buffer         ������ָ��
*       [in]        const char * file           �����Ŀ���ļ���
*       [in]        unsigned int table          һ������ĳ���
* ���ڣ�
*       �ɹ����� 0 ���򷵻ط� 0 ֵ��
*/
static int convert_table(const char * buffer,const char * file, unsigned int table)
{
    int scale=0;
    FILE * stream;

    if ((stream = open_files(NULL,file,NULL,"wb"))==NULL)
    {
        return 1;
    }

    while (*buffer)
    {
        switch(*buffer)
        {
        case '\t':
            scale += put_space(stream,scale,table);
            break;
        case '\n':
        case '\r':
            fputc(*buffer,stream);
            scale = 0;
            break;
        default:
            scale ++;
            fputc(*buffer,stream);
        }

        buffer ++;
    }

    fclose(stream);

    return 0;
}



static int get_param(int argc,char * argv[])
{
    char*  param[] = {"-t"};
    int i,count = sizeof(param)/sizeof(param[0]);
    char* * dest[] = {&t_size};
    char * err = check_param(argc,argv,param,count);

    if (err)
    {
        printf(MSG_INVALID_PARAM,err);
        return -1;
    }


    for (i=0;i<count;i++)
    {

        switch (get_paramstr(argc,argv,param[i],dest[i]))
        {
        case NO_FILE:
            printf(MSG_NOFOUND_NAME,*dest[i]);
            return NO_FILE;
        case INVALID_FILE:
            printf(MSG_LAWLESS_FILE,*dest[i]);
            return INVALID_FILE;
        case INVALID_PARAM:
            printf(MSG_INVALID_PARAM,*dest[i]);
            return INVALID_PARAM;
        }
    }

    return 0;
}

int main(int argc,char * argv[])
{
    char * buffer, * temp;

    if (get_help(argc,argv)!=0)
    {
        printf(MSG_HELP);
        return 0;
    }

    if (argc<2)
    {
        printf(MSG_INVALID_PCOUNT);
        return -1;
    }

    if (get_param(argc,argv)!=0)
    {
        return -1;
    }

    if ((i_file=get_filename(argv[1],o_file))==NULL)
    {
        printf(MSG_NOFOUND_FILE);
        return -1;
    }

    table_size = strtoul(t_size,&temp,0);

    if (table_size!=4 && table_size!=8)
    {
        printf(MSG_INVALID_SIZE,table_size);
        return -1;
    }

    if ((buffer=load_buffer(i_file))==NULL)
    {
        goto __fdown;
    }

    if (file_backup(buffer,o_file)!=0)
    {
        goto __fdown1;
    }

    if (convert_table(buffer,i_file,table_size)==0)
    {
        printf(MSG_LOGO);
        printf(MSG_INFILE,i_file);
        printf(MSG_OUTFILE,o_file);
        printf(MSG_TABLESIZE,table_size);
    }

__fdown1:
    free_buffer(buffer);
__fdown:
    
    return 0;
}

