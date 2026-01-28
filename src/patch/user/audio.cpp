#include "game/game.hpp"
#include "game/signatures.hpp"
#include "game/struct/variant.hpp"
#include "patch/patch.hpp"
#include "utils/utils.hpp"

// AudioManagerFMOD::Preload
REGISTER_GAME_FUNCTION(AudioManagerFMODPreload,
                       "40 55 56 57 41 54 41 55 41 56 41 57 48 8D AC 24 30 FF FF FF 48 81 EC "
                       "D0 01 00 00 48 C7 44 24 38 FE FF FF FF",
                       __fastcall, void, AudioManagerFMOD*, void*, bool, bool, bool, bool)

// AudioManagerFMOD::Play
REGISTER_GAME_FUNCTION(AudioManagerFMODPlay,
                       "40 55 53 56 57 41 54 41 56 41 57 48 8D 6C 24 E9 48 81 EC A0 00 00 00 48 C7",
                       __fastcall, void*, AudioManagerFMOD* this_, std::string fName, bool bLooping,
                       bool bIsMusic, bool bAddBasePath, bool bForceStreaming)

REGISTER_GAME_FUNCTION(
    OnProgressChangedMusic,
    "40 53 48 83 EC 60 48 C7 44 24 20 FE FF FF FF 0F 29 74 24 50 48 8B ? ? ? ? ? 48 33 C4 48 89 44 "
    "24 48 48 8B D9 83 39 00 75 1E C7 01 01 00 00 00 C7 41 10 00 00 00 00 48 8B 49 40 48 85 C9 74 "
    "08 48 8B D3 E8 ? ? ? ? F3 0F 10 73 10 48 C7 44 24 40 0F 00 00 00 48 C7 44 24 38 00 00 00 00 "
    "C6 44 24 28 00 41 B8 09 00 00 00 48 8D",
    __fastcall, void, Variant*);

REGISTER_GAME_FUNCTION(AudioManagerFMODSetMusicVol,
                       "40 53 48 83 EC 30 48 8B D9 0F 29 74 24 20 48 8B 89 A8 00 00 00", __thiscall,
                       void, AudioManagerFMOD*, float);

REGISTER_GAME_FUNCTION(AudioManagerFMODStopMusic,
                       "48 89 5C 24 10 57 48 83 EC 40 48 8B 19 48 8B F9 FF 93 E0 00 00 00",
                       __thiscall, void, AudioManagerFMOD*);

class AudioStutterPatch : public patch::BasePatch
{
  public:
    void apply() const override
    {
        auto& game = game::GameHarness::get();
        // Hook AudioManagerFMOD::Preload.
        game.hookFunctionPatternDirect(pattern::AudioManagerFMODPreload, AudioManagerFMODPreload,
                                       &real::AudioManagerFMODPreload);
    }

    static void __fastcall AudioManagerFMODPreload(AudioManagerFMOD* this_, void* unk2,
                                                   bool bLooping, bool bIsMusic, bool bAddBasePath,
                                                   bool bForceStreaming)
    {
        // Current assumption is PC client had streaming disabled because of Seth's attempt to
        // utilize extra memory resources in order to save performance and cache audio files.
        // However, in practice, the Growtopia client fails to properly utilize said caching and as
        // such creates an annoying stutter. This forces every call to AudioManagerFMOD::Preload to
        // use file streaming to fix said stutter.
        // nit: This approach can cause audio cutoff for those with the hearing to notice it.
        bForceStreaming = true;
        real::AudioManagerFMODPreload(this_, unk2, bLooping, bIsMusic, bAddBasePath,
                                      bForceStreaming);
    }
};
REGISTER_USER_GAME_PATCH(AudioStutterPatch, audio_stutter_fix);

class StartMusicSliderBackport : public patch::BasePatch
{
  public:
    void apply() const override
    {
        // Your ears will thank you.

        // This patch hooks to AudioManagerFMOD's SetMusicVol and Play functions.
        // In SetMusicVol, we check if we last played theme.ogg to determine which variable to
        // override the call with.
        // In Play, we check if to-be-played track name is theme.ogg for same reason.

        // We store the start volume value in same place the future version does, inside App
        // VariantDB's "start_vol" key.

        // nit / FIXME: Due to AudioManagerFMOD weirdness, the game will still play the title screen
        // theme silently even if the volume is set to near zero.

        auto& game = game::GameHarness::get();

        // We re-use "start_vol" variable from newer client. Modern client users get to keep their
        // preference.
        Variant* pVariant = real::GetApp()->GetVar("start_vol");
        if (pVariant->GetType() == Variant::TYPE_UNUSED)
            pVariant->Set(0.33f);

        auto& optionsMgr = game::OptionsManager::get();
        optionsMgr.addSliderOption("qol", "Audio", "start_vol", "Start Music",
                                   &StartVolumeSliderCallback);

        // Hook.
        game.hookFunctionPatternDirect(pattern::AudioManagerFMODPlay, AudioManagerFMODPlay,
                                       &real::AudioManagerFMODPlay);
        game.hookFunctionPatternDirect(pattern::OnProgressChangedMusic, OnProgressChangedMusic,
                                       &real::OnProgressChangedMusic);
    }

    static float getStartVol()
    {
        // We'll have to set it to 0.01f as FMOD doesn't seem to like audio level 0.00f. In order to
        // not cause glitchiness for option slider, we'll let the variable itself go down to 0.00f.
        float fStartVol = real::GetApp()->GetVar("start_vol")->GetFloat();
        if (fStartVol <= 0.01f)
            fStartVol = 0.01f;
        return fStartVol;
    }

    static void __fastcall OnProgressChangedMusic(Variant* pVariant)
    {
        // We will re-call adjust volume since we want our start vol to be on top here.
        real::OnProgressChangedMusic(pVariant);
        adjustMusicVolume(real::GetAudioManager());
    }

    static void StartVolumeSliderCallback(Variant* pVariant)
    {
        // We'll save the start_vol, our own detour of setmusicvol will calculate adequate volume
        // where needed.
        real::GetApp()->GetVar("start_vol")->Set(pVariant->GetFloat());
        adjustMusicVolume(real::GetAudioManager());
    }

    static void __fastcall adjustMusicVolume(AudioManagerFMOD* this_)
    {
        float volume = real::GetApp()->GetVar("music_vol")->GetFloat();
        if (this_->m_lastPlayedTrack.find("/theme.ogg") != std::string::npos)
        {
            volume *= getStartVol();
            real::GetAudioManager()->SetMusicVol(volume);
            return;
        }
        real::GetAudioManager()->SetMusicVol(volume);
    }

    static void* __fastcall AudioManagerFMODPlay(AudioManagerFMOD* this_, std::string fName,
                                                 bool bLooping, bool bIsMusic, bool bAddBasePath,
                                                 bool bForceStreaming)
    {
        void* ret = real::AudioManagerFMODPlay(this_, fName, bLooping, bIsMusic, bAddBasePath,
                                               bForceStreaming);
        if (bIsMusic)
            adjustMusicVolume(real::GetAudioManager());
        return ret;
    }
};
REGISTER_USER_GAME_PATCH(StartMusicSliderBackport, start_music_slider_backport);

class AudioMuteFix : public patch::BasePatch
{
    void apply() const override
    {
        // Lets tap onto music_vol changes and force AudioManager to shut up about music when it
        // doesn't have to play it.
        real::GetApp()->GetVar("music_vol")->GetSigOnChanged()->connect(onMusicVolChanged);

        if (real::GetApp()->GetVar("music_vol")->GetFloat() <= 0.00f)
            real::GetAudioManager()->SetMusicEnabled(false);

        auto& game = game::GameHarness::get();
        game.hookFunctionPatternDirect(pattern::AudioManagerFMODSetMusicVol,
                                       AudioManagerFMODSetMusicVol,
                                       &real::AudioManagerFMODSetMusicVol);
        game.hookFunctionPatternDirect(pattern::AudioManagerFMODStopMusic,
                                       AudioManagerFMODStopMusic, &real::AudioManagerFMODStopMusic);
    }

    static void onMusicVolChanged(Variant* pVariant)
    {
        if (!real::GetAudioManager()->m_bMusicEnabled && pVariant->GetFloat() > 0.00f)
        {
            // Re-enable. Set music state enabled, set volume BEFORE playing and then play. This
            // will then chain with StartMusicSliderBackport when enabled as well since
            // AudioManagerFMODPlay is hooked by it.
            real::GetAudioManager()->m_bMusicEnabled = true;
            real::GetAudioManager()->SetMusicVol(pVariant->GetFloat());
            if (real::GetAudioManager()->m_lastPlayedTrack != "")
                real::GetAudioManager()->Play(real::GetAudioManager()->m_lastPlayedTrack,
                                              real::GetAudioManager()->m_bLastMusicLooping, true,
                                              false, true);
        }
        else if (real::GetAudioManager()->m_bMusicEnabled && pVariant->GetFloat() <= 0.00f)
        {
            // Silence the music channel and mark that we do not wish to hear any music.
            // AudioManager will still save the filename and looping status.
            real::GetAudioManager()->SetMusicVol(0.00f);
            real::GetAudioManager()->SetMusicEnabled(false);
        }
    }

    static void __thiscall AudioManagerFMODSetMusicVol(AudioManagerFMOD* this_, float vol)
    {
        // Don't allow vol to go to 0.00f when we're supposed to play audio, it makes everything
        // super loud.
        if (vol < 0.01f && this_->m_bMusicEnabled)
            vol = 0.01f;
        real::AudioManagerFMODSetMusicVol(this_, vol);
    }

    static void __thiscall AudioManagerFMODStopMusic(AudioManagerFMOD* this_)
    {
        // Reset last played track on StopMusic - things like world select menu call this.
        real::AudioManagerFMODStopMusic(this_);
        this_->m_lastPlayedTrack = "";
    }
};
REGISTER_USER_GAME_PATCH(AudioMuteFix, audio_mute_fix);
