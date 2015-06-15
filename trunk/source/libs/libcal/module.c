/*
 *    Copyright(C) 2013-2015, Fans-rt development team.
 *
 *    All rights reserved.
 *
 *    This is open source software.
 *    Learning and research can be unrestricted to  modification, use and dissemination.
 *    If you need for commercial purposes, you should get the author's permission.
 *
 *    date           author          notes
 *    2014-09-07     JiangYong       new file
 */
#include <string.h>
#include <fadefs.h>
#include <faerror.h>
#include <fatypes.h>
#include <fapi.h>

#include "libcal.h"
#include "cadebug.h"
#include "modules.h"

#if (CONFIG_BUILD_DYNAMIC_SYMBOL == TRUE)
/** build some elf file modules  */

STATIC CONST FANSMODULE * g_SystemModules[CONFIG_SYSTEM_MODULES] = {NULL};

EXPORT E_STATUS RegisterModule(LPFANSMODULE lpModule)
{
    DWORD Id, Who = SIZEOF_ARRAY(g_SystemModules);
    
    if (NULL == lpModule)
    {
        LOG_ERROR(TRUE, "Invalid parameter.");
        return STATE_INVALID_VALUE;
    }
    
    if (MODULE_MAGIC != lpModule->dwModuleMagic ||
        NULL == lpModule->fnEntry)
    {
        LOG_ERROR(TRUE, "Invalid module object %p.", lpModule);
        return STATE_INVALID_OBJECT;
    }
    
    for (Id = 0; Id < SIZEOF_ARRAY(g_SystemModules); Id ++)
    {
        if (NULL == g_SystemModules[Id])
        {
            Who = Id;
        }
        else
        {
            if (g_SystemModules[Id]->dwModuleType == lpModule->dwModuleType ||
                0 == strcmp(g_SystemModules[Id]->lpModuleName, lpModule->lpModuleName))
            {
                LOG_ERROR(TRUE, "This module(%s) has been registered.", lpModule->lpModuleName);
                return STATE_EXISTING;
            }
        }
    }
    
    if (Who < SIZEOF_ARRAY(g_SystemModules))
    {
        g_SystemModules[Who] = lpModule;
        return (STATE_SUCCESS);
    }

    LOG_ERROR(TRUE, "No memory to register module(%s).", lpModule->lpModuleName);
    
    return (STATE_OUT_OF_MEMORY);
}
EXPORT_SYMBOL(RegisterModule);


PUBLIC E_STATUS ProbeModule(LPTSTR lpModuleName)
{
    DWORD Id;

    for (Id = 0; Id < SIZEOF_ARRAY(g_SystemModules); Id ++)
    {
        CONST FANSMODULE * lpFansModule = g_SystemModules[Id];
        
        if (NULL == lpFansModule)
        {
            continue;
        }

        if (MODULE_MAGIC != lpFansModule->dwModuleMagic || 
            0 != strcmp(lpFansModule->lpModuleName, lpModuleName))
        {
            continue;
        }
        
        LOG_INFOR(TRUE, "Probe module %s(%p - %p) ...",
            lpFansModule->lpModuleName, lpFansModule, lpFansModule->lpModuleName);
        
        return lpFansModule->fnEntry(lpFansModule);
    }

    LOG_INFOR(TRUE, "Not found module(%s).", lpModuleName);
    
    return (STATE_NOT_FOUND);
}

#else
/** single flash on this board */


PUBLIC E_STATUS ProbeModule(DWORD ModuleID)
{
    DWORD Id;

    for (Id = 0; Id < SIZEOF_ARRAY(g_SystemModules); Id ++)
    {
        LPCFANSMODULE lpcFansModule = g_SystemModules[Id];
        
        if (NULL == lpcFansModule)
        {
            continue;
        }

        if (lpcFansModule->un.ModuleID != ModuleID)
        {
            continue;
        }
        
        LOG_INFOR(TRUE, "Probe module '%s' ...", lpcFansModule->un.ModuleName);
        
        return lpcFansModule->fnEntry(lpcFansModule);
    }

    return (STATE_NOT_FOUND);
}

#endif

EXPORT_SYMBOL(ProbeModule);

