#include "game/game.hpp"
#include "game/signatures.hpp"
#include "game/struct/components/gamelogic.hpp"
#include "patch/patch.hpp"
#include "utils/utils.hpp"
#include <cstddef>
#include <cstdint>
#include <string>

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

// LogToConsoleSafe
REGISTER_GAME_FUNCTION(
    LogToConsoleSafe,
    "4C 8B DC 57 48 83 EC 60 49 C7 43 B8 FE FF FF FF 49 89 5B 10 49 89 6B 18 49 89 73 20 48 8B ? ? "
    "? ? ? 48 33 C4 48 89 44 24 50 48 8B E9 49 89 4B C0 49 C7 43 E0 0F 00 00 00",
    __fastcall, void, std::string);

REGISTER_GAME_FUNCTION(AddMainMenuControls,
                       "48 8B C4 55 41 54 41 55 41 56 41 57 48 8D A8 18 FA FF FF", __fastcall, void,
                       Entity*);

REGISTER_GAME_FUNCTION(
    OpenLogConsole,
    "48 8B C4 55 57 41 56 48 8D 68 A1 48 81 EC D0 00 00 00 48 C7 45 D7 FE FF FF FF 48 89 58 10",
    __fastcall, void, VariantList*);
REGISTER_GAME_FUNCTION(OnLogGrabEnd,
                       "48 8B C4 55 48 8B EC 48 83 EC 70 48 C7 45 C0 FE FF FF FF 48 89 58 08",
                       __fastcall, void, VariantList*);
REGISTER_GAME_FUNCTION(OnLogGrabMove,
                       "40 53 48 83 EC 30 48 8D 59 48 C6 05 ? ? ? ? ? 83 3B 00 75 1D", __fastcall,
                       void, VariantList*);
REGISTER_GAME_FUNCTION(OnInventoryTapToggle,
                       "48 8B C4 55 48 8D 68 A1 48 81 EC A0 00 00 00 48 C7 45 07 FE FF FF FF",
                       __fastcall, void, VariantList*);
REGISTER_GAME_FUNCTION(OnInventoryGrabMove,
                       "48 8B C4 55 48 8B EC 48 81 EC 80 00 00 00 48 C7 45 B0 FE FF FF FF 48 89 58 "
                       "10 48 89 70 18 48 89 78 20 0F 29 70 E8",
                       __fastcall, void, VariantList*);
REGISTER_GAME_FUNCTION(OnInventoryGrabEnd,
                       "48 8B C4 55 48 8D 68 A1 48 81 EC A0 00 00 00 48 C7 45 EF FE FF FF FF 48 89 "
                       "58 10 48 89 70 18 48 89 78 20",
                       __fastcall, void, VariantList*);

// For grouping info about iPadMapX(...) calls.
struct iPadMapXCallData
{
    std::string pattern;
    size_t offset;
    size_t size;
};

class LegacyChatPatch : public patch::BasePatch
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
        game.hookFunctionPatternDirect(pattern::LogToConsoleSafe, LogToConsoleSafe,
                                       &real::LogToConsoleSafe);

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

    static void __fastcall LogToConsoleSafe(std::string text)
    {
        // Re-implement pre-chat update LogToConsoleSafe function.
        // LogDisplay Component is located in Root -> ConsoleLogParent -> LogRectBG -> LogScroller
        // -> scroll_child -> LogDisplayEntity -> LogDisplay
        // nit: This is technically more performant than original since our Entity funct doesn't do
        // recursive search, but this is ugly at the same time.
        Entity* pEntityRoot = real::GetApp()->m_entityRoot;
        Entity* pLogParent = pEntityRoot->GetEntityByName("ConsoleLogParent");
        if (pLogParent == nullptr)
            return;
        Entity* pLogRectBG = pLogParent->GetEntityByName("LogRectBG");
        if (pLogRectBG == nullptr)
            return;
        Entity* pLogScroller = pLogRectBG->GetEntityByName("LogScroller");
        if (pLogScroller == nullptr)
            return;
        Entity* pScrollChild = pLogScroller->GetEntityByName("scroll_child");
        if (pScrollChild == nullptr)
            return;
        Entity* pLogDisplayEnt = pScrollChild->GetEntityByName("LogDisplayEntity");
        if (pLogDisplayEnt == nullptr)
            return;
        EntityComponent* pLogDisplayComp = pLogDisplayEnt->GetComponentByName("LogDisplay");
        if (pLogDisplayComp == nullptr)
            return;
        // todo: Replace CT:[W]_ stuff.
        std::string line = "`o" + text + "``";
        VariantList vl;
        vl.Get(0).Set(line);
        pLogDisplayComp->GetShared()->CallFunctionIfExists("AddLine", &vl);
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
REGISTER_USER_GAME_PATCH(LegacyChatPatch, enable_legacy_chat);

class NoGuildIconPatch : public patch::BasePatch
{
    void apply() const override
    {
        auto& game = game::GameHarness::get();
        // Remove the guild/leaderboards icon from MainMenuControls.
        // This patch spoofs the protocol version when there is no active event.
        // As a result, the gem counter is back on its legacy positioning.
        game.hookFunctionPatternDirect(pattern::AddMainMenuControls, AddMainMenuControls,
                                       &real::AddMainMenuControls);
    }

    static void __fastcall AddMainMenuControls(Entity* pEnt)
    {
        // nit: If the server supports toggles EventUI on the fly, then this will make it not appear
        // until GameMenu is recreated, not a problem for OSGT though. You'd probably have to hook
        // GameLogicComponent::OnClashEventIsActiveChanged to do anything about it.
        // Alternate solution: Patch size2d of EVENTS entity to be 0/0 and move GemsBux when needed.
        if (!real::GetApp()->GetGameLogic()->m_activeGuildEvent)
        {
            int originalProtocol = real::GetApp()->m_serverProtocol;
            // The check is `0x23 < GetApp()->m_serverProtocol`, so we'll just spoof to skip it.
            real::GetApp()->m_serverProtocol = 0x23;
            real::AddMainMenuControls(pEnt);
            real::GetApp()->m_serverProtocol = originalProtocol;
        }
        else
            real::AddMainMenuControls(pEnt);
        auto& patchMgr = patch::PatchManager::get();
        if (patchMgr.isPatchApplied("hide_my_ui"))
        {
            Variant* pVariant = real::GetApp()->GetVar("hide_ui_opacity");
            pVariant->Set(pVariant->GetFloat());
        }
    }
};
REGISTER_USER_GAME_PATCH(NoGuildIconPatch, no_guild_icon);

class ChatLimitExtended : public patch::BasePatch
{
    void apply() const override
    {
        Variant* pVariant = real::GetApp()->GetVar("osgt_qol_extend_console");
        if (pVariant->GetType() != Variant::TYPE_UINT32)
            pVariant->Set(uint32_t(1));

        auto& optionsMgr = game::OptionsManager::get();
        optionsMgr.addCheckboxOption("qol", "UI", "osgt_qol_extend_console",
                                     "Extend chat history limit to 500", &OnChatLimitCallback);
        SetConsoleLogLimit();
    }

    static void OnChatLimitCallback(VariantList* pVariant)
    {
        Entity* pCheckbox = pVariant->Get(1).GetEntity();
        bool bChecked = pCheckbox->GetVar("checked")->GetUINT32() != 0;
        real::GetApp()->GetVar("osgt_qol_extend_console")->Set(uint32_t(bChecked));
        SetConsoleLogLimit();
    }

    static void SetConsoleLogLimit()
    {
        real::GetApp()->m_logConsole.m_maxLines =
            real::GetApp()->GetVar("osgt_qol_extend_console")->GetUINT32() == 1 ? 500 : 125;
    }
};
REGISTER_USER_GAME_PATCH(ChatLimitExtended, chat_limit_extended);

static bool gIsUserDrag;
static unsigned int gStartedDrag = 0;
class FixHandleDrag : public patch::BasePatch
{
    void apply() const override
    {
        // This fixes the chat and inventory "grab" bouncing when releasing them too fast by adding
        // a forced 1ms "hold" time when moving the handles before the value is cleared.
        // We also determine any move done as "under 5ms" as not hold. Maybe that value should be
        // tweaked still, who knows!
        // Since you can interact with one GUI element at a time, we're reusing the timer variables
        // for both.
        // TODO: Make toggleable in-game maybe?
        auto& game = game::GameHarness::get();

        // Chat Handles
        game.hookFunctionPatternDirect(pattern::OpenLogConsole, OpenLogConsole,
                                       &real::OpenLogConsole);
        game.hookFunctionPatternDirect(pattern::OnLogGrabMove, OnLogGrabMove, &real::OnLogGrabMove);
        game.hookFunctionPatternDirect(pattern::OnLogGrabEnd, OnLogGrabEnd, &real::OnLogGrabEnd);

        // Inventory Handles
        game.hookFunctionPatternDirect(pattern::OnInventoryTapToggle, OnInventoryTapToggle,
                                       &real::OnInventoryTapToggle);
        game.hookFunctionPatternDirect(pattern::OnInventoryGrabMove, OnInventoryGrabMove,
                                       &real::OnInventoryGrabMove);
        game.hookFunctionPatternDirect(pattern::OnInventoryGrabEnd, OnInventoryGrabEnd,
                                       &real::OnInventoryGrabEnd);
    }

    // CHAT HANDLES
    static void __fastcall OpenLogConsole(VariantList* pVL)
    {
        // Block if we haven't released the lock yet. This causes the automatic sliding to occur.
        if (gIsUserDrag)
            return;
        real::OpenLogConsole(pVL);
    }
    static void __fastcall OnLogGrabMove(VariantList* pVL)
    {
        // Set game MS timer as our anchor point and lock OpenLogConsole.
        if (gStartedDrag == 0)
        {
            gStartedDrag = real::GetApp()->m_gameTimer.m_timeMS;
            gIsUserDrag = true;
        }
        real::OnLogGrabMove(pVL);
    }
    static void __fastcall OnLogGrabEnd(VariantList* pVL)
    {
        real::OnLogGrabEnd(pVL);
        // If we didn't even send any move events or we spent less than 5ms holding, treat it as a
        // click.
        if ((real::GetApp()->m_gameTimer.m_timeMS - gStartedDrag) < 5 || gStartedDrag == 0)
        {
            gIsUserDrag = false;
            gStartedDrag = 0;
        }
        else
        {
            // Add a callable function to the handle to release gIsUserDrag and gStartedDrag values.
            if (!pVL->Get(1).GetEntity()->GetShared()->GetFunctionIfExists("OnFlipUserDrag"))
                pVL->Get(1)
                    .GetEntity()
                    ->GetFunction("OnFlipUserDrag")
                    ->sig_function.connect(OnEndFlipUserDrag);
            real::MessageManagerCallEntityFunction(real::GetMessageManager(),
                                                   pVL->Get(1).GetEntity(), 1, "OnFlipUserDrag",
                                                   nullptr, 0);
        }
    }

    // INVENTORY HANDLES
    static void __fastcall OnInventoryTapToggle(VariantList* pVL)
    {
        if (gIsUserDrag)
            return;
        real::OnInventoryTapToggle(pVL);
    }
    static void __fastcall OnInventoryGrabMove(VariantList* pVL)
    {
        if (gStartedDrag == 0)
        {
            gStartedDrag = real::GetApp()->m_gameTimer.m_timeMS;
            gIsUserDrag = true;
        }
        real::OnInventoryGrabMove(pVL);
    }
    static void __fastcall OnInventoryGrabEnd(VariantList* pVL)
    {
        real::OnInventoryGrabEnd(pVL);
        if ((real::GetApp()->m_gameTimer.m_timeMS - gStartedDrag) < 5 || gStartedDrag == 0)
        {
            gIsUserDrag = false;
            gStartedDrag = 0;
        }
        else
        {
            if (!pVL->Get(1).GetEntity()->GetShared()->GetFunctionIfExists("OnFlipUserDrag"))
                pVL->Get(1)
                    .GetEntity()
                    ->GetFunction("OnFlipUserDrag")
                    ->sig_function.connect(OnEndFlipUserDrag);
            real::MessageManagerCallEntityFunction(real::GetMessageManager(),
                                                   pVL->Get(1).GetEntity(), 1, "OnFlipUserDrag",
                                                   nullptr, 0);
        }
    }

    // SHARED
    static void OnEndFlipUserDrag(VariantList* pVL)
    {
        gIsUserDrag = false;
        gStartedDrag = 0;
    }
};
REGISTER_USER_GAME_PATCH(FixHandleDrag, fix_handle_drag);
