#include <fauser.h>
#include <fadefs.h>
#include <faerror.h>
#include <famodule.h>

DECLARE_MODULE(Application);

STATIC CONST RO_DATA MODULE_HEADER * CONST RO_DATA g_SystemModuleArray[] =
{
    EXCERPT_MODULE(Application),
};

EXPORT RO_CODE LPMODULE_HEADER CONST * GetModuleArray(VOID)
{
    return (LPMODULE_HEADER CONST * ) g_SystemModuleArray;
}

EXPORT RO_CODE DWORD GetNumberOfSystemModules(VOID)
{
    return SIZEOF_ARRAY(g_SystemModuleArray);
}
