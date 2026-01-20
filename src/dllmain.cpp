#include "game/game.hpp"
#include "patch/patch.hpp"
#include <string>
#include <windows.h>

#define EXPORT extern "C" __declspec(dllexport)

// Entry point.
void setup()
{
    SetThreadDescription(GetCurrentThread(), L"OSGT-QOL Mod Thread");
#ifdef DEVELOPMENT
    // Create a console window for debug builds.
    AllocConsole();
    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stderr);
#endif

    auto& game = game::GameHarness::get();
    auto& patchMgr = patch::PatchManager::get();
    auto& optionsMgr = game::OptionsManager::get();
    auto& input = game::InputEvents::get();
    auto& itemAPI = game::ItemAPI::get();
    auto& weatherMgr = game::WeatherManager::get();
    try
    {
        // Initialize modding APIs and load patches.
        game.toggleLoadScreen();
        game.resolveRenderSigs();
        game.setWindowModdedIcon();
        game.setWindowTitle("Growtopia [OSGT-QOL]");
        optionsMgr.initialize();
        optionsMgr.addOptionPage("qol", "OSGT-QOL Settings");
        input.initialize();
        itemAPI.initialize();
        weatherMgr.initialize();
        patchMgr.applyPatchesFromFile("patches.txt");
        game.toggleLoadScreen();
        // reload icon as InitVideo may mess this up.
        game.setWindowModdedIcon();
        std::fprintf(stderr, "Done applying patches.\n");
    }
    catch (const std::exception& e)
    {
#ifdef DEVELOPMENT
        // Output error to console.
        std::fprintf(stderr, "FATAL: %s\n", e.what());
        return;
#else
        // Show error message box and exit.
        game.setWindowVisible(false);
        MessageBoxA(nullptr, e.what(), "Error", MB_ICONERROR | MB_OK);
        ExitProcess(EXIT_FAILURE);
#endif
    }
}

// Runs OSGT-QOL setup (entry point) if it hasn't ran yet.
void runSetupIfNeeded()
{
    static long done = 0;
    if (InterlockedCompareExchange(&done, 1, 0) == 0)
    {
        auto& game = game::GameHarness::get();
        // Delay loading the game until we've initialized game harness and can prevent loading the
        // MainMenuCreate call from App init. This allows us to use a loading screen while patching
        // is still ongoing. Some patches may clash with main menu existing and worse yet crash the
        // game.
        game.initialize();
        // Patch out CRC integrity check.
        auto addr = game.findMemoryPattern<uint8_t*>("00 3B C1 75 ? 85 C9");
        utils::nopMemory(addr + 1, 6);
        // Patch out MainMenuCreate call.
        addr = game.findMemoryPattern<uint8_t*>(
            "33 D2 49 8B CE E8 ? ? ? ? F3 0F 10 ? ? ? ? ? 0F 2F ? ? ? ? ? 0F");
        utils::nopMemory(addr + 5, 5);
        CreateThread(NULL, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(setup), NULL, 0, NULL);
    }
}

// Initializes the library if needed and calls original DirectInput8Create.
EXPORT HRESULT WINAPI DirectInput8Create(HINSTANCE instance, DWORD version, REFIID id, LPVOID* out,
                                         LPVOID unk)
{
    using DirectInput8Create_t = HRESULT(WINAPI*)(HINSTANCE, DWORD, REFIID, LPVOID*, LPVOID);
    runSetupIfNeeded();
    // GetSystemDirectoryW(nullptr, 0) will returns the size of the buffer needed to store the
    // path without the null terminator.
    uint32_t sysDirPathSize = GetSystemDirectoryW(nullptr, 0);
    std::wstring path(sysDirPathSize - 1, L'\0');
    GetSystemDirectoryW(&path[0], sysDirPathSize);
    path += L"\\dinput8.dll";
    // Assume dinput8 is on the system, the game depends on it anyway.
    HMODULE dinput8 = LoadLibraryW(path.c_str());
    auto fn = (DirectInput8Create_t)GetProcAddress(dinput8, "DirectInput8Create");
    return fn != nullptr ? fn(instance, version, id, out, unk) : E_FAIL;
}