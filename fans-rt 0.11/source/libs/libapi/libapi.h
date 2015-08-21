#ifndef __LIB_API_H
#define __LIB_API_H

#include <fauser.h>
#include <fadefs.h>
#include <fatypes.h>

#ifdef __cplusplus
extern "C" {
#endif
#if (defined(CONFIG_BUILD_LOCAL_STORE) && (TRUE == CONFIG_BUILD_LOCAL_STORE))
    PUBLIC E_STATUS uCreateLsot(HANDLE hTask, DWORD Total);
    PUBLIC E_STATUS uCloseLsotObject(HANDLE hTask);
#endif
#ifdef __cplusplus
}
#endif
#endif

