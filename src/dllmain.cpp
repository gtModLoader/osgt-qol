#include "game/game.hpp"
#include "patch/patch.hpp"
#include <string>
#include <version.h>
#include <windows.h>

static std::once_flag isInitialized;

using DirectInput8CreateFn = HRESULT(WINAPI*)(HINSTANCE, DWORD, REFIID, LPVOID*, LPVOID);
static DirectInput8CreateFn originalDirectInput8Create = nullptr;

void createDebugConsole()
{
    AllocConsole();
    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stderr);
    SetConsoleTitleA("osgt-qol " OSGT_QOL_DISPLAY_VERSION);
}

void showErrorMessageBox(const std::string& message)
{
    MessageBoxA(nullptr, message.c_str(), "Error",
                MB_ICONERROR | MB_OK | MB_TOPMOST | MB_SETFOREGROUND);
}

void setup()
{
#ifdef OSGT_QOL_DEVELOPMENT
    createDebugConsole();
    std::fprintf(stderr, "OSGT-QOL " OSGT_QOL_DISPLAY_VERSION " (" OSGT_QOL_VERSION ")\n");
#endif

    auto& game = game::GameHarness::get();
    auto& patchMgr = patch::PatchManager::get();
    auto& optionsMgr = game::OptionsManager::get();
    auto& input = game::InputEvents::get();
    auto& itemAPI = game::ItemAPI::get();
    auto& weatherMgr = game::WeatherManager::get();

    try
    {
        game.initialize();

        // Patch out the CRC integrity check.
        auto crcCheckAddr = game.findMemoryPattern<uint8_t*>("00 3B C1 75 ? 85 C9");
        utils::nopMemory(crcCheckAddr + 1, 6);

        // The client will try to add restrictive ACEs to the process DACL, likely trying to prevent
        // other processes from interacting with it in malicious ways, but it also makes it
        // impossible to, for example, shut down the game using another unprivileged process (e.g.,
        // by running taskkill). Annoying, patch this shit out.
        auto daclModAddr = game.findMemoryPattern<uint8_t*>(
            "E8 ? ? ? ? B0 01 EB 1B 4C 8D ? ? ? ? ? 33 C9 45 33 C9 48 8D ? ? ? ? ? FF 15");
        utils::nopMemory(daclModAddr, 5);

        game.resolveSharedSigs();
        // game.toggleLoadScreen();
        game.setWindowModdedIcon();

        std::string versionString = "OSGT-QOL " OSGT_QOL_DISPLAY_VERSION;
        game.setWindowTitle("Growtopia [" + versionString + "] - Patching the game - Please wait!");
        optionsMgr.initialize();
        optionsMgr.addOptionPage("qol", "OSGT-QOL Settings");
        input.initialize();
        itemAPI.initialize();
        weatherMgr.initialize();
        patchMgr.applyPatchesFromFile("patches.txt");
        // game.toggleLoadScreen();
        game.setWindowTitle("Growtopia [" + versionString + "]");
        std::fprintf(stderr, "Done applying patches.\n");
    }
    catch (const std::exception& e)
    {
        std::fprintf(stderr, "SETUP FAILED: %s\n", e.what());
        game.setWindowVisible(false);
        showErrorMessageBox(e.what());
        ExitProcess(EXIT_FAILURE);
    }
}

void loadOriginalDirectInput8Create()
{
    // Create path to system dinput8.dll
    uint32_t sysPathSize = GetSystemDirectoryW(nullptr, 0);
    std::wstring modulePath(sysPathSize - 1, L'\0');
    GetSystemDirectoryW(&modulePath[0], sysPathSize);
    modulePath += L"\\dinput8.dll";

    // Load original function
    HMODULE dinput8 = LoadLibraryW(modulePath.c_str());
    originalDirectInput8Create =
        reinterpret_cast<DirectInput8CreateFn>(GetProcAddress(dinput8, "DirectInput8Create"));
}

HRESULT WINAPI DirectInput8Create(HINSTANCE hInst, DWORD dwVersion, REFIID riidltf, LPVOID* ppvOut,
                                  LPVOID punkOuter)
{
    std::call_once(isInitialized,
                   []()
                   {
                       loadOriginalDirectInput8Create();
                       if (originalDirectInput8Create)
                       {
                           setup();
                       }
                   });

    if (!originalDirectInput8Create)
    {
        // loadOriginalDirectInput8Create must have failed
        showErrorMessageBox("Failed to load original DirectInput8Create function");
        return E_FAIL;
    }
    return originalDirectInput8Create(hInst, dwVersion, riidltf, ppvOut, punkOuter);
}

BOOL WINAPI DllMain(HINSTANCE hInstDll, DWORD fdwReason, LPVOID lpvReserved)
{
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hInstDll);
        break;
    case DLL_PROCESS_DETACH:
        // If we ever want to add proper cleanup code, it's either RAII or here
        break;
    }
    return TRUE;
}
