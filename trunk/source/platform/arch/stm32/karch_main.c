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
 
#include <string.h>
#include <stdio.h>
#include <stm32f10x.h>

#include <fadefs.h>
#include <faerror.h>
#include <fatypes.h>

#include "karch.h"
#include "kcore.h"

PUBLIC VOID M3_StackInitialize(VOID);

int main(VOID)
{
    CORE_DisableIRQ();
    M3_StackInitialize();
    CORE_Starting();
    return 0;
}

