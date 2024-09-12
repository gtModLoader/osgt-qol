#include "game.hpp"
#include <cstdio>

// Creates a console window.
void createConsole()
{
    AllocConsole();
    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stderr);
}

// Entry point.
void setup()
{
#ifndef NDEBUG
    // Create a console window for debug builds.
    createConsole();
#endif

    auto& game = game::GameHarness::get();
    game.initialize();
    std::printf("Game initialized!\n");
    try
    {
        game.applyAllPatches();
    }
    catch (const std::exception& e)
    {
#ifndef NDEBUG
        // Output error to console.
        std::fprintf(stderr, "%s\n", e.what());
        return;
#else
        // Show error message box and exit.
        MessageBoxA(nullptr, e.what(), "Error", MB_ICONERROR | MB_OK);
        ExitProcess(EXIT_FAILURE);
#endif
    }
    game.updateWindowTitle();
}