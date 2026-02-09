#pragma once

// Incomplete type
#include <stdint.h>
struct PlayerItems
{
    void* vftable;
    uint8_t pad[0x28];
    short m_quickSlots[4];
};