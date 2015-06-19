#include <stdio.h>
#include <string.h>
#include <fadefs.h>
#include <fatypes.h>
#include <faerror.h>
#include <fansmodule.h>
#include <kdebug.h>
#include <fanscore.h>

EXTERN FNREGISTERMODULE fnRegisterModule;

#define     DECLARE_SYSTEMMODULE(name)                                      \
            EXTERN MODULE_HEADER name##_Module
#define     EXCERPT_SYSTEMMODULE(file, name)                                \
            { file, &name##_Module }
#define     EXCERPT_SYSTEMSYMBOL(name)                                      \
            { #name, &name}

DECLARE_SYSTEMMODULE(CORE);
DECLARE_SYSTEMMODULE(HAL);
DECLARE_SYSTEMMODULE(BSP);
DECLARE_SYSTEMMODULE(MMS);
DECLARE_SYSTEMMODULE(TSS);
DECLARE_SYSTEMMODULE(APP);
DECLARE_SYSTEMMODULE(IDL);

typedef struct tagMODULEFILE{
    LPSTR               lpFileName;
    LPMODULE_HEADER        lpFansModule;
}MOLDULEFILE, * PMODULEFILE, FAR * LPMODULEFILE;

STATIC CONST MOLDULEFILE SystemModules[]= 
{
    EXCERPT_SYSTEMMODULE("hal.sys", HAL),
    EXCERPT_SYSTEMMODULE("tss.sys", TSS),
    EXCERPT_SYSTEMMODULE("bsp.sys", BSP),
#if (CONFIG_MMS_REGION_MAX != 0)
    EXCERPT_SYSTEMMODULE("mms.sys", MMS),
#endif
    EXCERPT_SYSTEMMODULE("app.module", APP),
//  EXCERPT_SYSTEMMODULE("libapi.so", FAPI),
//  EXCERPT_SYSTEMMODULE("hal.sys", HAL),
//  EXCERPT_SYSTEMMODULE("memory.sys", MEM),
//  EXCERPT_SYSTEMMODULE("fml.sys", FML),
//  EXCERPT_SYSTEMMODULE("dml.sys", DML),
};

STATIC LPMODULE_HEADER Find_Module(LPSTR lpFileName)
{
    INT i = 0;
    
    for (i = 0; i < sizeof(SystemModules) / sizeof(SystemModules[0]); i ++)
    {
        if (NULL == SystemModules[i].lpFileName || NULL == SystemModules[i].lpFansModule)
        {
            continue;
        }
        
        if (0 != strcmp(SystemModules[i].lpFileName, lpFileName))
        {
            continue;
        }

        return SystemModules[i].lpFansModule;
    }

    return NULL;
}

PUBLIC E_STATUS LDR_LoadModule(LPSTR lpFileName)
{
    LPMODULE_HEADER lpFansModule = NULL;
    
    lpFansModule = Find_Module(lpFileName);
    
    if (NULL == lpFansModule || NULL == lpFansModule->fnEntry)
    {
        return STATE_INVALID_OBJECT;
    }
    
    return fnRegisterModule(lpFansModule);
}

typedef struct tagSYMBOLTABLE{
    LPSTR           lpSymbolName;
    LPVOID          lpSymbolAddress;
}SYMBOLTABLE;

STATIC CONST SYMBOLTABLE SystemSymbols[] = {
    EXCERPT_SYSTEMSYMBOL(RegisterModule),
    EXCERPT_SYSTEMSYMBOL(FansEntry),
};


PUBLIC LPVOID LDR_FindSymbol(LPSTR lpSymbolName)
{
    int i;
    
    for (i = 0; i < SIZEOF_ARRAY(SystemSymbols); i ++)
    {
        if (NULL == SystemSymbols[i].lpSymbolName || NULL == SystemSymbols[i].lpSymbolAddress)
        {
            continue;
        }
        
        if (0 != strcmp(SystemSymbols[i].lpSymbolName, lpSymbolName))
        {
            continue;
        }

        return SystemSymbols[i].lpSymbolAddress;        
    }
    
    return NULL;
}


PUBLIC E_STATUS LDR_LoadFile(LPSTR lpFileName)
{
    return STATE_SUCCESS;
}

