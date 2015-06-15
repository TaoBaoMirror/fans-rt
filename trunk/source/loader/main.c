#include <stdio.h>
#include <string.h>
#include <fadefs.h>
#include <fatypes.h>
#include <faerror.h>
#include <fansmodule.h>

#include <fanscore.h>
#include "loader.h"


FNREGISTERMODULE    fnRegisterModule = NULL;

int main(void)
{
//  FANSBOOTENTRY fnFansBootEntry  = NULL;

    LDR_LoadFile("libmisc.so");
    LDR_LoadFile("core.sys");
    fnRegisterModule = (FNREGISTERMODULE) LDR_FindSymbol("RegisterModule");
    
    LDR_LoadModule("mms.sys");
    LDR_LoadModule("tss.sys");
    LDR_LoadModule("hal.sys");
    LDR_LoadModule("dml.sys");
    LDR_LoadModule("bsp.sys");
    
    LDR_LoadModule("app.module");

    //fnFansBootEntry = 
    ((FANSBOOTENTRY) LDR_FindSymbol("FansEntry"))();

    //fnFansBootEntry();
}

