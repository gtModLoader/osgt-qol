#pragma once
#include "entity.hpp"
#include "gametimer.hpp"
#include <cstdint>
#include <string>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// Expected size: 2912 bytes
class BaseApp
{
  public:
    // Virtual functions
    void* dtor;
    void* Kill;
    void* OnPreInitVideo;
    void* Draw;
    void* Update;
    void* OnEnterBackground;
    void* OnEnterForeground;
    void* OnScreenSizeChange;
    void* OnFullscreenToggleRequest;
    void* OnMessage;
    void* OnMemoryWarning;

    uint8_t m_sigs[40 * 16]; // m_sig_xyz boost signals

    bool m_bConsoleVisible;
    bool m_bFPSVisible;
    bool m_bInitted;
    GameTimer m_gameTimer;
    uint8_t m_console[176];
    uint8_t m_fontArray[384 * 4]; // 3 RTFont + destructor
    uint8_t m_OSMessages[40]; // A deque with <OSMessage>
    bool m_bManualRotation;
    uint8_t padding2[7];
    uint8_t m_resourceManager[24];
    int m_inputMode;
    int m_memUsed;
    int m_texMemUsed;
    int m_error;
    bool m_bIsInBackground;
    std::vector<std::string> m_commandLineParms;
    uint8_t m_projectionMatrix[64];
    Entity* m_entityRoot;
    uint8_t padding3[56];
    std::vector<uint8_t> m_touchTracker;
    std::string m_version;
    bool m_bDisableSubPixelBlits;
    uint8_t padding4[63];
};