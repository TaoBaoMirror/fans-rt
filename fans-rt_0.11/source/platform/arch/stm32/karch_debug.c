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
 
#include <string.h>
#include <stdio.h>
#include <stm32f10x.h>

#include <fadefs.h>
#include <faerror.h>
#include <fatypes.h>

#include "kcore.h"
#include "karch.h"
#include "ktask.h"
#include "kboard.h"
#include "kdebug.h"

#define     CM3_FSR         0xe000ed28
#define     CM3_HFSR        0xe000ed2c
#define     CM3_MMAR        0xe000ed34
#define     CM3_BFAR        0xe000ed38

PUBLIC VOID CORE_DebugMonitor(LPSTACK_DUMP lpStack)
{
    DWORD Exception_Fsr = * ((DWORD_PTR) CM3_FSR);
    DWORD Exception_Hsr = * ((DWORD_PTR) CM3_HFSR);
    LPINSTRUCTION_BIT_FIELD lpField = (LPVOID) lpStack->PC;

    if (Exception_Fsr)
    {
        kprintf(" **** Task '%p-%s' exception ****\n", CORE_GetCurrentTask(), CORE_GetCurrentTaskName());
        kprintf(" R0 = %08X  R1 = %08X  R2 = %08X  R3 = %08X\n",
                lpStack->R0, lpStack->R1, lpStack->R2, lpStack->R3);
        kprintf(" R4 = %08X  R5 = %08X  R6 = %08X  R7 = %08X\n",
                lpStack->R4, lpStack->R5, lpStack->R6, lpStack->R7);
        kprintf(" R8 = %08X  R9 = %08X  R10= %08X  R1 = %08X\n",
                lpStack->R8, lpStack->R9, lpStack->R10, lpStack->R11);
        kprintf(" R12= %08X  MSP= %08X  BP = %08X  PC = %08X\n",
                lpStack->R12, lpStack, lpStack->BP, lpStack->PC);
        kprintf(" PSR= %08X  LR = %08X\n", lpStack->xPSR, lpStack->LR);
        kprintf(" Exception msk: 0x%08LX\n", Exception_Fsr);
        
        if (Exception_Fsr & (1<<0))
        {
            kprintf(" ***Memory fault: Code fault !\n");
        }
        
        if (Exception_Fsr & (1<<1))
        {
            kprintf(" ***Memory fault: Data fault !\n");
        }
        
        if (Exception_Fsr & (1<<3))
        {
            kprintf(" ***Memory fault: Stack pop fault !\n");
        }
        
        if (Exception_Fsr & (1<<4))
        {
            kprintf(" ***Memory fault: Stack push fault !\n");
        }
        
        if (Exception_Fsr & (1<<7))
        {
            kprintf(" ***Fault Address: 0x%P !\n", *((DWORD_PTR) CM3_MMAR));
        }
        
        if (Exception_Fsr & (1<<8))
        {
            kprintf(" ***Bus fault: Code fault !\n");
        }
        
        if (Exception_Fsr & (1<<9))
        {
            kprintf(" ***Bus fault: Accurate data access fault !\n");
        }
        
        if (Exception_Fsr & (1<<10))
        {
            kprintf(" ***Bus fault: Unaccurate data access fault !\n");
        }
        
        if (Exception_Fsr & (1<<11))
        {
            kprintf(" ***Bus fault: Stack pop fault !\n");
        }
        
        if (Exception_Fsr & (1<<12))
        {
            kprintf(" ***Bus fault: Stack push fault !\n");
        }
        
        if (Exception_Fsr & (1<<15))
        {
            kprintf(" ***Fault Address: 0x%P !\n", *((DWORD_PTR) CM3_BFAR));
        }
        
        if (Exception_Fsr & (1<<16))
        {
            kprintf(" ***Usage fault: Undefined instruction !\n");
        }
        
        if (Exception_Fsr & (1<<17))
        {
            kprintf(" ***Usage fault: Switch to ARM mode !\n");
        }
        
        if (Exception_Fsr & (1<<18))
        {
            kprintf(" ***Usage fault: Return fault !\n");
        }
        
        if (Exception_Fsr & (1<<24))
        {
            kprintf(" ***Usage fault: Align fault !\n");
        }
        
        if (Exception_Fsr & (1<<25))
        {
            kprintf(" ***Usage fault: Divide fault !\n");
        }
        
        if (Exception_Hsr & (1<<1))
        {
            kprintf(" ***Hard fault: Vector fault!\n");
        }
        
        if (Exception_Hsr & (1<<30))
        {
            kprintf(" ***Hard fault: Core fault!\n");
        }
        
        if (Exception_Hsr & (1<<30))
        {
            kprintf(" ***Hard fault: Debug fault!\n");
        }
        
        
    }
    
    if (0x1c == lpField->iucb.OpCode)
    {
        lpStack->PC += 2;
    }
    else if (0x7 == lpField->i32b.Mark)
    {
        lpStack->PC += 4;
    }
    else
    {
        lpStack->PC += 2;
    }
}

