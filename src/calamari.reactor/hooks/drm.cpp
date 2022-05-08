#include "hooks.h"
#include "../../common/util.h"

BOOL Calamari::Hooks::EnumProcessModules(HANDLE hProcess, HMODULE *lphModule, DWORD cb, LPDWORD lpcbNeeded)
{
    VMP_BEGIN("EnumProcessModulesHook");
#ifndef _DEBUG
    RECheck();
#endif
    return false;
    VMP_END();
}
