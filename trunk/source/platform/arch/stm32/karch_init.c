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
 *    2015-06-25     JiangYong       new file
 */

#include <string.h>
#include <misc.h>
#include <stm32f10x.h>
#include <stm32f10x_flash.h>
#include <stm32f10x_rcc.h>

#include <facore.h>
#include <fadefs.h>
#include <faerror.h>
#include <fatypes.h>

#define IRQ_PRIORITY(P0, P1)        ((P0 << 2) | P1)

#if (__CM3_CMSIS_VERSION == 0x10030)
#define SetIRQPriority(IRQn, Preemption, Response)      NVIC_SetContextThisPriority(IRQn, IRQ_PRIORITY(Response, Preemption))
#elif (__CM3_CMSIS_VERSION == 0x30001)
#define SetIRQPriority(IRQn, Preemption, Response)      NVIC_SetPriority(IRQn, IRQ_PRIORITY(Response, Preemption))
#else
#error "Can't support this CMSIS version."
#endif



PUBLIC VOID NVIC_EnableFault(VOID)
{
    SCB->CCR |= 0x18;
    SCB->SHCSR |= 0x00007000;
}

PUBLIC VOID NVIC_Configuration(VOID)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    SetIRQPriority (SysTick_IRQn, 0x1, 0x1);
    SetIRQPriority (SVCall_IRQn, 3, 0);     /* 抢占优先级3，响应优先级0 */
    SetIRQPriority (BusFault_IRQn, 0, 0);
    SetIRQPriority (UsageFault_IRQn, 0, 0);
    SetIRQPriority (MemoryManagement_IRQn, 0, 0);
    NVIC_EnableFault();
    
}

void NVIC_SetContextThisPriority2(IRQn_Type IRQn, uint32_t priority)
{
  if(IRQn < 0) {
    SCB->SHP[((uint32_t)(IRQn) & 0xF)-4] = ((priority << (8 - __NVIC_PRIO_BITS)) & 0xff); } /* set Priority for Cortex-M3 System Interrupts */
  else {
    NVIC->IP[(uint32_t)(IRQn)] = ((priority << (8 - __NVIC_PRIO_BITS)) & 0xff);    }        /* set Priority for device specific Interrupts  */
}

uint32_t SysTick_Config2(uint32_t ticks)
{ 
  if (ticks > SysTick_LOAD_RELOAD_Msk)  return (1);            /* Reload value impossible */
                                                               
  SysTick->LOAD  = (ticks & SysTick_LOAD_RELOAD_Msk) - 1;      /* set reload register */
  NVIC_SetContextThisPriority2 (SysTick_IRQn, (1<<__NVIC_PRIO_BITS) - 1);  /* set Priority for Cortex-M0 System Interrupts */
  SysTick->VAL   = 0;                                          /* Load the SysTick Counter Value */
  SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk | 
                   SysTick_CTRL_TICKINT_Msk   | 
                   SysTick_CTRL_ENABLE_Msk;                    /* Enable SysTick IRQ and SysTick Timer */
  return (0);                                                  /* Function successful */
}


PUBLIC VOID RCC_Configuration(VOID)
{
    /* 定义枚举类型变量 HSEStartUpStatus */
    ErrorStatus HSEStartUpStatus;

    /* 复位系统时钟设置*/
    RCC_DeInit();
    /* 开启HSE*/
    RCC_HSEConfig(RCC_HSE_ON);
    /* 等待HSE起振并稳定*/
    HSEStartUpStatus = RCC_WaitForHSEStartUp();
    /* 判断HSE起是否振成功，是则进入if()内部 */
    if(HSEStartUpStatus == SUCCESS)
    {
        /* 选择HCLK（AHB）时钟源为SYSCLK 1分频 */
        RCC_HCLKConfig(RCC_SYSCLK_Div1);
        /* 选择PCLK2时钟源为 HCLK（AHB） 1分频 */
        RCC_PCLK2Config(RCC_HCLK_Div1);
        /* 选择PCLK1时钟源为 HCLK（AHB） 2分频 */
        RCC_PCLK1Config(RCC_HCLK_Div2);
        /* 设置FLASH延时周期数为2 */
        FLASH_SetLatency(FLASH_Latency_2);
        /* 使能FLASH预取缓存 */
        FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
        /* 选择锁相环（PLL）时钟源为HSE 1分频，倍频数为9，则PLL输出频率为 8MHz * 9 = 72MHz */
        RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
        /* 使能PLL */
        RCC_PLLCmd(ENABLE);
        /* 等待PLL输出稳定 */
        while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);
        /* 选择SYSCLK时钟源为PLL */
        RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
        /* 等待PLL成为SYSCLK时钟源 */
        while(RCC_GetSYSCLKSource() != 0x08);
    }
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
    while(SysTick_Config2(SystemCoreClock/CONFIG_SYSTICK_FREQUENCY));
}

PUBLIC E_STATUS initCoreHardwareParameterForARCH(VOID)
{
    RCC_Configuration();
    NVIC_Configuration();
    
    return STATE_SUCCESS;
}
