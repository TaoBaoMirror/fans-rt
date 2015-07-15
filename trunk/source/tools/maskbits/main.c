#include <stdio.h>

#ifndef CHAR_BITS
#define CHAR_BITS 8
#endif

#define MAX_BITS    (sizeof(unsigned int) * CHAR_BITS)

int main(int argc, char * argv[])
{
    unsigned int i;
    unsigned int value = 0;
    
    printf("CONST DWORD g_MaskBitsTable[%u] = {", MAX_BITS);

    for (i = 0; i < MAX_BITS; i++)
    {
        value |= 1 << i;
        
        if (0 == (i%8))
        {
            printf("\r\n    ");
        }
        else 
        {
            printf(" ");
        }

        printf("0x%08X,", value);
    }
    
    printf("\n};");
    
    return 0;
}

