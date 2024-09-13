#include "game.hpp"
#include <cstdio>
#include <fstream>
#include <set>

// Read patch list from a file.
std::set<std::string> createPatchListFromFile(std::ifstream& file)
{
    std::set<std::string> list;
    std::string line;
    while (std::getline(file, line))
    {
        // Treat lines starting with '+' as patch names.
        if (!line.starts_with('+'))
            continue;
        line.erase(0, 1); // Remove the '+' character.
        list.insert(line);
    }
    return list;
}

// Entry point.
void setup()
{
#ifndef NDEBUG
    // Create a console window for debug builds.
    AllocConsole();
    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stderr);
#endif

    // Try opening the patch list file.
    bool applyAll = false;
    std::set<std::string> patchList;
    std::ifstream file("patches.txt");
    if (!file.is_open())
    {
        // No patches file found, apply all.
        applyAll = true;
        fprintf(stderr, "No patches.txt found, applying all patches.\n");
    }
    else
    {
        // Otherwise, read the patch list from a file.
        patchList = createPatchListFromFile(file);
        fprintf(stderr, "Read %zu patches from patches.txt.\n", patchList.size());
        // Ensure integrity bypass is always applied.
        patchList.insert("integrity_bypass");
    }

    // Initialize game harness and apply patches.
    auto& game = game::GameHarness::get();
    try
    {
        game.initialize();
        if (applyAll)
            game.applyAllPatches();
        else
        {
            for (const auto& patch : patchList)
                game.applyPatch(patch);
        }
        fprintf(stderr, "Done applying patches.\n");
        game.updateWindowTitle();
    }
    catch (const std::exception& e)
    {
#ifndef NDEBUG
        // Output error to console.
        std::fprintf(stderr, "FATAL: %s\n", e.what());
        return;
#else
        // Show error message box and exit.
        MessageBoxA(nullptr, e.what(), "Error", MB_ICONERROR | MB_OK);
        ExitProcess(EXIT_FAILURE);
#endif
    }
}