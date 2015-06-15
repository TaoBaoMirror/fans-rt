/*****************************************************************************
*文 件 名:  table.c
*作    者:  姜勇
*修    订:
*            2011-10-15            姜勇              创建文件
*版    本:  beta 0.11.10.15
*功    能:  查找并替换文件中的 TAB 符为空格。
*编    译:  gcc -o table table.c
*****************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "language.h"

#define BAK_FILE            ".bak"                      /* 备份文件的扩展名 */
#define NO_FILE             -1                          
#define INVALID_FILE        -2
#define INVALID_PARAM       -3

static char o_file[1024];                               /* 输出的备份文件名 */
static char * i_file=NULL;                              /* 需要替换的文件名 */
static char * t_size="4";                               /* 跳格键的默认长度 */
static unsigned int table_size = 4;

/*
* 函数：get_filename
* 功能：判断文件名的有效性，并生成一个备份文件名
* 入口：
*       [in]        char * file                     输入文件名
*       [out]       char * out                      输出的备份文件名
* 出口：
*       如果成功，返回文件名指针，否则返回 NULL
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
* 函数：get_paramstr
* 功能：获取命令行中 text 选项的 value
* 说明：该函数会在命令行数组中搜索 test 指定的选项，并将选项后的参数输出 dest, 如果出
*       现错误， dest 会指出出错的位置。
* 入口：
*       [in]        int    argc                 数组中参数的个数
*       [in]        char * argv[]               参数数组
*       [in]        char * test                 输入的选项字符串(选项必须前缀 '-' 符号)
*       [out]       char **dest                 输出的字符串指针
* 出口：
*       如果成功，返回 0 否则返回非 0 值。
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
* 函数：check_param
* 功能：检测参数数组的有效性
* 说明：如果在 argv 中发现 param 数组中没有的选项，则返回该选项的指针以指出错误
*       的位置。
* 入口：
*       [in]        int    argc                 数组中参数的个数
*       [in]        char * argv[]               参数数组
*       [in]        char * param[]              本代码所能支持的参数列表
*       [in]        int    count                param 的成员个数
* 出口：
*       如果出错，返回导致错误的选项指针，否则返回 NULL
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
* 函数：get_help
* 功能：检查命令行参数中是否有帮助选项
* 入口：
*       [in]        int    argc                 数组中参数的个数
*       [in]        char * argv[]               参数数组
* 出口：
*       如果命令行中有帮助选项，返回一个非 0 值，否则返回 0
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
* 函数：open_files
* 功能：打开文件
* 入口：
*       [in]        const char * dir            目标文件所在的目录
*       [in]        const char * file           目标文件的文件名
*       [in]        const char * file           目标文件的扩展名
*       [in]        const char * ext_name       目标文件的打开方式
*                                               打开方式必须为 "rb" "wb" 等
* 出口：
*       返回一个文件流，如果为 NULL 则失败，否则成功
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
* 函数：load_buffer
* 功能：将文件装入到一个缓冲区
* 入口：
*       [in]        const char * file           目标文件的文件名
* 出口：
*       如果成功，返回缓冲区指针，如果失败，则返回 NULL
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
* 函数：file_backup
* 功能：将缓冲区中的内容写入到一个备份文件。
* 入口：
*       [in]        const char * file           目标文件的文件名
*       [in]        const char * buffer         缓冲区指针
* 出口：
*       如果成功返回 0，否则返回非 0 值。
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
* 函数：put_space
* 功能：向文件写入空格符
* 入口：
*       [in]        FILE * stream               目标文件流指针
*       [in]        unsigned int scale          行内的光标位置
*       [in]        unsigned int table          一个跳格的长度
* 出口：
*       返回输出了多少个空格。
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
* 函数：convert_table
* 功能：转换缓冲区中的 TAB 符
* 入口：
*       [in]        const char * buffer         缓冲区指针
*       [in]        const char * file           保存的目标文件名
*       [in]        unsigned int table          一个跳格的长度
* 出口：
*       成功返回 0 否则返回非 0 值。
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

