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

typedef struct tagMODULE_HEADER MODULE_HEADER;
typedef struct tagMODULE_HEADER * PMODULE_HEADER;
typedef struct tagMODULE_HEADER FAR * LPMODULE_HEADER;
typedef CONST struct tagMODULE_HEADER CMODULE_HEADER;
typedef CONST struct tagMODULE_HEADER * PCMODULE_HEADER;
typedef CONST struct tagMODULE_HEADER FAR * LPCMODULE_HEADER;

#define     MODULE_MAGIC        MAKE_DWORD('M', 'O', 'D')

#if (CONFIG_BUILD_DYNAMIC_SYMBOL == FALSE)

#define     DECLARE_MODULE(Name)                                                \
            EXTERN MODULE_HEADER CONST g_##Name##_module_descriptor
#define     EXCERPT_MODULE(Name)                                                \
            &g_##Name##_module_descriptor

#define     DEFINE_MODULE(Name, PrvData)                                        \
            RO_USER_DATA MODULE_HEADER CONST g_##Name##_module_descriptor = {   \
                #Name,                                                          \
                PrvData,                                                        \
                FansMain,                                                       \
            };

typedef E_STATUS (* FNMODULEENTRY)(LPMODULE_HEADER CONST lpModule);

struct tagMODULE_HEADER{
    LPCSTR              lpName;
    LPVOID              lpPrivate;
    FNMODULEENTRY       fnEntry;
};

#else /* CONFIG_BUILD_DYNAMIC_SYMBOL */

#define     DEFINE_MODULE(name, id, prvdata, fnEntry, fnLeave)                  \
            STATIC MODULE_HEADER g_module_descriptor = {                        \
                MODULE_MAGIC,                                                   \
                {NULL, NULL},                                                   \
                id,                                                             \
                prvdata,                                                        \
                fnEntry,                                                        \
                fnLeave,                                                        \
            };

struct tagMODULE_HEADER{
    DWORD               Magic;
    LIST_HEAD           ModuleTable;
    union{
        DWORD           ModuleID;
        CHAR            ModuleName[4];
    }un;
    LPVOID              lpPrivate;
    E_STATUS            (*fnEntry)(LPCMODULE_HEADER lpFansModule);
    VOID                (*fnLeave)(LPCMODULE_HEADER lpFansModule);
};
#endif /* CONFIG_BUILD_DYNAMIC_SYMBOL */

#ifdef __cplusplus
extern "C" {
#endif
    typedef E_STATUS (FAR * FNREGISTERMODULE)(LPMODULE_HEADER lpModule);
//    STATIC E_STATUS FansMain(LPMODULE_HEADER CONST lpModule);
    EXPORT LPMODULE_HEADER CONST * GetModuleArray(VOID);
    EXPORT DWORD GetNumberOfSystemModules(VOID);
#if (CONFIG_BUILD_DYNAMIC_SYMBOL == TRUE)
    PUBLIC E_STATUS ProbeModule(LPCSTR lpModuleName);
    EXPORT E_STATUS RegisterModule(LPMODULE_HEADER lpModule);
#endif
#ifdef __cplusplus
}
#endif 


#endif
