/*
 *    Copyright(C) 2013-2015, Fans-rt development team.
 *
 *    All rights reserved.
 *
 *    This is open source software.
 *    Learning and research can be unrestricted to  modification, use and dissemination.
 *    If you need for commercial purposes, you should get the author's permission.
 *
 *    date           author          notes
 *    2014-09-07     JiangYong       new file
 */
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <fadefs.h>
#include <faerror.h>
#include <fatypes.h>

#include "kboard.h"
#include "kdebug.h"
#include "libcmini.h"

static int put_kernel_char(INT Ch, LPVOID lpPrivate)
{
    if ('\n' == Ch)
    {
        CORE_DebugWriteByte('\r');
    }
    
    CORE_DebugWriteByte(Ch);

    return Ch;
}


EXPORT E_STATUS kprintf(CONST CHAR * Format,...)
{
    va_list vargs;

    va_start (vargs, Format);
    (void) fa_vxnprintf(put_kernel_char, NULL, ~0, Format, vargs);
    va_end(vargs);

    return STATE_SUCCESS;
}
EXPORT_SYMBOL(kprintf);

EXPORT E_STATUS kwprintf(CONST WCHAR * Format,...)
{
    va_list vargs;

    va_start (vargs, Format);
    (void) fa_wvxnprintf(put_kernel_char, NULL, ~0, Format, vargs);
    va_end(vargs);

    return STATE_SUCCESS;
}
EXPORT_SYMBOL(kwprintf);
