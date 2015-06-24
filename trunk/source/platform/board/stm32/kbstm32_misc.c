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
#include <stm32f10x.h>
#include <facore.h>
#include <fadefs.h>
#include <faerror.h>
#include <fatypes.h>

#include "kirq_define_enum.h"
#include "kirq_define_name.h"
#include "kboard_name.h"

PUBLIC LPCSTR CORE_GetBoardName(VOID)
{
    return BOARD_NAME_STRING;
}

PUBLIC LPCSTR CORE_GetIRQNameString(DWORD IrqID)
{
    if (IrqID < SIZEOF_ARRAY(g_IRQNameString))
    {
        return g_IRQNameString[IrqID];
    }
    
    return "Unknow IRQ";
}
