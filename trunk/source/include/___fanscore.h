#ifndef __FANSCORE_H
#define __FANSCORE_H

#include <fadefs.h>
#include <faerror.h>
#include <fatypes.h>
#include <faklist.h>
#include <fapi.h>

#define     INVALID_CLASSID             INVALID_VALUE   /**< ��Ч�������� */
#define     INVALID_MAGIC               INVALID_VALUE

#define     POL_MAGIC                   MAKE_DWORD('P', 'O', 'L')
#define     HEADER_MAGIC                MAKE_DWORD('H', 'D', 'R')
#define     CLOCK_MAGIC                 MAKE_DWORD('T', 'I', 'M')
#define     CLASS_MAGIC                 MAKE_DWORD('C', 'L', 'S')
#define     STK_MAGIC                   MAKE_DWORD('S', 'T', 'K')

#define     IDL_MAGIC                   MAKE_DWORD('I', 'D', 'L')
#define     APP_MAGIC                   MAKE_DWORD('A', 'P', 'P')

#define     LPC_MAGIC                   MAKE_DWORD('L', 'P', 'C')
#define     IRQ_MAGIC                   MAKE_DWORD('I', 'R', 'Q')
#define     SMM_MAGIC                   MAKE_DWORD('S', 'M', 'M')
#define     STM_MAGIC                   MAKE_DWORD('T', 'S', 'S')


#define     SCM_MAGIC                   MAKE_DWORD('S', 'C', 'M')               /**< System clock manager */
#define     SDM_MAGIC                   MAKE_DWORD('S', 'D', 'M')               /**< System debug manager */
#define     SYSTEM_CALL_OOPS()          kSystemOops()


#if 0
typedef enum{
    SIR_DIVISION        =       0x00,               /**< �����쳣*/
    SIR_STEPINT         =       0x01,               /**< �����ж� */
    SIR_NMICALL         =       0x02,               /**< NMI �ж� */
    SIR_BREAK           =       0x03,               /**< �ϵ��ж� */
    SIR_OVERFLOW        =       0x04,               /**< ����ж� */
    SIR_BOUND           =       0x05,               /**< Խ���ж� (X86)*/
    SIR_INVALID         =       0x06,               /**< ��Чָ�� */
    SIR_FPUEM           =       0x07,               /**< Э���������� */
    SIR_DOUBLE          =       0x08,               /**< ˫�ع��� (X86)*/
    SIR_TASKFAULT       =       0x09,               /**< ������� (X86 ��ЧTSS)*/
    SIR_STACKFAULT      =       0x0a,               /**< ��ջ�쳣 */
    SIR_SECTFAULT       =       0x0b,               /**< �ι���(X86ͨ�ñ����쳣-��Խ�硢������������) */
    SIR_TLBSAVE         =       0x0c,               /**< ��һ�������ڵ��ڴ��ַ */
    SIR_TLBLOAD         =       0x0d,               /**< дһ�������ڵ��ڴ��ַ */
    SIR_TLBPROTECT      =       0x0e,               /**< �ڴ汣���쳣(������λ�дֻ����) */
    SIR_IOPROTECT       =       0x0f,               /**< IO�����쳣 */
    SIR_EXECFAULT       =       0x10,               /**< ִ�д���(ִ�в���ִ�еĶ�) */
    SIR_CTRLFAULT       =       0x11,               /**< ���ƹ���-X86 CR0 */
    SIR_PAGELACK        =       0x12,               /**< ȱҳ�ж� */
    SIR_SECTLACK        =       0x13,               /**< �β����� (X86) */
    SIR_MEMDEBUG        =       0x14,               /**< �ڴ����쳣(MIPS) */
    SIR_SYSCALL         =       0x15,               /**< ϵͳ���� */

    SIR_MAXCODE,
}E_SIR_CODE;
#endif




typedef struct tagKOBJECT_HEADER KOBJECT_HEADER;
typedef struct tagKOBJECT_HEADER * PKOBJECT_HEADER;
typedef struct tagKOBJECT_HEADER FAR * LPKOBJECT_HEADER;

typedef enum{
    KOBJECT_STATE_DEATH     =       0,                          /**< ����״̬ */
    KOBJECT_STATE_CREATE        =       1,                          /**< ����״̬ */
    KOBJECT_STATE_ACTIVE        =       2,                          /**< �״̬ */
    KOBJECT_STATE_REMOVE        =       3,                          /**< ɾ��״̬ */
    KOBJECT_STATE_DETACH        =       4,                          /**< ����״̬ */
    KOBJECT_STATE_MAX       =       5,
}KOBJECT_STATE, * PKOBJECT_STATE, FAR * LPKOBJECT_STATE;

typedef struct tagOBJECT_METHODS OBJECT_METHODS;
typedef struct tagOBJECT_METHODS * POBJECT_METHODS;
typedef struct tagOBJECT_METHODS FAR * LPOBJECT_METHODS;

typedef struct tagOBJECT_CLASS OBJECT_CLASS;
typedef struct tagOBJECT_CLASS * POBJECT_CLASS;
typedef struct tagOBJECT_CLASS FAR * LPOBJECT_CLASS;


struct tagOBJECT_METHODS
{
    E_STATUS        (FAR * CreateObject)(LPKOBJECT_HEADER Header, LPVOID lpParam);
    E_STATUS        (FAR * DeleteObject)(LPKOBJECT_HEADER Header);                  /* ɾ������ */
};


#define DEFINE_CLASS(Symbol, Model, Length, Methods)                            \
        STATIC OBJECT_CLASS Symbol = {                                          \
            CLASS_MAGIC,                                    /* dwMagic */       \
            { Model },                                      /* ClassModel */    \
            0,                                              /* ClassID */       \
            1,                                              /* RefCount */      \
            Length,                                         /* ObjectLength */  \
            { &Symbol.ClassHead, &Symbol.ClassHead },       /* ClassHead */     \
            Methods                                         /* Funcs */         \
        }

struct tagOBJECT_CLASS{
    DWORD                       Magic;
    union{
        DWORD                   ClassModel;
        CHAR                    ClassName[4];
    }u;
    BYTE                        ClassID;
    BYTE                        RefCount;
    WORD                        ObjectLength;
    LIST_HEAD                   ClassHead;
    LPOBJECT_METHODS            Funcs;
};

typedef struct tagTIMER_CLOCK TIMER_CLOCK;
typedef struct tagTIMER_CLOCK * PTIMER_CLOCK;
typedef struct tagTIMER_CLOCK FAR * LPTIMER_CLOCK;

#ifdef __cplusplus
extern "C" {
#endif
    EXPORT VOID kSystemOops(VOID);
    EXPORT E_STATUS kprintf(CONST CHAR * Format,...);
    EXPORT LPVOID CORE_GetIdleStackBuffer(VOID);
    
#ifdef __cplusplus
}
#endif

#endif
