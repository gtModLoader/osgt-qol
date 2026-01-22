
#include <minhook.h>

#include "game.hpp"
#include "game/struct/component.hpp"
#include "game/struct/components/mapbg.hpp"
#include "game/struct/entityutils.hpp"
#include "patch/patch.hpp"
#include "signatures.hpp"
#include <cstdint>
#include <debugapi.h>

REGISTER_GAME_FUNCTION(GetApp, "44 0F 28 F8 E8 ? ? ? ? 48 8B C8 48 8D", __fastcall, App*);

// DrawFilledRect
REGISTER_GAME_FUNCTION(DrawFilledRect,
                       "48 83 EC 58 F3 41 0F 10 01 48 8D 44 24 40 F3 41 0F 10 49 04 0F", __fastcall,
                       void, const Rectf& rect, uint32_t rgba, float unk3, CL_Vec2f* unk4);

// GetScreenRect
REGISTER_GAME_FUNCTION(GetScreenRect,
                       "66 0F 6E 05 0C 7C 4B 00 33 C0 66 0F 6E 0D FE 7B 4B 00 0F 5B C0 48",
                       __fastcall, void, Rectf&);

// Surface::Surface
REGISTER_GAME_FUNCTION(
    SurfaceCtor,
    "48 89 5C 24 10 48 89 74 24 18 57 48 83 EC 20 48 8D 59 08 33 F6 48 89 5C 24 30 48 8B F9 48",
    __thiscall, void*, void*);

// Surface::~Surface
REGISTER_GAME_FUNCTION(SurfaceDtor,
                       "40 57 48 83 EC 30 48 C7 44 24 20 FE FF FF FF 48 89 5C 24 40 48 8B F9 48 8D "
                       "05 92 61 3C 00 48 89 01 BB",
                       __thiscall, void, void*);

// Surface::Load
REGISTER_GAME_FUNCTION(SurfaceLoadFile,
                       "40 57 48 83 EC 50 48 C7 44 24 30 FE FF FF FF 48 89 5C 24 70 48 8B 05 65 75 "
                       "4B 00 48 33 C4 48 89 44 24 40 48 8B FA",
                       __fastcall, bool, void*, std::string, bool);

// Surface::BlitScaled
REGISTER_GAME_FUNCTION(SurfaceBlitScaled,
                       "48 81 EC 88 00 00 00 F3 41 0F 10 01 0F B6 84 24 D8 00 00 00 F3 41 0F",
                       __fastcall, void, void* pSurf, float x, float y, CL_Vec2f& vScale,
                       int alignment, unsigned int rgba, float rotation, void* pRenderBatcher,
                       bool flipX, bool flipY);

// SurfaceAnim::SurfaceAnim
REGISTER_GAME_FUNCTION(SurfaceAnimCtor,
                       "40 53 48 83 EC 20 48 8B D9 ? ? ? ? ? 48 8D 05 9B 43 3C 00 C7 43 70 01",
                       __thiscall, void*, void*);

// SurfaceAnim::~SurfaceAnim
REGISTER_GAME_FUNCTION(SurfaceAnimDtor,
                       "40 57 48 83 EC 30 48 C7 44 24 20 FE FF FF FF 48 89 5C 24 40 48 89 74 24 48 "
                       "8B F2 48 8B D9 F6 C2 02 ? ? 4C 8D 0D 46 6E F2",
                       __thiscall, void, void*);

// SurfaceAnim::BlitScaledAnim
REGISTER_GAME_FUNCTION(SurfaceAnimBlitScaledAnim,
                       "48 8B C4 48 89 58 18 55 56 41 55 48 8D 68 E1 48 81 EC F0 00 00 00 48 8B 75",
                       __thiscall, void, void* pAnimSurf, float x, float y, int frameX, int frameY,
                       CL_Vec2f* vScale, int alignment, unsigned int rgba, float rotation,
                       CL_Vec2f* vRotationPt, bool flipX, bool flipY, void* pBatcher, int padding);

// BaseApp::Draw
REGISTER_GAME_FUNCTION(BaseAppDraw,
                       "48 8B C4 55 57 41 56 48 8D A8 28 FD FF FF 48 81 EC C0 03 00 00 48 C7 44 24",
                       __thiscall, void, BaseApp*);

// CreateOverlayEntity
REGISTER_GAME_FUNCTION(
    CreateOverlayEntity,
    "40 55 53 56 57 41 56 48 8D 6C 24 D1 48 81 EC C0 00 00 00 48 C7 45 DF FE FF FF FF 0F",
    __fastcall, Entity*, Entity* pParentEnt, const std::string name, const std::string fileName,
    float x, float y);

// CreateTextLabelEntity
REGISTER_GAME_FUNCTION(CreateTextLabelEntity, "48 8B C4 55 57 41 56 48 8D 68 A9 48 81 EC D0 00",
                       __fastcall, Entity*, Entity* pParentEnt, std::string name, float vPosX,
                       float vPosY, std::string);

// SetTextEntity
REGISTER_GAME_FUNCTION(SetTextEntity,
                       "48 8B C4 55 57 41 56 48 8D 68 A1 48 81 EC B0 00 00 00 48 C7 45 B7 FE FF FF "
                       "FF 48 89 58 18 48 89 70 20 48",
                       __fastcall, void, Entity*, std::string);

// GetFontAndScaleToFitThisLinesPerScreenY
REGISTER_GAME_FUNCTION(GetFontAndScaleToFitThisLinesPerScreenY,
                       "48 89 5C 24 08 57 48 83 EC 50 0F 29 74 24 40 48 8B DA", __fastcall, void,
                       uint32_t& fontID, float& fontScale, float lines);

// SetupTextEntity
REGISTER_GAME_FUNCTION(SetupTextEntity,
                       "48 8B C4 55 57 41 54 41 56 41 57 48 8D 68 A1 48 81 EC E0 00 00 00 48 C7 44",
                       __fastcall, void, Entity*, uint32_t eFontID, float fontScale);

// EntitySetScaleBySize(longlong param_1,float *param_2,char param_3,char param_4)
REGISTER_GAME_FUNCTION(EntitySetScaleBySize,
                       "48 8B C4 55 56 57 41 56 41 57 48 81 EC 80 00 00 00 48 C7 40 80", __fastcall,
                       void, Entity*, CL_Vec2f&, bool, bool);

// AddBMPRectAroundEntity
REGISTER_GAME_FUNCTION(
    AddBMPRectAroundEntity,
    "48 8B C4 55 56 57 41 56 41 57 48 8D 68 98 48 81 EC 40 01 00 00 48 C7 44 24 68 FE FF FF FF",
    __fastcall, void, Entity* pEnt, uint32_t col1, uint32_t col2, float padding, bool bUnk,
    float fontScale, uint32_t fontID, bool bUnk4);

// FadeInEntity
REGISTER_GAME_FUNCTION(FadeInEntity,
                       "40 55 53 56 57 48 8D 6C 24 D8 48 81 EC 28 01 00 00 48 C7 44 24 60",
                       __fastcall, void, Entity* pEnt, bool bRecursive, int timeMS, int delayMS,
                       float fadeTarget, int timing);

REGISTER_GAME_FUNCTION(SetupEntityIconFromItem,
                       "48 8B C4 55 57 41 56 48 8D 68 A9 48 81 EC A0 00 00 00 48 C7 45 0F FE FF FF "
                       "FF 48 89 58 18 48 89 70 20 48 8B ? ? ? ? ? 48 33 C4 48 89 45 37",
                       __fastcall, Entity*, int* ItemID, Entity* ParentEntity, CL_Vec2f* Position,
                       int, bool bDrawBorder);
REGISTER_GAME_FUNCTION(SendPacket,
                       "4D 85 C0 74 78 48 89 5C 24 10 48 89 6C 24 18 57 48 83 EC 20 48 8B DA 48 89 "
                       "74 24 30 48 8B 52 10 8B E9 33 C9 49 8B F8 48 83 C2 05 44 8D 41 01 E8",
                       __fastcall, void, int, std::string, void*);
namespace game
{
void GameHarness::resolveSharedSigs()
{
    real::GetApp =
        utils::resolveRelativeCall<GetApp_t>(findMemoryPattern<uint8_t*>(pattern::GetApp) + 4);
    real::GetScreenRect = findMemoryPattern<GetScreenRect_t>(pattern::GetScreenRect);
    real::SetupTextEntity = findMemoryPattern<SetupTextEntity_t>(pattern::SetupTextEntity);
    real::CreateOverlayEntity =
        findMemoryPattern<CreateOverlayEntity_t>(pattern::CreateOverlayEntity);
    real::CreateTextLabelEntity =
        findMemoryPattern<CreateTextLabelEntity_t>(pattern::CreateTextLabelEntity);
    real::EntitySetScaleBySize =
        findMemoryPattern<EntitySetScaleBySize_t>(pattern::EntitySetScaleBySize);
    real::GetFontAndScaleToFitThisLinesPerScreenY =
        findMemoryPattern<GetFontAndScaleToFitThisLinesPerScreenY_t>(
            pattern::GetFontAndScaleToFitThisLinesPerScreenY);
    real::DrawFilledRect = findMemoryPattern<DrawFilledRect_t>(pattern::DrawFilledRect);
    real::SurfaceCtor = findMemoryPattern<SurfaceCtor_t>(pattern::SurfaceCtor);
    real::SurfaceDtor = findMemoryPattern<SurfaceDtor_t>(pattern::SurfaceDtor);
    real::SurfaceLoadFile = findMemoryPattern<SurfaceLoadFile_t>(pattern::SurfaceLoadFile);
    real::SurfaceAnimBlitScaledAnim =
        findMemoryPattern<SurfaceAnimBlitScaledAnim_t>(pattern::SurfaceAnimBlitScaledAnim);
    real::SurfaceBlitScaled = findMemoryPattern<SurfaceBlitScaled_t>(pattern::SurfaceBlitScaled);

    real::SurfaceAnimCtor = findMemoryPattern<SurfaceAnimCtor_t>(pattern::SurfaceAnimCtor);
    real::SurfaceAnimDtor = findMemoryPattern<SurfaceAnimDtor_t>(pattern::SurfaceAnimDtor);

    real::SetTextEntity = findMemoryPattern<SetTextEntity_t>(pattern::SetTextEntity);
    real::AddBMPRectAroundEntity =
        findMemoryPattern<AddBMPRectAroundEntity_t>(pattern::AddBMPRectAroundEntity);
    real::FadeInEntity = findMemoryPattern<FadeInEntity_t>(pattern::FadeInEntity);
    real::SetupEntityIconFromItem =
        findMemoryPattern<SetupEntityIconFromItem_t>(pattern::SetupEntityIconFromItem);
    real::SendPacket = findMemoryPattern<SendPacket_t>(pattern::SendPacket);
}

// /!\ Out of Order!
// This was used when osgt-qol spun the setup of mod into its own thread.
// This would be ideally still used right now, but we need to figure out a way to draw while App
// isn't fully initialized yet.
static uint8_t loadScreenState = 0;
void GameHarness::toggleLoadScreen()
{
    // FIXME: Resolution changes from save reload will make loading screen offset and not scale.
    if (loadScreenState == 0)
    {
        // Create our loading screen, we'll opt to use an overlay entity with same style as other
        // game menus.
        loadScreenState = 1;
        Entity* pLoadScreen = real::GetApp()->m_entityRoot->AddEntity(new Entity("LoadScreenMenu"));
        Entity* pOverEnt = real::CreateOverlayEntity(pLoadScreen, "LoadScreen",
                                                     "interface/large/generic_menu.rttex", 0, 0);
        // We will size it to match our screen.
        Rectf screenRect;
        real::GetScreenRect(screenRect);
        CL_Vec2f vScreenSize(screenRect.right, screenRect.bottom);
        real::EntitySetScaleBySize(pOverEnt, vScreenSize, 0, 0);

        // We inform the user about the patching.
        // bug: RTFont may not be properly loaded at times.. text ends up shifting or there will be
        // access violation on render thread.
        Entity* pTextLabel = real::CreateTextLabelEntity(
            pOverEnt, "loadTxt", screenRect.right / 2.0f, screenRect.bottom / 2.0f,
            "`$Please wait!`` Game is currently being patched.");
        // Center-upper align the text.
        pTextLabel->GetVar("alignment")->Set(5U);
        SetTextShadowColor(pTextLabel, 150);

        // Scale text
        uint32_t fontID;
        float fontScale;
        real::GetFontAndScaleToFitThisLinesPerScreenY(fontID, fontScale, 20);
        real::SetupTextEntity(pTextLabel, fontID, fontScale);
    }
    else if (loadScreenState == 1)
    {
        loadScreenState = 2;
        // Delete our loadscreen.
        // bug: may be running OnUpdate on main thread when we do this sometimes, so it causes an
        // access violation.
        real::GetApp()->m_entityRoot->RemoveEntityByAddress(
            real::GetApp()->m_entityRoot->GetEntityByName("LoadScreenMenu"));
    }
    else
    {
        printf("toggleLoadScreen has been called after it's lived its purpose!?\n");
    }
}
}; // namespace game