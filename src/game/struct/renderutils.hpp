#pragma once

#include <stdint.h>

#define MAKE_RGBA(r, g, b, a)                                                                      \
    (((uint32_t)(r) << 8) + ((uint32_t)(g) << 16) + ((uint32_t)(b) << 24) + ((uint32_t)(a)))
#define GET_BLUE(p) ((p) >> 24)
#define GET_GREEN(p) (((p) & 0x00FF0000) >> 16)
#define GET_RED(p) (((p) & 0x0000FF00) >> 8)
#define GET_ALPHA(p) ((p) & 0x000000FF)

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