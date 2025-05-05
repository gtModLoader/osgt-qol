#pragma once
#include <cstdint>
#include <string>
#include <vector>

// STUB: Expanded when theres any actual need for it.
struct ItemInfo
{
    unsigned int ID;
    short category;
    void* pad;
    std::string name;
    uint8_t pad2[184];
    // 232
    std::string altPath;
    uint8_t pad3[4];
    int animationMS;
    uint8_t pad4[616];
};
class ItemInfoManager
{
  public:
    void* m_pUnk;
    std::vector<ItemInfo> m_items;
    uint8_t m_filler[16];

    ItemInfo* GetItemByIDSafe(int ID)
    {
        if (ID < 0 || ID >= m_items.size())
            return nullptr;
        return &m_items[ID];
    }
};
static_assert(sizeof(ItemInfoManager) == 48, "ItemInfoManager class size mismatch.");
static_assert(sizeof(ItemInfo) == 888, "ItemInfo struct size mismatch.");