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
#ifndef __FA_MODULE_H
#define __FA_MODULE_H

#include <fadefs.h>
#include <fatypes.h>
#include <faerror.h>

typedef struct tagFANSMODULE FANSMODULE;
typedef struct tagFANSMODULE * PFANSMODULE;
typedef struct tagFANSMODULE FAR * LPFANSMODULE;
typedef const struct tagFANSMODULE CFANSMODULE;
typedef const struct tagFANSMODULE * PCFANSMODULE;
typedef const struct tagFANSMODULE FAR * LPCFANSMODULE;

#define     MODULE_MAGIC        MAKE_DWORD('M', 'O', 'D')

#if (CONFIG_BUILD_DYNAMIC_SYMBOL == FALSE)

#define     DECLARE_MODULE(name)                                                \
            EXTERN CFANSMODULE g_##name##_module_descriptor
#define     EXCERPT_MODULE(name)                                                \
            &g_##name##_module_descriptor

#ifdef __GNUC__
#define     DEFINE_MODULE(name, id, prvdata, Entry, Leave)                      \
            PUBLIC CFANSMODULE g_##name##_module_descriptor = {                 \
                .un.ModuleID        =       id,                                 \
                .lpPrivate          =       prvdata,                            \
                .fnEntry            =       Entry,                              \
                .fnLeave            =       Leave,                              \
            };
#else
#define     DEFINE_MODULE(name, id, prvdata, Entry, Leave)                      \
            PUBLIC CFANSMODULE g_##name##_module_descriptor = {                 \
                id,                                                             \
                prvdata,                                                        \
                Entry,                                                          \
                Leave,                                                          \
            };
#endif
struct tagFANSMODULE{
    union{
        DWORD           ModuleID;
        CHAR            ModuleName[4];
    }un;
    LPVOID              lpPrivate;
    E_STATUS            (*fnEntry)(LPCFANSMODULE lpFansModule);
    VOID                (*fnLeave)(LPCFANSMODULE lpFansModule);
};

typedef struct tagUSER_MODULES_DESCRIPTOR USER_MODULES_DESCRIPTOR;
typedef struct tagUSER_MODULES_DESCRIPTOR * PUSER_MODULES_DESCRIPTOR;
typedef struct tagUSER_MODULES_DESCRIPTOR FAR * LPUSER_MODULES_DESCRIPTOR;

typedef const struct tagUSER_MODULES_DESCRIPTOR CUSER_MODULES_DESCRIPTOR;
typedef const struct tagUSER_MODULES_DESCRIPTOR * PCUSER_MODULES_DESCRIPTOR;
typedef const struct tagUSER_MODULES_DESCRIPTOR FAR * LPCUSER_MODULES_DESCRIPTOR;

struct tagUSER_MODULES_DESCRIPTOR{
    DWORD               UserModules;
    LPCFANSMODULE *     lpcModuleTable;
};


#else

#define     DEFINE_MODULE(name, id, prvdata, fnEntry, fnLeave)                  \
            STATIC FANSMODULE g_module_descriptor = {                           \
                MODULE_MAGIC,                                                   \
                {NULL, NULL},                                                   \
                id,                                                             \
                prvdata,                                                        \
                fnEntry,                                                        \
                fnLeave,                                                        \
            };

struct tagFANSMODULE{
    DWORD               Magic;
    LIST_HEAD           ModuleTable;
    union{
        DWORD           ModuleID;
        CHAR            ModuleName[4];
    }un;
    LPVOID              lpPrivate;
    E_STATUS            (*fnEntry)(LPCFANSMODULE lpFansModule);
    VOID                (*fnLeave)(LPCFANSMODULE lpFansModule);
};
#endif

#ifdef __cplusplus
extern "C" {
#endif
    typedef E_STATUS (FAR * FNREGISTERMODULE)(LPFANSMODULE lpModule);
    EXPORT E_STATUS ProbeModule(DWORD ModuleID);
#if (CONFIG_BUILD_DYNAMIC_SYMBOL == TRUE)
    EXPORT E_STATUS RegisterModule(LPFANSMODULE lpModule);
#endif
#ifdef __cplusplus
}
#endif 


#endif
