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
#define     CONFIG_BUILD_DYNAMIC_SYMBOL         FALSE       /* ��̬���ż��أ�����Ϊ ELF �ļ� */
#define     CONFIG_MEM_DEBUG_ENABLE             FALSE       /* �ڴ����ģ�� DEBUG ֧�� */
#define     CONFIG_MEM_STATIC_FUNCTION          FALSE       /* �ڴ����ģ�龲̬����֧�� */
#define     CONFIG_MEM_STATIC_PAGETABLE         FALSE       /* �ڴ����ģ�龲̬ҳ��֧�� */
#define     CONFIG_MEM_REGION_MAX               0           /* �ڴ������� */
#define     CONFIG_MEM_BUDDY_MAX                8           /* �ڴ����ģ��֧�ֵ� BUDDY ���� */
#define     CONFIG_MEM_PAGE_SIZE                512         /* ϵͳҳ���С */
#define     CONFIG_MEM_ONCHIP_ADDRESS           0           /* Ƭ���ڴ��ַ */
#define     CONFIG_MEM_ONCHIP_LENGTH            0           /* Ƭ���ڴ泤�� */
#define     CONFIG_MEM_ONCHIP_PAGES                         \
            (CONFIG_MEM_ONCHIP_LENGTH/CONFIG_MEM_PAGE_SIZE) /* Ƭ���ڴ�ҳ���� */
#define     CONFIG_CORE_POOL_MAX                1           /**< ֧�ֵ���� POOL ���� */
#define     CONFIG_POOL_BLOCK_MAX               8           /**< ÿ��POOL�Ŀ����� */
#define     CONFIG_TASK_PRIORITY_MAX            3           /**< ���������֧�ֵ��������ȼ����� */

#define     CONFIG_CLASS_HASH_FUNCTION          Magic2Hash09
#define     CONFIG_SYSTEM_CLASS_MAX             8           /**< �ں˶��������������Ϊ32 */
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
#define     CONFIG_DEFAULT_STACK_SIZE           1024        /**< Ĭ�ϵĶ�ջ���� */
#define     CONFIG_BOOT_STACK_SIZE              1024        /**< ��������Ķ�ջ���� */
#define     CONFIG_IDLE_STACK_SIZE              1024        /**< ��������Ķ�ջ���� */
#define     CONFIG_KERNEL_STACK_SIZE            0           /**< �ں˶�ջ���� */
#ifndef     CONFIG_ARCH_SUPPORT_KSTACK
#define     CONFIG_ARCH_SUPPORT_KSTACK          FALSE       /**< CPU �Ƿ�֧���ں˶�ջ */
#endif
#define     CONFIG_ARCH_SUPPORT_SCHEDULE        FALSE       /**< CPU �Ƿ�֧����������ж� */
#define     CONFIG_ARCH_SUPPORT_MPU             FALSE       /**< CPU �Ƿ�֧���ڴ汣����Ԫ */
#define     CONFIG_DYNAMIC_STACK_ENABLE         FALSE       /**< ��̬��ջ����֧�� */
#define     CONFIG_BUILD_TASK_PATH              FALSE       /**< ����·��֧�� */
#define     CONFIG_DEFAULT_PATH                 "/"         /**< Ĭ�ϵ�����·�� */
#define     CONFIG_SYSTICK_FREQUENCY            1000        /**< �ں�ʱ��Ƶ�� */
#define     CONFIG_PROFILER_CYCLE               1000        /**< ϵͳ����ͳ��ˢ������ */
#define     CONFIG_TIME_SLICE_NORMAL            10          /**< Ĭ������ʱ��Ƭ */
#define     MILLI_SECOND_TO_TICK(Ms)            ((Ms) * (CONFIG_SYSTICK_FREQUENCY / 1000))
#define     CONFIG_ENABLE_FAST_SCHEDULE         TRUE
#define     CONFIG_LPC_HASH_FUNCTION            Magic2Hash01
#define     CONFIG_LPC_SERVICE_MAX              16
#define     CONFIG_SYSTEM_MODULES               8           /**< ϵͳģ������ */

#endif

