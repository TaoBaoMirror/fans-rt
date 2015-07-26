target remote localhost:2331
b HardFault_Handler
b MemManage_Handler
b BusFault_Handler
b UsageFault_Handler
load
c
x /8xw $sp
