// kryst4lmeth stable
// Hastily put together by toast
// Pushed on: 1/4/2017

#include "Hooks.hpp"

BOOL WINAPI DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
    if(dwReason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(hModule);
		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)Hooks::Initialize, NULL, NULL, NULL);
		return TRUE;
    }
	else if(dwReason == DLL_PROCESS_DETACH)
	{
        Hooks::Restore();
    }
    return TRUE;
}