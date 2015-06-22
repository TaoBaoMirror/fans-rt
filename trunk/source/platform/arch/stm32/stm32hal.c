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
#include "ktask.h"
#include "stm32hal.h"

PUBLIC VOID CORE_Starting(VOID);

PUBLIC DWORD CORE_GetCPUNumbers(VOID)
{
    return 1;
}

PUBLIC VOID CORE_SetArchContextParam(LPARCH_CONTEXT lpArchContext, LPVOID lpParam)
{
    LPKTASK_CREATE_PARAM lpTaskParam = lpParam;
    SetArchContextStackCapacity(lpArchContext, lpTaskParam->Param.StackSize);
    SetArchContextStackBuffer(lpArchContext, lpTaskParam->lpStackBuffer); 
    SetArchContextStackPosition(lpArchContext, lpTaskParam->lpStackPosition);
}

int main(VOID)
{
    CORE_DisableIRQ();
    RCC_Configuration();
    NVIC_Configuration();
    USART_Configuration();
    CORE_Starting();
    return 0;
}
