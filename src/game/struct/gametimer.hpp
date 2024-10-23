#pragma once
#include <deque>

class GameTimer
{
  public:
    void* dtor;
    unsigned int m_lastTimeMS;
    unsigned int m_timeMS;
    unsigned int m_fpsTimer;
    unsigned int m_gameTimer;
    unsigned int m_shadowOffset;
    int m_fps;
    int m_fpsTemp;
    bool m_bGameTimerPaused;
    int m_deltaMS;
    float m_deltaFloat;
    std::deque<float> m_tickHistory;
    unsigned int m_shadowGameTick;
};