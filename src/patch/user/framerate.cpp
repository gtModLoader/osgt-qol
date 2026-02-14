#include "game/game.hpp"
#include "game/signatures.hpp"
#include "patch/patch.hpp"

// App::SetFPSLimit
REGISTER_GAME_FUNCTION(SetFPSLimit,
                       "4C 8B DC 48 81 EC D8 00 00 00 48 C7 44 24 20 FE FF FF FF 48 8B ? ? ? ? ? "
                       "48 33 C4 48 89 84 24 C0 00 00 00 0F 57 C0",
                       __fastcall, void, void*, float);

// PetRenderData::Update
REGISTER_GAME_FUNCTION(PetRenderDataUpdate,
                       "48 8B C4 48 89 58 10 48 89 68 18 56 57 41 56 48 81 EC A0 00 00 00 0F B7",
                       __fastcall, void, void*, void*, float);

class FramerateUnlockPatch : public patch::BasePatch
{
  public:
    void apply() const override
    {
        auto& game = game::GameHarness::get();
        // Hook SetFPSLimit.
        game.hookFunctionPatternDirect<SetFPSLimit_t>(pattern::SetFPSLimit, SetFPSLimit,
                                                      &real::SetFPSLimit);
        SetFPSLimit(nullptr, 60.0f);

        // Users may have a reason to want a bit more generous fps limit.
        // Some game mechanics can be somewhat unreliable on the 59-60 fps mark.
        // This also allows 75-90Hz display users to get more tangible benefit out of this patch as
        // Proton slices some of the fps off anyway.
        DEVMODE dm;
        dm.dmSize = sizeof(DEVMODE);
        if (EnumDisplaySettingsA(NULL, ENUM_CURRENT_SETTINGS, &dm))
        {
            if (dm.dmDisplayFrequency < 90)
            {
                auto& optionsMgr = game::OptionsManager::get();
                optionsMgr.addCheckboxOption(
                    "qol", "Performance", "osgt_qol_fps_min90", "Set FPS Limit minimum value to 90 (may get V-Synced)\n",
                    &OnFPSMinimumCallback);
            }
        }

        auto& optionsMgr = game::OptionsManager::get();
                optionsMgr.addCheckboxOption(
                    "qol", "Performance", "osgt_qol_fps_unlock", "Raise FPS to match display refresh rate (may affect game physics, parkour)\n",
                    &OnFPSUnlockCallback);

        // Fix crazy pet movement.
        game.hookFunctionPatternDirect<PetRenderDataUpdate_t>(
            pattern::PetRenderDataUpdate, PetRenderDataUpdate, &real::PetRenderDataUpdate);
    }

    static void OnFPSUnlockCallback(VariantList* pVariant)
    {
        Entity* pCheckbox = pVariant->Get(1).GetEntity();
        bool bChecked = pCheckbox->GetVar("checked")->GetUINT32() != 0;
        real::GetApp()->GetVar("osgt_qol_fps_unlock")->Set(uint32_t(bChecked));
        SetFPSLimit(nullptr, 60.0f);
    }

    static void OnFPSMinimumCallback(VariantList* pVariant)
    {
        Entity* pCheckbox = pVariant->Get(1).GetEntity();
        bool bChecked = pCheckbox->GetVar("checked")->GetUINT32() != 0;
        real::GetApp()->GetVar("osgt_qol_fps_min90")->Set(uint32_t(bChecked));
        SetFPSLimit(nullptr, 60.0f);
    }

    static void __fastcall SetFPSLimit(void* app, float fps)
    {
        // Default to 60 FPS. Only raise to display refresh rate if user opts in.
        DEVMODE dm;
        dm.dmSize = sizeof(DEVMODE);
        if (EnumDisplaySettingsA(NULL, ENUM_CURRENT_SETTINGS, &dm) && real::GetApp()->GetVar("osgt_qol_fps_unlock")->GetUINT32())
        {
            float fpsLimit = static_cast<float>(dm.dmDisplayFrequency);
            // Do not allow over >400fps. The client has a bug where if the game is open for long
            // enough time it won't respect the fps limit properly anymore, causing it to shoot up.
            // We do not want that scenario to allow exceeding 500 fps as that's where physics start
            // to slowly break.
            if (fpsLimit > 400)
                fpsLimit = 400;
            // We also do not want someone unfortunate enough to have a 59Hz default to actively
            // lose frames.
            if (fpsLimit < 60)
                fpsLimit = 60;

            // We present users under 90Hz a checkbox in options whether they want to unlock game
            // side minimum framerate to 90 in order to remove jankiness introduced by 59 fps
            // vanilla. They will most likely get bound by GPU driver V-Sync though. On VirtualBox
            // with a 60Hz display, it did lock the fps to 63 as opposed to 59.
            // On a no V-Sync scenario, it should be ~84 fps initially.
            if (fpsLimit < 90 && real::GetApp()->GetVar("osgt_qol_fps_min90")->GetUINT32())
                fpsLimit = 90;

            real::SetFPSLimit(app, fpsLimit);
        }
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
REGISTER_USER_GAME_PATCH(FramerateUnlockPatch, framerate_unlock);
