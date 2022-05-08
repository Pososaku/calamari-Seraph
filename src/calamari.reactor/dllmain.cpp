#include "reactor.h"
#include "../common/util.h"

extern "C" void __declspec(dllexport) a(HMODULE h)
{
    VMP_BEGIN("LoadingFunction:a");
#ifndef _DEBUG
    RECheck();
#endif
    Calamari::Reactor::Initialize(h);
    VMP_END();
}

/*
BOOL  WINAPI DllMain(
    _In_ HINSTANCE hinstDLL,
    _In_ DWORD fdwReason,
    _In_ LPVOID lpvReserved)
{
    VMP_BEGIN("DllMain");
    if(fdwReason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(NULL);
		a();
    }
    return TRUE;
    VMP_END();
}
*/
