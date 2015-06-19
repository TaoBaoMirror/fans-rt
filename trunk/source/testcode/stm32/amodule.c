#include <fauser.h>
#include <fadefs.h>
#include <faerror.h>
#include <famodule.h>

DECLARE_MODULE(Application);

STATIC RO_DATA LPCMODULE_HEADER g_SystemModuleArray[] =
{
    EXCERPT_MODULE(Application),
};

EXPORT CODE_TEXT LPCMODULE_HEADER * GetModuleArray(VOID)
{
    return g_SystemModuleArray;
}

EXPORT CODE_TEXT DWORD GetNumberOfSystemModules(VOID)
{
    return SIZEOF_ARRAY(g_SystemModuleArray);
}
