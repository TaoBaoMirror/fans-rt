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
#ifndef __CAL_REQUEST_H
#define __CAL_REQUEST_H

#include "klpc.h"
#include "kcore.h"

#ifdef __cplusplus
extern "C" {
#endif
    PUBLIC E_STATUS ntSystemCall(LPLPC_REQUEST_PACKET lpPacket, DWORD ServiceID, BYTE FunctionID);
#ifdef __cplusplus
}
#endif
#endif

