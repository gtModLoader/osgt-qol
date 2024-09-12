#include "game.hpp"
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
        auto msg = std::format("Failed to initialize MinHook: {}", MH_StatusToString(status));
        throw std::runtime_error(msg);
    }

    // Block function until game processes this message. This lets us know the game is in an
    // "initialized" state.
    SendMessageA(window, WM_NULL, 0, 0);
}

void GameHarness::registerPatch(const std::string& name, BasePatch* patch)
{
    patches[name] = patch;
}

void GameHarness::applyPatch(const std::string& name)
{
    // Look up the patch by name and apply it.
    auto it = patches.find(name);
    if (it == patches.end())
        throw std::runtime_error(std::format("Patch '{}' not found.", name));
    it->second->apply();
};

void GameHarness::applyAllPatches()
{
    for (const auto& [name, patch] : patches)
    {
        try
        {
            patch->apply();
        }
        catch (const std::exception& e)
        {
            auto msg = std::format("Failed to apply patch {}: {}", name, e.what());
            throw std::runtime_error(msg);
        }
    }
}

void GameHarness::updateWindowTitle()
{
    std::string newTitle = std::format("Growtopia [OSGT-QOL] - {} patches applied", patches.size());
    SetWindowTextA(window, newTitle.c_str());
}

} // namespace game