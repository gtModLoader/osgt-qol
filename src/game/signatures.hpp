#pragma once
#include "game/struct/app.hpp"
#include "game/struct/audiomanagerfmod.hpp"
#include "game/struct/gamepadmanager.hpp"

#define DEFINE_GAME_FUNCTION(name, conv, ret, ...)                                                 \
    using name##_t = ret(conv*)(__VA_ARGS__);                                                      \
    namespace real                                                                                 \
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
DEFINE_GAME_FUNCTION(SlideScreen, __fastcall, void, Entity*, bool, int, int);
DEFINE_GAME_FUNCTION(TouchHandlerComponent, __fastcall, EntityComponent*, void*);
DEFINE_GAME_FUNCTION(TouchHandlerArcadeComponent, __fastcall, EntityComponent*, void*);
DEFINE_GAME_FUNCTION(MessageManagerCallEntityFunction, __fastcall, void, void*, Entity*, int,
                     std::string, VariantList*, int);
DEFINE_GAME_FUNCTION(GetMessageManager, __fastcall, void*);
DEFINE_GAME_FUNCTION(SetScrollProgressEntity, __fastcall, void, Entity*, CL_Vec2f*);
// Declared in drawing.cpp
DEFINE_GAME_FUNCTION(GetApp, __fastcall, App*);
DEFINE_GAME_FUNCTION(DrawFilledRect, __fastcall, void, const Rectf& rect, uint32_t rgba, float unk3,
                     CL_Vec2f* unk4);
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
                     uint32_t col2, float padding, bool bUnk, float fontScale, uint32_t fontID,
                     bool bUnk4);
DEFINE_GAME_FUNCTION(FadeInEntity, __fastcall, void, Entity* pEnt, bool bRecursive, int timeMS,
                     int delayMS, float fadeTarget, int timing);
DEFINE_GAME_FUNCTION(MainMenuCreate, __fastcall, void, Entity*, bool);
DEFINE_GAME_FUNCTION(SetupEntityIconFromItem, __fastcall, Entity*, int* ItemID,
                     Entity* ParentEntity, CL_Vec2f* Position, int, bool bDrawBorder);
DEFINE_GAME_FUNCTION(CreateOverlayEntity, __fastcall, Entity*, Entity* pParentEnt,
                     const std::string name, const std::string fileName, float x, float y);
DEFINE_GAME_FUNCTION(EntitySetScaleBySize, __fastcall, void, Entity*, CL_Vec2f&, bool, bool);
DEFINE_GAME_FUNCTION(SendPacket, __fastcall, void, int, std::string, void*);
DEFINE_GAME_FUNCTION(GetAudioManager, __fastcall, AudioManagerFMOD*);
DEFINE_GAME_FUNCTION(GetDevicePixelsPerInchDiagonal, __fastcall, int);
// Declared in events.cpp
DEFINE_GAME_FUNCTION(GetArcadeComponent, __fastcall, EntityComponent*);
DEFINE_GAME_FUNCTION(AddKeyBinding, __fastcall, void, EntityComponent* pComp, std::string name,
                     uint32_t inputcode, uint32_t outputcode, bool bAlsoSendAsNormalRawKey,
                     uint32_t modifiersRequired);
DEFINE_GAME_FUNCTION(GetGamepadManager, __fastcall, GamepadManager*);

// Declared in weathermanager.cpp
DEFINE_GAME_FUNCTION(WorldRendererForceBackground, __thiscall, void, uint8_t*, int, void*, void*);