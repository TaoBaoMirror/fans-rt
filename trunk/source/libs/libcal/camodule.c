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
#include <fauser.h>
#include <fadefs.h>
#include <faerror.h>
#include <fatypes.h>
#include <famodule.h>
#include <fapi.h>

#include "libcal.h"
#include "cadebug.h"

#if (CONFIG_BUILD_DYNAMIC_SYMBOL == TRUE)
/** build some elf file modules  */

STATIC CONST RO_DATA MODULE_HEADER * g_SystemModuleArray[CONFIG_SYSTEM_MODULES] = {NULL};

EXPORT RO_CODE E_STATUS RegisterModule(LPMODULE_HEADER lpModule)
{
    DWORD Id, Who = SIZEOF_ARRAY(g_SystemModuleArray);
    
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
    
    for (Id = 0; Id < SIZEOF_ARRAY(g_SystemModuleArray); Id ++)
    {
        if (NULL == g_SystemModuleArray[Id])
        {
            Who = Id;
        }
        else
        {
            if (g_SystemModuleArray[Id]->dwModuleType == lpModule->dwModuleType ||
                0 == strcmp(g_SystemModuleArray[Id]->lpModuleName, lpModule->lpModuleName))
            {
                LOG_ERROR(TRUE, "This module(%s) has been registered.", lpModule->lpModuleName);
                return STATE_EXISTING;
            }
        }
    }
    
    if (Who < SIZEOF_ARRAY(g_SystemModuleArray))
    {
        g_SystemModuleArray[Who] = lpModule;
        return (STATE_SUCCESS);
    }

    LOG_ERROR(TRUE, "No memory to register module(%s).", lpModule->lpModuleName);
    
    return (STATE_OUT_OF_MEMORY);
}
EXPORT_SYMBOL(RegisterModule);


EXPORT RO_CODE E_STATUS ProbeModule(LPTSTR lpModuleName)
{
    DWORD Id;

    for (Id = 0; Id < SIZEOF_ARRAY(g_SystemModuleArray); Id ++)
    {
        CONST MODULE_HEADER * lpFansModule = g_SystemModuleArray[Id];
        
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
EXPORT_SYMBOL(ProbeModule);
#else

EXPORT RO_CODE E_STATUS initSystemApplicationModulesStartup(VOID)
{
    DWORD Id;

    for (Id = 0; Id < GetNumberOfSystemModules(); Id ++)
    {
        LPMODULE_HEADER CONST lpModule = GetModuleArray()[Id];
        
        if (NULL == lpModule)
        {
            continue;
        }

        if (NULL == lpModule->fnEntry)
        {
            LOG_INFOR(TRUE, "Invalid module '%s' ...", lpModule->lpName);
            continue;
        }
        
        LOG_INFOR(TRUE, "Probe module '%s' ...", lpModule->lpName);
        
        if (STATE_SUCCESS != lpModule->fnEntry(lpModule))
        {
            LOG_INFOR(TRUE, "Probe module '%s' ... failed.", lpModule->lpName);
        }
    }

    return STATE_SUCCESS;
}

#endif
