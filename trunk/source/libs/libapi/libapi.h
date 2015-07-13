#ifndef __LIB_API_H
#define __LIB_API_H

#include <fauser.h>
#include <fadefs.h>
#include <fatypes.h>

#ifdef __cplusplus
extern "C" {
#endif
    PUBLIC E_STATUS uCreateLsot(HANDLE hTask, DWORD Total);
    EXPORT E_STATUS uCloseLsotObject(HANDLE hTask);
    
#ifdef __cplusplus
}
#endif
#endif

