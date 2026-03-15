#include "renderutils.hpp"
#include "game/signatures.hpp"
#include <cmath>

float SinPulseByMS(int ms)
{
    int tick = real::GetApp()->m_gameTimer.m_timeMS % ms;
    return (float)(sin((float(tick) / float(ms)) * 3.141592f * 2));
}

float SinPulseByCustomTimerMS(int ms, unsigned int timerMS)
{
    int tick = timerMS % ms;
    return (float)(sin((float(tick) / float(ms)) * 3.141592f * 2));
}

void HSVToRGB(float h, float s, float v, int* r, int* g, int* b)
{
    // Taken and cleaned from client.
    if (s <= 0.0f)
    {
        int c = (int)(v * 255.0f);
        *r = *g = *b = c;
        return;
    }

    if (h >= 360.0f)
        h = 0.0f;

    float hf = h / 60.0f;
    int i = (int)hf;
    float f = hf - i;

    float p = v * (1.0f - s);
    float q = v * (1.0f - s * f);
    float t = v * (1.0f - s * (1.0f - f));

    float rf, gf, bf;

    switch (i)
    {
    default:
    case 0:
        rf = v;
        gf = t;
        bf = p;
        break;
    case 1:
        rf = q;
        gf = v;
        bf = p;
        break;
    case 2:
        rf = p;
        gf = v;
        bf = t;
        break;
    case 3:
        rf = p;
        gf = q;
        bf = v;
        break;
    case 4:
        rf = t;
        gf = p;
        bf = v;
        break;
    case 5:
        rf = v;
        gf = p;
        bf = q;
        break;
    }

    *r = (int)(rf * 255.0f);
    *g = (int)(gf * 255.0f);
    *b = (int)(bf * 255.0f);
}