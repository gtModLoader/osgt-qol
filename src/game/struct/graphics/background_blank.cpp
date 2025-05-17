#include "background_blank.hpp"
#include "game/signatures.hpp"
#include "game/struct/renderutils.hpp"

Background_Blank::Background_Blank() {}

Background_Blank::~Background_Blank() {}

void Background_Blank::Render(Vec2f& vScreenPos, float graphicDetail)
{
    // Need to find out what this vec is for. Looks like it's related to some bgfx rotation matrix?
    Vec2f unk4(0.0, 0.0);
    // Get our screen size
    Rectf screenRect;
    real::GetScreenRect(screenRect);
    // Draw the default backdrop (same as Background_Default)
    // 0x141400ff
    // MAKE_RGBA(96, 215, 242, 255)
    real::DrawFilledRect(screenRect, 0x141400ff, 0.0f, &unk4);
    // Draw the weather change fade effect.
    if (m_fadeProgress > 0.0f)
    {
        real::DrawFilledRect(screenRect, -0x100 - (int)(m_fadeProgress * -255.0), 0.0f, &unk4);
        m_fadeProgress = m_fadeProgress - (real::GetApp()->GetDeltaTick() / 1000.0f);
    }
}