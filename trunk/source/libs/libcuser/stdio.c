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
 *    2014-09-07     JiangYong       new file
 */
#include "config.h"
#include <limits.h>
#include <stddef.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <fauser.h>
#include <fadefs.h>
#include <fatypes.h>

struct __FILE{
    int             fd;
};

EXPORT RW_DATA FILE * stdin = NULL;
EXPORT RW_DATA FILE * stdout = NULL;
EXPORT RW_DATA FILE * stderr = NULL;

