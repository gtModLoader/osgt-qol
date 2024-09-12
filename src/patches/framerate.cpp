#include "game.hpp"

// App::SetFPSLimit
REGISTER_GAME_FUNCTION(SetFPSLimit,
                       "4C 8B DC 48 81 EC D8 00 00 00 48 C7 44 24 20 FE FF FF FF 48 8B ? ? ? ? ? "
                       "48 33 C4 48 89 84 24 C0 00 00 00 0F 57 C0",
                       __fastcall, void, void*, float);

// PetRenderData::Update
REGISTER_GAME_FUNCTION(PetRenderDataUpdate,
                       "48 8B C4 48 89 58 10 48 89 68 18 56 57 41 56 48 81 EC A0 00 00 00 0F B7",
                       __fastcall, void, void*, void*, float);

class FramerateUnlockPatch : public game::BasePatch
{
  public:
    void apply() const override
    {
        auto& game = game::GameHarness::get();
        // Hook SetFPSLimit.
        game.hookFunctionPatternDirect<SetFPSLimit_t>(pattern::SetFPSLimit, SetFPSLimit,
                                                      &real::SetFPSLimit);
        SetFPSLimit(nullptr, 60.0f);
        // Fix crazy pet movement.
        game.hookFunctionPatternDirect<PetRenderDataUpdate_t>(
            pattern::PetRenderDataUpdate, PetRenderDataUpdate, &real::PetRenderDataUpdate);
    }

    static void __fastcall SetFPSLimit(void* app, float fps)
    {
        // Set the FPS limit to the primary display refresh rate if possible. Otherwise, default
        // to 60.
        DEVMODE dm;
        dm.dmSize = sizeof(DEVMODE);
        if (EnumDisplaySettingsA(NULL, ENUM_CURRENT_SETTINGS, &dm))
            real::SetFPSLimit(app, static_cast<float>(dm.dmDisplayFrequency));
        else
            real::SetFPSLimit(app, 60.f);
    }

    static void __fastcall PetRenderDataUpdate(void* this_, void* unk2, float delta)
    {
        // The game relies on using this for pet movement correction during fps fluctuations.
        // However, this logic only works properly until 60 FPS. So for our high-fps mod, we patch
        // this to force game to always run an update on pets movement, causing them to move
        // smoothly and as expected again. There are some slight drawbacks to this approach, but
        // it's relatively stable and good enough for vast majority of players.
        *(float*)((uint8_t*)(this_) + 92) = 0.016666668f;
        real::PetRenderDataUpdate(this_, unk2, delta);
    }
};
REGISTER_GAME_PATCH(FramerateUnlockPatch, framerate_unlock);
