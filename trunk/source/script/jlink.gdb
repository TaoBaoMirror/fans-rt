target remote localhost:2331
#b HardFault_Handler
#b MemManage_Handler
#b BusFault_Handler
#b UsageFault_Handler
#b DebugMon_Handler
#b TEST_CASE01_TASK
#b MUTEX_TEST_CASE01
load
c
x /8xw $sp
