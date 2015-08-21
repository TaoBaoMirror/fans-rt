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
 *    2015-07-19     JiangYong       new file
 */
#include "config.h"
#include <stm32f10x.h>
#include <facore.h>
#include <fadefs.h>
#include <faerror.h>
#include <fatypes.h>

RW_BOOT_STACK

STATIC DWORD RW_BOOT_STACK g_BootStackBuffer[CONFIG_BOOT_STACK_SIZE/sizeof(DWORD)];

PUBLIC LPVOID CORE_BootStackAddress(VOID)
{
    return g_BootStackBuffer;
}