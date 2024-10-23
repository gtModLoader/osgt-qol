#include "game/game.hpp"
#include "game/signatures.hpp"
#include "game/struct/app.hpp"
#include "game/struct/entity.hpp"
#include "game/struct/component.hpp"
#include "game/struct/components/gamelogic.hpp"
#include "patch/patch.hpp"
#include "utils/utils.hpp"

// NetControllerLocal::OnArcadeInput
// Params: this, keyCode, bKeyFired
REGISTER_GAME_FUNCTION(NetControllerLocalOnArcadeInput,
                       "48 89 5C 24 08 48 89 74 24 10 57 48 83 EC 20 41 0F B6 D8 8B F2 48 8B F9 ? "
                       "? ? ? ? 48 8B C8 ? ? ? ? ? 48",
                       __fastcall, void, void*, int, bool);

// GetArcadeComponent
// Returns: ArcadeComponent* (derived from EntityComponent)
REGISTER_GAME_FUNCTION(GetArcadeComponent,
                       "E8 ? ? ? ? 48 8B C8 45 33 C9 44 0F B6 C3 33 D2 E8 ? ? ? ?", __fastcall,
                       EntityComponent*);

// AddKeyBinding
// Params: Parent EntityComponent*, Keybind name, ASCII input code, Output code for callback, bool
// bAlsoSendAsNormalRawKey (?), modifiersRequired (1 requries CTRL, 0 none)
REGISTER_GAME_FUNCTION(AddKeyBinding, "40 55 56 57 48 8D AC 24 50 FD FF FF 48 81 EC B0", __fastcall,
                       void, EntityComponent* pComp, std::string name, uint32_t inputcode,
                       uint32_t outputcode, bool bAlsoSendAsNormalRawKey,
                       uint32_t modifiersRequired);

// AddWASDKeys
REGISTER_GAME_FUNCTION(AddWASDKeys,
                       "40 55 48 8B EC 48 83 EC 50 F3 0F 10 05 1F 72 22 00 F3 0F 10 0D BF 7C",
                       __fastcall, void);

// ToolSelectComponent::OnTouchStart
// Params: this
REGISTER_GAME_FUNCTION(ToolSelectComponentOnTouchStart,
                       "48 89 5C 24 10 48 89 6C 24 20 56 48 81 EC A0 00 00 00 F3 0F 2C 05 FE 6D 35",
                       __fastcall, void, void*);

// GameLogicComponent::DialogIsOpen
// Returns: bool
// Params: this
REGISTER_GAME_FUNCTION(
    GameLogicComponentDialogIsOpen,
    "40 55 48 8B EC 48 83 EC 50 48 C7 45 D0 FE FF FF FF 48 89 5C 24 60 48 89 7C 24 68 48 8B",
    __fastcall, bool, GameLogicComponent*);

class HotkeyPatch : public patch::BasePatch
{
  public:
    void apply() const override
    {
        // With this patch, we add our own custom hotkeys to ArcadeInputComponent.
        // The current method used involves injecting them after AddWASDKeys.

        // This approach has one key benefit - the game handles removal and rebinding of our hotkeys
        // on demand. When you open the in-game chat, it will call OnNativeInputStateChanged. That
        // function will respectively remove or rebind WASD keys (and our own custom ones) so you
        // don't leak out any inputs while chatting.

        // As ArcadeInputComponent events fire these so-called "virtual keys", we can use 600000+
        // keycode range for our custom inputs quite safely.
        // This also should allow the patch to be expanded into a full on backport of newer client
        // "Edit Hotkeys" menu.

        // Right now, the patch adds custom keybinds for switching between inventory quickbar slots.
        // We achieve that by sending ToolSelectComponent a fake "touch" event when we get a key
        // press on our custom keycode range.
        // ToolSelectComponents reside in GameMenu -> ItemsParent -> ToolSelectMenu -> ToolX.

        auto& game = game::GameHarness::get();
        // Resolve functions we need.
        real::GetArcadeComponent = utils::resolveRelativeCall<GetArcadeComponent_t>(
            game.findMemoryPattern<uint8_t*>(pattern::GetArcadeComponent));
        real::AddKeyBinding = game.findMemoryPattern<AddKeyBinding_t>(pattern::AddKeyBinding);
        real::ToolSelectComponentOnTouchStart =
            game.findMemoryPattern<ToolSelectComponentOnTouchStart_t>(
                pattern::ToolSelectComponentOnTouchStart);
        real::GameLogicComponentDialogIsOpen =
            game.findMemoryPattern<GameLogicComponentDialogIsOpen_t>(
                pattern::GameLogicComponentDialogIsOpen);

        // AddWASDKeys already is invoked by game. Add our custom binds here.
        AddCustomKeybinds();

        // Hook.
        game.hookFunctionPatternDirect<NetControllerLocalOnArcadeInput_t>(
            pattern::NetControllerLocalOnArcadeInput, NetControllerLocalOnArcadeInput,
            &real::NetControllerLocalOnArcadeInput);
        game.hookFunctionPatternDirect<AddWASDKeys_t>(pattern::AddWASDKeys, AddWASDKeys,
                                                      &real::AddWASDKeys);
    }

    static void __fastcall NetControllerLocalOnArcadeInput(void* this_, int keyCode, bool bKeyFired)
    {
        // Our custom mappings right now are just on keycode >= 600000
        // See AddCustomKeybinds function.
        // nit: InputProvider or the likes for patches to share arcadeinput callbacks?
        if (keyCode >= 600000)
        {
            // nit: GameLogicComponent and App struct/class
            if (real::GameLogicComponentDialogIsOpen(real::GetApp()->GetGameLogic()))
                return;

            // We only want to act if they key is pressed, not released.
            if (bKeyFired)
            {
                Entity* pGUI = real::GetEntityRoot()->GetEntityByName("GUI");
                // We don't want the key presses to happen when we can't even see our quickbar.
                if (pGUI->GetEntityByName("OptionsMenu"))
                    return;
                // GUI -> WorldSpecificGUI always exists. GameMenu only does when in a world.
                Entity* pGameMenu =
                    pGUI->GetEntityByName("WorldSpecificGUI")->GetEntityByName("GameMenu");
                if (pGameMenu != nullptr)
                {
                    // When GameMenu is constructed, so is the inventory.
                    // We fake a "touch" event on a quickbar Tool to do the item switch cleanly.
                    int ToolIndex = keyCode - 600000;
                    EntityComponent* pToolSelect =
                        pGameMenu->GetEntityByName("ItemsParent")
                            ->GetEntityByName("ToolSelectMenu")
                            ->GetEntityByName("Tool" + std::to_string(ToolIndex))
                            ->GetComponentByName("ToolSelect");
                    real::ToolSelectComponentOnTouchStart(pToolSelect);
                }
            }
            return;
        }
        // Pass on rest of the keys (e.g. WASD/Space)
        real::NetControllerLocalOnArcadeInput(this_, keyCode, bKeyFired);
    }

    static void __fastcall AddWASDKeys()
    {
        real::AddWASDKeys();
        AddCustomKeybinds();
    }

    static void AddCustomKeybinds()
    {
        // Map our custom keybinds for switching between quickbar slots.
        real::AddKeyBinding(real::GetArcadeComponent(), "chatkey_ToolSelect1", 49, 600001, 0, 0);
        real::AddKeyBinding(real::GetArcadeComponent(), "chatkey_ToolSelect2", 50, 600002, 0, 0);
        real::AddKeyBinding(real::GetArcadeComponent(), "chatkey_ToolSelect3", 51, 600003, 0, 0);
    }
};
REGISTER_USER_GAME_PATCH(HotkeyPatch, hotkey_patch);
