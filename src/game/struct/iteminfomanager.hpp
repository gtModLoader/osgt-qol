#pragma once
#include <cstdint>
#include <string>
#include <vector>

// STUB: Expanded when theres any actual need for it.
struct ItemInfo
{
    unsigned int ID;
    void* pad;
    std::string name;
    uint8_t pad2[840];
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
