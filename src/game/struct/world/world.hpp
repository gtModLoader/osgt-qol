#include <cstdint>

struct Tile
{
    int pad;
    short m_itemID;
    short m_itemBGID;
    short m_tileProperties;
    uint8_t x;
    uint8_t y;
    uint8_t cachedIdx;
    uint8_t pad2;
    short m_tileVisual;
    short m_tileBGVisual;
    uint8_t pad3[22];
    void* m_pTileExtra;
    uint8_t pad4[20];
    float m_lightLevel;
    uint8_t pad5[56];
};
static_assert(sizeof(Tile) == 128, "Tile size mismatch.");

class WorldTileMap
{
  public:
    void* vftable;
    int m_width;
    int m_height;
    void* m_unk;
    Tile* m_tiles;
    void* m_pWorldParent;
};
