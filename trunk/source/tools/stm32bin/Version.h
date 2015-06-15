#ifndef __VERSION_H
#define __VERSION_H

#include <stdio.h>
#include "Language.h"

#define MAJOR_VERSION 00
#define MINOR_VERSION 01
#define DEBUG_VERSION 0001

static void show_version(void)
{
    printf(PROJECT_NAME " V%02d.%02d.%04d\r\n\r\n", MAJOR_VERSION, MINOR_VERSION, DEBUG_VERSION);
}

#endif
