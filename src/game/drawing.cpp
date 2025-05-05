#include "MinHook.h"
#include "game.hpp"
#include "game/struct/component.hpp"
#include "game/struct/components/mapbg.hpp"
#include "patch/patch.hpp"
#include "signatures.hpp"
#include <cstdint>
#include <debugapi.h>

REGISTER_GAME_FUNCTION(GetApp, "44 0F 28 F8 E8 ? ? ? ? 48 8B C8 48 8D", __fastcall, App*);

// DrawFilledRect
REGISTER_GAME_FUNCTION(DrawFilledRect,
                       "48 83 EC 58 F3 41 0F 10 01 48 8D 44 24 40 F3 41 0F 10 49 04 0F", __fastcall,
                       void, const Rectf& rect, uint32_t rgba, float unk3, Vec2f* unk4);

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

// GetAudioManager
REGISTER_GAME_FUNCTION(
    GetAudioManager,
    "F3 44 0F 10 ? ? ? ? ? F3 44 0F 10 ? ? ? ? ? 44 38 B6 D9 02 00 00 0F 84 ? ? ? ? E8 ? ? ? ?",
    __fastcall, void*);

// AudioManagerFMOD::SetMusicVol
REGISTER_GAME_FUNCTION(AudioManagerFMODSetMusicVol,
                       "40 53 48 83 EC 30 48 8B D9 0F 29 74 24 20 48 8B 89 A8 00", __fastcall, void,
                       void* this_, float musicVol);

namespace game
{
void GameHarness::resolveRenderSigs()
{
    real::GetApp =
        utils::resolveRelativeCall<GetApp_t>(findMemoryPattern<uint8_t*>(pattern::GetApp) + 4);
    real::GetAudioManager = utils::resolveRelativeCall<GetAudioManager_t>(
        findMemoryPattern<uint8_t*>(pattern::GetAudioManager) + 31);
    real::AudioManagerFMODSetMusicVol =
        findMemoryPattern<AudioManagerFMODSetMusicVol_t>(pattern::AudioManagerFMODSetMusicVol);
    real::DrawFilledRect = findMemoryPattern<DrawFilledRect_t>(pattern::DrawFilledRect);
    real::GetScreenRect = findMemoryPattern<GetScreenRect_t>(pattern::GetScreenRect);
    real::SurfaceCtor = findMemoryPattern<SurfaceCtor_t>(pattern::SurfaceCtor);
    real::SurfaceDtor = findMemoryPattern<SurfaceDtor_t>(pattern::SurfaceDtor);
    real::SurfaceLoadFile = findMemoryPattern<SurfaceLoadFile_t>(pattern::SurfaceLoadFile);
    real::SurfaceAnimBlitScaledAnim =
        findMemoryPattern<SurfaceAnimBlitScaledAnim_t>(pattern::SurfaceAnimBlitScaledAnim);
    real::SurfaceBlitScaled = findMemoryPattern<SurfaceBlitScaled_t>(pattern::SurfaceBlitScaled);

    real::SurfaceAnimCtor = findMemoryPattern<SurfaceAnimCtor_t>(pattern::SurfaceAnimCtor);
    real::SurfaceAnimDtor = findMemoryPattern<SurfaceAnimDtor_t>(pattern::SurfaceAnimDtor);

    real::CreateOverlayEntity =
        findMemoryPattern<CreateOverlayEntity_t>(pattern::CreateOverlayEntity);
    real::CreateTextLabelEntity =
        findMemoryPattern<CreateTextLabelEntity_t>(pattern::CreateTextLabelEntity);
}

bool bLoadingScreen = false;
void GameHarness::toggleLoadScreen()
{
    if (!bLoadingScreen)
    {
        // We may be a bit too early into game's loading phase, do some safety checks
        // If we disable something too early, it'll cause an early crash.
        Entity* pGUI = real::GetApp()->m_entityRoot->GetEntityByName("GUI");
        while (!pGUI)
        {
            Sleep(10);
            pGUI = real::GetApp()->m_entityRoot->GetEntityByName("GUI");
        }
        // CustomInput component may take a while to show up
        Entity* pMainMenu = pGUI->GetEntityByName("MainMenu");
        while (!pMainMenu)
        {
            Sleep(10);
            pMainMenu = pGUI->GetEntityByName("MainMenu");
        }
        while (!pMainMenu->GetComponentByName("CustomInput"))
        {
            Sleep(10);
        }
        // Disable mainmenu inputs so we don't fall through any inputs.
        // Menu Buttons
        for (const auto& ent : *pMainMenu->GetChildren())
        {
            EntityComponent* m_pButton2D = ent->GetComponentByName("Button2D");
            if (m_pButton2D)
            {
                m_pButton2D->GetShared()->GetVar("disabled")->Set(1U);
            }
        }
        // ESC to quit
        pMainMenu->GetComponentByName("CustomInput")->GetVar("disabled")->Set(1U);

        // Create our loading screen, we'll opt to use an overlay entity with same style as other
        // game menus.
        bLoadingScreen = true;
        real::AudioManagerFMODSetMusicVol(real::GetAudioManager(), 0.0f);
        Entity* pLoadScreen = real::GetApp()->m_entityRoot->AddEntity(new Entity("LoadScreenMenu"));
        Entity* pOverEnt = real::CreateOverlayEntity(pLoadScreen, "LoadScreen",
                                                     "interface/large/generic_menu.rttex", 0, 0);
        // We will size it to our screen without currently really regarding much for scale.
        Rectf screenRect;
        real::GetScreenRect(screenRect);
        pOverEnt->GetVar("size2d")->Set(Vec2f(screenRect.right, screenRect.bottom));
        // We inform the user about the patching.
        Entity* pTextLabel = real::CreateTextLabelEntity(
            pOverEnt, "loadTxt", screenRect.right / 2.0f, screenRect.bottom / 2.0f,
            "`$Please wait!`` Game is currently being patched.");
        // Center-upper align the text.
        pTextLabel->GetVar("alignment")->Set(5U);
    }
    else
    {
        bLoadingScreen = false;
        // Delete our loadscreen.
        real::GetApp()->m_entityRoot->RemoveEntityByAddress(
            real::GetApp()->m_entityRoot->GetEntityByName("LoadScreenMenu"));
        // Reset the fade as well
        Entity* pMainMenu =
            real::GetApp()->m_entityRoot->GetEntityByName("GUI")->GetEntityByName("MainMenu");
        MapBGComponent* pMapBG = (MapBGComponent*)pMainMenu->GetComponentByName("MapBGComponent");
        pMapBG->m_pBackground->m_fadeProgress = 1.0f;
        // Re-enable inputs
        // Menu Buttons
        for (const auto& ent : *pMainMenu->GetChildren())
        {
            EntityComponent* m_pButton2D = ent->GetComponentByName("Button2D");
            if (m_pButton2D)
            {
                m_pButton2D->GetShared()->GetVar("disabled")->Set(0U);
            }
        }
        // ESC to quit
        pMainMenu->GetComponentByName("CustomInput")->GetVar("disabled")->Set(0U);
        // nit: ugly, need better way to figue this out
        float musicVol = real::GetApp()->GetVar("music_vol")->GetFloat();
        std::vector<std::string> patches = patch::PatchManager::get().getAppliedUserPatchList();
        if (std::find(patches.begin(), patches.end(), "start_music_slider_backport") !=
            patches.end())
        {
            musicVol = real::GetApp()->GetVar("start_vol")->GetFloat();
        }
        real::AudioManagerFMODSetMusicVol(real::GetAudioManager(), musicVol);
    }
}
}; // namespace game