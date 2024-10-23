#pragma once
#include "game/struct/component.hpp"

// Expected size: 264 bytes
class MapBGComponent : public EntityComponent
{
  public:
    uint8_t pad[32];
    void* m_pBackground;
    void* m_pUnk1;
};