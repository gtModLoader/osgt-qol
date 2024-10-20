#include "game/game.hpp"
#include "patch/patch.hpp"
#include <string>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#define EXPORT extern "C" __declspec(dllexport)

// Entry point.
void setup()
{
#ifndef NDEBUG
    // Create a console window for debug builds.
    AllocConsole();
    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stderr);
#endif

    auto& game = game::GameHarness::get();
    auto& patchMgr = patch::PatchManager::get();
    auto& optionsMgr = game::OptionsManager::get();
    try
    {
        // Initialize game harness and apply patches.
        game.initialize();
        game.setWindowVisible(false);
        optionsMgr.initialize();
        patchMgr.applyPatchesFromFile("patches.txt");
        game.setWindowTitle("Growtopia [OSGT-QOL]");
        game.setWindowVisible(true);
        std::fprintf(stderr, "Done applying patches.\n");
    }
    catch (const std::exception& e)
    {
#ifndef NDEBUG
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
        CreateThread(NULL, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(setup), NULL, 0, NULL);
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