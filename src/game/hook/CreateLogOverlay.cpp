#include "game/game.hpp"

void __fastcall hook::CreateLogOverlay(float* pos2d, float* size2d, bool unk3)
{
    // Readjust the chat overlay size by removing the left-padding and placing it to size2d instead
    size2d[0] += pos2d[0];
    size2d[1] += pos2d[1];
    pos2d[0] = 0.0f;
    pos2d[1] = 0.0f;
    game::CreateLogOverlay(pos2d, size2d, unk3);
}