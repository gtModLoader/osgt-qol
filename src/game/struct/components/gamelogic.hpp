#pragma once
#include "game/struct/component.hpp"

// Expected size: 880 bytes
class GameLogicComponent : public EntityComponent
{
  public:
    void* GetTileMap() { return (uint8_t*)m_pWorld + 16; }

    uint8_t pad1[88];
    void* m_pWorld;         // World*
    void* m_pWorldRenderer; // WorldRenderer*
    uint8_t pad2[16];
    uint8_t m_netObjManager[48];
    uint8_t pad3[16];
    uint8_t m_textManager[40];
    uint8_t m_playerItems[88];
    uint8_t pad4[24];
    uint8_t m_effectManager[24];

    uint8_t pad5[300];
};