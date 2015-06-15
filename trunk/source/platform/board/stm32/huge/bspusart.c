#include <string.h>
#include <stdio.h>
#include <misc.h>
#include <stm32f10x.h>
#include <stm32f10x_tim.h>
#include <stm32f10x_usart.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>

#include <fadefs.h>
#include <faerror.h>
#include <fatypes.h>
#include <famodule.h>


PUBLIC VOID BSP_UsartWriteByte(DWORD Data)
{
    while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    USART_SendData(USART1, Data); 
}

PUBLIC VOID BSP_DebugWriteByte(DWORD Data)
{
    if ('\0' != Data)
    {
        BSP_UsartWriteByte(Data);
    }
}


PUBLIC VOID USART_Configuration(VOID)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE); 
    USART_DeInit(USART1);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 
    GPIO_Init(GPIOA, &GPIO_InitStructure); 


    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure); 

    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; 

    USART_Init(USART1, &USART_InitStructure); 

    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure); 
    //USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

    USART_Cmd(USART1, ENABLE);
}

#if 0

#define USART_MAGIC         MAKE_DWORD('C','O','M')
STATIC E_STATUS IRQHandler(LPVOID lpPacket, LPVOID lpParam)
{
    if (TRUE != kDebugGetState())
    {
        return STATE_NOT_SUPPORT;
    }

    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        DEBUG_SaveRX(USART_ReceiveData(USART1));
        USART_ClearFlag(USART1, USART_FLAG_RXNE);
        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    }
    
    if(USART_GetITStatus(USART1, USART_IT_TXE) == SET)
    {
        DWORD dwData = DEBUG_LoadTX();
        if (INVALID_VALUE != dwData)
        {
            USART_SendData(USART1, dwData);
            USART_ClearFlag(USART1, USART_FLAG_TXE);
            USART_ClearITPendingBit(USART1, USART_IT_TXE);
        }
        else
        {
            USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
            DEBUG_StopSend();
        }
    }
    
    return STATE_SUCCESS;
}

DEFINE_IRQ_SERVICE(IRQDescriptor, USART_MAGIC, IRQ_SERVICE_USART, NULL, IRQHandler);


VOID HALBUG_StartSend(LPVOID lpPrivate)
{
    USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
}
#endif


