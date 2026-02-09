#include "game/game.hpp"
#include "game/signatures.hpp"
#include "game/struct/app.hpp"
#include "game/struct/component.hpp"
#include "game/struct/components/gamelogic.hpp"
#include "game/struct/entity.hpp"
#include "patch/patch.hpp"
#include "utils/utils.hpp"

// ToolSelectComponent::OnTouchStart
// Params: this
REGISTER_GAME_FUNCTION(ToolSelectComponentOnTouchStart,
                       "48 89 5C 24 10 48 89 6C 24 20 56 48 81 EC A0 00 00 00 F3 0F 2C 05 FE 6D 35",
                       __fastcall, void, void*);

REGISTER_GAME_FUNCTION(AddSpacebarBinding,
                       "48 83 EC 58 48 83 3D ? ? ? ? ? F3 0F 10 ? ? ? ? ? F3 0F 58 ? ? ? ? ? F3 0F "
                       "11 ? ? ? ? ? 74 7B E8 ? ? ? ? E8",
                       __fastcall, void);
REGISTER_GAME_FUNCTION(GenericDialogMenuOnSelect,
                       "48 8b c4 55 41 54 41 55 41 56 41 57 48 8d a8 68 fe ff ff 48 81 ec 70 02 00 "
                       "00 48 c7 45 90 fe ff ff ff",
                       __fastcall, void, VariantList*);

REGISTER_GAME_FUNCTION(OpenDropOptions,
                       "40 53 48 81 ec b0 00 00 00 48 c7 44 24 30 fe ff ff ff 48 8b 05 c7 29 2f 00 "
                       "48 33 c4 48 89 84 24 a0 00 00 00",
                       __fastcall, void);

class QuickbarHotkeys : public patch::BasePatch
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
        real::ToolSelectComponentOnTouchStart =
            game.findMemoryPattern<ToolSelectComponentOnTouchStart_t>(
                pattern::ToolSelectComponentOnTouchStart);

        auto& events = game::EventsAPI::get();
        events.m_sig_netControllerInput.connect(&NetControllerLocalOnArcadeInput);
        events.m_sig_addWasdKeys.connect(&AddCustomKeybinds);
        m_toolSelect0 = events.acquireKeycode();
        m_toolSelect1 = events.acquireKeycode();
        m_toolSelect2 = events.acquireKeycode();
        m_toolSelect3 = events.acquireKeycode();
    }

    static void __fastcall NetControllerLocalOnArcadeInput(void* this_, int keyCode, bool bKeyFired)
    {
        // Our custom mappings right now are just on keycode >= 600000
        // See AddCustomKeybinds function.
        if (keyCode >= m_toolSelect0 && keyCode <= m_toolSelect3)
        {
            if (real::GetApp()->GetGameLogic()->IsDialogOpened())
                return;

            // We only want to act if they key is pressed, not released.
            if (bKeyFired)
            {
                Entity* pGUI = real::GetApp()->m_entityRoot->GetEntityByName("GUI");
                // We don't want the key presses to happen when we can't even see our quickbar.
                if (pGUI->GetEntityByName("OptionsMenu") ||
                    pGUI->GetEntityByName("ResolutionMenu") || pGUI->GetEntityByName("OptionsPage"))
                    return;
                // GUI -> WorldSpecificGUI always exists. GameMenu only does when in a world.
                Entity* pGameMenu =
                    pGUI->GetEntityByName("WorldSpecificGUI")->GetEntityByName("GameMenu");
                if (pGameMenu != nullptr)
                {
                    // When GameMenu is constructed, so is the inventory.
                    // We fake a "touch" event on a quickbar Tool to do the item switch cleanly.
                    int ToolIndex = keyCode - m_toolSelect0;
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
    }

    static void AddCustomKeybinds()
    {
        // Map our custom keybinds for switching between quickbar slots.
        real::AddKeyBinding(real::GetArcadeComponent(), "chatkey_ToolSelect1", 49, m_toolSelect1, 0,
                            0);
        real::AddKeyBinding(real::GetArcadeComponent(), "chatkey_ToolSelect2", 50, m_toolSelect2, 0,
                            0);
        real::AddKeyBinding(real::GetArcadeComponent(), "chatkey_ToolSelect3", 51, m_toolSelect3, 0,
                            0);
        // Also the numpad keys with 0 resetting to Fist/Wrench.
        real::AddKeyBinding(real::GetArcadeComponent(), "chatkey_NmpToolSelect0", 96, m_toolSelect0,
                            0, 0);
        real::AddKeyBinding(real::GetArcadeComponent(), "chatkey_NmpToolSelect1", 97, m_toolSelect1,
                            0, 0);
        real::AddKeyBinding(real::GetArcadeComponent(), "chatkey_NmpToolSelect2", 98, m_toolSelect2,
                            0, 0);
        real::AddKeyBinding(real::GetArcadeComponent(), "chatkey_NmpToolSelect3", 99, m_toolSelect3,
                            0, 0);
    }

  private:
    static int m_toolSelect0;
    static int m_toolSelect1;
    static int m_toolSelect2;
    static int m_toolSelect3;
};
int QuickbarHotkeys::m_toolSelect0;
int QuickbarHotkeys::m_toolSelect1;
int QuickbarHotkeys::m_toolSelect2;
int QuickbarHotkeys::m_toolSelect3;
REGISTER_USER_GAME_PATCH(QuickbarHotkeys, quickbar_hotkey_patch);

class QuickToggleSpaceToPunch : public patch::BasePatch
{
  public:
    void apply() const override
    {
        auto& game = game::GameHarness::get();

        // Resolve functions we need.
        real::AddSpacebarBinding =
            game.findMemoryPattern<AddSpacebarBinding_t>(pattern::AddSpacebarBinding);

        auto& events = game::EventsAPI::get();
        events.m_sig_netControllerInput.connect(&NetControllerLocalOnArcadeInput);
        events.m_sig_addWasdKeys.connect(&AddCustomKeybinds);
        m_stpKeycode = events.acquireKeycode();
    }

    static void __fastcall NetControllerLocalOnArcadeInput(void* this_, int keyCode, bool bKeyFired)
    {
        if (keyCode == m_stpKeycode)
        {
            if (real::GetApp()->GetGameLogic()->IsDialogOpened())
                return;

            // We only want to act if they key is pressed, not released.
            if (bKeyFired)
            {
                Entity* pGUI = real::GetApp()->m_entityRoot->GetEntityByName("GUI");
                // We don't want the key presses to happen when we're still in settings.
                if (pGUI->GetEntityByName("OptionsMenu") ||
                    pGUI->GetEntityByName("ResolutionMenu") || pGUI->GetEntityByName("OptionsPage"))
                    return;
            }

            Variant* pVariant = real::GetApp()->GetVar("useSpacebarForPunch");
            pVariant->Set(pVariant->GetUINT32() == 1 ? 0U : 1U);
            real::AddSpacebarBinding();
        }
    }

    static void AddCustomKeybinds()
    {
        // CTRL+P
        real::AddKeyBinding(real::GetArcadeComponent(), "chatkey_togglestp", 80, m_stpKeycode, 1,
                            1);
    }

  private:
    static int m_stpKeycode;
};
int QuickToggleSpaceToPunch::m_stpKeycode;
REGISTER_USER_GAME_PATCH(QuickToggleSpaceToPunch, quick_toggle_space_to_punch);

class FixURLButtons : public patch::BasePatch
{
  public:
    void apply() const override
    {
        // Yeah... this is kinda broken in vanilla client. If your GDPR status isn't 0 or -1
        // (uninitialised), you can't open any URL buttons in dialogs.
        auto& game = game::GameHarness::get();
        game.hookFunctionPatternDirect<GenericDialogMenuOnSelect_t>(
            pattern::GenericDialogMenuOnSelect, GenericDialogMenuOnSelect,
            &real::GenericDialogMenuOnSelect);
    }

    static void __fastcall GenericDialogMenuOnSelect(VariantList* pVL)
    {
        // If "gdprState" is >= 1, the game won't even entertain you a popup.
        int gdprState = real::GetApp()->m_playerGDPRState;
        real::GetApp()->m_playerGDPRState = 0;
        real::GenericDialogMenuOnSelect(pVL);
        real::GetApp()->m_playerGDPRState = gdprState;
    }
};
REGISTER_USER_GAME_PATCH(FixURLButtons, fix_url_buttons);

class ToggleCtrlJump : public patch::BasePatch
{
  public:
    void apply() const override
    {
        auto& game = game::GameHarness::get();

        // Nop out the original key binding, we'll make our own.
        auto ctrlBindAddr = game.findMemoryPattern<uint8_t*>(
            "E8 ? ? ? ? 48 C7 45 07 0F 00 00 00 48 89 5D FF C6 45 EF 00 41 B8 07 00 00 00 48 8D ? "
            "? ? ? ? 48 8D 4D EF E8 ? ? ? ? 89 5C 24 28");
        utils::nopMemory(ctrlBindAddr, 5);

        // Default this to off, some may prefer Ctrl for accessiblity reasons.
        Variant* pVariant = real::GetApp()->GetVar("osgt_qol_toggle_ctrl_jump");
        if (pVariant->GetType() != Variant::TYPE_UINT32)
            pVariant->Set(0U);

        auto& optionsMgr = game::OptionsManager::get();
        optionsMgr.addCheckboxOption("qol", "Input", "osgt_qol_toggle_ctrl_jump",
                                     "Disable CTRL key to Jump", &HideUIScrollHandlesCallback);

        auto& events = game::EventsAPI::get();
        events.m_sig_addWasdKeys.connect(&AddCustomKeybinds);
    }

    static void HideUIScrollHandlesCallback(VariantList* pVariant)
    {
        Entity* pCheckbox = pVariant->Get(1).GetEntity();
        bool bChecked = pCheckbox->GetVar("checked")->GetUINT32() != 0;
        real::GetApp()->GetVar("osgt_qol_toggle_ctrl_jump")->Set(uint32_t(bChecked));
        AddCustomKeybinds();
    }

    static void AddCustomKeybinds()
    {
        // Toggle CTRL to Jump depending on var state.
        if (real::GetApp()->GetVar("osgt_qol_toggle_ctrl_jump")->GetUINT32() == 0)
            real::AddKeyBinding(real::GetArcadeComponent(), "tcj_Jump", 500013, 500056, 0, 0);
        else
        {
            // Remove the binding if it exists.
            VariantList keyToRemove;
            keyToRemove.m_variant[0].Set("tcj_Jump");
            real::GetArcadeComponent()
                ->GetShared()
                ->GetFunction("RemoveKeyBindingsStartingWith")
                ->sig_function(&keyToRemove);
        }
    }
};
REGISTER_USER_GAME_PATCH(ToggleCtrlJump, toggle_ctrl_jump);

class QuickDropPatch : public patch::BasePatch
{
  public:
    void apply() const override
    {
        auto& events = game::EventsAPI::get();
        events.m_sig_netControllerInput.connect(&NetControllerLocalOnArcadeInput);
        events.m_sig_addWasdKeys.connect(&AddCustomKeybinds);
        m_keycode = events.acquireKeycode();

        real::OpenDropOptions =
            game::GameHarness::get().findMemoryPattern<OpenDropOptions_t>(pattern::OpenDropOptions);

        auto& optionsMgr = game::OptionsManager::get();
        optionsMgr.addCheckboxOption("qol", "Input", "osgt_qol_quick_drop",
                                     "Enable Q key to open drop current item dialog",
                                     &OnQuickDropToggledCallback);
        m_isEnabled = real::GetApp()->GetVar("osgt_qol_quick_drop")->GetUINT32() == 1;
    }

    static void __fastcall NetControllerLocalOnArcadeInput(void* this_, int keyCode, bool bKeyFired)
    {
        if (!m_isEnabled)
        {
            return;
        }

        if (keyCode == m_keycode)
        {
            if (real::GetApp()->GetGameLogic()->IsDialogOpened())
            {
                return;
            }

            if (bKeyFired)
            {
                Entity* pGUI = real::GetApp()->m_entityRoot->GetEntityByName("GUI");
                if (pGUI->GetEntityByName("OptionsMenu") ||
                    pGUI->GetEntityByName("ResolutionMenu") || pGUI->GetEntityByName("OptionsPage"))
                {
                    return;
                }
                real::OpenDropOptions();
            }
        }
    }

    static void AddCustomKeybinds()
    {
        real::AddKeyBinding(real::GetArcadeComponent(), "chatkey_QuickDrop", 81, m_keycode, 0, 0);
    }

    static void OnQuickDropToggledCallback(VariantList* pVariant)
    {
        Entity* pCheckbox = pVariant->Get(1).GetEntity();
        bool bChecked = pCheckbox->GetVar("checked")->GetUINT32() != 0;
        real::GetApp()->GetVar("osgt_qol_quick_drop")->Set(uint32_t(bChecked));
        m_isEnabled = bChecked;
    }

  private:
    static bool m_isEnabled;
    static int m_keycode;
};
bool QuickDropPatch::m_isEnabled = false;
int QuickDropPatch::m_keycode;
REGISTER_USER_GAME_PATCH(QuickDropPatch, quick_drop);
