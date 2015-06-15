#ifndef __BSP_MODULES_H
#define __BSP_MODULES_H

#include <fadefs.h>
#include <faerror.h>
#include <famodule.h>

#include "kcore.h"

DECLARE_MODULE(APP);

STATIC CFANSMODULE * g_SystemModules[] =
{
    EXCERPT_MODULE(APP),
};


PUBLIC E_STATUS initSystemApplicationModulesStartup(VOID)
{
    ProbeModule(APP_MAGIC);
//  ProbeModule("DML");
//  ProbeModule("BSP");
    return STATE_SUCCESS;
}

#endif

