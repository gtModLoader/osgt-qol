#include "game.hpp"
#include "game/struct/graphics/background.hpp"
#include "signatures.hpp"
#include "struct/world/worldrenderer.hpp"

REGISTER_GAME_FUNCTION(
    WorldRendererForceBackground,
    "40 55 56 57 48 8B EC 48 83 EC 40 48 C7 45 E0 FE FF FF FF 48 89 5C 24 70 8B F2 48 8B F9 33",
    __thiscall, void, uint8_t*, int, void*, void*);

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

    auto& events = game::EventsAPI::get();
    events.m_sig_loadFromMem.connect(&game::WeatherManager::refreshItemDB);
}

void game::WeatherManager::refreshItemDB()
{
    auto& weatherMgr = game::WeatherManager::get();
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
}

void game::WeatherManager::registerWeather(std::string prettyName, WeatherCallback pCallback,
                                           int weatherID)
{
    CustomWeather weather;
    weather.mappedID = weatherID;
    weather.callback = pCallback;

    auto& weatherMgr = game::WeatherManager::get();
    auto ret = weatherMgr.weathers.insert(make_pair(prettyName, weather));
    CustomWeatherEvent* evt = new CustomWeatherEvent;
    evt->m_prettyName = prettyName;
    evt->m_pCustWeather = &ret.first->second;
    (weatherMgr.m_sig_eventSubscribe)(evt);
    delete evt;
}

void __thiscall game::WeatherManager::WorldRendererForceBackground(uint8_t* this_, int WeatherID,
                                                                   void* unk3, void* unk4)
{
    auto& weatherMgr = game::WeatherManager::get();

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