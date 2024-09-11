#include "game/game.hpp"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

void __fastcall hook::SetFPSLimit(void* this_, float fps)
{
    DEVMODE dm;
    dm.dmSize = sizeof(DEVMODE);
    // Retrieve primary display refresh rate or fall back to vanilla 60 fps.
    if (EnumDisplaySettingsA(NULL, ENUM_CURRENT_SETTINGS, &dm))
        game::SetFPSLimit(this_, static_cast<float>(dm.dmDisplayFrequency));
    else
        game::SetFPSLimit(this_, 60.f);
}