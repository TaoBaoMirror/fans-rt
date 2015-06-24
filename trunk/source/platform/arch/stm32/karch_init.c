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
    SetIRQPriority (SVCall_IRQn, 3, 0);     /* ��ռ���ȼ�3����Ӧ���ȼ�0 */
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
    /* ����ö�����ͱ��� HSEStartUpStatus */
    ErrorStatus HSEStartUpStatus;

    /* ��λϵͳʱ������*/
    RCC_DeInit();
    /* ����HSE*/
    RCC_HSEConfig(RCC_HSE_ON);
    /* �ȴ�HSE�����ȶ�*/
    HSEStartUpStatus = RCC_WaitForHSEStartUp();
    /* �ж�HSE���Ƿ���ɹ����������if()�ڲ� */
    if(HSEStartUpStatus == SUCCESS)
    {
        /* ѡ��HCLK��AHB��ʱ��ԴΪSYSCLK 1��Ƶ */
        RCC_HCLKConfig(RCC_SYSCLK_Div1);
        /* ѡ��PCLK2ʱ��ԴΪ HCLK��AHB�� 1��Ƶ */
        RCC_PCLK2Config(RCC_HCLK_Div1);
        /* ѡ��PCLK1ʱ��ԴΪ HCLK��AHB�� 2��Ƶ */
        RCC_PCLK1Config(RCC_HCLK_Div2);
        /* ����FLASH��ʱ������Ϊ2 */
        FLASH_SetLatency(FLASH_Latency_2);
        /* ʹ��FLASHԤȡ���� */
        FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
        /* ѡ�����໷��PLL��ʱ��ԴΪHSE 1��Ƶ����Ƶ��Ϊ9����PLL���Ƶ��Ϊ 8MHz * 9 = 72MHz */
        RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
        /* ʹ��PLL */
        RCC_PLLCmd(ENABLE);
        /* �ȴ�PLL����ȶ� */
        while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);
        /* ѡ��SYSCLKʱ��ԴΪPLL */
        RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
        /* �ȴ�PLL��ΪSYSCLKʱ��Դ */
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
