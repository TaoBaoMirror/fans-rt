#include <fauser.h>
#include <fadefs.h>
#include <faerror.h>
#include <famodule.h>

DECLARE_MODULE(Application);

STATIC CONST RO_DATA MODULE_HEADER * g_SystemModuleArray[] =
{
    EXCERPT_MODULE(Application),
};

EXPORT CODE_TEXT LPMODULE_HEADER CONST * GetModuleArray(VOID)
{
    return (LPMODULE_HEADER CONST * ) g_SystemModuleArray;
}

EXPORT CODE_TEXT DWORD GetNumberOfSystemModules(VOID)
{
    return SIZEOF_ARRAY(g_SystemModuleArray);
}
