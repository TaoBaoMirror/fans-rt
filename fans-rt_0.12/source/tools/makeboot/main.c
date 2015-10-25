/*
 *    Copyright(C) 2013-2015, Fans-rt development team.
 *
 *    All rights reserved.
 *
 *    This is open source software.
 *    Learning and research can be unrestricted to modification, use and dissemination.
 *    If you need for commercial purposes, you should get the author's permission.
 *
 *    date           author          notes
 *    2015-10-11     JiangYong       new file
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "libtools.h"
#include "makeboot.h"
#include "language.h"


extern BOOT_SECTORS boot_sectors;

#define     SECTOR_LENGTH(Start, Final)     ((size_t)((char *)(&(Final)) - (char *)(&(Start))))


TOOL_RUNTIME_OBJECT g_Object =
{
    &boot_sectors,
    {0},
    FSID_NULL,
    0,
    {0}
};

#define         IMAGE_PATH              "g:/fans-rt/virtualmachine/images/floppy.img"
#define         ARGUMENT_TOTAL          SIZEOF_ARRAY(g_parameter_descriptor)

static parameter_descriptor g_parameter_descriptor[] = 
{
    {"-help", DT_HELP, 0, NULL,       COMMENT1_HELP,  COMMENT2_HELP,   NULL,  0},
    {"-f",    DT_FILE, 0, IMAGE_PATH, COMMENT1_IMAGE, COMMENT2_IMAGE,  g_Object.FullPath,  MAX_PATH},
};

static tool_descriptor g_tool_descriptor =
{
    TOOL_NAME,
    NULL,
    NOTES_HELP,
    USAGE_HELP,
    ARGUMENT_HELP,
    EXAMPLE_HELP,
    80,
    20,
    ARGUMENT_TOTAL,
    g_parameter_descriptor,
    &g_Object,
};

int main(int argc,char * argv[])
{
    int result = 0;

    if(0 != (result = tool_scan_argument(argc, argv, &g_tool_descriptor)))
    {
        return result;
    }

    printf("pppp: %p.\n", g_Object.FullPath);

    return tool_run(&g_tool_descriptor);
}


