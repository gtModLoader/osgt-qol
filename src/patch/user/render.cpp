#include "game/game.hpp"
#include "game/signatures.hpp"
#include "game/struct/graphics/background.hpp"
#include "game/struct/graphics/background_blood.hpp"
#include "game/struct/graphics/background_default.hpp"
#include "game/struct/graphics/surface.hpp"
#include "patch/patch.hpp"
#include "utils/utils.hpp"

#include "game/struct/component.hpp"
#include "game/struct/components/mapbg.hpp"
#include "game/struct/entity.hpp"
#include "game/struct/renderutils.hpp"
#include "game/struct/rtrect.hpp"
#include "game/struct/variant.hpp"
#include <vcruntime_new_debug.h>

// BackgroundNight::BackgroundNight
REGISTER_GAME_FUNCTION(
    BackgroundNight,
    "48 89 4C 24 08 55 56 57 41 54 41 55 41 56 41 57 48 8B EC 48 81 EC 80 00 00 00 48 C7 45 B0 FE "
    "FF FF FF 48 89 9C 24 C8 00 00 00",
    __fastcall, Background*, void*, int);

// BackgroundNight::Init
REGISTER_GAME_FUNCTION(
    BackgroundNightInit,
    "40 55 41 56 48 8B EC 48 83 EC 78 48 8B ? ? ? ? ? 48 33 C4 48 89 45 D8 4C 8B F1 84 D2 74 4A",
    __fastcall, void, Background*, bool);

// BackgroundDefault::~Background_Default
REGISTER_GAME_FUNCTION(BackgroundDefaultDtor,
                       "40 53 48 83 EC 20 48 8D 05 B3 C9 30 00 48 8B D9 48 89 01 48 8B 89 10 01",
                       __fastcall, void, Background*);

// DrawFilledBitmapRect
REGISTER_GAME_FUNCTION(DrawFilledBitmapRect,
                       "48 83 EC 48 66 0F 6E 01 66 0F 6E 49 04 0F B6 44 24 70", __fastcall, void,
                       rtRectf&, uint32_t, uint32_t, void*, bool);

class CustomizedTitleScreen : public patch::BasePatch
{
  public:
    void apply() const override
    {
        auto& game = game::GameHarness::get();
        // Resolve functions
        real::BackgroundNight = game.findMemoryPattern<BackgroundNight_t>(pattern::BackgroundNight);
        real::BackgroundNightInit =
            game.findMemoryPattern<BackgroundNightInit_t>(pattern::BackgroundNightInit);
        real::BackgroundDefaultDtor =
            game.findMemoryPattern<BackgroundDefaultDtor_t>(pattern::BackgroundDefaultDtor);

        // Hook
        // pattern::MainMenuCreate collision with drawing.cpp - so we define pattern manually here.
        game.hookFunctionPatternDirect<MainMenuCreate_t>(
            "48 8B C4 55 57 41 54 41 56 41 57 48 8D A8 E8 F8 FF FF 48 81 EC F0 07 00 00 48 C7 85 "
            "80 01",
            MainMenuCreate, &real::MainMenuCreate);
        patched::MainMenuCreate = MainMenuCreate;
    }

    static void __fastcall MainMenuCreate(Entity* pEnt, bool unk2)
    {
        // Let the game construct main menu for us.
        real::MainMenuCreate(pEnt, unk2);
        // After which we can change the weather without issue.
        ChangeMainMenuWeather(pEnt);

        // Lets also add modloader version label to the menuscreen to the opposite of version
        Entity* pVerLabel = pEnt->GetEntityByName("MainMenu")->GetEntityByName("version");
        CL_Vec2f m_verLabelPos = pVerLabel->GetVar("pos2d")->GetVector2();
        CL_Vec2f m_verLabelSize = pVerLabel->GetVar("size2d")->GetVector2();

        Entity* pTextLabel = real::CreateTextLabelEntity(pEnt->GetEntityByName("MainMenu"), "mltxt",
                                                         0, m_verLabelPos.y - m_verLabelSize.y,
                                                         "`wOSGT-QOL V1.0-ALPHA``");
        // Retrieve fontscale and scale created entity
        uint32_t fontID;
        float fontScale;
        real::GetFontAndScaleToFitThisLinesPerScreenY(fontID, fontScale, 20);
        real::SetupTextEntity(pTextLabel, fontID, fontScale);
        // Also needs fadeinentity
    }

    static void ChangeMainMenuWeather(Entity* pGUIEnt)
    {
        // Lets retrieve MapBGComponent from GUI -> MainMenu
        MapBGComponent* pMapBGComponent = reinterpret_cast<MapBGComponent*>(
            pGUIEnt->GetEntityByName("MainMenu")->GetComponentByName("MapBGComponent"));

        // Lets create our buffer for BackgroundNight, the struct size is 0x348
        void* buffer = operator new(0x348);
        // Pass it on to constructor, 2 is our "Active Weather ID". Game uses this to determine
        // if to create comet weather or just normal night weather.
        Background* pBackgroundNight = real::BackgroundNight(buffer, 2);
        real::BackgroundNightInit(pBackgroundNight, false);

        // Take a note of current weather.
        Background_Default* pOriginalWeather = (Background_Default*)pMapBGComponent->m_pBackground;
        // Assign our new one in place.
        pMapBGComponent->m_pBackground = pBackgroundNight;
        // Discard the original.
        real::BackgroundDefaultDtor(pOriginalWeather);
    }
};
REGISTER_USER_GAME_PATCH(CustomizedTitleScreen, customized_title_screen);

class BubbleOpacityBackport : public patch::BasePatch
{
  public:
    void apply() const override
    {
        auto& game = game::GameHarness::get();

        // Bubble opacity is a vanilla feature, albeit from future version, it should go in save.dat
        Variant* pVariant = real::GetApp()->GetVar("speech_bubble_opacity");
        if (pVariant->GetType() != Variant::TYPE_FLOAT)
            pVariant->Set(1.00f);

        // Add the slider back into options. Normally it's between the music/sfx/gfx sliders, but
        // it'd be too messy to ram it between them. OptionsManager will move it to dedicated
        // "OSGT-QOL Options" area.
        auto& optionsMgr = game::OptionsManager::get();
        optionsMgr.addSliderOption("speech_bubble_opacity", "Bubble Opacity",
                                   &BubbleOpacitySliderCallback);

        // Hook
        game.hookFunctionPatternDirect<DrawFilledBitmapRect_t>(
            pattern::DrawFilledBitmapRect, DrawFilledBitmapRect, &real::DrawFilledBitmapRect);
    }

    static void BubbleOpacitySliderCallback(Variant* pVariant)
    {
        real::GetApp()->GetVar("speech_bubble_opacity")->Set(pVariant->GetFloat());
    }

    static void __fastcall DrawFilledBitmapRect(rtRectf& r, uint32_t middleColor,
                                                uint32_t borderColor, void* pSurf,
                                                bool bFillMiddleCloserToEdges)
    {
        float opacity = real::GetApp()->GetVar("speech_bubble_opacity")->GetFloat();
        middleColor = ModAlpha(middleColor, (((float)GET_ALPHA(middleColor)) / 255.0f) * opacity);
        borderColor = ModAlpha(borderColor, (((float)GET_ALPHA(borderColor)) / 255.0f) * opacity);
        real::DrawFilledBitmapRect(r, middleColor, borderColor, pSurf, bFillMiddleCloserToEdges);
    }
};
REGISTER_USER_GAME_PATCH(BubbleOpacityBackport, bubble_opacity_backport);

class RemoveCheckboxPadding : public patch::BasePatch
{
  public:
    void apply() const override
    {
        // Reverts the checkbox spacings back to how they were since 2013.

        auto& game = game::GameHarness::get();
        auto addr = game.findMemoryPattern<uint8_t*>(
            "E8 ? ? ? ? C7 00 03 00 00 00 F3 44 0F 11 40 10 F3 0F 11");
        // They multiplied some values related to size2d at end of function here in V2.997+. By
        // patching out the 2 MOVSS instructions in CreateCheckbox, we essentially revert that
        // change.
        utils::nopMemory(addr + 11, 11);
    }
};
REGISTER_USER_GAME_PATCH(RemoveCheckboxPadding, old_checkboxes);

class BloodMoonDemoWeather : public patch::BasePatch
{
  public:
    void apply() const override
    {
        // This is a demo mod showcasing how to register a custom weather.
        // See src/game/struct/graphics/backgound_blood.cpp for the weather implementation.
        auto& weatherMgr = game::WeatherManager::get();
        weatherMgr.registerWeather("blood_moon", &onWeatherCreate);
    }

    static Background* onWeatherCreate() { return new Background_Blood(); }
};
REGISTER_USER_GAME_PATCH(BloodMoonDemoWeather, blood_moon_demo_weather);
