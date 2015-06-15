#ifndef __H_E_IRQ_ID
#define __H_E_IRQ_ID

typedef enum{
    E_HARD_USART_IRQ_ID  =  0,
    E_HARDWARE_IRQ_MAX  =  1,
    E_CORE_SYSTICK_IRQ_ID  =  1,
    E_CORE_SYSCALL_IRQ_ID  =  2,
    E_SYSTEM_IRQ_MAX  =  3,
    E_CORE_IRQ_MAX  =  E_CORE_SYSCALL_IRQ_ID-E_HARDWARE_IRQ_MAX,
}E_IRQ_ID;

#endif

