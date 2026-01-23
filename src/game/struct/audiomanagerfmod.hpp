#pragma once

#include "audiomanager.hpp"

// Incomplete, made for accessing vtable.
class AudioManagerFMOD : public AudioManager
{
  public:
    AudioManagerFMOD();
    virtual ~AudioManagerFMOD();
    virtual void Init();
    virtual void Kill();
    virtual void Play(std::string fName, bool bLooping = false, bool bIsMusic = false,
                      bool bAddBasePath = true, bool bForceStreaming = false);
    virtual void vfunction5();
    virtual void vfunction6();
    virtual void Preload(std::string fName, bool bLooping = false, bool bIsMusic = false,
                         bool bAddBasePath = true, bool bForceStreaming = false);
    virtual void vfunction8();
    virtual void vfunction9();
    virtual void vfunction10();
    virtual void vfunction11();
    virtual void vfunction12();
    virtual void KillCachedSounds();
    virtual void vfunction14();
    virtual void vfunction15();
    virtual void vfunction16();
    virtual void vfunction17();
    virtual void vfunction18();
    virtual void vfunction19();
    virtual void vfunction20();
    virtual void vfunction21();
    virtual void vfunction22();
    virtual void SetMusicVol(float vol);
    virtual void vfunction24();
    virtual void vfunction25();
    virtual void Suspend();
    virtual void Resume();
    virtual void vfunction28();
    virtual void vfunction29();
};