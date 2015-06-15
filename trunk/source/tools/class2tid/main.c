#include <stdio.h>
#include <string.h>
#include "language.h"

#define NO_FILE             -1
#define INVALID_FILE        -2
#define INVALID_PARAM       -3
#define CONFIG_MAX          1024

typedef struct tagCONFIG{
    char *          name;
    char *          value;
}CONFIG;

char * output_file = "tableid.c";
char * input_file  = "config.h";
int object[8] = {-1, -1, -1, -1, -1, -1, -1, -1};
CONFIG g_config[CONFIG_MAX] = {NULL};

static int get_filename(int argc,char * argv[], char * text, char * * dest)
{
    int i;
    for(i=1;i<argc-1;i++)
    {
        if(strcmp(argv[i],text))
        {
            continue;
        }

        if(argv[i+1]==NULL)
        {
            *dest = argv[i];
            return NO_FILE;
        }

        *dest = argv[i+1];

        if(*argv[i+1]=='\0')
        {
            *dest = argv[i];
            return NO_FILE;
        }

        if(strchr(argv[i+1],'*') || strchr(argv[i+1],'?'))
        {
            *dest = argv[i+1];
            return INVALID_FILE;
        }

        if(*(argv[i+1])=='-')
        {
            *dest = argv[i+1];
            return INVALID_PARAM;
        }
        break;
    }
    return 0;
}

static char * check_param(int argc,char * argv[], char * param[],int count)
{
    int i,j;

    for(i=1;i<argc;i++)
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

static int get_help(int argc,char * argv[])
{
    int i,j;
    char * help[] = {"-h","-H","--h","--H","/?","-?","--help"};

    for(i=1;i<argc;i++)
    {
        for(j=0;j<sizeof(help)/sizeof(help[0]);j++)
        {
            if(!strcmp(argv[i],help[j])) return -1;
        }
    }

    return 0;
}

static FILE * open_files(const char * dir, const char * file,const char * ext_name,const char * mode)
{
    FILE * stream;
    char buffer[1024] = {0};

    if(dir)
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


    if(NULL==file)
    {
        return NULL;
    }

    strcat(buffer,file);

    if(ext_name)
    {
        strcat(buffer,ext_name);
    }

    if ((stream=fopen(buffer,mode))==NULL)
    {
        printf(MSG_FAILED_OPEN,buffer);
    }

    return stream;
}


static int create_table(unsigned char * buffer, char * name)
{
    int i = 0;
    FILE * fp = open_files(NULL, name, NULL,"wb");

    if (NULL == fp)
    {
        printf(MSG_FAILED_OPEN, name);
        fclose(fp);
        return 1;
    }

    fprintf(fp,LAB_FILEINFOR,name);
    fprintf(fp,"CONST BYTE g_ObjectSizeTable[256]{\x0d\x0a");
    
    //load_config(buffer, object);

    fclose(fp);
    
    return 0;
}


static char * get_define(char * src, char * * value, char * * next)
{
    char * novalue = "";
    char * define = NULL;
    char * find = strstr(src, "#define");
    
    *next = NULL;
    *value = novalue;

    if (find)
    {
        find += strlen("#define");

        while(' ' == *find || '\t' == *find) find ++;
        if ('\0' == *find) return NULL;
        
        define = find;

        while(' ' != *find && '\t' != *find) 
        {
            if ('\0' == *find) return define;
            if ('\x0d' == *find ||'\x0a' == *find)
            {
                *(find++) = '\0';
                *next = find;
                return define;
            }
            find ++;
        }
        *(find++) = '\0';
        //*value = find;
        
        while(' ' == *find || '\t' == *find) find ++;
        if ('\0' == *find) return define;
        if ('\x0d' == *find ||'\x0a' == *find)
        {
            *(find++) = '\0';
            *next = find;
            return define;
        }
        
        *value = find;
        
        while(' ' != *find && '\t' != *find) 
        {
            if ('\0' == *find) return define;
            if ('\x0d' == *find ||'\x0a' == *find)
            {
                *(find++) = '\0';
                *next = find;
                return define;
            }
            find ++;
        }
        
        *(find++) = '\0';
        *next = find;
    }
    
    return define;
}



static int load_config(char * buffer)
{
    char * next = buffer;
    char * define = NULL;
    char * value = NULL;

    define = get_define(next, &value, &next);
    printf("define : '%s'\n", define);
    printf("value : '%s'\n", value);

#if 0
    snprintf(define, sizeof(define)-1, "CONFIG_OBJECT%d_SIZE", 0);


    printf("find %s, buffer is %p, find is %p:\n", define, buffer, find);
    printf("%s", find);
#endif
    return 0;
}

static char * load_buffer(const char * file)
{
    FILE * temp;
    long length;
    char * buffer;

    if((temp=open_files(NULL,file,NULL,"rb"))==NULL)
    {
        return NULL;
    }

    fseek(temp,0,SEEK_END);
    length = ftell(temp) + 2;
    fseek(temp,0,SEEK_SET);
    if(length==0 || length < 256)
    {
        printf (MSG_INVALID_FILESIZE, file, length);
        fclose(temp);
        return NULL;
    }
    
    buffer = calloc(1,length);
    fread(buffer,length,1,temp);
    fclose(temp);

    return buffer;
}

static void free_buffer(char * buffer)
{
    free(buffer);
}


static int get_argv(int argc,char * argv[])
{
    char*  param[] = {"-o","-c"};
    int i,count = sizeof(param)/sizeof(param[0]);
    char* * dest[] = {&output_file,&input_file};
    char * err = check_param(argc,argv,param,count);

    if(err)
    {
        printf(MSG_INVALID_PARAM,err);
        return -1;
    }

    for(i=0;i<count;i++)
    {

        switch(get_filename(argc,argv,param[i],dest[i]))
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
    char * buffer;

    if(get_help(argc,argv)!=0)
    {
        printf(MSG_HELP);
        return 0;
    }

    if(get_argv(argc,argv)!=0)
    {
        return -1;
    }


    if ((buffer=load_buffer(input_file))==NULL)
    {
        return -1;
    }
    
    if (0 != load_config(buffer))
    {
        free_buffer(buffer);
        return -1;
    }

    printf(MSG_OUTFILE,output_file);
    printf(MSG_INFILE,input_file);
    
    if (0 != create_table(buffer, output_file))
    {
        free_buffer(buffer);
        return -1;
    }

__fdown:
    free_buffer(buffer);
    return 0;
}


