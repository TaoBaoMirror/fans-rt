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
#ifndef __FA_APP_H
#define __FA_APP_H
#include <fadefs.h>
#include <fatypes.h>
#include <faerror.h>
#include <famodule.h>

#include "cadebug.h"
#include "kcore.h"

E_STATUS FansMain(VOID);

STATIC E_STATUS ModuleEntry(LPCFANSMODULE lpcFansModule)
{
    LOG_INFOR(TRUE, "APP Starting ...");
    return FansMain();
}

STATIC VOID ModuleLeave(LPCFANSMODULE lpcFansModule)
{
}

DEFINE_MODULE(APP, APP_MAGIC, NULL, ModuleEntry, ModuleLeave);
#endif

