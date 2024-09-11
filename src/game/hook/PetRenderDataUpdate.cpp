#include "game/game.hpp"

void __fastcall hook::PetRenderDataUpdate(void* this_, void* unk2, float delta)
{
    // The game relies on using this for pet movement correction during fps fluctuations. However,
    // this logic only works properly until 60 FPS. So for our high-fps mod, we patch this to force
    // game to always run an update on pets movement, causing them to move smoothly and as expected
    // again. There are some slight drawbacks to this approach, but it's relatively stable and good
    // enough for vast majority of players.
    *(float*)((uint8_t*)(this_) + 92) = 0.016666668f;
    game::PetRenderDataUpdate(this_, unk2, delta);
}