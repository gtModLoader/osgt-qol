#include "game.hpp"

// AudioManagerFMOD::Preload
REGISTER_GAME_FUNCTION(AudioManagerFMODPreload,
                       "40 55 56 57 41 54 41 55 41 56 41 57 48 8D AC 24 30 FF FF FF 48 81 EC "
                       "D0 01 00 00 48 C7 44 24 38 FE FF FF FF",
                       __fastcall, void, void*, void*, bool, bool, bool, bool)

class AudioStutterPatch : public game::BasePatch
{
  public:
    void apply() const override
    {
        auto& game = game::GameHarness::get();
        // Hook AudioManagerFMOD::Preload.
        game.hookFunctionPatternDirect(pattern::AudioManagerFMODPreload, AudioManagerFMODPreload,
                                       &real::AudioManagerFMODPreload);
    }

    static void __fastcall AudioManagerFMODPreload(void* this_, void* unk2, bool bLooping,
                                                   bool bIsMusic, bool bAddBasePath,
                                                   bool bForceStreaming)
    {
        // Current assumption is PC client had streaming disabled because of Seth's attempt to
        // utilize extra memory resources in order to save performance and cache audio files.
        // However, in practice, the Growtopia client fails to properly utilize said caching and as
        // such creates an annoying stutter. This forces every call to AudioManagerFMOD::Preload to
        // use file streaming to fix said stutter.
        bForceStreaming = true;
        real::AudioManagerFMODPreload(this_, unk2, bLooping, bIsMusic, bAddBasePath,
                                      bForceStreaming);
    }
};
REGISTER_GAME_PATCH(AudioStutterPatch, audio_stutter_fix);