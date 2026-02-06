#pragma once
#include "game/struct/component.hpp"

class WorldTileMap;

// Expected size: 880 bytes
class GameLogicComponent : public EntityComponent
{
  public:
    WorldTileMap* GetTileMap() { return (WorldTileMap*)((uint8_t*)m_pWorld + 16); }

    int GetTileWidth() { return *(int*)(reinterpret_cast<uint8_t*>(GetTileMap()) + 8); }
    int GetTileHeight() { return *(int*)(reinterpret_cast<uint8_t*>(GetTileMap()) + 12); }

    bool IsDialogOpened();

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

    uint8_t pad5[176];
    bool m_activeGuildEvent;
    uint8_t pad6[127];
};
static_assert(sizeof(GameLogicComponent) == 880, "GameLogicComponent class size mismatch.");