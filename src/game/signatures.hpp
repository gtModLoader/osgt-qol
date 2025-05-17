#pragma once
#include "game/struct/app.hpp"

#define DEFINE_GAME_FUNCTION(name, conv, ret, ...)                                                 \
    using name##_t = ret(conv*)(__VA_ARGS__);                                                      \
    namespace real                                                                                 \
    {                                                                                              \
    extern name##_t name;                                                                          \
    }                                                                                              \
    namespace patched                                                                              \
    {                                                                                              \
    extern name##_t name;                                                                          \
    }

// Declared in options.cpp
DEFINE_GAME_FUNCTION(CreateSlider, __fastcall, EntityComponent*, Entity* pBG, float x, float y,
                     float sizeX, std::string buttonFileName, std::string left, std::string middle,
                     std::string right, std::string, std::string, std::string, std::string);
DEFINE_GAME_FUNCTION(CreateCheckBox, __fastcall, Entity*, Entity* pBG, std::string name,
                     std::string text, float x, float y, bool bChecked, uint32_t fontID,
                     float fontScale, bool unclickable, std::string unk10, std::string unk11,
                     std::string unk12)
DEFINE_GAME_FUNCTION(CreateTextButtonEntity, __fastcall, Entity*, Entity* pParentEnt,
                     std::string name, float x, float y, std::string text, bool bUnderline,
                     int unk7, std::string unk8, bool unk9, std::string unk10, bool unk11,
                     bool unk12)
DEFINE_GAME_FUNCTION(ResizeScrollBounds, __fastcall, void, VariantList* pVList);
DEFINE_GAME_FUNCTION(iPadMapX, __fastcall, float, float);
DEFINE_GAME_FUNCTION(iPadMapY, __fastcall, float, float);
DEFINE_GAME_FUNCTION(iPhoneMapX, __fastcall, float, float);
DEFINE_GAME_FUNCTION(iPhoneMapY, __fastcall, float, float);

// Declared in drawing.cpp
DEFINE_GAME_FUNCTION(GetApp, __fastcall, App*);
DEFINE_GAME_FUNCTION(DrawFilledRect, __fastcall, void, const Rectf& rect, uint32_t rgba, float unk3,
                     Vec2f* unk4);
DEFINE_GAME_FUNCTION(GetScreenRect, __fastcall, void, Rectf&);
DEFINE_GAME_FUNCTION(SurfaceCtor, __thiscall, void*, void*);
DEFINE_GAME_FUNCTION(SurfaceDtor, __thiscall, void, void*);
DEFINE_GAME_FUNCTION(SurfaceLoadFile, __fastcall, bool, void*, std::string, bool);
DEFINE_GAME_FUNCTION(SurfaceBlitScaled, __fastcall, void, void* pSurf, float x, float y,
                     CL_Vec2f& vScale, int alignment, unsigned int rgba, float rotation,
                     void* pRenderBatcher, bool flipX, bool flipY);
DEFINE_GAME_FUNCTION(SurfaceAnimCtor, __thiscall, void*, void*);
DEFINE_GAME_FUNCTION(SurfaceAnimDtor, __thiscall, void, void*);
DEFINE_GAME_FUNCTION(SurfaceAnimBlitScaledAnim, __thiscall, void, void* pAnimSurf, float x, float y,
                     int frameX, int frameY, CL_Vec2f* vScale, int alignment, unsigned int rgba,
                     float rotation, CL_Vec2f* vRotationPt, bool flipX, bool flipY, void* pBatcher,
                     int padding);
DEFINE_GAME_FUNCTION(SetTextEntity, __fastcall, void, Entity*, std::string);
DEFINE_GAME_FUNCTION(CreateTextLabelEntity, __fastcall, Entity*, Entity* pParentEnt,
                     std::string name, float vPosX, float vPosY, std::string);
DEFINE_GAME_FUNCTION(SetupTextEntity, __fastcall, void, Entity*, uint32_t eFontID, float fontScale);
DEFINE_GAME_FUNCTION(GetFontAndScaleToFitThisLinesPerScreenY, __fastcall, void, uint32_t& fontID,
                     float& fontScale, float lines);
DEFINE_GAME_FUNCTION(AddBMPRectAroundEntity, __fastcall, void, Entity* pEnt, uint32_t col1,
                     uint32_t col2, float padding);
DEFINE_GAME_FUNCTION(FadeInEntity, __fastcall, void, Entity* pEnt, bool bRecursive, int timeMS,
                     int delayMS, float fadeTarget, int timing);
DEFINE_GAME_FUNCTION(MainMenuCreate, __fastcall, void, Entity*, bool);

// Declared in weathermanager.cpp
DEFINE_GAME_FUNCTION(WorldRendererForceBackground, __thiscall, void, uint8_t*, int, void*, void*);