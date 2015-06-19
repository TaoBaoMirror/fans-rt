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
#ifndef __CONFIG_H
#define __CONFIG_H

#define     CONFIG_BUILD_MINI_MAP               FALSE
#define     CONFIG_BUILD_DYNAMIC_SYMBOL         FALSE       /* 动态符号加载，编译为 ELF 文件 */
#define     CONFIG_MEM_DEBUG_ENABLE             FALSE       /* 内存管理模块 DEBUG 支持 */
#define     CONFIG_MEM_STATIC_FUNCTION          FALSE       /* 内存管理模块静态函数支持 */
#define     CONFIG_MEM_STATIC_PAGETABLE         FALSE       /* 内存管理模块静态页表支持 */
#define     CONFIG_MEM_REGION_MAX               0           /* 内存域数量 */
#define     CONFIG_MEM_BUDDY_MAX                8           /* 内存管理模块支持的 BUDDY 级数 */
#define     CONFIG_MEM_PAGE_SIZE                512         /* 系统页面大小 */
#define     CONFIG_MEM_ONCHIP_ADDRESS           0           /* 片上内存地址 */
#define     CONFIG_MEM_ONCHIP_LENGTH            0           /* 片上内存长度 */
#define     CONFIG_MEM_ONCHIP_PAGES                         \
            (CONFIG_MEM_ONCHIP_LENGTH/CONFIG_MEM_PAGE_SIZE) /* 片上内存页数量 */
#define     CONFIG_CORE_POOL_MAX                1           /**< 支持的最大 POOL 数量 */
#define     CONFIG_POOL_BLOCK_MAX               8           /**< 每个POOL的块数量 */
#define     CONFIG_TASK_PRIORITY_MAX            3           /**< 任务管理器支持的任务优先级数量 */

#define     CONFIG_CLASS_HASH_FUNCTION          Magic2Hash09
#define     CONFIG_SYSTEM_CLASS_MAX             8           /**< 内核对象类数量，最大为32 */
#define     CONFIG_SYSTEM_CLASS_MASK            0x7

#define     CONFIG_OBJECT_HASH_TABLE_MAX        1			/**< The number of hash table */
#define     CONFIG_OBJECT_POOL_TABLE_MAX        8           /**< Max number is 8,  see @HANDLE_OBJECT_TID_MASK */
#define     CONFIG_OBJECT_POOL_TABLE_MASK       0x7         /**< Max number is 8,  see @HANDLE_OBJECT_TID_MASK */
#define     CONFIG_OBJECT_SIZE_MAX              4096
#define     CONFIG_OBJECT_SIZE_MIN              32
#define     CONFIG_OBJECT_SIZE_SHIFT            5

#define     CONFIG_OBJECT0_POOL_MAX             1           /**< The max pools for 32 bytes class */
#define     CONFIG_OBJECT0_BLOCK_MAX            8           /**< The max blocks for 32 bytes class */

#define     CONFIG_OBJECT1_POOL_MAX             0           /**< The max pools for 64 bytes class */
#define     CONFIG_OBJECT1_BLOCK_MAX            0           /**< The max blocks for 64 bytes class */

#define     CONFIG_OBJECT2_POOL_MAX             1           /**< The max pools for 128 bytes class */
#define     CONFIG_OBJECT2_BLOCK_MAX            1           /**< The max blocks for 128 bytes class */

#define     CONFIG_OBJECT3_POOL_MAX             0           /**< The max pools for 256 bytes class */
#define     CONFIG_OBJECT3_BLOCK_MAX            0           /**< The max blocks for 256 bytes class */

#define     CONFIG_OBJECT4_POOL_MAX             0           /**< The max pools for 512 bytes class */
#define     CONFIG_OBJECT4_BLOCK_MAX            0           /**< The max blocks for 512 bytes class */

#define     CONFIG_OBJECT5_POOL_MAX             1           /**< The max pools for 1024 bytes class */
#define     CONFIG_OBJECT5_BLOCK_MAX            1           /**< The max blocks for 1024 bytes class */

#define     CONFIG_OBJECT6_POOL_MAX             0           /**< The max pools for 2048 bytes class */
#define     CONFIG_OBJECT6_BLOCK_MAX            0           /**< The max blocks for 2048 bytes class */

#define     CONFIG_OBJECT7_POOL_MAX             0           /**< The max pools for 4096 bytes class */
#define     CONFIG_OBJECT7_BLOCK_MAX            0           /**< The max blocks for 4096 bytes class */

#define     CONFIG_BUILD_MINI_MEMORY            TRUE
#define     CONFIG_DEFAULT_STACK_SIZE           1024        /**< 默认的堆栈长度 */
#define     CONFIG_BOOT_STACK_SIZE              1024        /**< 引导任务的堆栈长度 */
#define     CONFIG_IDLE_STACK_SIZE              1024        /**< 空闲任务的堆栈长度 */
#define     CONFIG_KERNEL_STACK_SIZE            0           /**< 内核堆栈长度 */
#ifndef     CONFIG_ARCH_SUPPORT_KSTACK
#define     CONFIG_ARCH_SUPPORT_KSTACK          FALSE       /**< CPU 是否支持内核堆栈 */
#endif
#define     CONFIG_ARCH_SUPPORT_SCHEDULE        FALSE       /**< CPU 是否支持任务调度中断 */
#define     CONFIG_ARCH_SUPPORT_MPU             FALSE       /**< CPU 是否支持内存保护单元 */
#define     CONFIG_DYNAMIC_STACK_ENABLE         FALSE       /**< 动态堆栈长度支持 */
#define     CONFIG_BUILD_TASK_PATH              FALSE       /**< 任务路径支持 */
#define     CONFIG_DEFAULT_PATH                 "/"         /**< 默认的任务路径 */
#define     CONFIG_SYSTICK_FREQUENCY            1000        /**< 内核时钟频率 */
#define     CONFIG_PROFILER_CYCLE               1000        /**< 系统性能统计刷新周期 */
#define     CONFIG_TIME_SLICE_NORMAL            10          /**< 默认任务时间片 */
#define     MILLI_SECOND_TO_TICK(Ms)            ((Ms) * (CONFIG_SYSTICK_FREQUENCY / 1000))
#define     CONFIG_ENABLE_FAST_SCHEDULE         TRUE
#define     CONFIG_LPC_HASH_FUNCTION            Magic2Hash01
#define     CONFIG_LPC_SERVICE_MAX              16
#define     CONFIG_SYSTEM_MODULES               8           /**< 系统模块数量 */

#endif

