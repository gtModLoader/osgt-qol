#pragma once
#include <string>

// Macro for declaring a game function pointer and its type.
#define DECLARE_GAME_FUNCTION(name, ptrn, conv, ret, ...)                                          \
    using name##_t = ret(conv*)(__VA_ARGS__);                                                      \
    namespace game                                                                                 \
    {                                                                                              \
    inline name##_t name = nullptr;                                                                \
    }                                                                                              \
    namespace pattern                                                                              \
    {                                                                                              \
    inline std::string name = ptrn;                                                                \
    }

// Macro for declaring a game function pointer and its type, as well as its respective hook
// function.
#define DECLARE_GAME_FUNCTION_HOOK(name, ptrn, conv, ret, ...)                                     \
    DECLARE_GAME_FUNCTION(name, ptrn, conv, ret, __VA_ARGS__)                                      \
    namespace hook                                                                                 \
    {                                                                                              \
    ret conv name(__VA_ARGS__);                                                                    \
    }

DECLARE_GAME_FUNCTION_HOOK(SetFPSLimit,
                           "4C 8B DC 48 81 EC D8 00 00 00 48 C7 44 24 20 FE FF FF FF 48 8B ? ? ? ? "
                           "? 48 33 C4 48 89 84 24 C0 00 00 00 0F 57 C0",
                           __fastcall, void, void*, float);

// PetRenderData::Update
DECLARE_GAME_FUNCTION_HOOK(
    PetRenderDataUpdate, "48 8B C4 48 89 58 10 48 89 68 18 56 57 41 56 48 81 EC A0 00 00 00 0F B7",
    __fastcall, void, void*, void*, float);

// AudioManagerFMOD::Preload
DECLARE_GAME_FUNCTION_HOOK(AudioManagerFMODPreload,
                           "40 55 56 57 41 54 41 55 41 56 41 57 48 8D AC 24 30 FF FF FF 48 81 EC "
                           "D0 01 00 00 48 C7 44 24 38 FE FF FF FF",
                           __fastcall, void, void*, void*, bool, bool, bool, bool)
// CreateLogOverlay
DECLARE_GAME_FUNCTION_HOOK(CreateLogOverlay,
                           "48 8B C4 55 41 54 41 55 41 56 41 57 48 8D A8 E8 FC FF FF 48 81 EC F0 "
                           "03 00 00 48 C7 45 A0 FE FF FF FF 48 89 58 08 48",
                           __fastcall, void, float*, float*, bool)
