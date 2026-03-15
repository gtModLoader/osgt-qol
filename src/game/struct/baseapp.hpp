#pragma once
#include "entity.hpp"
#include "gametimer.hpp"
#include "resourcemanager.hpp"
#include "rtfont.hpp"
#include <cstdint>
#include <mutex>
#include <string>
#include <windows.h>

class Console
{
  public:
    Console();
    virtual ~Console(){};
    void SetMaxLines(unsigned int num) { m_maxLines = num; }

    boost::signal<void()> m_sig_on_text_added;

    unsigned int m_maxLines;
    std::deque<std::string> m_log;
    std::mutex m_logLock;
};
static_assert(sizeof(Console) == 176, "Console class size mismatch. ");

// Expected size: 2920 bytes
class BaseApp
{
  public:
    // Virtual functions
    virtual ~BaseApp();
    virtual void Kill();
    virtual void OnPreInitVideo();
    virtual void Draw();
    virtual void Update();
    virtual void OnEnterBackground();
    virtual void OnEnterForeground();
    virtual void OnScreenSizeChange();

    float GetDelta() { return m_gameTimer.m_deltaFloat; }
    int GetDeltaTick() { return m_gameTimer.m_deltaMS; }

    boost::signal<void(VariantList*)> m_sig_input;
    boost::signal<void(VariantList*)> m_sig_input_move;
    boost::signal<void(VariantList*)> m_sig_os;
    boost::signal<void(VariantList*)> m_sig_update;
    boost::signal<void(VariantList*)> m_sig_render;
    boost::signal<void(VariantList*)> m_sig_pre_enterbackground;
    boost::signal<void(VariantList*)> m_sig_enterbackground;
    boost::signal<void(VariantList*)> m_sig_enterforeground;
    boost::signal<void(VariantList*)> m_sig_accel;
    boost::signal<void(VariantList*)> m_sig_trackball;
    boost::signal<void(VariantList*)> m_sig_arcade_input;
    boost::signal<void(VariantList*)> m_sig_raw_keyboard;
    boost::signal<void(VariantList*)> m_sig_hardware;
    boost::signal<void(void)> m_sig_onScreenSizeChanged;
    boost::signal<void(void)> m_sig_unloadSurfaces;
    boost::signal<void(void)> m_sig_loadSurfaces;
    boost::signal<void(VariantList*)> m_sig_joypad_events;
    boost::signal<void(VariantList*)> m_sig_native_input_state_changed;

    bool m_bConsoleVisible;
    bool m_bFPSVisible;
    bool m_bInitted;
    GameTimer m_gameTimer;
    Console m_console;
    RTFont m_fontArray[4];
    uint8_t m_OSMessages[40]; // A deque with <OSMessage>
    bool m_bManualRotation;
    uint8_t padding2[7];
    ResourceManager m_resourceManager;
    int m_inputMode;
    int m_memUsed;
    int m_texMemUsed;
    int m_error;
    bool m_bIsInBackground;
    std::vector<std::string> m_commandLineParms;
    uint8_t m_projectionMatrix[64];
    Entity* m_pEntityRoot;
    bool m_bCheatMode;
    uint8_t padding3[55];
    std::vector<uint8_t> m_touchTracker;
    std::string m_version;
    bool m_bDisableSubPixelBlits;
    uint8_t padding4[7];
    std::string* savedVideoMode;
    unsigned int* isBorderlessWindowModeOn;
    unsigned int* isFullscreenModeOn;
    unsigned int* savedResolutionX;
    unsigned int* savedResolutionY;
    short m_vendorID;
    void* m_pProfileManager;
    bool m_isShuttingDown;
};
static_assert(sizeof(BaseApp) == 2920, "BaseApp class size mismatch.");