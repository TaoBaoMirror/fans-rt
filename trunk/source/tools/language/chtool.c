#include <stdio.h>
#include "language.h"

#define FNT_FILE        ".fnt"
#define MSG_FILE        ".msg"


#define NO_FILE             -1
#define INVALID_FILE        -2
#define INVALID_PARAM       -3


char * font_file   = "d:\\fansdev\\fonts\\hzk16";
char * output_dir  = "..\\language";
char * output_file = "chinese";
char * input_file  = "message.txt";


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

static int create_msg(char * buffer,char * dir,char * name)
{
    int i;
    FILE * msg;
    unsigned int code = 0x80;


    if((msg = open_files(dir,name,MSG_FILE,"wb"))==NULL)
    {
        return 1;
    }

    fprintf(msg,LAB_FILEINFOR,name,MSG_FILE);
    fprintf(msg,"@LANGUAGE_MESSAGE:");

    for(i=0;i<strlen(buffer)+1;i++)
    {
        if((i & 0x07)==0)
        {
            fprintf(msg,"\x0d\x0a\tdb ");
        }
        else
        {
            fputc(',',msg);
        }
        if(buffer[i]>0)
        {
            fprintf(msg,"0x%02x",buffer[i] & 0xff);
        }
        else
        {
            fprintf(msg,"0x%02x",code++);
        }
        
    }

    fclose(msg);

    return 0;
}

static void out_fnt(FILE * fnt,unsigned char * buffer)
{
    int i,j;

    for(i=0;i<2;i++)
    {
        for(j=0;j<16;j++)
        {
            if((j&15)==0)
            {
                fprintf(fnt,"\x0d\x0a\tdb ");
            }
            else
            {
                fputc(',',fnt);
            }
            fprintf(fnt,"%3u",buffer[j*2+i] & 0xff);
        }
    }

}

static int create_fnt(unsigned char * buffer,char * dir,char * name,char * infile)
{
    int i = 0;
    FILE * fnt, * hzk;

    if((hzk = open_files(NULL,infile,NULL,"rb"))==NULL)
    {
        return 1;
    }

    if ((fnt = open_files(dir,name,FNT_FILE,"wb"))==NULL)
    {
        fclose(hzk);
        return 1;
    }

    fprintf(fnt,LAB_FILEINFOR,name,FNT_FILE);
    fprintf(fnt,"@LANGUAGE_BITMAPS:");
    
    while(buffer[i])
    {
        if(buffer[i]>=0x80)
        {
            unsigned char bitdata[32];
            off_t pos = ((buffer[i]-161) * 94 + (buffer[i+1]-161)) * 32;
            fseek(hzk,pos,SEEK_SET);
            fread(bitdata,32,1,hzk);
            out_fnt(fnt,bitdata);
            i++;
        }
        i++;
    }
    
    fclose(hzk);
    fclose(fnt);

    return 0;
}

static char * load_buffer(const char * file)
{
    FILE * temp;
    off_t length;
    char * buffer;

    if((temp=open_files(NULL,file,NULL,"rb"))==NULL)
    {
        return NULL;
    }

    fseek(temp,0,SEEK_END);
    length = ftell(temp) + 2;
    fseek(temp,0,SEEK_SET);
    if(length==0 || length >=128)
    {
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
    char*  param[] = {"-o","-m","-p","-font"};
    int i,count = sizeof(param)/sizeof(param[0]);
    char* * dest[] = {&output_file,&input_file,&output_dir,&font_file};
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

    printf(MSG_OUTDIR,output_dir);
    printf(MSG_OUTFILE,output_file,output_file);
    printf(MSG_INFILE,input_file);
    printf(MSG_CONTENT,buffer);

    if(create_msg(buffer,output_dir,output_file)!=0)
    {
        goto __fdown;
    }
    create_fnt( buffer,output_dir,output_file,font_file);

__fdown:
    free_buffer(buffer);
    return 0;
}


