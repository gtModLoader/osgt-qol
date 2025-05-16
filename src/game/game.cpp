#include "game.hpp"
#include <mmeapi.h>
#include <psapi.h>

namespace game
{

GameHarness& GameHarness::get()
{
    static GameHarness instance;
    return instance;
}

void GameHarness::initialize()
{
    // Get game module base address and image size.
    baseAddr = GetModuleHandle(nullptr);
    MODULEINFO mi = {0};
    if (!GetModuleInformation(GetCurrentProcess(), baseAddr, &mi, sizeof(mi)))
        throw std::runtime_error("Failed to get game module information.");
    imageSize = mi.SizeOfImage;

    // Get game window handle.
    window = FindWindowA(nullptr, "Growtopia");
    if (window == nullptr)
        throw std::runtime_error("Failed to get game window handle.");

    // Initialize minhook.
    MH_STATUS status = MH_Initialize();
    if (status != MH_OK)
    {
        throw std::runtime_error("Failed to initialize MinHook: " + std::string(MH_StatusToString(status)));
    }

    // Block function until game processes this message. This lets us know the game is in an
    // "initialized" state.
    SendMessageA(window, WM_NULL, 0, 0);
}

void GameHarness::setWindowTitle(const std::string& title)
{
    SetWindowTextA(window, title.c_str());
}

} // namespace game