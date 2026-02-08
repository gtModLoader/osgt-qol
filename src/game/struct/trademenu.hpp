#pragma once

#include "game/struct/entity.hpp"

// Incomplete type.
class TradeMenu
{
  public:
    virtual ~TradeMenu();
    uint8_t pad1[10];
    Entity* m_pTradeGUI;
};