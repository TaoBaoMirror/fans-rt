#ifndef __LIBFW_H
#define __LIBFW_H
#include <fadefs.h>
#include <fatypes.h>


#ifdef __cplusplus
extern "C" {
#endif
    PUBLIC DWORD FW_GetTickBase(VOID);
    PUBLIC DWORD FW_GetTickCount(VOID);
    PUBLIC VOID FW_SystemIdle(VOID);
    PUBLIC LPVOID FW_StackPosition(VOID);
    PUBLIC VOID FW_SystemCall(LPVOID lpParam, DWORD ServiceID, BYTE FunctionID);
#ifdef __cplusplus
}
#endif

#endif

