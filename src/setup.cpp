#include "game.hpp"

void createConsole()
{
    AllocConsole();
    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stderr);
}

// Entry point.
void setup()
{
    createConsole();
    auto& game = game::GameHarness::get();
    game.initialize();
    std::printf("Game initialized!\n");
    try
    {
        game.applyAllPatches();
    }
    catch (const std::exception& e)
    {
        std::printf("Error applying patch: %s\n", e.what());
    }
}