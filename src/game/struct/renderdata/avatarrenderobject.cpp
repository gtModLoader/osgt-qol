#include "game/signatures.hpp"
#include "game/struct/renderdata/avatarrenderobject.hpp"
#include "game/struct/components/gamelogic.hpp"
#include "game/struct/renderutils.hpp"

void GenericAvatarRenderObject::Init()
{
    SurfaceAnim* pSurf = real::GetApp()->GetTileSheetManager()->GetTileSheetByID(m_id, false);
    m_pSurface = pSurf;
    if (!(m_fxFlags & 1))
    {
        ItemInfo* pItemInfo = real::GetApp()->GetItemInfoManager()->GetItemByIDSafe(m_id);
        if (pItemInfo)
        {
            CL_Vec2i animFrame(pItemInfo->m_frameX, pItemInfo->m_frameY);
            m_animFrames.push_back(animFrame);
        }
    }
}

void GenericAvatarRenderObject::Update()
{
    if (!m_pSurface)
        Init();
    if (m_fxFlags & 1)
    {
        // WIP
    }
    if (m_fxFlags & 0x20)
    {
        // WIP
    }
    if (m_fxFlags & 0x400)
    {
        float i = real::GetApp()->GetGameLogic()->m_pWorldRenderer->m_rainbowCycle;
        for (; i >= 360.0; i -= 360.0)
            ;
        int r = 0;
        int g = 0;
        int b = 0;
        HSVToRGB(i, 1.0, 1.0, &r, &g, &b);
        m_tint1 = MAKE_RGBA(r, g, b, 255);
    }
    if (m_fxFlags & 0x800)
    {
        float i = real::GetApp()->GetGameLogic()->m_pWorldRenderer->m_rainbowCycle;
        for (; i >= 360.0; i -= 360.0)
            ;
        int r = 0;
        int g = 0;
        int b = 0;
        HSVToRGB(i, 1.0, 1.0, &r, &g, &b);
        m_tint2 = MAKE_RGBA(r, g, b, 255);
    }
}

void GenericAvatarRenderObject::RenderBackpack(float cx, float cy, int itemID, unsigned int tint, bool faceLeft)
{
    if (m_currentFrame < m_animFrames.size())
    {
        int i = 0;
        int unk = ((m_fxFlags & 0x40000) != 0) + 1;
        do
        {
            if ((m_fxFlags & 0x40000) != 0)
            {
                if (i)
                {
                    if (i == 1)
                        cx = cx + (m_localScale.x * 35.0);
                }
                else
                {
                    cx -= ((SinPulseByMS(1800) * 2.0) + 20.0) * m_localScale.x;
                    cy -= ((SinPulseByMS(1000) * 4.0) + 26.0) * m_localScale.y;
                }
                RenderBackpackBySpecifiedFrame(cx, cy, itemID, tint, faceLeft, m_animFrames[m_currentFrame].x,
                                               m_animFrames[m_currentFrame].y);
            }
            ++i;
        } while (i < unk);
    }
}

void GenericAvatarRenderObject::RenderBackpackBySpecifiedFrame(float cx, float cy, int itemID, unsigned int tint,
                                                               bool faceLeft, int frameX, int frameY)
{
    // STUB
}