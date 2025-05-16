#include "game.hpp"
#include "game/struct/entity.hpp"
#include "game/struct/graphics/background.hpp"
#include "signatures.hpp"
#include "struct/variant.hpp"

REGISTER_GAME_FUNCTION(
    WorldRendererForceBackground,
    "40 55 56 57 48 8B EC 48 83 EC 40 48 C7 45 E0 FE FF FF FF 48 89 5C 24 70 8B F2 48 8B F9 33",
    __thiscall, void, uint8_t*, int, void*, void*);

// Incomplete type
struct WorldRenderer
{
    uint8_t pad[200];
    Background* m_pWeather;
    int m_activeWeather;
};

namespace game
{

WeatherManager& WeatherManager::get()
{
    static WeatherManager instance;
    return instance;
}

void game::WeatherManager::initialize()
{
    auto& game = game::GameHarness::get();

    // Hook.
    game.hookFunctionPatternDirect<WorldRendererForceBackground_t>(
        pattern::WorldRendererForceBackground, WorldRendererForceBackground,
        &real::WorldRendererForceBackground);
}

void game::WeatherManager::registerWeather(std::string prettyName, WeatherCallback pCallback,
                                           int weatherID)
{
    CustomWeather weather;
    weather.mappedID = weatherID;
    weather.callback = pCallback;

    auto& weatherMgr = game::WeatherManager::get();
    weatherMgr.weathers.insert(make_pair(prettyName, weather));
}

void __thiscall game::WeatherManager::WorldRendererForceBackground(uint8_t* this_, int WeatherID,
                                                                   void* unk3, void* unk4)
{
    auto& weatherMgr = game::WeatherManager::get();
    // Have we enumerated custom weathers yet?
    // This runs pretty much instant on a modern PC, but I don't like the solution. Ideally this
    // should run after ItemInfoManager::LoadFromMem. Maybe we need subscribe events for those,
    // since there may be others that want to know that too down the line.
    if (!weatherMgr.mappedWeathers)
    {
        for (auto item : real::GetApp()->GetItemInfoManager()->m_items)
        {
            // We only care about weathers
            if (item.category != 41)
                continue;
            // Registering weather server-side works by setting alt-path to
            // loader/weather/pretty_name.
            if (item.altPath.size() > 0)
            {
                if (item.altPath.rfind("loader/weather/", 0) == 0)
                {
                    std::string prettyName = item.altPath.substr(15);
                    auto pair = weatherMgr.weathers.find(prettyName);
                    if (pair != weatherMgr.weathers.end())
                    {
                        // a match, map it
                        pair->second.mappedID = item.animationMS;
                    }
                }
            }
        }
        weatherMgr.mappedWeathers = true;
    }
    for (auto pair : weatherMgr.weathers)
    {
        if (WeatherID == pair.second.mappedID)
        {
            WorldRenderer* pRender = reinterpret_cast<WorldRenderer*>(this_);
            if (pRender->m_pWeather != 0)
                delete pRender->m_pWeather;
            Background* pNewBG = pair.second.callback();
            pNewBG->Init(true);
            pRender->m_activeWeather = WeatherID;
            pRender->m_pWeather = pNewBG;
            return;
        }
    }
    real::WorldRendererForceBackground(this_, WeatherID, unk3, unk4);
    return;
}

}; // namespace game