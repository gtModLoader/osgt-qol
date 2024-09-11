#include "game/game.hpp"

__int64 __fastcall hook::TabComponentAddTabButton(void* this_, void* entity, float* xCoord,
                                                  void* unk4, void* unk5, void* unk6)
{
    // Tabs pass on xCoord parameter to continiously align themselves after one another.
    // We can force this to always be off-screen, essentially removing them from visible
    // game area.
    *xCoord = -500.0f;
    return game::TabComponentAddTabButton(this_, entity, xCoord, unk4, unk5, unk6);
}