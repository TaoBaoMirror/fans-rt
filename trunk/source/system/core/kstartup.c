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

#include "libfw.h"
#include "ktask.h"
#include "kcore.h"
#include "kboard.h"
#include "kdebug.h"
#include "compiler.h"
#include "version.h"

PUBLIC E_STATUS initCoreHardwareParameterForARCH(VOID);
PUBLIC E_STATUS initCoreHardwareParameterForBoard(VOID);
PUBLIC E_STATUS initCoreLocalStorageOfTaskManager(VOID);
PUBLIC E_STATUS initCoreSystemTaskStackManager(VOID);
PUBLIC E_STATUS initCoreSystemObjectPoolManager(VOID);
PUBLIC E_STATUS initCoreInterProcessCommunicationManager(VOID);
PUBLIC E_STATUS initCoreInterruptServiceRoutinesManager(VOID);
PUBLIC E_STATUS initCoreLocalProcedureCallManager(VOID);
PUBLIC E_STATUS initCoreSystemTaskScheduleManager(VOID);
PUBLIC E_STATUS initCoreSystemGlobalMemoryManager(VOID);
PUBLIC E_STATUS initSystemApplicationModulesStartup(VOID);
PUBLIC E_STATUS initCoreSystemTimerManager(VOID);
PUBLIC E_STATUS initCoreSystemDebugManager(VOID);


STATIC VOID ShowSystemInformation(VOID)
{
    CORE_INFOR(TRUE, "Starting Fans-RT %d.%02d.%04d for board %s ...",
            MAJOR_VERSION, MINOR_VERSION, DEBUG_VERSION, CORE_GetBoardName());
    CORE_INFOR(TRUE, "Build for %s %02d.%02d.%04d, data: %s, time: %s.", __CC_COMPILER_NAME,
                __CC_MAJOR_VERSION, __CC_MINOR_VERSION, __CC_BUILD_VERSION, __DATE__, __TIME__);

    CORE_INFOR(TRUE, "Show the size of core type for debug:");
    CORE_INFOR(TRUE, "CHAR: %d  SHORT: %d  INT: %d  LONG: %d  LONG LONG: %d  E_STATUS: %d",
        sizeof(CHAR), sizeof(SHORT), sizeof(INT), sizeof(LONG), sizeof(long long), sizeof(E_STATUS));
    CORE_INFOR(TRUE, "BYTE: %d  WORD: %d  DWORD: %d  QWORD: %d  SIZE_T: %d,  LPVOID: %d",
        sizeof(BYTE), sizeof(WORD), sizeof(DWORD), sizeof(QWORD), sizeof(SIZE_T), sizeof(LPVOID));
    CORE_INFOR(TRUE, "BOOL: %d  FLOAT:%d  DOUBLE:%d", sizeof(BOOL), sizeof(float), sizeof(double));
    CORE_INFOR(TRUE, "Hardware tick max value is %d.", FW_GetTickBase());
    CORE_INFOR(TRUE, "The integer printf test: %8d, %08d, %lld, %03lld",
            12345678, 123, 20150630011218ll, 2013ll);
    CORE_INFOR(TRUE, "The float printf test: %f, %.2f, %.3f", 3.141592f, 3.141592f, 3.141592f);
}

PUBLIC VOID CORE_Starting(VOID)
{
    initCoreHardwareParameterForARCH();
    
    initCoreHardwareParameterForBoard();
    
    ShowSystemInformation();
    /* Local procedure call initialize */
    initCoreLocalProcedureCallManager();
    
    /* Debug initialize */
    initCoreSystemDebugManager();
    
    /* Timer initialize */
    initCoreSystemTimerManager();
    
    /* IRQ initialize */
    initCoreInterruptServiceRoutinesManager();
    
    /* Memory manager initialize */
    initCoreSystemGlobalMemoryManager();
    
    /* Object service initialize */
    initCoreSystemObjectPoolManager();
    
    /* IPC initialize */
    initCoreInterProcessCommunicationManager();
    
    /* Stack initialize */
    initCoreSystemTaskStackManager();
    
    /* The local storage of task initialize */ 
    initCoreLocalStorageOfTaskManager();
    
    /* Task initialize*/
    initCoreSystemTaskScheduleManager();
    
    /* Load application modules*/
    initSystemApplicationModulesStartup();
    while(1);
}
