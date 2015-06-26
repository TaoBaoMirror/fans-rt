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
 
#ifndef __FA_ERROR_H
#define __FA_ERROR_H

typedef enum emSTATUS{
    STATE_SUCCESS                   =       0,
    /**/
    STATE_PERMISSION_DENIED         =       -1,
    STATE_NOT_IMPLEMENTED           =       -2,
    STATE_NOT_SUPPORT               =       -3,
    STATE_NOT_READY                 =       -4,
    STATE_NOT_MATCH                 =       -5,
    STATE_NOT_FOUND                 =       -6,
    STATE_EXISTING                  =       -7,
    STATE_REMOVED                   =       -8,
    /**/
    STATE_UNKNOW_ERROR              =       -21,
    STATE_OVER_RANGE                =       -22,
    STATE_OUT_OF_MEMORY             =       -23,
    STATE_MEMORY_OVERFLOW           =       -24,
    STATE_HARDWARE_FAULT            =       -25,
    STATE_SYSTEM_FAULT              =       -26,
    STATE_SYSTEM_BUSY               =       -27,
    STATE_TIME_OUT                  =       -28,
    /**/
    STATE_INVALID_SIZE              =       -40,
    STATE_INVALID_NAME              =       -41,
    STATE_INVALID_FILE              =       -42,
    STATE_INVALID_PATH              =       -43,
    STATE_INVALID_VALUE             =       -44,
    STATE_INVALID_TASK              =       -45,
    STATE_INVALID_OBJECT            =       -46,
    STATE_INVALID_CLASS             =       -47,
    STATE_INVALID_SERVICE           =       -48,
    STATE_INVALID_METHOD            =       -49,
    STATE_INVALID_REQUEST           =       -50,
    STATE_INVALID_HANDLER           =       -51,
    STATE_INVALID_PAGE              =       -52,
    STATE_INVALID_REGION            =       -53,
    STATE_INVALID_STATE             =       -54,
    STATE_INVALID_POOL              =       -55,
    STATE_INVALID_HASH              =       -56,
    STATE_INVALID_TIME              =       -57,
    STATE_INVALID_MODULE            =       -58,
    STATE_INVALID_CONTEXT           =       -59,
    STATE_INVALID_PRIORITY          =       -60,
    STATE_INVALID_PARAMETER         =       -61,
    STATE_INVALID_POINTER           =       -62,
    STATE_INVALID_STACK             =       -63,
    STATE_INVALID_PERMISSION        =       -64,
}E_STATUS, * PE_STATUS, FAR * LPE_STATUS;

#define SET_STATUS(status, value)       \
        do{                             \
            if (NULL != status)         \
            {                           \
                *(status) = (value);    \
            }                           \
        }while(0)

#endif
