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

    STATE_INVALID_INTCODE           =       -60,
    STATE_INVALID_INTMAGIC          =       -61,
    STATE_INVALID_INTMETHOD         =       -62,
    STATE_INVALID_INTHANDLER        =       -63,

    STATE_INVALID_IRQCODE           =       -70,
    STATE_INVALID_IRQMAGIC          =       -71,
    STATE_INVALID_IRQMETHOD         =       -72,
    STATE_INVALID_IRQHANDLER        =       -73,
    STATE_INVALID_IRQPACKET         =       -74,
    
    STATE_INVALID_REQCODE           =       -80,
    STATE_INVALID_REQMAGIC          =       -81,
    STATE_INVALID_REQMETHOD         =       -82,
    STATE_INVALID_REQHANDLER        =       -83,
    STATE_INVALID_REQPACKET         =       -83,
    
    STATE_INVALID_CONTEXT           =       -90,
    STATE_INVALID_PRIORITY          =       -91,
    
    STATE_OBJECT_BUSY               =       -100,
    
    
    
    STATE_INVALID_PARAMETER         =       -250,
    /**/
    //STATE_PIPE_ERROR              =       -80,
}E_STATUS, * PE_STATUS, FAR * LPE_STATUS;

#define SET_STATUS(status, value)       \
        do{                             \
            if (NULL != status)         \
            {                           \
                *(status) = (value);    \
            }                           \
        }while(0)

#endif
