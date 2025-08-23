// dllmain.cpp : Defines the entry point for the DLL application.
#include <Windows.h>
#include <stdio.h>

#include "moddn/moddn.h"

static HANDLE gThread = NULL;
static HMODULE gModule = NULL;

DWORD WINAPI ModdnThread(LPVOID lpParam)
{
    moddn_entry();
    return 0;
}

__declspec(dllexport) BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD ul_reason_for_call,
    LPVOID lpReserved)
{
    puts("DllMain Called");
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        gModule = hModule;
        DisableThreadLibraryCalls(hModule);
        gThread = CreateThread(NULL, 0, ModdnThread, NULL, 0, NULL);
        break;

    case DLL_PROCESS_DETACH:
        moddn_exit();
        if (gThread) {
            WaitForSingleObject(gThread, 1000);
            CloseHandle(gThread);
            gThread = NULL;
        }
        break;
    }
    return TRUE;
}
