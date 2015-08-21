#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fadefs.h>
#include <faerror.h>
#include <fatypes.h>
#include <kdebug.h>
#include <fansmodule.h>
#include <fanscore.h>
#include "libtable.h"

EXTERN E_STATUS FansMain(VOID);
EXTERN WORD g_SectionBuddyMaskTable[17];

#define     WINAPI      __stdcall
EXTERN DWORD WINAPI GetTickCount(VOID);


#define     CHIP_REGION_SIZE            ((CONFIG_MMS_ONCHIP_PAGES+1) * CONFIG_MMS_PAGE_SIZE)
#define     BUDDY_REGION_SIZE           (CONFIG_MMS_PAGE_SIZE * ((1<<CONFIG_MMS_BUDDY_MAX) + 1))
STATIC      BYTE MemoryOnChip[CHIP_REGION_SIZE];
STATIC      BYTE MemoryOffChip[31][CONFIG_MMS_PAGE_SIZE * 4096];

PUBLIC E_STATUS InsertRegion(DWORD RegionMask)
{
    BYTE RegionID = 0;
    E_STATUS State;

    if (RegionMask & 0x1)
    {
        State = CreateMMRegion(&MemoryOnChip, sizeof(MemoryOnChip));

        if (STATE_SUCCESS != State)
        {
            CORE_ERROR(TRUE, "Create chip region failed return %d.", State);
            return State;
        }
    }

    for (RegionID = 0; RegionID < CONFIG_MMS_REGION_MAX-1; RegionID ++)
    {
        if (0 != (RegionMask & (1 <<RegionID)))
        {
            State = CreateMMRegion(MemoryOffChip[RegionID], sizeof(MemoryOffChip[RegionID]));

            if (STATE_SUCCESS != State)
            {
                CORE_ERROR(TRUE, "Create region failed return %d.", State);
                return State;
            }
        }
    }

    return STATE_SUCCESS;
}


PUBLIC BYTE CORE_GetRegionID(MMADDR PageAddress)
{
    BYTE ID = 0;

    if (PageAddress >= (MMADDR) MemoryOnChip && PageAddress < (MMADDR) MemoryOnChip + sizeof(MemoryOnChip))
    {
        return 0;
    }

    for (ID = 0; ID < CONFIG_MMS_REGION_MAX; ID ++)
    {
        if (PageAddress >= ((MMADDR) MemoryOffChip[ID]) && PageAddress < ((MMADDR) MemoryOffChip[ID]) + sizeof(MemoryOffChip[ID]))
            return ID + 1;
    }

    return CONFIG_MMS_REGION_MAX;
}

#if 0

EXPORT VOID kDebugShowData(E_LOG_LEVEL emLevel, LPVOID lpBuffer, SIZE_T Length)
{
    DWORD Scale = 0;
    LPBYTE lpData = lpBuffer;

    for (Scale = 0; Scale < Length; Scale ++)
    {
        if (0 ==(Scale & 0xf))
        {
            if (Scale)
            {
                printf("\r\n");
            }

            printf("%08X: ", Scale);
        }
        else if (0 == (Scale & 0x7))
        {
            printf("-");
        }
        else
        {
            printf(" ");
        }

        printf("%02X", lpData[Scale] & 0xff);
    }

    printf("\r\n");
}
#endif

STATIC VOID ShowByte2C(LPSTR lpName, LPVOID lpBuffer, SIZE_T Length)
{
    DWORD Scale = 0;
    LPBYTE lpData = lpBuffer;

    printf("/* The size of %s is %d bytes */\r\n{\r\n", lpName, Length);
    for (Scale = 0; Scale < Length; Scale ++)
    {
        if (0x0 ==((Scale & 0xf)))
        {
            printf("    ");
        }
        printf("%d, ", lpData[Scale] & 0xff);

        if (0xf ==((Scale & 0xf)))
        {
            printf("/* 0x%02X ~ 0x%02X */\r\n", Scale - 0xf, Scale);
        }
    }

    printf("};\r\n");
}

STATIC VOID ShowWord2C(LPSTR lpName, LPVOID lpBuffer, DWORD Count)
{
    DWORD Scale = 0;
    LPWORD lpData = lpBuffer;

    printf("STATIC\t\t%-16s%s[%d] =\r\n", "WORD", lpName, Count);
    printf("/* The size of %s is %d bytes */\r\n{\r\n", lpName, Count * sizeof(WORD));

    for (Scale = 0; Scale < Count; Scale ++)
    {
        if (0x0 ==((Scale & 0x7)))
        {
            printf("    ");
        }
        printf("0x%04x, ", lpData[Scale] & 0xffff);

        if (0x7 ==((Scale & 0x7)))
        {
            printf("/* Buddy 0x%02x ~ 0x%02x */\r\n", Scale - 0x7, Scale);
        }
    }

    printf("};\r\n");
}


STATIC BYTE CalcLowestBit(BYTE Value)
{

    BYTE Bits = 8;

    if (Value & (1 << 7)) Bits = 7;
    if (Value & (1 << 6)) Bits = 6;
    if (Value & (1 << 5)) Bits = 5;
    if (Value & (1 << 4)) Bits = 4;
    if (Value & (1 << 3)) Bits = 3;
    if (Value & (1 << 2)) Bits = 2;
    if (Value & (1 << 1)) Bits = 1;
    if (Value & (1 << 0)) Bits = 0;

    return (BYTE) Bits;

}

STATIC BYTE CalcHighestBit(BYTE Value)
{
    BYTE Bits = 8;

    if (Value & (1 << 0)) Bits = 0;
    if (Value & (1 << 1)) Bits = 1;
    if (Value & (1 << 2)) Bits = 2;
    if (Value & (1 << 3)) Bits = 3;
    if (Value & (1 << 4)) Bits = 4;
    if (Value & (1 << 5)) Bits = 5;
    if (Value & (1 << 6)) Bits = 6;
    if (Value & (1 << 7)) Bits = 7;

    return Bits;
}

STATIC WORD CalcBuddyMask(BYTE Value)
{
    int i = 0;
    WORD Mask = 0;

    for (i = 15; i >=Value; i--)
    {
        Mask |= (1<<i);
    }

    return Mask;
}

STATIC BYTE CalcHowManyBits(BYTE Value)
{
    int i = 0;

    for (; Value; Value >>= 1)
    {
        if (Value &1) i ++;
    }

    return (BYTE) i;
}

STATIC VOID TableInitialize(VOID)
{
    int i = 0;

    for (i = 0; i < SIZEOF_ARRAY(g_LowestBitTable); i ++)
    {
        g_LowestBitTable[i] = CalcLowestBit((BYTE)i);
    }

    for (i = 0; i < SIZEOF_ARRAY(g_HighestBitmapTable); i ++)
    {
        g_HighestBitmapTable[i] = CalcHighestBit((BYTE)i);
    }

    for (i = 0; i < SIZEOF_ARRAY(g_SectionBuddyMaskTable); i++)
    {
        g_SectionBuddyMaskTable[i] = CalcBuddyMask((BYTE)i);
    }

    for (i = 0; i < SIZEOF_ARRAY(g_HowManyBitsTable); i ++)
    {
        g_HowManyBitsTable[i] = CalcHowManyBits((BYTE)i);
    }

    ShowByte2C("lowest bitmap table", g_LowestBitTable, sizeof(g_LowestBitTable));
    ShowByte2C("highest bitmap table", g_HighestBitmapTable, sizeof(g_HighestBitmapTable));
    ShowByte2C("how many bits table", g_HowManyBitsTable, sizeof(g_HowManyBitsTable));
    ShowWord2C("g_SectionBuddyMaskTable", g_SectionBuddyMaskTable, SIZEOF_ARRAY(g_SectionBuddyMaskTable));
}

PUBLIC E_STATUS FW_SystemCall(DWORD ServiceID, BYTE FunctionID, LPVOID lpParam)
{
    return CORE_HandlerLPC(ServiceID, FunctionID, lpParam);
}

EXPORT VOID DEBUG_WritePort(BYTE Data)
{
    putchar(Data);
}

EXPORT VOID CORE_RestoreIRQ(DWORD dwFlags)
{
    return;
}

EXPORT DWORD CORE_DisableIRQ(VOID)
{
    return 0;
}



PUBLIC E_STATUS RegisterSystemModule(LPBOOTMODULE lpModule)
{
    return STATE_SUCCESS;
}


PUBLIC TICK GetSystemTick(VOID)
{
    return (TICK) GetTickCount();
}

PUBLIC DWORD FW_GetTickCount(VOID)
{
    return GetTickCount();
}

PUBLIC DWORD FW_GetTickBase(VOID)
{
    return 1000;
}

EXTERN VOID MMS_Start(VOID);

int main(int argc, char * argv[])
{
    TableInitialize();
    MMS_Start();
    InsertRegion(0xFFFFFFFF);
    FansMain();
//  getchar();

    return 0;   
}