#include "game/game.hpp"

void __fastcall hook::AudioManagerFMODPreload(void* this_, void* unk2, bool bLooping, bool bIsMusic,
                                              bool bAddBasePath, bool bForceStreaming)
{
    // Current assumption is PC client had streaming disabled because of Seth's attempt to utilize
    // extra memory resources in order to save performance and cache audio files. However, in
    // practice, the Growtopia client fails to properly utilize said caching and as such creates an
    // annoying stutter. This forces every call to AudioManagerFMOD::Preload to use file streaming
    // to fix said stutter.
    bForceStreaming = true;
    game::AudioManagerFMODPreload(this_, unk2, bLooping, bIsMusic, bAddBasePath, bForceStreaming);
}