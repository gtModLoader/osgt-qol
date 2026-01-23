#include "game/game.hpp"
#include "patch/patch.hpp"
#include "utils/utils.hpp"

#include "game/signatures.hpp"
#include "game/struct/videomode.hpp"

REGISTER_GAME_FUNCTION(GetSavePath,
                       "48 8B C4 55 48 8D A8 38 FF FF FF 48 81 EC C0 01 00 00 48 C7 44 24 38 FE FF "
                       "FF FF 48 89 58 10 48 89 70 18 48 89 78 20",
                       __fastcall, std::string);

REGISTER_GAME_FUNCTION(
    RemoveFile,
    "4C 8B DC 57 48 81 EC 80 00 00 00 49 C7 43 98 FE FF FF FF 49 89 5B 10 48 8B ? ? ? ? ? 48 33 C4",
    __fastcall, void, std::string);

REGISTER_GAME_FUNCTION(InitVideo,
                       "48 89 5C 24 08 48 89 74 24 10 57 48 81 EC C0 00 00 00 48 8B ? ? ? ? ? 48 "
                       "33 C4 48 89 84 24 B0 00 00 00 E8",
                       __fastcall, bool);

REGISTER_GAME_FUNCTION(GetVideoModeManager,
                       "48 83 EC 38 48 C7 44 24 20 FE FF FF FF 48 8B ? ? ? ? ? 48 85 C0 75 31 8D "
                       "48 20 E8 ? ? ? ? 48 89 44 24 40",
                       __fastcall, VideoModeManager*);

REGISTER_GAME_FUNCTION(VideoModeManagerSetVideoMode,
                       "40 53 48 83 EC 60 48 C7 44 24 30 FE FF FF FF 48 8B ? ? ? ? ? 48 33 C4 48 "
                       "89 44 24 58 48 8B D9 48 C7 44 24 50 0F 00 00 00",
                       __fastcall, void, VideoModeManager*);

REGISTER_GAME_FUNCTION(
    GamepadConnectToArcadeComponent,
    "40 55 56 57 41 54 41 55 41 56 41 57 48 8D AC 24 60 FF FF FF 48 81 EC A0 01 00 00 48 C7 44 24 "
    "50 FE FF FF FF 48 89 9C 24 F0 01 00 00 48 8B ? ? ? ? ? 48 33 C4 48 89 85 90 00 00 00 45 0F B6",
    __fastcall, void, void*, EntityComponent* pComp, bool, bool);

class SaveAndLogLocationFixer : public patch::BasePatch
{
  public:
    void apply() const override
    {
        auto& game = game::GameHarness::get();

        // Check if we have environment value asking us to not do this.
        // This would primarily be set by launchers that can do this operation cleaner.
        TCHAR lpBuffer[64];
        DWORD dwRet;

        dwRet = GetEnvironmentVariable("LAUNCHER_CHDIR", lpBuffer, 64);

        if (dwRet != 0 && dwRet < 64)
        {
            if (std::string(lpBuffer) == "1")
                return;
        }

        // We need to reset videomode after save.dat reload.
        real::GetVideoModeManager =
            game.findMemoryPattern<GetVideoModeManager_t>(pattern::GetVideoModeManager);
        real::VideoModeManagerSetVideoMode = game.findMemoryPattern<VideoModeManagerSetVideoMode_t>(
            pattern::VideoModeManagerSetVideoMode);

        // We also need to rearm gamepad stuff when InitVideo is called.
        real::GamepadConnectToArcadeComponent =
            game.findMemoryPattern<GamepadConnectToArcadeComponent_t>(
                pattern::GamepadConnectToArcadeComponent);

        // We will replace normal AppData path with Current Directory.
        game.hookFunctionPatternDirect<GetSavePath_t>(pattern::GetSavePath, GetSavePath,
                                                      &real::GetSavePath);
        // Also hook InitVideo so we can refresh handle and title
        game.hookFunctionPatternDirect<InitVideo_t>(pattern::InitVideo, InitVideo,
                                                    &real::InitVideo);

        // We'll need to truncate the logfile like the client normally would. Our log location fix
        // happens after this call, so we'll need to call it again ourselves instead.
        real::RemoveFile = game.findMemoryPattern<RemoveFile_t>(pattern::RemoveFile);
        real::RemoveFile("log.txt");

        // Since we control the save path, we can reload save.dat from our new location.
        real::GetApp()->LoadVarDB();

        // Also set sfx/music vols since they don't get assigned to AudioManager
        real::GetAudioManager()->SetMusicVol(real::GetApp()->m_sharedDB.GetVarWithDefault("music_vol", 1.0f)->GetFloat());
        real::GetAudioManager()->m_defaultVol = real::GetApp()->m_sharedDB.GetVarWithDefault("sfx_vol", 1.0f)->GetFloat();

        // Restore proper videomode for this save.dat
        Variant* pVariant = real::GetApp()->GetVar("savedVideoMode");
        if (pVariant->GetType() == Variant::TYPE_STRING)
        {
            VideoModeManager* vidMgr = real::GetVideoModeManager();
            auto it = vidMgr->m_videoModes.find(pVariant->GetString());
            if (it != vidMgr->m_videoModes.end())
            {
                printf("Found vidmode, assigning %s..\n", it->second->m_name.c_str());
                vidMgr->m_pActiveVidMode = it->second;
                vidMgr->m_screenW = it->second->m_screenW;
                vidMgr->m_screenH = it->second->m_screenH;
            }
            else
            {
                printf("Couldn't find vidmode, may be a new save, ignoring...\n");
            }
        }

        InitVideo();
    }

    static std::string __fastcall GetSavePath()
    {
        TCHAR lpBuffer[MAX_PATH];
        DWORD dwRet;
        dwRet = GetCurrentDirectory(MAX_PATH, lpBuffer);
        if (dwRet == 0)
        {
            return real::GetSavePath();
        }
        return std::string(lpBuffer) + "\\";
    }

    static bool __fastcall InitVideo()
    {
        bool retVal = real::InitVideo();
        auto& game = game::GameHarness::get();
        game.updateWindowHandle();
        game.setWindowTitle("");
        game.setWindowModdedIcon();

        // When the game makes a new window, it doesn't rebind gamepads, so we'll have to do it
        // ourself here.
        GamepadManager* ptr = real::GetGamepadManager();

        // Maybe we're still initialising from the setup, don't prematurely kill anything yet...
        if (ptr->m_providers.size() == 0)
            return retVal;
        ptr->ClearGamepads();
        (*ptr->m_providers.begin())->Kill();
        (*ptr->m_providers.begin())->Init();

        // We also need to reconnect to arcadecomp since that is also gone when we got rid of the
        // gamepads.

        if (real::GetArcadeComponent())
        {
            for (auto it = ptr->m_gamepads.begin(); it != ptr->m_gamepads.end(); it++)
            {
                real::GamepadConnectToArcadeComponent(*it, real::GetArcadeComponent(), 1, 1);
            }
        }
        return retVal;
    }
};
REGISTER_CORE_GAME_PATCH(SaveAndLogLocationFixer, save_and_log_location_fixer);