#include "patch.hpp"
#include <cstdio>
#include <fstream>
#include <stdexcept>

namespace patch
{

PatchManager& PatchManager::get()
{
    static PatchManager instance;
    return instance;
}

void PatchManager::applyPatchesFromFile(const std::string& fileName)
{
    // First apply all core patches.
    for (auto patch : patchMap)
    {
        if (patch.second.type == PatchType::Core)
            applyPatch(patch.first);
    }
    // Then apply user patches, according to user patch list file.
    std::ifstream file(fileName);
    if (!file.is_open())
    {
        // If the file doesn't exist, then all user patches should be applied.
        std::fprintf(stderr, "No patch configuration file found, applying all user patches.\n");
        for (auto patch : patchMap)
        {
            if (patch.second.type == PatchType::User)
                applyPatch(patch.first);
        }
        return;
    }
    // Otherwise, only apply the patches that are listed in the file.
    std::fprintf(stderr, "Found config file, applying patches from that.\n");
    std::string line;
    while (std::getline(file, line))
    {
        // Treat lines starting with '+' as patch names.
        if (line.size() == 0 || line.at(0) != '+')
            continue;
        line.erase(0, 1); // Remove the '+' character.
        applyPatch(line);
    }
}

void PatchManager::applyPatch(const std::string& name)
{
    if (patchMap.find(name) == patchMap.end())
        throw std::runtime_error("Patch " + name + " does not exist.");
    if (patchMap[name].applied)
        return;
    // Try to apply the patch.
    const char* type = patchMap[name].type == PatchType::Core ? "core" : "user";
    try
    {
        patchMap[name].instance->apply();
        patchMap[name].applied = true;
        std::fprintf(stdout, "Applied %s patch %s\n", type, name.c_str());
    }
    catch (const std::exception& e)
    {
        throw std::runtime_error("Failed to apply patch " + name + ": " + e.what());
    }
}

std::vector<std::string> PatchManager::getAppliedUserPatchList() const
{
    std::vector<std::string> patches;
    for (auto patch : patchMap)
    {
        if (patch.second.type == PatchType::User && patch.second.applied)
            patches.push_back(patch.first);
    }
    return patches;
}

bool PatchManager::isPatchApplied(const std::string& name)
{
    if (patchMap.find(name) == patchMap.end())
        return false;
    return patchMap[name].applied;
}

} // namespace patch