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
#ifndef __APP_MODULES_H
#define __APP_MODULES_H

#include <fadefs.h>
#include <faerror.h>
#include <famodule.h>

#include "kcore.h"

DECLARE_MODULE(APP);

STATIC CMODULE_HEADER * g_SystemModules[] =
{
    EXCERPT_MODULE(APP),
};


PUBLIC E_STATUS initSystemApplicationModulesStartup(LPVOID lpArgment)
{
    ProbeModule(APP_MAGIC);
//  ProbeModule("DML");
//  ProbeModule("BSP");
    return STATE_SUCCESS;
}

#endif

