#include "game.hpp"
#include "game/struct/gamepadmanager.hpp"
#include "signatures.hpp"

REGISTER_GAME_FUNCTION(NetControllerLocalOnArcadeInput,
                       "48 89 5C 24 08 48 89 74 24 10 57 48 83 EC 20 41 0F B6 D8 8B F2 48 8B F9 ? "
                       "? ? ? ? 48 8B C8 ? ? ? ? ? 48",
                       __fastcall, void, void* this_, int keyCode, bool bKeyFired);

REGISTER_GAME_FUNCTION(
    OnArcadeInput,
    "48 8B C4 55 57 41 56 48 8D 68 C8 48 81 EC 20 01 00 00 48 C7 44 24 28 FE FF FF FF", __fastcall,
    void, VariantList*);
REGISTER_GAME_FUNCTION(AddWASDKeys,
                       "40 55 48 8B EC 48 83 EC 50 F3 0F 10 05 1F 72 22 00 F3 0F 10 0D BF 7C",
                       __fastcall, void);

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

REGISTER_GAME_FUNCTION(GetGamepadManager,
                       "E8 ? ? ? ? 48 8B 48 28 48 2B 48 20 48 C1 F9 03 48 85 C9 74 3E", __fastcall,
                       GamepadManager*);
namespace game
{

InputEvents& InputEvents::get()
{
    static InputEvents instance;
    return instance;
}

void game::InputEvents::initialize()
{
    auto& game = game::GameHarness::get();

    real::GetArcadeComponent = utils::resolveRelativeCall<GetArcadeComponent_t>(
        game.findMemoryPattern<uint8_t*>(pattern::GetArcadeComponent));
    real::AddKeyBinding = game.findMemoryPattern<AddKeyBinding_t>(pattern::AddKeyBinding);

    real::GetGamepadManager = utils::resolveRelativeCall<GetGamepadManager_t>(
        game.findMemoryPattern<uint8_t*>(pattern::GetGamepadManager));

    // Hook.
    game.hookFunctionPatternDirect<NetControllerLocalOnArcadeInput_t>(
        pattern::NetControllerLocalOnArcadeInput, NetControllerLocalOnArcadeInput,
        &real::NetControllerLocalOnArcadeInput);
    game.hookFunctionPatternDirect<OnArcadeInput_t>(pattern::OnArcadeInput, OnArcadeInput,
                                                    &real::OnArcadeInput);
    game.hookFunctionPatternDirect<AddWASDKeys_t>(pattern::AddWASDKeys, AddWASDKeys,
                                                  &real::AddWASDKeys);
}

void game::InputEvents::OnArcadeInput(VariantList* pVL)
{
    real::OnArcadeInput(pVL);
    auto& inputEvents = game::InputEvents::get();
    (inputEvents.m_sig_onArcadeInput)(pVL);
}
void game::InputEvents::NetControllerLocalOnArcadeInput(void* this_, int keyCode, bool bKeyFired)
{
    real::NetControllerLocalOnArcadeInput(this_, keyCode, bKeyFired);
    auto& inputEvents = game::InputEvents::get();
    (inputEvents.m_sig_netControllerInput)(this_, keyCode, bKeyFired);
}
void game::InputEvents::AddWASDKeys()
{
    real::AddWASDKeys();
    auto& inputEvents = game::InputEvents::get();
    (inputEvents.m_sig_addWasdKeys)();
}

}; // namespace game