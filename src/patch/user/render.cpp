#include "game/game.hpp"
#include "patch/patch.hpp"

#include "game/struct/entity.hpp"
#include "utils/utils.hpp"

// BackgroundNight::BackgroundNight
REGISTER_GAME_FUNCTION(
    BackgroundNight,
    "48 89 4C 24 08 55 56 57 41 54 41 55 41 56 41 57 48 8B EC 48 81 EC 80 00 00 00 48 C7 45 B0 FE "
    "FF FF FF 48 89 9C 24 C8 00 00 00",
    __fastcall, __int64, void*, int);

// BackgroundNight::Init
REGISTER_GAME_FUNCTION(
    BackgroundNightInit,
    "40 55 41 56 48 8B EC 48 83 EC 78 48 8B ? ? ? ? ? 48 33 C4 48 89 45 D8 4C 8B F1 84 D2 74 4A",
    __fastcall, void, __int64, bool);

// BackgroundDefault::~Background_Default
REGISTER_GAME_FUNCTION(BackgroundDefaultDtor,
                       "40 53 48 83 EC 20 48 8D 05 B3 C9 30 00 48 8B D9 48 89 01 48 8B 89 10 01",
                       __fastcall, void, __int64);

// MainMenuCreate
REGISTER_GAME_FUNCTION(
    MainMenuCreate,
    "48 8B C4 55 57 41 54 41 56 41 57 48 8D A8 E8 F8 FF FF 48 81 EC F0 07 00 00 48 C7 85 80 01",
    __fastcall, void, Entity*, bool);

// GetEntityRoot
REGISTER_GAME_FUNCTION(GetEntityRoot,
                       "E8 ? ? ? ? E8 ? ? ? ? 48 8B C8 33 D2 E8 ? ? ? ? 48 8B 4D F8 48 33 CC E8 ? "
                       "? ? ? 4C 8D 9C 24 80 00 00 00 49 8B 5B 28",
                       __fastcall, Entity*);

class WeatherRenderFix : public patch::BasePatch
{
  public:
    void apply() const override
    {
        auto& game = game::GameHarness::get();
        // Avoid setting smoothening on for sunset_water.rttex Surface
        // This makes the dark line less noticable on 3.02 clients.
        // TODO: Bisect client verions to see where it wasn't broken
        auto addr =
            game.findMemoryPattern<uint8_t*>("89 B7 BC 01 00 00 33 D2 48 8D 8F 80 01 00 00 E8");
        utils::nopMemory(addr, 20);
    }
};
REGISTER_USER_GAME_PATCH(WeatherRenderFix, weather_render_fix);

class GoodNightTitleScreen : public patch::BasePatch
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
        auto addr = game.findMemoryPattern<uint8_t*>(pattern::GetEntityRoot);
        real::GetEntityRoot = utils::resolveRelativeCall<GetEntityRoot_t>(addr + 5);

        // Hook
        game.hookFunctionPatternDirect<MainMenuCreate_t>(pattern::MainMenuCreate, MainMenuCreate,
                                                         &real::MainMenuCreate);

        // Manually invoke weather change
        ChangeMainMenuWeather(real::GetEntityRoot()->GetEntityByName("GUI"));
    }

    static void __fastcall MainMenuCreate(Entity* pEnt, bool unk2)
    {
        // Let the game construct main menu for us.
        real::MainMenuCreate(pEnt, unk2);
        // After which we can change the weather without issue.
        ChangeMainMenuWeather(pEnt);
    }

    static void ChangeMainMenuWeather(Entity* pGUIEnt)
    {
        // Lets retrieve MapBGComponent from GUI->MainMenu->MapBGComponent
        uint8_t* pMapBGComponent =
            (uint8_t*)pGUIEnt->GetEntityByName("MainMenu")->GetComponentByName("MapBGComponent");

        // Lets create our buffer for BackgroundNight, the struct size is 0x348
        void* buffer = operator new(0x348);
        // Pass it on to constructor, 2 is our "Active Weather ID". Game uses this to determine
        // if to create comet weather or just normal night weather.
        int64_t pBackgroundNight = real::BackgroundNight(buffer, 2);
        real::BackgroundNightInit(pBackgroundNight, false);

        // Take a note of current weather.
        int64_t pOriginalWeather = *(int64_t*)(pMapBGComponent + 248);
        // Assign our new one in place.
        *(int64_t*)(pMapBGComponent + 248) = pBackgroundNight;
        // Discard the original.
        real::BackgroundDefaultDtor(pOriginalWeather);
    }
};
REGISTER_USER_GAME_PATCH(GoodNightTitleScreen, goodnight_title_screen);
