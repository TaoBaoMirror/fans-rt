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
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <fauser.h>
#include <fadefs.h>
#include "libcmini.h"

EXPORT RO_CODE int fputc(int ch, FILE * stream)
{
    return ch;
}

EXPORT RO_CODE int wfputc(int ch, FILE * stream)
{
    return ch;
}

