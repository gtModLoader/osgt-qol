#include "game.hpp"
#include "game/struct/gamepadmanager.hpp"

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

REGISTER_GAME_FUNCTION(
    ItemInfoManagerLoadFromMem,
    "48 89 5C 24 10 48 89 6C 24 18 48 89 74 24 20 57 41 56 41 57 48 83 EC 40 48 8B 71", __thiscall,
    void, void*, char*, bool);

REGISTER_GAME_FUNCTION(OnMapLoaded,
                       "40 53 48 83 EC 20 F3 0F 10 ? ? ? ? ? 48 8B D9 C6 81 D4 00 00 00 00",
                       __fastcall, void, void*, __int64, __int64, __int64);
namespace game
{

EventsAPI& EventsAPI::get()
{
    static EventsAPI instance;
    return instance;
}

void game::EventsAPI::initialize()
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
    game.hookFunctionPatternDirect<ItemInfoManagerLoadFromMem_t>(
        pattern::ItemInfoManagerLoadFromMem, ItemInfoManagerLoadFromMem,
        &real::ItemInfoManagerLoadFromMem);
    game.hookFunctionPatternDirect<OnMapLoaded_t>(pattern::OnMapLoaded, OnMapLoaded,
                                                  &real::OnMapLoaded);

    m_lastKeycode = 600000;
}

int game::EventsAPI::acquireKeycode()
{
    return m_lastKeycode++;
}

void game::EventsAPI::ItemInfoManagerLoadFromMem(void* this_, char* pBytes, bool arg3)
{
    real::ItemInfoManagerLoadFromMem(this_, pBytes, arg3);
    auto& EventsAPI = game::EventsAPI::get();
    (EventsAPI.m_sig_loadFromMem)();
}
void game::EventsAPI::OnArcadeInput(VariantList* pVL)
{
    real::OnArcadeInput(pVL);
    auto& EventsAPI = game::EventsAPI::get();
    (EventsAPI.m_sig_onArcadeInput)(pVL);
}
void game::EventsAPI::NetControllerLocalOnArcadeInput(void* this_, int keyCode, bool bKeyFired)
{
    real::NetControllerLocalOnArcadeInput(this_, keyCode, bKeyFired);
    auto& EventsAPI = game::EventsAPI::get();
    (EventsAPI.m_sig_netControllerInput)(this_, keyCode, bKeyFired);
}
void game::EventsAPI::AddWASDKeys()
{
    real::AddWASDKeys();
    auto& EventsAPI = game::EventsAPI::get();
    (EventsAPI.m_sig_addWasdKeys)();
}
void game::EventsAPI::OnMapLoaded(void* this_, __int64 p1, __int64 p2, __int64 p3)
{
    real::OnMapLoaded(this_, p1, p2, p3);
    auto& EventsAPI = game::EventsAPI::get();
    (EventsAPI.m_sig_onMapLoaded)(this_, p1, p2, p3);
}

}; // namespace game