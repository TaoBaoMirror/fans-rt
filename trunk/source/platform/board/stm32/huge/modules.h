#ifndef __BSP_MODULES_H
#define __BSP_MODULES_H

#include <fadefs.h>
#include <faerror.h>
#include <famodule.h>

#include "kcore.h"

DECLARE_MODULE(APP);

STATIC CMODULE_HEADER * g_SystemModules[] =
{
    EXCERPT_MODULE(APP),
};


E_STATUS BootEntry(LPVOID lpArgment)
{
    ProbeModule(APP_MAGIC);
//  ProbeModule("DML");
//  ProbeModule("BSP");
    return STATE_SUCCESS;
}

#endif

