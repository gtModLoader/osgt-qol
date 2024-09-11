#include "game/game.hpp"
#include "utils/hooking.hpp"
#include "utils/memory.hpp"
#include "utils/pattern.hpp"
#include <winuser.h>

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
    ok = hooking::hookFunctionPatternDirect<SetFPSLimit_t>(pattern::SetFPSLimit, hook::SetFPSLimit,
                                                           &game::SetFPSLimit);
    if (!ok)
    {
        std::printf("Failed to hook SetFPSLimit!\n");
        return;
    }
    hook::SetFPSLimit(nullptr, 60.0f);
    printf("Hooked SetFPSLimit.\n");

    // Hook PetRenderData::Update.
    ok = hooking::hookFunctionPatternDirect<PetRenderDataUpdate_t>(
        pattern::PetRenderDataUpdate, hook::PetRenderDataUpdate, &game::PetRenderDataUpdate);
    if (!ok)
    {
        std::printf("Failed to hook PetRenderData::Update!\n");
        return;
    }
    printf("Hooked PetRenderData::Update.\n");

    // Hook AudioManagerFMOD::Preload.
    ok = hooking::hookFunctionPatternDirect<AudioManagerFMODPreload_t>(
        pattern::AudioManagerFMODPreload, hook::AudioManagerFMODPreload,
        &game::AudioManagerFMODPreload);
    if (!ok)
    {
        std::printf("Failed to hook AudioManagerFMOD::Preload!\n");
        return;
    }
    printf("Hooked AudioManagerFMOD::Preload.\n");

    // UI modding - Attempt to restore chat to pre-rework state
    // Hook CreateLogOverlay
    ok = hooking::hookFunctionPatternDirect<CreateLogOverlay_t>(
        pattern::CreateLogOverlay, hook::CreateLogOverlay, &game::CreateLogOverlay);
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