#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "libtools.h"
#include "language.h"

#define         ROUTINE_PATH            "c:\\windows\\system32\\rt-vdisk.sys"
#define         IMAGE_PATH              "c:\\fans-rt\\fans-rt_0.12\\fans-rt.img"
#define         DRIVER_PATH             "A"
#define         ARGUMENT_TOTAL          SIZEOF_ARRAY(g_parameter_descriptor)

static parameter_descriptor g_parameter_descriptor[] = 
{
    {"-h",     DT_HELP,      0,    NULL,          COMMENT1_HELP,     COMMENT2_HELP       },
    {"-u",     DT_FLAG,      0,    NULL,          COMMENT1_UNSTALL,  COMMENT2_UNSTALL    },
    {"-r",     DT_STRING,    0,    ROUTINE_PATH,  COMMENT1_ROUTINE,  COMMENT2_ROUTINE    },
    {"-i",     DT_STRING,    0,    IMAGE_PATH,    COMMENT1_IMAGE,    COMMENT2_IMAGE      },
    {"-d",     DT_STRING,    0,    DRIVER_PATH,   COMMENT1_DRIVER,   COMMENT2_DRIVER     },
};

static tool_descriptor g_tool_descriptor =
{
    TOOL_NAME,
    USAGE_HELP,
    EXAMPLE_HELP,
    ARGUMENT_TOTAL,
    g_parameter_descriptor,
    NULL,
};

int main(int argc,char * argv[])
{
    int value = -2;
    int result = 0;
    char * driver = NULL;
    char * device = NULL;

    if(0 != (result = descriptor_scan_argument(argc, argv, &g_tool_descriptor)))
    {
        return result;
    }
    
    descriptor_get_number(&g_tool_descriptor, "-h", &value);
    
    if (1 == value)
    {
        descriptor_show_help(&g_tool_descriptor, 80, 20);
        return 0;
    }
    
    if (0 == descriptor_get_string(&g_tool_descriptor, "-r", &driver))
    {
        printf("driver : %s\n", driver);
    }
    
    if (0 == descriptor_get_string(&g_tool_descriptor, "-d", &device))
    {
        printf("device : %s\n", device);
    }

    return 0;
}


