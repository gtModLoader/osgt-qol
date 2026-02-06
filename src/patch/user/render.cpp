#include "game/game.hpp"
#include "game/signatures.hpp"
#include "game/struct/graphics/background.hpp"
#include "game/struct/graphics/background_blank.hpp"
#include "game/struct/graphics/background_blood.hpp"
#include "game/struct/graphics/background_default.hpp"
#include "patch/patch.hpp"
#include "utils/utils.hpp"

#include "game/struct/component.hpp"
#include "game/struct/components/gamelogic.hpp"
#include "game/struct/components/mapbg.hpp"
#include "game/struct/entity.hpp"
#include "game/struct/entityutils.hpp"
#include "game/struct/renderutils.hpp"
#include "game/struct/rtrect.hpp"
#include "game/struct/variant.hpp"

#include "game/struct/world/world.hpp"
#include "game/struct/world/worldcamera.hpp"
#include "game/struct/world/worldrenderer.hpp"
#include <version.h>

// MainMenuCreate
REGISTER_GAME_FUNCTION(
    MainMenuCreate,
    "48 8B C4 55 57 41 54 41 56 41 57 48 8D A8 E8 F8 FF FF 48 81 EC F0 07 00 00 48 C7 85 80 01",
    __fastcall, void, Entity*, bool);

// Background_Sunset::Background_Sunset
REGISTER_GAME_FUNCTION(BackgroundSunset,
                       "48 89 4C 24 08 55 56 57 41 56 41 57 48 8B EC 48 81 EC 80 00 00 00 48 C7 45 "
                       "B0 FE FF FF FF 48 89 9C 24 C8 00 00 00",
                       __fastcall, Background*, void*);

// Background_Night::Background_Night
REGISTER_GAME_FUNCTION(
    BackgroundNight,
    "48 89 4C 24 08 55 56 57 41 54 41 55 41 56 41 57 48 8B EC 48 81 EC 80 00 00 00 48 C7 45 B0 FE "
    "FF FF FF 48 89 9C 24 C8 00 00 00",
    __fastcall, Background*, void*, int);

// Background_Desert::Background_Desert
REGISTER_GAME_FUNCTION(BackgroundDesert,
                       "48 89 4C 24 08 55 56 57 41 56 41 57 48 8B EC 48 81 EC 80 00 00 00 48 C7 45 "
                       "B0 FE FF FF FF 48 89 9C 24 C0 00 00 00 48 8B F1 E8 ? ? ? ? 90",
                       __fastcall, Background*, void*);

// Background_Harvest::Background_Harvest
REGISTER_GAME_FUNCTION(
    BackgroundHarvest,
    "48 8B C4 48 89 48 08 55 57 41 54 41 56 41 57 48 8B EC 48 83 EC 70 48 C7 45 B0 FE FF FF FF",
    __fastcall, Background*, void*);

// Background_Mars::Background_Mars
REGISTER_GAME_FUNCTION(BackgroundMars,
                       "48 89 4C 24 08 55 56 57 41 56 41 57 48 8B EC 48 81 EC 80 00 00 00 48 C7 45 "
                       "B0 FE FF FF FF 48 89 9C 24 C0 00 00 00 4C 8B F1 E8",
                       __fastcall, Background*, void*);

// Background_Undersea::Background_Undersea
REGISTER_GAME_FUNCTION(BackgroundUndersea,
                       "48 89 4C 24 08 55 56 57 48 83 EC 70 48 C7 44 24 20 FE FF FF FF", __fastcall,
                       Background*, void*);

// Background_Warp::Background_Warp
REGISTER_GAME_FUNCTION(BackgroundWarp,
                       "48 8B C4 48 89 48 08 56 57 41 56 48 83 EC 70 48 C7 40 98 FE FF FF FF 48 89 "
                       "58 10 48 89 68 20 8B DA 4C 8B F1 E8",
                       __fastcall, Background*, void*, int);

// Background_Wolf::Background_Wolf
REGISTER_GAME_FUNCTION(BackgroundWolf,
                       "48 8B C4 48 89 48 08 55 56 57 41 54 41 55 41 56 41 57 48 8D 68 A1 48 81 EC "
                       "B0 00 00 00 48 C7 45 A7 FE FF FF FF",
                       __fastcall, Background*, void*);

// Background_Pagoda::Background_Pagoda
REGISTER_GAME_FUNCTION(BackgroundPagoda,
                       "48 8B C4 48 89 48 08 55 57 41 54 41 56 41 57 48 8B EC 48 81 EC 80 00 00 00 "
                       "48 C7 45 B0 FE FF FF FF",
                       __fastcall, Background*, void*);

// Background_Monochrome::Background_Monochrome
REGISTER_GAME_FUNCTION(
    BackgroundMonochrome,
    "48 89 4C 24 08 55 56 57 48 8B EC 48 81 EC 80 00 00 00 48 C7 45 B0 FE FF FF FF 48 89 9C 24 B8 "
    "00 00 00 48 8B F9 E8 ? ? ? ? 90 48 8D ? ? ? ? ? 48 89 07 48 8D 8F 10 01 00 00 E8 ? ? ? ?",
    __fastcall, Background*, void*);

// Background_Treasure::Background_Treasure
REGISTER_GAME_FUNCTION(BackgroundTreasure,
                       "48 89 4C 24 08 55 53 56 57 41 54 41 56 41 57 48 8B EC 48 81 EC 80 00 00 00 "
                       "48 C7 45 B0 FE FF FF FF",
                       __fastcall, Background*, void*);

// Background_Surgery::Background_Surgery
REGISTER_GAME_FUNCTION(BackgroundSurgery,
                       "48 8B C4 48 89 48 08 55 56 57 41 54 41 55 41 56 41 57 48 8D 68 A1 48 81 EC "
                       "90 00 00 00 48 C7 45 C7 FE FF FF FF",
                       __fastcall, Background*, void*);

// Background_Bountiful::Background_Bountiful
REGISTER_GAME_FUNCTION(
    BackgroundBountiful,
    "48 89 4C 24 08 55 56 57 48 8B EC 48 81 EC 80 00 00 00 48 C7 45 B0 FE FF FF FF 48 89 9C 24 B8 "
    "00 00 00 48 8B F9 E8 ? ? ? ? 90 48 8D ? ? ? ? ? 48 89 07 48 8D 8F 18 01 00 00",
    __fastcall, Background*, void*);

// DrawFilledBitmapRect
REGISTER_GAME_FUNCTION(DrawFilledBitmapRect,
                       "48 83 EC 48 66 0F 6E 01 66 0F 6E 49 04 0F B6 44 24 70", __fastcall, void,
                       rtRectf&, uint32_t, uint32_t, void*, bool);

REGISTER_GAME_FUNCTION(AvatarDataGetSetAsUint16,
                       "0F 10 41 0C 0F 11 02 0F B7 41 1C 66 89 42 10 F3 0F 10 ? ? ? ? ? F3 0F 58 ? "
                       "? ? ? ? F3 0F 11 ? ? ? ? ? C3",
                       __fastcall, void, void*, uint16_t*);

REGISTER_GAME_FUNCTION(WorldRendererTileLineOfSight,
                       "48 8B C4 48 89 78 10 55 48 8D 68 A9 48 81 EC E0 00 00 00 0F 29 70 E8",
                       __fastcall, bool, void*, Tile*, Tile*, float, float*);
REGISTER_GAME_FUNCTION(WorldRendererDrawTiles, "48 8B C4 55 57 41 54 41 55 41 57", __fastcall, void,
                       void*, std::vector<Tile*>*, int);

REGISTER_GAME_FUNCTION(SetGrabBarSize, "48 89 5C 24 08 57 48 83 EC 20 48 8B F9 BB A3 00 00 00 E8",
                       __fastcall, void, Entity*);

REGISTER_GAME_FUNCTION(CreateOptionsMenu,
                       "48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 18 57 48 83 EC 20 83 39 00 48 8B "
                       "F9 75 1E C7 01 05 00 00 00 C7 41 10 00 00 00 00",
                       __fastcall, void, VariantList*);
REGISTER_GAME_FUNCTION(InitLog, "40 55 48 8B EC 48 81 EC 80 00 00 00 48 C7 45 D0 FE FF FF FF",
                       __fastcall, void);
REGISTER_GAME_FUNCTION(KillGameMenu,
                       "40 55 48 8B EC 48 83 EC 50 48 C7 45 D0 FE FF FF FF 48 89 5C 24 60 48 8B ? "
                       "? ? ? ? 48 33 C4 48 89 45 F8 0F B6 D9 E8",
                       __fastcall, void, bool);
REGISTER_GAME_FUNCTION(
    GameMenuCreate,
    "48 8B C4 55 48 8D 68 A1 48 81 EC 00 01 00 00 48 C7 45 87 FE FF FF FF 48 89 58 10 48 89 70 18",
    __fastcall, void, Entity*);
REGISTER_GAME_FUNCTION(OnPressingBackDuringGameplay,
                       "48 8B C4 55 41 56 41 57 48 8D 68 C8 48 81 EC 20 01 00 00 48 C7 44 24 28 FE "
                       "FF FF FF 48 89 58 08 48 89 70 10 48 89 78 18",
                       __fastcall, void);
REGISTER_GAME_FUNCTION(WorldCameraOnUpdate,
                       "40 53 48 83 EC 60 41 8B 00 48 8B D9 89 41 20 41 8B 40 04 89 41 24 F3 0F 2C",
                       __fastcall, void, WorldCamera*, CL_Vec2f*, CL_Vec2f*);
REGISTER_GAME_FUNCTION(WorldCameraGetCamWorldPos, "8B 41 10 89 02 8B 41 14 89 42 04 48 8B C2 C3",
                       __fastcall, CL_Vec2f*, WorldCamera*, CL_Vec2f*);
REGISTER_GAME_FUNCTION(
    WorldRendererOnRender,
    "48 8B C4 55 41 54 41 55 41 56 41 57 48 8D 68 88 48 81 EC 50 01 00 00 48 C7 45 A8 FE FF FF FF",
    __fastcall, void, WorldRenderer*, CL_Vec2f*);

static std::vector<std::string> displayNames;
static uint32_t vanillaWeatherBound = 16;
class CustomizedTitleScreen : public patch::BasePatch
{
  public:
    void apply() const override
    {
        auto& game = game::GameHarness::get();
        // Resolve functions
        real::BackgroundSunset =
            game.findMemoryPattern<BackgroundSunset_t>(pattern::BackgroundSunset);
        real::BackgroundNight = game.findMemoryPattern<BackgroundNight_t>(pattern::BackgroundNight);
        real::BackgroundDesert =
            game.findMemoryPattern<BackgroundDesert_t>(pattern::BackgroundDesert);
        real::BackgroundHarvest =
            game.findMemoryPattern<BackgroundHarvest_t>(pattern::BackgroundHarvest);
        real::BackgroundMars = game.findMemoryPattern<BackgroundMars_t>(pattern::BackgroundMars);
        real::BackgroundUndersea =
            game.findMemoryPattern<BackgroundUndersea_t>(pattern::BackgroundUndersea);
        real::BackgroundWarp = game.findMemoryPattern<BackgroundWarp_t>(pattern::BackgroundWarp);
        real::BackgroundWolf = game.findMemoryPattern<BackgroundWolf_t>(pattern::BackgroundWolf);
        real::BackgroundPagoda =
            game.findMemoryPattern<BackgroundPagoda_t>(pattern::BackgroundPagoda);
        real::BackgroundMonochrome =
            game.findMemoryPattern<BackgroundMonochrome_t>(pattern::BackgroundMonochrome);
        real::BackgroundTreasure =
            game.findMemoryPattern<BackgroundTreasure_t>(pattern::BackgroundTreasure);
        real::BackgroundSurgery =
            game.findMemoryPattern<BackgroundSurgery_t>(pattern::BackgroundSurgery);
        real::BackgroundBountiful =
            game.findMemoryPattern<BackgroundBountiful_t>(pattern::BackgroundBountiful);

        game.hookFunctionPatternDirect<MainMenuCreate_t>(pattern::MainMenuCreate, MainMenuCreate,
                                                         &real::MainMenuCreate);

        // Are we launching OSGT-QOL for the first time? If so, set a default other than Sunny.
        Variant* pVariant = real::GetApp()->GetVar("osgt_qol_title_bg");
        if (pVariant->GetType() == Variant::TYPE_UNUSED)
            pVariant->Set(10U);

        // We will allow the end-user to change their title screen weather preference
        auto& optionsMgr = game::OptionsManager::get();
        // Populate our options
        displayNames = {"Sunny",     "Sunset",      "Night",    "Desert",     "Harvest",
                        "Mars",      "Nothingness", "Undersea", "Warp Speed", "Comet (Blue)",
                        "Spring",    "Howling",     "Pagoda",   "Monochrome", "Treasure",
                        "SurgWorld", "Bountiful"};
        // Any custom weathers
        auto& weatherMgr = game::WeatherManager::get();
        for (auto it = weatherMgr.weathers.begin(); it != weatherMgr.weathers.end(); ++it)
        {
            // Signify custom weathers with golden colour
            displayNames.push_back("`6" + it->first);
        }
        weatherMgr.m_sig_eventSubscribe.connect(&customWeatherEvent);
        // Register the Multi-Choice option
        optionsMgr.addMultiChoiceOption("qol", "UI", "osgt_qol_title_bg", "Title Screen Background",
                                        displayNames, &TitleBackgroundOnSelect, 80.0f);
    }

    static void customWeatherEvent(game::WeatherManager::CustomWeatherEvent* pCustomWeather)
    {
        // Signify custom weathers with golden colour
        displayNames.push_back("`6" + pCustomWeather->m_prettyName);
    }

    static void TitleBackgroundOnSelect(VariantList* pVariant)
    {
        // Update the weather index
        Entity* pClickedEnt = pVariant->Get(1).GetEntity();
        Variant* pOptVar = real::GetApp()->GetVar("osgt_qol_title_bg");
        uint32_t idx = pOptVar->GetUINT32();
        if (pClickedEnt->GetName() == "back")
        {
            if (idx == 0)
                idx = (uint32_t)displayNames.size() - 1;
            else
                idx--;
        }
        else if (pClickedEnt->GetName() == "next")
        {
            if (idx >= displayNames.size() - 1)
                idx = 0;
            else
                idx++;
        }
        pOptVar->Set(idx);
        if (idx > vanillaWeatherBound)
        {
            // Custom weather, save the pretty name. These may shift around, but won't affect any
            // already applied weathers on subsequent boots.
            real::GetApp()->GetVar("osgt_qol_title_bg_modded")->Set(displayNames[idx].substr(2));
        }
        else
        {
            real::GetApp()->GetVar("osgt_qol_title_bg_modded")->Reset();
        }
        // Update the option label
        Entity* pTextLabel = pClickedEnt->GetParent()->GetEntityByName("txt");
        real::SetTextEntity(pTextLabel, displayNames[idx]);
        Entity* pGUIEnt = real::GetApp()->m_entityRoot->GetEntityByName("GUI");
        if (pGUIEnt->GetEntityByName("MainMenu"))
            ChangeMainMenuWeather(pGUIEnt);
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

        std::string versionText = "`wOSGT-QOL " OSGT_QOL_DISPLAY_VERSION "``";
        Entity* pTextLabel = real::CreateTextLabelEntity(
            pEnt->GetEntityByName("MainMenu"), "mltxt", real::iPhoneMapX(5.0f),
            m_verLabelPos.y - m_verLabelSize.y, versionText);

        // Retrieve fontscale and scale created entity
        uint32_t fontID;
        float fontScale;
        real::GetFontAndScaleToFitThisLinesPerScreenY(fontID, fontScale, 20);
        real::SetupTextEntity(pTextLabel, fontID, fontScale);
        // Fade in at approximately same time as version text
        real::FadeInEntity(pTextLabel, true, 600, 400, 1.0, true);
    }

    static void ChangeMainMenuWeather(Entity* pGUIEnt)
    {
        // Lets retrieve MapBGComponent from GUI -> MainMenu
        MapBGComponent* pMapBGComponent = reinterpret_cast<MapBGComponent*>(
            pGUIEnt->GetEntityByName("MainMenu")->GetComponentByName("MapBGComponent"));

        uint32_t weatherIdx = real::GetApp()->GetVar("osgt_qol_title_bg")->GetUINT32();
        Background* pNewBG;
        if (weatherIdx > vanillaWeatherBound)
        {
            // Get custom weather from weather manager - if we don't have it, most likely the mod
            // has been uninstalled or save.dat is manually tampered.
            auto& weatherMgr = game::WeatherManager::get();
            auto pair = weatherMgr.weathers.find(
                real::GetApp()->GetVar("osgt_qol_title_bg_modded")->GetString());
            if (pair == weatherMgr.weathers.end())
            {
                pNewBG = new Background_Default();
                real::GetApp()->GetVar("osgt_qol_title_bg_modded")->Reset();
                real::GetApp()->GetVar("osgt_qol_title_bg")->Set(0U);
            }
            else
            {
                pNewBG = pair->second.callback();
            }
        }
        else
        {
            // Vanilla weathers
            switch (weatherIdx)
            {
            case 0:
            {
                pNewBG = new Background_Default();
                break;
            }
            case 1:
            {
                // For the weathers we don't have a reversed class for, create a raw buffer.
                // In case of Background_Sunset, it should be 0x270 in size.
                pNewBG = real::BackgroundSunset(operator new(0x270));
                break;
            }
            case 2:
            {
                // Background_Night takes an additional value in constructor, namely the weather ID
                // which dictates if it should render it as normal night or a comet weather.
                pNewBG = real::BackgroundNight(operator new(0x348), 2);
                break;
            }
            case 3:
            {
                // Desert / Arid
                pNewBG = real::BackgroundDesert(operator new(0x280));
                break;
            }
            case 4:
            {
                // Harvest Blast
                pNewBG = real::BackgroundHarvest(operator new(0x2c8));
                break;
            }
            case 5:
            {
                // Mars
                pNewBG = real::BackgroundMars(operator new(0x298));
                break;
            }
            case 6:
            {
                // Blank (Nothingness)
                pNewBG = new Background_Blank();
                break;
            }
            case 7:
            {
                // Undersea
                pNewBG = real::BackgroundUndersea(operator new(0x6a0));
                break;
            }
            case 8:
            {
                // Warp Speed
                // 15 for Warp Speed, anything else for Stargazing
                pNewBG = real::BackgroundWarp(operator new(0x218), 15);
                break;
            }
            case 9:
            {
                // Comet (Blue/Weather)
                pNewBG = real::BackgroundNight(operator new(0x348), 17);
                break;
            }
            case 10:
            {
                // Spring
                pNewBG = new Background_Default();
                ((Background_Default*)pNewBG)->m_bIsSpring = true;
                break;
            }
            case 11:
            {
                // Wolf
                pNewBG = real::BackgroundWolf(operator new(0x330));
                break;
            }
            case 12:
            {
                // Pagoda
                pNewBG = real::BackgroundPagoda(operator new(0x2a0));
                break;
            }
            case 13:
            {
                // Monochrome
                pNewBG = real::BackgroundMonochrome(operator new(0x198));
                break;
            }
            case 14:
            {
                // Treasure
                pNewBG = real::BackgroundTreasure(operator new(0x250));
                break;
            }
            case 15:
            {
                // Surgery
                pNewBG = real::BackgroundSurgery(operator new(0x1b8));
                break;
            }
            case 16:
            {
                // Bountiful
                pNewBG = real::BackgroundBountiful(operator new(0x1a0));
                break;
            }
            default:
                pNewBG = new Background_Default();
                printf("Unhandled weather %d\n", weatherIdx);
                break;
            }
        }

        if (pNewBG != nullptr)
        {
            // Initialize the weather with bInWorld as false.
            pNewBG->Init(false);
            // Do an update as well before anything renders it. This is needed in rare cases such as
            // Background_Treasure where one specific variable doesn't have an initial value, so it
            // gets trashed and causes a game crash.
            pNewBG->Update();
            // Save our old one to delete in a brief second.
            Background* pOriginalWeather = (Background*)pMapBGComponent->m_pBackground;
            // Assign our new one in place.
            pMapBGComponent->m_pBackground = pNewBG;
            // Discard the original.
            delete pOriginalWeather;
        }
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
        optionsMgr.addSliderOption("qol", "UI", "speech_bubble_opacity", "Bubble Opacity",
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

static bool g_lastHideState = false;
class HideMyUI : public patch::BasePatch
{
  public:
    void apply() const override
    {
        // This patch disables the right-hand UI icons on demand by pressing Ctrl+H.
        // Particularly useful for world builders who might keep accidentally hitting the buttons
        // instead.
        auto& events = game::EventsAPI::get();
        events.m_sig_onArcadeInput.connect(&OnArcadeInput);
        events.m_sig_addWasdKeys.connect(&AddCustomKeybinds);
        events.m_sig_onMapLoaded.connect(&OnMapLoaded);

        // Make the opacity toggleable, default at 33%.
        Variant* pVariant = real::GetApp()->GetVar("hide_ui_opacity");
        if (pVariant->GetType() != Variant::TYPE_FLOAT)
            pVariant->Set(0.33f);
        pVariant->GetSigOnChanged()->connect(&OnOpacitySigFire);

        auto& optionsMgr = game::OptionsManager::get();
        optionsMgr.addSliderOption("qol", "UI", "hide_ui_opacity", "Hide UI Opacity",
                                   &HideUIOpacitySliderCallback,
                                   "`5(Use CTRL+H while inside a world to toggle this)``");
        optionsMgr.addCheckboxOption("qol", "UI", "hide_ui_scrollers",
                                     "Hide chat and inventory handles too",
                                     &HideUIScrollHandlesCallback);
    }

    static void OnMapLoaded(void*, __int64, __int64, __int64)
    {
        // Log slider may lose opacity on world change
        if (g_lastHideState)
            SetSlidersOpacity(real::GetApp()->GetVar("hide_ui_opacity")->GetFloat());
    }

    static void OnOpacitySigFire(Variant* pVariant)
    {
        if (!g_lastHideState)
            return;
        Entity* pMenu = real::GetApp()
                            ->m_entityRoot->GetEntityByName("GUI")
                            ->GetEntityByName("WorldSpecificGUI")
                            ->GetEntityByName("GameMenu");
        if (pMenu)
        {
            SetGameMenuOpacity(pMenu, pVariant->GetFloat());
        }
    }

    static void SetSlidersOpacity(float alphaLevel)
    {
        Entity* pMenu = real::GetApp()
                            ->m_entityRoot->GetEntityByName("GUI")
                            ->GetEntityByName("WorldSpecificGUI")
                            ->GetEntityByName("GameMenu");
        if (pMenu)
        {
            pMenu->GetEntityByName("ItemsParent")
                ->GetEntityByName("InventoryGrab")
                ->GetVar("alpha")
                ->Set(alphaLevel);
        }
        if (real::GetApp()->m_entityRoot->GetEntityByName("ConsoleLogParent"))
        {
            Entity* ConsoleGrab = real::GetApp()
                                      ->m_entityRoot->GetEntityByName("ConsoleLogParent")
                                      ->GetEntityByName("ConsoleGrab");
            ConsoleGrab->RemoveComponentByName("Interpolate");
            ConsoleGrab->GetVar("alpha")->Set(alphaLevel);
        }
    }

    static void SetGameMenuOpacity(Entity* pMenu, float alphaLevel)
    {
        // Check for mods support, each mod must be supported individually.
        Entity* pTimerOverlay = pMenu->GetParent()->GetEntityByName("TimerOverlay");
        if (pTimerOverlay)
            pTimerOverlay->GetVar("alpha")->Set(alphaLevel);

        // If the fade-in animation is going on, suspend it.
        Entity* pEventMenu = pMenu->GetEntityByName("EVENTS");
        if (g_lastHideState)
        {
            // TouchHandler is responsible for sinking the input for the entity, we want to
            // get rid of it.
            pMenu->GetEntityByName("MENU")->RemoveComponentByName("TouchHandler");
            pMenu->GetEntityByName("CHAT")->RemoveComponentByName("TouchHandler");
            pMenu->GetEntityByName("FRIENDS")->RemoveComponentByName("TouchHandler");
            if (pEventMenu)
                pEventMenu->RemoveComponentByName("TouchHandler");
            pMenu->GetEntityByName("GemTouch")->RemoveComponentByName("TouchHandler");

            // If the fade-in animation is going on, suspend it.
            if (pMenu->GetEntityByName("MENU")->GetComponentByName("Interpolate"))
            {
                pMenu->GetEntityByName("MENU")->RemoveComponentByName("Interpolate");
                pMenu->GetEntityByName("CHAT")->RemoveComponentByName("Interpolate");
                pMenu->GetEntityByName("FRIENDS")->RemoveComponentByName("Interpolate");
                if (pEventMenu)
                    pEventMenu->RemoveComponentByName("Interpolate");
                pMenu->GetEntityByName("GemTouch")->RemoveComponentByName("Interpolate");
                pMenu->GetEntityByName("BuxEnt")->RemoveComponentByName("Interpolate");
            }
        }
        pMenu->GetEntityByName("MENU")->GetVar("alpha")->Set(alphaLevel);
        pMenu->GetEntityByName("CHAT")->GetVar("alpha")->Set(alphaLevel);
        pMenu->GetEntityByName("FRIENDS")->GetVar("alpha")->Set(alphaLevel);
        pMenu->GetEntityByName("GemTouch")->GetVar("alpha")->Set(alphaLevel);
        pMenu->GetEntityByName("BuxEnt")->GetVar("alpha")->Set(alphaLevel);
        if (pEventMenu)
        {
            Variant* pAlphaVar = pEventMenu->GetVar("alpha");
            if (pAlphaVar->GetFloat() != 0.00f)
                pAlphaVar->Set(alphaLevel);
        }
        // Also dim the scroll handles for inv/chat
        if (real::GetApp()->GetVar("hide_ui_scrollers")->GetUINT32() == 1)
            SetSlidersOpacity(alphaLevel);
    }

    static void HideUIOpacitySliderCallback(Variant* pVariant)
    {
        real::GetApp()->GetVar("hide_ui_opacity")->Set(pVariant->GetFloat());
    }

    static void HideUIScrollHandlesCallback(VariantList* pVariant)
    {
        Entity* pCheckbox = pVariant->Get(1).GetEntity();
        bool bChecked = pCheckbox->GetVar("checked")->GetUINT32() != 0;
        real::GetApp()->GetVar("hide_ui_scrollers")->Set(uint32_t(bChecked));
        Entity* pGUI = real::GetApp()->m_entityRoot->GetEntityByName("GUI");
        Entity* pMenu = pGUI->GetEntityByName("WorldSpecificGUI")->GetEntityByName("GameMenu");
        if (pMenu != nullptr)
        {
            if (!pMenu->GetEntityByName("MENU")->GetComponentByName("TouchHandler"))
            {
                if (bChecked)
                    SetSlidersOpacity(real::GetApp()->GetVar("hide_ui_opacity")->GetFloat());
                else
                    SetSlidersOpacity(1.00f);
            }
        }
    }

    static void __fastcall OnArcadeInput(VariantList* pVL)
    {
        if (pVL->Get(0).GetUINT32() == 610001)
        {
            if (real::GetApp()->GetGameLogic()->IsDialogOpened())
                return;

            Entity* pGUI = real::GetApp()->m_entityRoot->GetEntityByName("GUI");
            if (pGUI->GetEntityByName("OptionsMenu") || pGUI->GetEntityByName("ResolutionMenu") ||
                pGUI->GetEntityByName("OptionsPage"))
                return;
            // GUI -> WorldSpecificGUI always exists. GameMenu only does when in a world.
            Entity* pMenu = pGUI->GetEntityByName("WorldSpecificGUI")->GetEntityByName("GameMenu");
            if (pMenu != nullptr)
            {
                // Change opacity of disabled elements to 33% and remove their TouchHandler
                float alphaLevel = real::GetApp()->GetVar("hide_ui_opacity")->GetFloat();
                g_lastHideState = !g_lastHideState;

                Entity* pEventMenu = pMenu->GetEntityByName("EVENTS");
                if (!g_lastHideState)
                {
                    alphaLevel = 1.00f;
                    // Bring back the input sink for buttons.
                    pMenu->GetEntityByName("MENU")->AddComponent(
                        real::TouchHandlerComponent(operator new(0x120)));
                    pMenu->GetEntityByName("CHAT")->AddComponent(
                        real::TouchHandlerComponent(operator new(0x120)));
                    pMenu->GetEntityByName("FRIENDS")->AddComponent(
                        real::TouchHandlerComponent(operator new(0x120)));
                    if (pEventMenu)
                        pEventMenu->AddComponent(real::TouchHandlerComponent(operator new(0x120)));
                    pMenu->GetEntityByName("GemTouch")
                        ->AddComponent(real::TouchHandlerComponent(operator new(0x120)));
                }

                SetGameMenuOpacity(pMenu, alphaLevel);
            }
        }
    }

    static void AddCustomKeybinds()
    {
        // Binds Ctrl+H key to hide UI
        real::AddKeyBinding(real::GetArcadeComponent(), "chatkey_hideUI", 72, 610001, 1, 1);
    }
};
REGISTER_USER_GAME_PATCH(HideMyUI, hide_my_ui);

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

class DaVinciFakeID : public patch::BasePatch
{
  public:
    void apply() const override
    {
        // Modifies avatars to portray old Da Vinci Wings ID as new one.
        // Cosmetic issue with this approach is that it shows as unequipped in inventory.
        auto& game = game::GameHarness::get();
        game.hookFunctionPatternDirect<AvatarDataGetSetAsUint16_t>(
            pattern::AvatarDataGetSetAsUint16, AvatarDataGetSetAsUint16,
            &real::AvatarDataGetSetAsUint16);
    }

    static void __fastcall AvatarDataGetSetAsUint16(void* this_, uint16_t* set)
    {
        real::AvatarDataGetSetAsUint16(this_, set);
        if (set[6] == 3308)
            set[6] = 8286;
    }
};
REGISTER_USER_GAME_PATCH(DaVinciFakeID, fake_davinci_id);

class LegendaryWingsPatch : public patch::BasePatch
{
  public:
    void apply() const override
    {
        // V3.02 unpatched will render Da Vinci wings when wearing Legendary Wings, likely as
        // outcome of changing Da Vinci Wings ID earlier. You can see from the game disassembly,
        // that legendary wings get pointed to a correct surface at first.
        // ---
        // if (ItemID == ITEM_ID_LEGENDARY_WINGS) {
        //    plVar14 = *(Surface **)(this + 0xd8);
        //    pIVar25 = pIVar26;
        //    goto LAB_14012fb26;
        // }
        // ---
        // However, when the unconditional jump is done, it will reach a code path that instead
        // points the surface to (this + 0xe0) (Da Vinci Wings).
        // Since we already have a valid surface pointed, we can just skip a little bit forwards to
        // just rendering the item. This fixes the issue while keeping both items functional and
        // does not seem to break any other back items.

        auto& game = game::GameHarness::get();
        auto addr =
            game.findMemoryPattern<uint8_t*>("E9 ? ? ? ? 81 FE CE 24 00 00 75 0C 4D 8B A6 E8 00 00 "
                                             "00 E9 ? ? ? ? 81 FE 9A 1E 00 00 74 08");
        // E9 84 00 00 00 -> E9 DF 00 00 00, rel32 jump changed from 132 bytes to 223.
        utils::fillMemory(addr + 1, 1, 0xDF);
    }
};
REGISTER_USER_GAME_PATCH(LegendaryWingsPatch, legendary_wing_patch);

static bool g_bLightSourcesOptimized = true;
static int g_fLightSourceOptimizeLevel = 5;
class LightSourceOptimizer : public patch::BasePatch
{
  public:
    void apply() const override
    {
        // The way Growtopia calculates where light should spread is very inefficient - for every
        // light source, it will scan the entire tilemap in your viewport. That is rather expensive
        // with a ton of light emitters on a large zoom (such as mod zoom).
        //
        // This mod partially rewrites that logic by instead doing a maximum 5x5 rectangular search
        // for darkened tiles and applies light calculation to them only.
        //
        // On a machine using Ryzen 7 8845HS with Radeon 780M - the performance difference on a
        // 100x56 area full of Chandelier blocks. For control to see efficiency of the light
        // calculations, a reference world with same parameters was created, but instead of
        // Chandelier (a light emitter, animated block), it was filled with Party Cacti (non-light
        // emitter, animated block).
        //
        // Unpatched: 10-11 FPS
        // Patched: 158-172 FPS
        // Reference world: 168-178 FPS
        // Frame cap: 200 FPS (240 Hz)

        auto& game = game::GameHarness::get();

        game.hookFunctionPatternDirect<WorldRendererDrawTiles_t>(
            pattern::WorldRendererDrawTiles, WorldRendererDrawTiles, &real::WorldRendererDrawTiles);

        real::WorldRendererTileLineOfSight = game.findMemoryPattern<WorldRendererTileLineOfSight_t>(
            pattern::WorldRendererTileLineOfSight);

        Variant* pVariant = real::GetApp()->GetVar("osgt_qol_lightopt_rad");
        if (pVariant->GetType() != Variant::TYPE_FLOAT)
            pVariant->Set(1.00f);
        else
            g_fLightSourceOptimizeLevel = (int)(pVariant->GetFloat() * 5.0f);

        pVariant = real::GetApp()->GetVar("osgt_qol_lightopt_enabled");
        if (pVariant->GetType() != Variant::TYPE_UINT32)
            pVariant->Set(uint32_t(1));
        else
            g_bLightSourcesOptimized = pVariant->GetUINT32() == 1;

        auto& optionsMgr = game::OptionsManager::get();
        optionsMgr.addCheckboxOption("qol", "Performance", "osgt_qol_lightopt_enabled",
                                     "Optimize Light Emitters", &LightOptimizerToggle);
        optionsMgr.addSliderOption("qol", "Performance", "osgt_qol_lightopt_rad",
                                   "Light Emitter Strength", &LightOptimizerSlider,
                                   "`5(See more at expense of fps)``");
    }

    static void LightOptimizerSlider(Variant* pVariant)
    {
        // We scale float 0.0-1.0 to 0-5 radius during light calculation.
        float origLvl = pVariant->GetFloat();
        if (origLvl >= 1.0f)
        {
            g_fLightSourceOptimizeLevel = 5;
            real::GetApp()->GetVar("osgt_qol_lightopt_rad")->Set(origLvl);
        }
        else
        {
            float radLvl = float(uint32_t(origLvl * 5.0f)) / 5.0f;
            g_fLightSourceOptimizeLevel = (int)(radLvl * 5.0f);
            real::GetApp()->GetVar("osgt_qol_lightopt_rad")->Set(radLvl);
        }
    }

    static void LightOptimizerToggle(VariantList* pVariant)
    {
        Entity* pCheckbox = pVariant->Get(1).GetEntity();
        bool bChecked = pCheckbox->GetVar("checked")->GetUINT32() != 0;
        g_bLightSourcesOptimized = bChecked;
        real::GetApp()->GetVar("osgt_qol_lightopt_enabled")->Set(uint32_t(bChecked));
    }

    static void __fastcall WorldRendererDrawTiles(void* this_, std::vector<Tile*>* tiles, int unk3)
    {
        // Disarm original light calc logic by tricking original renderer.
        // We will be doing our own "light pass" before calling DrawTiles itself.
        if (unk3 == 0 || !g_bLightSourcesOptimized)
        {
            real::WorldRendererDrawTiles(this_, tiles, unk3);
            return;
        }
        // This value happens to dictate if DrawTiles will calculate light levels, so we can
        // temporarily assign it to a value where it won't do that, we will be doing the light
        // calculation pass instead.
        float original = *(float*)(((__int64)this_) + 0x15c);
        *(float*)(((__int64)this_) + 0x15c) = 0.1f;

        std::vector<ItemInfo>& items = real::GetApp()->GetItemInfoManager()->m_items;
        int w = real::GetApp()->GetGameLogic()->GetTileWidth();
        int h = real::GetApp()->GetGameLogic()->GetTileHeight();
        WorldTileMap* tilemap = (WorldTileMap*)real::GetApp()->GetGameLogic()->GetTileMap();
        Tile* m_tiles = tilemap->m_tiles;
        size_t max = tiles->size();

        int r = g_fLightSourceOptimizeLevel;

        int m_timeMS = real::GetApp()->m_gameTimer.m_timeMS;

        for (size_t i = 0; i < max; i++)
        {
            Tile* tile = (*tiles)[i];
            int vis = items[tile->m_itemID].visualType;
            // Lightsource / Lightsource Pulse / Lightsource If On
            if ((vis == 34 || vis == 45) || (vis == 35 && (tile->m_tileProperties & 64)))
            {
                int x = tile->x;
                int y = tile->y;

                // Interpolation logic same as original.
                float lightStrength = SinPulseByCustomTimerMS(4000, (x + y) * 100 + m_timeMS);
                lightStrength = (lightStrength * 0.25f + 1.0f) * 128.0f;

                int minX = x - r;
                if (minX < 0)
                    minX = 0;
                int maxX = x + r;
                if (maxX > w)
                    maxX = w;

                int minY = y - r;
                if (minY < 0)
                    minY = 0;
                int maxY = y + r;
                if (maxY > h)
                    maxY = h;

                Tile* target = nullptr;
                for (x = minX; x < maxX; x++)
                {
                    for (y = minY; y < maxY; y++)
                    {
                        target = &m_tiles[x + (y * w)];
                        vis = items[target->m_itemBGID].visualType;
                        // Only do light calculations if we're on a darkened tile.
                        if (vis == 33 || vis == 41)
                        {
                            // If we are on a light source, we are already on light level 0.0, skip
                            // a no-op.
                            if (target->m_lightLevel > 0.0f)
                            {
                                float rayPower;
                                if (real::WorldRendererTileLineOfSight(this_, tile, target,
                                                                       lightStrength, &rayPower))
                                {
                                    // Game resets tile->m_lightLevel across the board on each
                                    // render cycle, so we don't need to worry about perma-stuck
                                    // light tiles.
                                    float light = rayPower / lightStrength;
                                    if (light < target->m_lightLevel)
                                        target->m_lightLevel = light;
                                }
                            }
                        }
                    }
                }
                tile->m_lightLevel = 0.0;
            }
        }
        real::WorldRendererDrawTiles(this_, tiles, unk3);
        *(float*)(((__int64)this_) + 0x15c) = original;
    }
};
REGISTER_USER_GAME_PATCH(LightSourceOptimizer, lightsource_optimized);

static int* g_devicePixelsPerInch = nullptr;
class BetterLogGrabDPI : public patch::BasePatch
{
  public:
    void apply() const override
    {
        // The DPI values provided by game look rather amusing on a desktop on high resolutions,
        // they were more meant for mobile platforms or touchscreens. Considering the core
        // demographic of this mod is going to be mouse & keyboard users, we can afford to make the
        // DPI a bit more sensible.
        // This mod also adds 4K scaling to grab handles.
        g_devicePixelsPerInch =
            utils::resolveMovCall<int*>((uint8_t*)real::GetDevicePixelsPerInchDiagonal + 6);

        auto& game = game::GameHarness::get();
        real::SetGrabBarSize = game.findMemoryPattern<SetGrabBarSize_t>(pattern::SetGrabBarSize);

        auto& events = game::EventsAPI::get();
        events.m_sig_postInitVideo.connect(&postInitVideo);

        Variant* pVariant = real::GetApp()->GetVar("osgt_qol_loggrab_dpi");
        if (pVariant->GetType() == Variant::TYPE_UNUSED)
            pVariant->Set(1U);

        auto& optionsMgr = game::OptionsManager::get();
        optionsMgr.addCheckboxOption("qol", "UI", "osgt_qol_loggrab_dpi",
                                     "Use DPI scales more fit for desktop use on high resolutions",
                                     &DPIScaleToggle);
    }

    static void DPIScaleToggle(VariantList* pVariant)
    {
        Entity* pCheckbox = pVariant->Get(1).GetEntity();
        bool bChecked = pCheckbox->GetVar("checked")->GetUINT32() != 0;
        real::GetApp()->GetVar("osgt_qol_loggrab_dpi")->Set(uint32_t(bChecked));

        // Reset DPI scale
        *g_devicePixelsPerInch = 0;
        real::GetDevicePixelsPerInchDiagonal();
        if (bChecked)
            postInitVideo();

        Entity* pConsoleLogParent =
            real::GetApp()->m_entityRoot->GetEntityByName("ConsoleLogParent");
        if (pConsoleLogParent)
        {
            Entity* pConsoleGrab = pConsoleLogParent->GetEntityByName("ConsoleGrab");
            if (pConsoleGrab)
            {
                // SetGrabBarSize will use scale2d as a reference point, reset it to 1.0.
                pConsoleGrab->GetVar("scale2d")->Set(CL_Vec2f{1.0, 1.0});
                real::SetGrabBarSize(pConsoleGrab);
            }
        }
        else
            return;

        Entity* pGUI = real::GetApp()->m_entityRoot->GetEntityByName("GUI");
        if (!pGUI)
            return;
        Entity* pWorldGUI = pGUI->GetEntityByName("WorldSpecificGUI");
        if (!pWorldGUI)
            return;
        Entity* pGameMenu = pWorldGUI->GetEntityByName("GameMenu");
        if (!pGameMenu)
            return;

        Entity* pInventoryGrab = pGameMenu->GetEntityByNameRecursively("InventoryGrab");
        if (pInventoryGrab)
        {
            pInventoryGrab->GetVar("scale2d")->Set(CL_Vec2f{1.0, 1.0});
            real::SetGrabBarSize(pInventoryGrab);
        }
    }

    static void postInitVideo()
    {
        Rectf screenRect;
        real::GetScreenRect(screenRect);

        // Already rather low DPI, don't attempt.
        if (*g_devicePixelsPerInch < 273)
            return;

        // 2K (2560 x H) - Height range 1440-1800
        if (screenRect.right == 2560 && (screenRect.bottom >= 1440 && screenRect.bottom <= 1800))
        {
            *g_devicePixelsPerInch = 350;
            return;
        }

        // 4K (W x 2160) - Width range 3840-4096
        if ((screenRect.right >= 3840 && screenRect.right <= 4096) && screenRect.bottom == 2160)
        {
            *g_devicePixelsPerInch = 500;
            return;
        }

        // 8K would need to be added here as well if anyone cares/has a display with it.

        // Fallback scaling for resolutions originally supported, but not covered (2K by this
        // formula is ~365 DPI)
        // 4K would still be at 273 since the game never sets it a proper DPI.
        if (*g_devicePixelsPerInch > 273)
            *g_devicePixelsPerInch = 273 + ((*g_devicePixelsPerInch - 273) / 3);
    }
};
REGISTER_USER_GAME_PATCH(BetterLogGrabDPI, better_log_grab_dpi);

class LiveGUIRebuilder : public patch::BasePatch
{
  public:
    void apply() const override
    {
        // Since the PC release, if you were to toggle Touch Controls or change between fullscreen,
        // you would get disconnected from the game to rebuild the user interface. It's a major
        // point of annoyance and it wasn't addressed when custom windowed resolutions were released
        // either.
        //
        // With this mod, we disarm the disconnect logic, in its place we will be killing the active
        // user interface and recreating it on the fly. The game makes it somewhat easy for us, we
        // do not really need to implement anything ourself, InitLog destroys the chat itself if it
        // exists before creating new one, the only caveat is that it'll send whatever the player
        // had typed out but not sent to chat. There is also KillGameMenu and GameMenuCreate
        // functions to recreate rest of the world specific UI.
        //
        // We also disable using Pause Menu hotkey while Options or ResolutionMenu is open since it
        // will mess up chat grab slider post-UI-recreation.
        //
        // The mod does have some issues, notably your chat and inventory scroll progress will be
        // bit weird switching between resolutions and you do not go directly back to Options menu
        // after changing resolution, but rather back to the game window itself.
        //
        // There are some other vanilla bugs present such as chat not reflowing previous
        // conversations when changing resolutions, but that isn't the scope of this mod to fix in
        // this stage.

        auto& game = game::GameHarness::get();

        // Disarm disconnect logic.
        auto addr = game.findMemoryPattern<uint8_t*>(
            "83 7F 58 00 74 38 F3 0F 2C ? ? ? ? ? 0F BE ? ? ? ? ? 03 C8 29 ? ? ? ? ? E8 ? ? ? ? B2 "
            "01 48 8B 88 48 0F 00 00 E8 ? ? ? ? 48 8D ? ? ? ? ? E8 ? ? ? ? EB 3D");
        utils::nopMemory(addr, 69);

        game.hookFunctionPatternDirect<CreateOptionsMenu_t>(
            pattern::CreateOptionsMenu, CreateOptionsMenu, &real::CreateOptionsMenu);
        real::InitLog = game.findMemoryPattern<InitLog_t>(pattern::InitLog);
        real::KillGameMenu = game.findMemoryPattern<KillGameMenu_t>(pattern::KillGameMenu);
        real::GameMenuCreate = game.findMemoryPattern<GameMenuCreate_t>(pattern::GameMenuCreate);

        game.hookFunctionPatternDirect<OnPressingBackDuringGameplay_t>(
            pattern::OnPressingBackDuringGameplay, OnPressingBackDuringGameplay,
            &real::OnPressingBackDuringGameplay);
    }

    static void __fastcall OnPressingBackDuringGameplay()
    {
        Entity* pGUI = real::GetApp()->m_entityRoot->GetEntityByName("GUI");
        if (!pGUI->GetEntityByName("OptionsMenu") && !pGUI->GetEntityByName("ResolutionMenu") &&
            !pGUI->GetEntityByName("OptionsPage"))
            real::OnPressingBackDuringGameplay();
    }

    static void __fastcall CreateOptionsMenu(VariantList* pVL)
    {
        if (pVL != nullptr)
        {
            // Same logic as the actual game - we only act if a rebuild action is done (e.g. screen
            // res change, touch buttons) AND if it's not in main menu.
            if (pVL->Get(0).GetUINT32() == 0)
            {
                if (pVL->Get(1).GetUINT32() != 0)
                {
                    // Kill gamemenu, rebuild chat, then recreate gamemenu and prevent options from
                    // showing up again. If we allow options to show up again, it'll mess up the
                    // chatgrab button position.
                    real::KillGameMenu(false);
                    real::InitLog();
                    Entity* pWGUI =
                        real::GetApp()->m_entityRoot->GetEntityByName("GUI")->GetEntityByName(
                            "WorldSpecificGUI");
                    Entity* pGameMenu = pWGUI->GetEntityByName("GameMenu");
                    pWGUI->RemoveEntityByAddress(pGameMenu);
                    real::GameMenuCreate(pWGUI);
                    return;
                }
            }
        }
        real::CreateOptionsMenu(pVL);
    }
};
REGISTER_USER_GAME_PATCH(LiveGUIRebuilder, live_gui_rebuilder);

class InstantWorldButtonsPatch : public patch::BasePatch
{
  public:
    void apply() const override
    {
        auto& game = game::GameHarness::get();

        // Since we already use FadeInEntity elsewhere, I don't really think there's any clean way
        // to hook it? Instead, patch on (kinda) the callsite of FadeInEntity inside AddFloater.
        //
        // However... FadeInEntity will not work right if you pass 0 as duration. So we can't just
        // XOR the argument register to 0. To put an immediate value into the register would require
        // too much space on the callsite directly.
        //
        // Since the fade-in effect is random (via RandomRange), we can clamp the random range to be
        // [1, 1], effectively making FadeInEntity always get 1 as duration.
        //
        // I promise that once we rewrite we'll replace this with a proper hook.
        uint8_t* p = game.findMemoryPattern<uint8_t*>("BA DC 05 00 00 "
                                                      "B9 58 02 00 00 "
                                                      "E8 ? ? ? ? "
                                                      "44 8B F0 "
                                                      "BA 40 06 00 00 "
                                                      "B9 F4 01 00 00 "
                                                      "E8");

        const uint32_t one = 1;
        utils::writeMemoryBuffer(p + 1, &one, sizeof(one));  // edx
        utils::writeMemoryBuffer(p + 6, &one, sizeof(one));  // ecx
        utils::writeMemoryBuffer(p + 19, &one, sizeof(one)); // edx
        utils::writeMemoryBuffer(p + 24, &one, sizeof(one)); // ecx
    }
};
REGISTER_USER_GAME_PATCH(InstantWorldButtonsPatch, instant_world_buttons);

class AnchorCameraToPlayerPatch : public patch::BasePatch
{
  public:
    void apply() const override
    {
        auto& game = game::GameHarness::get();

        // Nop out the original WorldCamera::OnUpdate clamping logic. We'll add our own toggle for
        // it.
        uint8_t* p = game.findMemoryPattern<uint8_t*>("0F 2F 43 10 0F 5B D2 0F 5B C9 F3 0F 59 15");
        utils::nopMemory(p, 80);

        // WorldCamera::OnUpdate hook for toggling between the anchoring.
        game.hookFunctionPatternDirect<WorldCameraOnUpdate_t>(
            pattern::WorldCameraOnUpdate, WorldCameraOnUpdate, &real::WorldCameraOnUpdate);
        // WorldCamera::GetCamWorldPos for clamping weathers separately while using anchored view,
        // prevents glitched out weathers.
        game.hookFunctionPatternDirect<WorldCameraGetCamWorldPos_t>(
            pattern::WorldCameraGetCamWorldPos, WorldCameraGetCamWorldPos,
            &real::WorldCameraGetCamWorldPos);
        // WorldRenderer::OnRender for rendering out-of-bounds greyed out area, otherwise it's not
        // really obvious where the world ends.
        game.hookFunctionPatternDirect<WorldRendererOnRender_t>(
            pattern::WorldRendererOnRender, WorldRendererOnRender, &real::WorldRendererOnRender);

        // These will default to 0 on new vars.
        m_centerCameraOnPlayer = real::GetApp()->GetVar("osgt_qol_camera_clamp")->GetUINT32();
        m_hotkeyEnabled = real::GetApp()->GetVar("osgt_qol_quick_clamp")->GetUINT32();

        // Our options, maybe they're better grouped together, but by categorization they should be
        // in UI and Input respectively.
        auto& optionsMgr = game::OptionsManager::get();
        optionsMgr.addCheckboxOption("qol", "UI", "osgt_qol_camera_clamp",
                                     "Always center camera on player (shows out of bounds)",
                                     &OnClampCallback);
        optionsMgr.addCheckboxOption("qol", "Input", "osgt_qol_quick_clamp",
                                     "Toggle centered camera with Ctrl+C hotkey",
                                     &OnHotkeyCallback);

        // Subscribe to events for hotkeys
        auto& events = game::EventsAPI::get();
        events.m_sig_onArcadeInput.connect(&OnArcadeInput);
        events.m_sig_addWasdKeys.connect(&AddCustomKeybinds);
    }

    // Option callbacks
    static void OnClampCallback(VariantList* pVariant)
    {
        Entity* pCheckbox = pVariant->Get(1).GetEntity();
        m_centerCameraOnPlayer = pCheckbox->GetVar("checked")->GetUINT32() != 0;
        real::GetApp()->GetVar("osgt_qol_camera_clamp")->Set(uint32_t(m_centerCameraOnPlayer));
        // Camera update is called anyway each update cycle, so we don't really need to do anything
        // here.
    }
    static void OnHotkeyCallback(VariantList* pVariant)
    {
        Entity* pCheckbox = pVariant->Get(1).GetEntity();
        m_hotkeyEnabled = pCheckbox->GetVar("checked")->GetUINT32() != 0;
        real::GetApp()->GetVar("osgt_qol_quick_clamp")->Set(uint32_t(m_hotkeyEnabled));
    }

    // Helper functions for amount of pixels drawn out of world bounds
    static float getClampedRightBorder(WorldCamera& pCamera, float& clampX)
    {
        return pCamera.m_screenSize.x - ((pCamera.m_position.x - clampX) * pCamera.m_zoomLevel.x);
    }
    static float getClampedLeftBorder(WorldCamera& pCamera)
    {
        return abs(pCamera.m_position.x) * pCamera.m_zoomLevel.x;
    }
    static float getClampedTopBorder(WorldCamera& pCamera)
    {
        return abs(pCamera.m_position.y) * pCamera.m_zoomLevel.y;
    }
    static float getClampedBottomBorder(WorldCamera& pCamera, float& clampY)
    {
        return pCamera.m_screenSize.y - ((pCamera.m_position.y - clampY) * pCamera.m_zoomLevel.y);
    }

    // Camera & render tweaks
    static void WorldRendererOnRender(WorldRenderer* this_, CL_Vec2f* p2)
    {
        real::WorldRendererOnRender(this_, p2);

        if (m_centerCameraOnPlayer)
        {
            // We will shadow out the out of bounds areas to prevent confusion with centered camera.
            // For left and top sides, it's simple, we only need to use current camera position
            // (this is not directly relative to NetAvatar) and multiply it by the zoom level.
            // For right and botttom sides, we have to calculate against the tilemap clamping to get
            // appropriate x/y level.
            WorldTileMap* pTileMap = real::GetApp()->GetGameLogic()->GetTileMap();
            float xMax = (float)pTileMap->m_width * 32.0f;
            float yMax = (float)pTileMap->m_height * 32.0f;
            float clampX = xMax - this_->m_worldCamera.m_zoomedScreenSize.x;
            float clampY = yMax - this_->m_worldCamera.m_zoomedScreenSize.y;

            Rectf rect;              // shared rect struct for all the sides
            CL_Vec2f unk4(0.0, 0.0); // bgfx related, mandatory

            // Left side of world
            if (this_->m_worldCamera.m_position.x < 0.0)
            {
                // Draw from left (0) to right (clamp point) on entire height.
                rect.right = getClampedLeftBorder(this_->m_worldCamera);
                rect.bottom = this_->m_worldCamera.m_screenSize.y;
                rect.ceil();
                real::DrawFilledRect(rect, 0xAA, 0.0f, &unk4);
            }
            // Right side of world
            if (clampX < this_->m_worldCamera.m_position.x)
            {
                // Draw from left (clamp point) to edge of screen on entire height.
                rect.left = getClampedRightBorder(this_->m_worldCamera, clampX);
                rect.right = this_->m_worldCamera.m_screenSize.x;
                rect.top = 0;
                rect.bottom = this_->m_worldCamera.m_screenSize.y;
                rect.ceil();
                real::DrawFilledRect(rect, 0xAA, 0.0f, &unk4);
            }
            // Top side of world
            if (this_->m_worldCamera.m_position.y < 0.0)
            {
                rect.right = rect.left = 0;
                // Exclude world sides on clamp points so we don't overlap.
                if (this_->m_worldCamera.m_position.x < 0.0)
                    rect.left = getClampedLeftBorder(this_->m_worldCamera);
                if (clampX < this_->m_worldCamera.m_position.x)
                    rect.right = getClampedRightBorder(this_->m_worldCamera, clampX);
                else
                    rect.right = this_->m_worldCamera.m_screenSize.x;

                // Draw from top (0) to bottom (clamp point) on entire non-overlapping width.
                rect.bottom = getClampedTopBorder(this_->m_worldCamera);
                rect.ceil();
                real::DrawFilledRect(rect, 0xAA, 0.0f, &unk4);
            }
            // Bottom side of world
            if (clampY < this_->m_worldCamera.m_position.y)
            {
                rect.left = rect.right = 0;
                // Exclude world sides on clamp points so we don't overlap.
                if (this_->m_worldCamera.m_position.x < 0.0)
                    rect.left = getClampedLeftBorder(this_->m_worldCamera);
                if (clampX < this_->m_worldCamera.m_position.x)
                    rect.right = getClampedRightBorder(this_->m_worldCamera, clampX);
                else
                    rect.right = this_->m_worldCamera.m_screenSize.x;

                // Draw from top (clamp point) to bottom of screen on entire non-overlapping width.
                rect.top = getClampedBottomBorder(this_->m_worldCamera, clampY);
                rect.bottom = this_->m_worldCamera.m_screenSize.y;
                rect.ceil();
                real::DrawFilledRect(rect, 0xAA, 0.0f, &unk4);
            }
        }
    }

    static CL_Vec2f* __fastcall WorldCameraGetCamWorldPos(WorldCamera* this_, CL_Vec2f* p2)
    {
        CL_Vec2f* res = real::WorldCameraGetCamWorldPos(this_, p2);
        // Lock the background position on out-of-bounds
        if (m_centerCameraOnPlayer)
        {
            // Idiomatic way would be to retrieve TileMap through parent WorldRenderer of
            // WorldCamera, but this will suffice.
            WorldTileMap* pTileMap = real::GetApp()->GetGameLogic()->GetTileMap();
            if (this_->m_position.x <= 0.0 && this_->m_position.x != 0.0)
                res->x = 0.0f;
            if (this_->m_position.y <= 0.0 && this_->m_position.y != 0.0)
                res->y = 0.0;

            // Only clamp right side if we haven't clamped left side. This makes weathers not fall
            // apart when both sides are visible.
            if (res->x != 0.0f)
            {
                float clampX = (float)pTileMap->m_width * 32.0f - this_->m_zoomedScreenSize.x;
                if (clampX < this_->m_position.x)
                    res->x = clampX;
            }
            if (res->y != 0.0f)
            {
                float clampY = (float)pTileMap->m_height * 32.0f - this_->m_zoomedScreenSize.y;
                if (clampY < this_->m_position.y)
                    res->y = clampY;
            }
        }
        return res;
    }

    static void __fastcall WorldCameraOnUpdate(WorldCamera* this_, CL_Vec2f* screenSize,
                                               CL_Vec2f* zoom)
    {
        real::WorldCameraOnUpdate(this_, screenSize, zoom);
        if (!m_centerCameraOnPlayer)
        {
            // Restores the vanilla client clamping logic if we're not using the modification.
            WorldTileMap* pTileMap = real::GetApp()->GetGameLogic()->GetTileMap();
            if (this_->m_position.x <= 0.0 && this_->m_position.x != 0.0)
                this_->m_position.x = 0.0;
            if (this_->m_position.y <= 0.0 && this_->m_position.y != 0.0)
                this_->m_position.y = 0.0;

            float clampX = (float)pTileMap->m_width * 32.0f - this_->m_zoomedScreenSize.x;
            if (clampX < this_->m_position.x)
                this_->m_position.x = clampX;
            float clampY = (float)pTileMap->m_height * 32.0f - this_->m_zoomedScreenSize.y;
            if (clampY < this_->m_position.y)
                this_->m_position.y = clampY;
        }
    }

    // Inputs
    static void __fastcall OnArcadeInput(VariantList* pVL)
    {
        if (!m_hotkeyEnabled)
            return;
        if (pVL->Get(0).GetUINT32() == 600007)
        {
            if (real::GetApp()->GetGameLogic()->IsDialogOpened())
                return;

            Entity* pGUI = real::GetApp()->m_entityRoot->GetEntityByName("GUI");
            if (pGUI->GetEntityByName("OptionsMenu") || pGUI->GetEntityByName("ResolutionMenu") ||
                pGUI->GetEntityByName("OptionsPage"))
                return;
            m_centerCameraOnPlayer = !m_centerCameraOnPlayer;
            real::GetApp()->GetVar("osgt_qol_camera_clamp")->Set(uint32_t(m_centerCameraOnPlayer));
        }
    }
    static void AddCustomKeybinds()
    {
        real::AddKeyBinding(real::GetArcadeComponent(), "chatkey_camera", 67, 600007, 1, 1);
    }

  private:
    static bool m_centerCameraOnPlayer;
    static bool m_hotkeyEnabled;
};
bool AnchorCameraToPlayerPatch::m_centerCameraOnPlayer = false;
bool AnchorCameraToPlayerPatch::m_hotkeyEnabled = false;
REGISTER_USER_GAME_PATCH(AnchorCameraToPlayerPatch, anchor_camera_to_player);