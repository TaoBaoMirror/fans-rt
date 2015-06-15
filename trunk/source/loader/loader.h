#ifndef __BOOT_LOADER_H
#define __BOOT_LOADER_H

#include <fadefs.h>
#include <fatypes.h>
#include <FansError.h>


#ifdef __cplusplus
extern "C" {
#endif
    PUBLIC E_STATUS LDR_LoadFile(LPSTR lpFileName);
    PUBLIC E_STATUS LDR_LoadModule(LPSTR lpFileName);
    PUBLIC LPVOID LDR_FindSymbol(LPSTR lpSymbolName);
    
#ifdef __cplusplus
}
#endif 

#endif

