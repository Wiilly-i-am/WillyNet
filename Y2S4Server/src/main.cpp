#include "utils/includes.h"

#pragma comment(lib, "ws2_32.lib")


static std::string response_data;

DWORD WINAPI MainThread(LPVOID lpReserved)
{
    bool init_hook = false;
    do
    {
        if (kiero::init(kiero::RenderType::D3D11) == kiero::Status::Success)
        {
            kiero::bind(8, (void**)&oPresent, hkPresent);
            init_hook = true;
        }
    } while (!init_hook);
    return TRUE;
}

BOOL WINAPI DllMain(HMODULE hMod, DWORD dwReason, LPVOID lpReserved)
{
    switch (dwReason)
    {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hMod);

        // imgui hook setup
        CreateThread(nullptr, 0, MainThread, hMod, 0, nullptr);

        // second thread (proabably backend)
        
        break;

    case DLL_PROCESS_DETACH:
        kiero::shutdown();
        break;
    }
    return TRUE;
}


// make backend
// make it redirect in game
// merge about, credits & help into settings page.

// if it compiles its good, if it runs its perfect - Linus Torvalds, Creator of Linux Kernel