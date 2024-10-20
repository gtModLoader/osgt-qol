#pragma once
#include "vec.hpp"

enum eAlignment
{
    ALIGNMENT_UPPER_LEFT,
    ALIGNMENT_CENTER,
    ALIGNMENT_DOWN_CENTER,
    ALIGNMENT_UPPER_RIGHT,
    ALIGNMENT_DOWN_LEFT,
    ALIGNMENT_UPPER_CENTER,
    ALIGNMENT_LEFT_CENTER,
    ALIGNMENT_DOWN_RIGHT,
    ALIGNMENT_RIGHT_CENTER,
};

struct rtRectf
{
    rtRectf();

    rtRectf(float l, float t, float r, float b) : left(l), top(t), right(r), bottom(b){};
    float left, top, right, bottom;
    void Clear() { left = top = right = bottom = 0; }
    float GetHeight() const { return bottom - top; }
    float GetWidth() const { return right - left; }
    void AdjustPosition(float x, float y)
    {
        left += x;
        right += x;
        top += y;
        bottom += y;
    }
    void Scale(eAlignment alignment, CL_Vec2f vScale)
    {
        vScale.x -= 1.0f;
        vScale.y -= 1.0f;

        float changeX = ((GetWidth()) * vScale.x);
        float changeY = ((GetHeight()) * vScale.y);

        switch (alignment)
        {
        case ALIGNMENT_CENTER:
            changeX /= 2;
            changeY /= 2;

            left -= changeX;
            right += changeX;

            top -= changeY;
            bottom += changeY;
            break;

        case ALIGNMENT_UPPER_LEFT:
            right += changeX;
            bottom += changeY;
            break;

        case ALIGNMENT_UPPER_RIGHT:
            left -= changeX;
            bottom += changeY;
            break;

        case ALIGNMENT_DOWN_CENTER:
            left -= changeX / 2;
            right += changeX / 2;
            top -= changeY;
            break;
        case ALIGNMENT_UPPER_CENTER:
            left -= changeX / 2;
            right += changeX / 2;
            bottom += changeY;
            break;

        case ALIGNMENT_DOWN_LEFT:
            right += changeX;
            top -= changeY;
            break;

        case ALIGNMENT_DOWN_RIGHT:
            left -= changeX;
            top -= changeY;
            break;

        case ALIGNMENT_LEFT_CENTER:
            right += changeX;
            top -= changeY / 2;
            bottom += changeY / 2;
            break;

        case ALIGNMENT_RIGHT_CENTER:
            left -= changeX;
            top -= changeY / 2;
            bottom += changeY / 2;
            break;

        default:
            break;
        }
    }

    rtRectf operator+(const rtRectf& r) const
    {
        return rtRectf(left + r.left, top + r.top, right + r.right, bottom + r.bottom);
    }
};
