#ifndef __K_TLOCAL_H
#define __K_TLOCAL_H

#include "ktask.h"

enum{
    KLSOT_METHOD_GET_KEY    =   0,
    KLSOT_METHOD_PUT_KEY    =   1,
    KLSOT_METHOD_GET_VALUE  =   2,
    KLSOT_METHOD_SET_VALUE  =   3,
    KLSOT_CLASS_METHODS
};

typedef union tagKLSOT_REQUEST_PACKET KLSOT_REQUEST_PACKET;
typedef union tagKLSOT_REQUEST_PACKET * PKLSOT_REQUEST_PACKET;
typedef union tagKLSOT_REQUEST_PACKET FAR * LPKLSOT_REQUEST_PACKET;

union tagKLSOT_REQUEST_PACKET{
    struct{
        DWORD               Total;
    }Create;
    struct{
        HANDLE              hTask;
    }Active;
    struct{
        LSOT_KEY_T          KeyID;
    }GetPut;
    struct{
        LSOT_KEY_T          KeyID;
        DWORD               Value;
    }Value;
};

#define     GetKLPTotal(lpVoid)                 (((LPKLSOT_REQUEST_PACKET)(lpVoid))->Create.Total)
#define     SetKLPTotal(lpVoid, Value)          do {(((LPKLSOT_REQUEST_PACKET)(lpVoid))->Create.Total) = (Value);} while(0)
#define     GetKLPhTask(lpVoid)                 (((LPKLSOT_REQUEST_PACKET)(lpVoid))->Active.hTask)
#define     SetKLPhTask(lpVoid, handle)         do {(((LPKLSOT_REQUEST_PACKET)(lpVoid))->Active.hTask) = (handle);} while(0)
#define     GetKLPKeyID(lpVoid)                 (((LPKLSOT_REQUEST_PACKET)(lpVoid))->GetPut.KeyID)
#define     SetKLPKeyID(lpVoid, Id)             do {(((LPKLSOT_REQUEST_PACKET)(lpVoid))->GetPut.KeyID) = (Id);} while(0)
#define     GetKLPValue(lpVoid)                 (((LPKLSOT_REQUEST_PACKET)(lpVoid))->Value.Value)
#define     SetKLPValue(lpVoid, V)              do {(((LPKLSOT_REQUEST_PACKET)(lpVoid))->Value.Value) = (V);} while(0)


#endif

