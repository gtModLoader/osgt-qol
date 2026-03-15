#pragma once

#include <stdint.h>

#define MAKE_RGBA(r, g, b, a) (((uint32_t)(r) << 8) + ((uint32_t)(g) << 16) + ((uint32_t)(b) << 24) + ((uint32_t)(a)))
#define GET_BLUE(p) ((p) >> 24)
#define GET_GREEN(p) (((p) & 0x00FF0000) >> 16)
#define GET_RED(p) (((p) & 0x0000FF00) >> 8)
#define GET_ALPHA(p) ((p) & 0x000000FF)
#define rt_min(rangeMin, rangeMax) (((rangeMin) < (rangeMax)) ? (rangeMin) : (rangeMax))
#define rt_max(rangeMin, rangeMax) (((rangeMin) > (rangeMax)) ? (rangeMin) : (rangeMax))
inline uint32_t ModAlpha(uint32_t c1, float alphaMod)
{
    int r, g, b, a;

    if (alphaMod == 1)
        return c1;
    r = GET_RED(c1);
    g = GET_GREEN(c1);
    b = GET_BLUE(c1);
    a = GET_ALPHA(c1);

    return MAKE_RGBA(r, g, b, uint8_t(float(a) * alphaMod));
}
inline uint32_t ColorCombine(uint32_t c1, uint32_t c2, float alphaMod)
{
    int r, g, b, a;

    if (c2 != MAKE_RGBA(255, 255, 255, 255))
    {
        r = GET_RED(c1) - (255 - GET_RED(c2));
        g = GET_GREEN(c1) - (255 - GET_GREEN(c2));
        b = GET_BLUE(c1) - (255 - GET_BLUE(c2));
        a = GET_ALPHA(c1) - (255 - GET_ALPHA(c2));

        r = rt_max(0, r);
        r = rt_min(255, r);
        g = rt_max(0, g);
        g = rt_min(255, g);
        b = rt_max(0, b);
        b = rt_min(255, b);
        a = rt_max(0, a);
        a = rt_min(255, a);
    }
    else
    {
        if (alphaMod == 1)
            return c1;
        r = GET_RED(c1);
        g = GET_GREEN(c1);
        b = GET_BLUE(c1);
        a = GET_ALPHA(c1);
    }

    return MAKE_RGBA(r, g, b, uint8_t(float(a) * alphaMod));
}
float SinPulseByMS(int ms);
float SinPulseByCustomTimerMS(int ms, unsigned int timerMS);
void HSVToRGB(float h, float s, float v, int* r, int* g, int* b);