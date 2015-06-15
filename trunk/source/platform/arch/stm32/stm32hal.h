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
#ifndef __STM32HAL_H
#define __STM32HAL_H

#include <fadefs.h>
#include <faerror.h>
#include <fatypes.h>

#ifdef __cplusplus
extern "C" {
#endif
    PUBLIC VOID RCC_Configuration(VOID);
    PUBLIC VOID NVIC_Configuration(VOID);
    PUBLIC VOID GPIO_Configuration(VOID);
    PUBLIC VOID EXTI_Configuration(VOID);
    PUBLIC VOID USART_Configuration(VOID);
#ifdef __cplusplus
}
#endif

#endif

