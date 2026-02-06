#pragma once

#include "game/struct/graphics/background.hpp"
#include "worldcamera.hpp"
// Incomplete
class WorldRenderer
{
  public:
    WorldRenderer();
    virtual ~WorldRenderer();

    void* m_pWorld;
    uint8_t pad[120];
    WorldCamera m_worldCamera;
    Background* m_pWeather;
    int m_activeWeather;
};