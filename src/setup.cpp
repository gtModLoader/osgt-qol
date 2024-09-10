#include "utils/hooking.hpp"
#include "utils/memory.hpp"
#include "utils/pattern.hpp"
#include <winuser.h>

using SetFPSLimit_t = void(__fastcall*)(void*, float);
using PetRenderDataUpdate_t = void(__fastcall*)(void*, void*, float);
using AudioManagerFMODPreload_t = void(__fastcall*)(void*, void*, bool, bool, bool, bool);
using CreateLogOverlay_t = void(__fastcall*)(float*, float*, bool);

namespace real
{
SetFPSLimit_t SetFPSLimit = nullptr;
PetRenderDataUpdate_t PetRenderData_Update = nullptr;
AudioManagerFMODPreload_t AudioManagerFMOD_Preload = nullptr;
CreateLogOverlay_t CreateLogOverlay = nullptr;
} // namespace real

namespace hooked
{

void SetFPSLimit(void* this_, float fps)
{
    DEVMODE dm = {0};
    dm.dmSize = sizeof(DEVMODE);
    // Retrieve primary display refresh rate or fall back to vanilla 60 fps.
    if (EnumDisplaySettingsA(NULL, ENUM_CURRENT_SETTINGS, &dm))
        real::SetFPSLimit(this_, static_cast<float>(dm.dmDisplayFrequency));
    else
        real::SetFPSLimit(this_, 60.f);
}

void PetRenderDataUpdate(void* this_, void* unk2, float delta)
{
    // The game relies on using this for pet movement correction during fps fluctuations. However,
    // this logic only works properly until 60 FPS. So for our high-fps mod, we patch this to force
    // game to always run an update on pets movement, causing them to move smoothly and as expected
    // again. There are some slight drawbacks to this approach, but it's relatively stable and good
    // enough for vast majority of players.
    *(float*)((uint8_t*)(this_) + 92) = 0.016666668f;
    real::PetRenderData_Update(this_, unk2, delta);
}

void AudioManagerFMODPreload(void* this_, void* unk2, bool bLooping, bool bIsMusic,
                             bool bAddBasePath, bool bForceStreaming)
{
    // Current assumption is PC client had streaming disabled because of Seth's attempt to utilize
    // extra memory resources in order to save performance and cache audio files. However, in
    // practice, the Growtopia client fails to properly utilize said caching and as such creates an
    // annoying stutter. This forces every call to AudioManagerFMOD::Preload to use file streaming
    // to fix said stutter.
    bForceStreaming = true;
    real::AudioManagerFMOD_Preload(this_, unk2, bLooping, bIsMusic, bAddBasePath, bForceStreaming);
}

void CreateLogOverlay(float* pos2d, float* size2d, bool unk3)
{
    // Readjust the chat overlay size by removing the left-padding and placing it to size2d instead
    size2d[0] += pos2d[0];
    size2d[1] += pos2d[1];
    pos2d[0] = 0.0f;
    pos2d[1] = 0.0f;
    real::CreateLogOverlay(pos2d, size2d, unk3);
}

} // namespace hooked

void waitForGameInit()
{
    // Blocks until game processes this message.
    SendMessageA(memory::getMainWindow(), WM_NULL, 0, 0);
}

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
    waitForGameInit();
    std::printf("Game initialized!\n");

    // Patch out the the memory CRC check.
    auto patternCrcCheck = pattern::parseIDA("00 3B C1 75 ? 85 C9");
    if (!patternCrcCheck.has_value())
    {
        std::printf("Failed to parse CRC check pattern!\n");
        return;
    }
    auto addrCrcCheck = memory::find<uint8_t*>(patternCrcCheck.value());
    if (addrCrcCheck == nullptr)
    {
        std::printf("Failed to find CRC check address!\n");
        return;
    }
    memory::nop(((uint8_t*)addrCrcCheck) + 1, 6);
    printf("Patched out CRC check.\n");

    bool ok = false;
    // Hook SetFPSLimit.
    ok = hooking::hookFunctionPatternDirect<SetFPSLimit_t>(
        "4C 8B DC 48 81 EC D8 00 00 00 48 C7 44 24 20 FE FF FF FF "
        "48 8B ? ? ? ? ? 48 33 C4 48 89 84 24 C0 00 00 00 0F 57 C0",
        hooked::SetFPSLimit, &real::SetFPSLimit);
    if (!ok)
    {
        std::printf("Failed to hook SetFPSLimit!\n");
        return;
    }
    hooked::SetFPSLimit(nullptr, 60.0f);
    printf("Hooked SetFPSLimit.\n");

    // Hook PetRenderData::Update.
    ok = hooking::hookFunctionPatternDirect<PetRenderDataUpdate_t>(
        "48 8B C4 48 89 58 10 48 89 68 18 56 57 41 56 48 81 EC A0 00 00 00 0F B7",
        hooked::PetRenderDataUpdate, &real::PetRenderData_Update);
    if (!ok)
    {
        std::printf("Failed to hook PetRenderData::Update!\n");
        return;
    }
    printf("Hooked PetRenderData::Update.\n");

    // Hook AudioManagerFMOD::Preload.
    ok = hooking::hookFunctionPatternDirect<AudioManagerFMODPreload_t>(
        "40 55 56 57 41 54 41 55 41 56 41 57 48 8D AC 24 30 "
        "FF FF FF 48 81 EC D0 01 00 00 48 C7 44 24 38 FE FF FF FF",
        hooked::AudioManagerFMODPreload, &real::AudioManagerFMOD_Preload);
    if (!ok)
    {
        std::printf("Failed to hook AudioManagerFMOD::Preload!\n");
        return;
    }
    printf("Hooked AudioManagerFMOD::Preload.\n");

    // UI modding - Attempt to restore chat to pre-rework state
    // Hook CreateLogOverlay
    ok = hooking::hookFunctionPatternDirect<CreateLogOverlay_t>(
        "48 8B C4 55 41 54 41 55 41 56 41 57 48 8D A8 E8 FC FF FF "
        "48 81 EC F0 03 00 00 48 C7 45 A0 FE FF FF FF 48 89 58 08 48",
        hooked::CreateLogOverlay, &real::CreateLogOverlay);
    if (!ok)
    {
        std::printf("Failed to hook CreateLogOverlay!\n");
        return;
    }
    printf("Hooked CreateLogOverlay.\n");

    // We have to seek out each iPadMapX(45) call to make elements align proper again
    // Patch out iPadMapX(45) call in CreateLogOverlay
    auto patternMapXCall =
        pattern::parseIDA("48 8D 4D 38 E8 ? ? ? ? F3 0F 10 ? ? ? ? ? E8 ? ? ? ? 0F 28 F0");
    if (!patternMapXCall.has_value())
    {
        std::printf("Failed to parse CreateLogOverlay padding pattern!\n");
        return;
    }
    auto addrMapXCall = memory::find<uint8_t*>(patternMapXCall.value());
    if (addrMapXCall == nullptr)
    {
        std::printf("Failed to find CreateLogOverlay padding call!\n");
        return;
    }
    // Nop the call instruction and the movaps instruction after it.
    // FIXME: Changing resolution causes chat to stay blank until a complete relogin from OnlineMenu
    memory::nop(((uint8_t*)addrMapXCall) + 17, 8);
    printf("Patched out CreateLogOverlay padding.\n");

    // Patch out iPadMapX(45) call in CreateInputChatBar
    patternMapXCall =
        pattern::parseIDA("48 8B C8 E8 ? ? ? ? F3 0F 10 ? ? ? ? ? E8 ? ? ? ? 0F 28 F0 45 0F 57 DB");
    if (!patternMapXCall.has_value())
    {
        std::printf("Failed to parse CreateInputChatBar padding pattern!\n");
        return;
    }
    addrMapXCall = memory::find<uint8_t*>(patternMapXCall.value());
    if (addrMapXCall == nullptr)
    {
        std::printf("Failed to find CreateInputChatBar padding call!\n");
        return;
    }
    // Nop the call instruction and the movaps instruction after it.
    memory::nop(((uint8_t*)addrMapXCall) + 16, 8);
    printf("Patched out CreateInputChatBar padding.\n");

    // Patch out iPadMapX(45) call in AddMainMenuControls
    patternMapXCall =
        pattern::parseIDA("41 0F 28 C3 E8 ? ? ? ? F3 0F 10 ? ? ? ? ? E8 ? ? ? ? 44 0F 28 C0");
    if (!patternMapXCall.has_value())
    {
        std::printf("Failed to parse AddMainMenuControls padding pattern!\n");
        return;
    }
    addrMapXCall = memory::find<uint8_t*>(patternMapXCall.value());
    if (addrMapXCall == nullptr)
    {
        std::printf("Failed to find AddMainMenuControls padding call!\n");
        return;
    }
    // Nop out the movss/call
    memory::nop(((uint8_t*)addrMapXCall) + 9, 13);
    printf("Patched out AddMainMenuControls padding.\n");

    // Patch out iPadMapX(55) call for large displays in AddMainMenuControls
    // NIT: Should probably be replaced with an iPadMapX(10) to keep original aspect ratio?
    patternMapXCall =
        pattern::parseIDA("3D D0 07 00 00 7C 11 F3 0F 10 ? ? ? ? ? E8 ? ? ? ? 44 0F 28 C0 E8");
    if (!patternMapXCall.has_value())
    {
        std::printf("Failed to parse AddMainMenuControls widescreen padding pattern!\n");
        return;
    }
    addrMapXCall = memory::find<uint8_t*>(patternMapXCall.value());
    if (addrMapXCall == nullptr)
    {
        std::printf("Failed to find AddMainMenuControls widescreen padding call!\n");
        return;
    }
    // Nop out the movss/call
    memory::nop(((uint8_t*)addrMapXCall) + 7, 13);
    printf("Patched out AddMainMenuControls widescreen padding.\n");

    // Null the [S] string used for system messages.
    auto patternSystemMessage = pattern::parseIDA("60 37 5b 53 5d 20 60 60 00");
    if (!patternSystemMessage.has_value())
    {
        std::printf("Failed to parse `7[S]`` string bytes!\n");
        return;
    }
    auto addrSystemMessage = memory::find<uint8_t*>(patternSystemMessage.value());
    if (addrSystemMessage == nullptr)
    {
        std::printf("Failed to find `7[S]`` string!\n");
        return;
    }
    memory::fill(addrSystemMessage, 8, 0);
    printf("Nulled `7[S]`` string.\n");
}