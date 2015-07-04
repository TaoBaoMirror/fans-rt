#include <fauser.h>
#include <fadefs.h>
#include <faerror.h>
#include <famodule.h>

DECLARE_MODULE(Application);

STATIC CONST RO_USER_DATA MODULE_HEADER * CONST g_SystemModuleArray[] =
{
    EXCERPT_MODULE(Application),
};

EXPORT RO_USER_CODE LPMODULE_HEADER CONST * GetModuleArray(VOID)
{
    return (LPMODULE_HEADER CONST * ) g_SystemModuleArray;
}

EXPORT RO_USER_CODE DWORD GetNumberOfSystemModules(VOID)
{
    return SIZEOF_ARRAY(g_SystemModuleArray);
}
