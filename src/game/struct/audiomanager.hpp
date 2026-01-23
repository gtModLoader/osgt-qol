#pragma once

#include <string>

// Incomplete vtable
class AudioManager
{
 public:
    AudioManager();
    virtual ~AudioManager();

    std::string m_lastPlayedTrack;
    bool m_bLastMusicLooping;
	bool m_bMusicEnabled;
	uintptr_t m_lastMusicID;
	bool m_bVibrationDisabled;
	std::string m_midiSoundBankFile;
	float m_musicVol;
	// float m_midiVolumeMod;
	bool m_bSoundEnabled;
	bool m_bStreamMusic;
	std::string m_forcedAudioExtension;
	bool m_bPreferOGG;
	float m_defaultVol;
	std::string m_requestedPartialDriverName;
};
