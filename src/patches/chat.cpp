#include "game.hpp"
#include "utils.hpp"
#include <cstddef>
#include <cstdint>

// CreateLogOverlay
REGISTER_GAME_FUNCTION(CreateLogOverlay,
                       "48 8B C4 55 41 54 41 55 41 56 41 57 48 8D A8 E8 FC FF FF 48 81 EC F0 "
                       "03 00 00 48 C7 45 A0 FE FF FF FF 48 89 58 08 48",
                       __fastcall, void, float*, float*, bool)

// TabComponent::AddTabButton
REGISTER_GAME_FUNCTION(TabComponentAddTabButton,
                       "48 8B C4 55 56 57 41 54 41 55 41 56 41 57 48 8D A8 28 FF FF FF 48 81 "
                       "EC A0 01 00 00 48 C7 45 B0 FE FF FF FF 48 89 58 20 0F 29 70 B8 0F 29 "
                       "78 A8 44 0F 29 48 98 48 8B 05 04 4A 4C 00",
                       __fastcall, __int64, void*, void*, float*, void*, void*, void*)

// For grouping info about iPadMapX(...) calls.
struct iPadMapXCallData
{
    std::string pattern;
    size_t offset;
    size_t size;
};

class LegacyChatPatch : public game::BasePatch
{
  public:
    void apply() const override
    {
        auto& game = game::GameHarness::get();
        // Hook CreateLogOverlay.
        game.hookFunctionPatternDirect(pattern::CreateLogOverlay, CreateLogOverlay,
                                       &real::CreateLogOverlay);
        // Hook TabComponent::AddTabButton.
        game.hookFunctionPatternDirect(pattern::TabComponentAddTabButton, TabComponentAddTabButton,
                                       &real::TabComponentAddTabButton);

        // Patch out various iPadMapX(...) calls.
        // These patches primarily focus on the iPadMapX call and prepending MOVSS instructions.
        // Nopping these out result in scaling and alignment being as they were in old chat UI.
        // On CreateInputChatBar, it left-aligns the input bar. MainMenuControls nops patch icon
        // scaling. In order: CreateInputChatBar, AddMainMenuControls (+ Large Screen variant).
        std::vector<iPadMapXCallData> padMapCalls = {
            {"48 8B C8 E8 ? ? ? ? F3 0F 10 ? ? ? ? ? E8 ? ? ? ? 0F 28 F0 45 0F 57 DB", 16, 8},
            {"41 0F 28 C3 E8 ? ? ? ? F3 0F 10 ? ? ? ? ? E8 ? ? ? ? 44 0F 28 C0", 9, 13},
            {"3D D0 07 00 00 7C 11 F3 0F 10 ? ? ? ? ? E8 ? ? ? ? 44 0F 28 C0 E8", 7, 13}};
        for (const auto& call : padMapCalls)
        {
            auto addr = game.findMemoryPattern<uint8_t*>(call.pattern);
            utils::nopMemory(addr + call.offset, call.size);
        }

        // Patch out CreateLogOverlay one separately, need to XORPS XMM6 register afterwards.
        // We need to do this, because XMM0 was set with data before. We are skipping over a
        // MOVSS instruction and subsequent call to iPadMapX. The game originally then tries
        // to copy XMM0 register into XMM6, causing text at random chance to appear way off-
        // screen. We need XMM6 to start at 0.0, so we replace the MOVSS with a XORPS.
        auto addr = game.findMemoryPattern<uint8_t*>(
            "48 8D 4D 38 E8 ? ? ? ? F3 0F 10 ? ? ? ? ? E8 ? ? ? ? 0F 28 F0");
        utils::nopMemory(addr + 9, 16);
        utils::writeMemoryPattern(addr + 22, "0F 57 F6");

        // Patch out TabComponent padding in LogTextOffset.
        // This is required, because otherwise, even though we patched out tabs from never
        // appearing on-screen, LogOverlay's text container gets resized when TabContainer
        // exists. By changing JBE instruction to a JMP, this resizing logic is always skipped.
        addr = game.findMemoryPattern<uint8_t*>(
            "76 40 0F BE ? ? ? ? ? F3 0F 2C ? ? ? ? ? 03 C8 29 ? ? ? ? ? F3 0F 10 B3 00 02");
        utils::fillMemory(addr, 1, 0xEB);

        // Patch out the "`7[S]``" string.
        // The old chat UI never had any classification for System/World/Private/etc messages,
        // instead it relied on client giving contextual clues by message formatting on what
        // the message type was.
        addr = game.findMemoryPattern<uint8_t*>("60 37 5B 53 5D 20 60 60 00");
        utils::fillMemory(addr, 8, 0);
    }

    static void __fastcall CreateLogOverlay(float* pos2d, float* size2d, bool unk3)
    {
        // Readjust the chat overlay size by removing the left-padding and placing it to size2d
        // instead.
        size2d[0] += pos2d[0];
        size2d[1] += pos2d[1];
        pos2d[0] = 0.0f;
        pos2d[1] = 0.0f;
        real::CreateLogOverlay(pos2d, size2d, unk3);
    }

    static __int64 __fastcall TabComponentAddTabButton(void* this_, void* entity, float* xCoord,
                                                       void* unk4, void* unk5, void* unk6)
    {
        // Tabs pass on xCoord parameter to continiously align themselves after one another.
        // We can force this to always be off-screen, essentially removing them from visible
        // game area.
        *xCoord = -500.0f;
        return real::TabComponentAddTabButton(this_, entity, xCoord, unk4, unk5, unk6);
    }
};
REGISTER_GAME_PATCH(LegacyChatPatch, enable_legacy_chat);

class NoGuildIconPatch : public game::BasePatch
{
    void apply() const override
    {
        auto& game = game::GameHarness::get();
        // Remove the guild/leaderboards icon from MainMenuControls.
        // This patch nops the ADDSS instructions responsible for offsetting the gems counter.
        // As a result, the gem counter is back on its legacy positioning.
        auto addr =
            game.findMemoryPattern<uint8_t*>("E8 ? ? ? ? F3 0F 58 78 04 F3 0F 58 F7 F3 0F 10");
        utils::nopMemory(addr + 5, 9);
    }
};
REGISTER_GAME_PATCH(NoGuildIconPatch, no_guild_icon);