#pragma once
#include "game/game.hpp"
#include "game/struct/entity.hpp"

// GetApp
REGISTER_GAME_FUNCTION(GetApp, "44 0F 28 F8 E8 ? ? ? ? 48 8B C8 48 8D", __fastcall, void*);

// App::GetVar
REGISTER_GAME_FUNCTION(
    AppGetVar,
    "E8 ? ? ? ? 48 8B D8 48 8B 4C 24 70 48 83 F9 10 72 51 48 FF C1 48 8B 54 24 58 48 81 F9 00 10 "
    "00 00 72 38 F6 C2 1F 74 06 E8 ? ? ? ? CC 48 8B 42 F8 48 3B C2 72 06 E8 ? ? ? ? CC 48 2B D0 48 "
    "83 FA 08 73 06 E8 ? ? ? ? CC 48 83 FA 27 76 06 E8 ? ? ? ? CC 48 8B D0 48 8B CA E8 ? ? ? ? 48 "
    "C7 44 24 70 0F 00 00 00 4C 89",
    __fastcall, Variant*, void* pApp, std::string varName);

// CreateSlider
// NOTE: Need to investigate the final strings. They're not there originally in Proton SDK.
REGISTER_GAME_FUNCTION(
    CreateSlider,
    "48 8B C4 55 53 56 57 41 54 41 55 41 56 41 57 48 8D A8 78 FE FF FF 48 81 EC 48 02 00 00 48 C7 "
    "45 30 FE FF FF FF 0F 29 70 A8 0F 29 78 98 44 0F 29 40 88 44 0F 29 88 78 FF FF FF 44 0F 29 A0",
    __fastcall, EntityComponent*, Entity* pBG, float x, float y, float sizeX,
    std::string buttonFileName, std::string left, std::string middle, std::string right,
    std::string, std::string, std::string, std::string);

// BoostSigFire
// Can be removed when boost signals1 exists
REGISTER_GAME_FUNCTION(BoostSigFire,
                       "48 8B C4 56 57 41 56 48 81 EC 70 01 00 00 48 C7 44 24 40 FE FF FF FF 48 89 "
                       "58 18 48 89 68 20 48 8B DA",
                       __fastcall, void, void* pSig_onChanged, void*);

// GetEntityRoot
REGISTER_GAME_FUNCTION(GetEntityRoot,
                       "E8 ? ? ? ? E8 ? ? ? ? 48 8B C8 33 D2 E8 ? ? ? ? 48 8B 4D F8 48 33 CC E8 ? "
                       "? ? ? 4C 8D 9C 24 80 00 00 00 49 8B 5B 28",
                       __fastcall, Entity*);

// GetFontAndScaleToFitThisLinesPerScreenY
REGISTER_GAME_FUNCTION(GetFontAndScaleToFitThisLinesPerScreenY,
                       "48 89 5C 24 08 57 48 83 EC 50 0F 29 74 24 40 48 8B DA", __fastcall, void,
                       uint32_t& fontID, float& fontScale, float lines);

// CreateTextLabelEntity
REGISTER_GAME_FUNCTION(CreateTextLabelEntity, "48 8B C4 55 57 41 56 48 8D 68 A9 48 81 EC D0 00",
                       __fastcall, Entity*, Entity* pParentEnt, std::string name, float vPosX,
                       float vPosY, std::string);

// SetupTextEntity
REGISTER_GAME_FUNCTION(SetupTextEntity,
                       "48 8B C4 55 57 41 54 41 56 41 57 48 8D 68 A1 48 81 EC E0 00 00 00 48 C7 44",
                       __fastcall, void, Entity*, uint32_t eFontID, float fontScale);

// ResizeScrollBounds
// Params: VariantList with Entity containing "Scroll" and "scroll_child"
REGISTER_GAME_FUNCTION(ResizeScrollBounds,
                       "48 8B C4 55 48 8D 68 A1 48 81 EC D0 00 00 00 48 C7 45 A7 FE FF FF FF 48 89 "
                       "58 10 48 89 70 18 48 89 78",
                       __fastcall, void, VariantList* pVList);

// iPadMapX
REGISTER_GAME_FUNCTION(iPadMapX, "66 0F 6E 0D 9C 76 4B 00 0F 5B C9 F3 0F 59 C8 F3 0F", __fastcall,
                       float, float);

// iPadMapY
REGISTER_GAME_FUNCTION(
    iPadMapY,
    "66 0F 6E 0D 68 76 4B 00 0F 5B C9 F3 0F 59 C8 F3 0F 2C C1 66 0F 6E C0 0F 5B C0 F3 0F 5E",
    __fastcall, float, float);

// iPhoneMapX
REGISTER_GAME_FUNCTION(iPhoneMapX, "66 0F 6E 0D 7C 75 4B 00 0F 5B C9 F3 0F 59 C8 F3 0F 2C C1",
                       __fastcall, float, float);

// iPhoneMapY
REGISTER_GAME_FUNCTION(iPhoneMapY,
                       "66 0F 6E 0D 08 75 4B 00 0F 5B C9 F3 0F 59 C8 F3 0F 2C C1 66 0F 6E C0 0F 5B "
                       "C0 F3 0F 5E 05 AE 28",
                       __fastcall, float, float);
