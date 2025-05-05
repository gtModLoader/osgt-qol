#include "background_clouds.hpp"
#include "background.hpp"
#include "game/signatures.hpp"
#include "game/struct/renderutils.hpp"
#include "game/struct/vec.hpp"

#include <string>

// This should ideally move to a different place, but since it's the only consumer right now, it's
// here.
float RandomRangeFloat(float rangeMin, float rangeMax)
{
    float random = ((float)rand()) / (float)RAND_MAX;
    float diff = rangeMax - rangeMin;
    float r = random * diff;
    return rangeMin + r;
}

void Cloud::Init(Background* pParentBG, int moveType, int tintType)
{
    this->distance = RandomRangeFloat(0.1f, 1.0f);
    this->flipX = RandomRangeFloat(0.0f, 1.0f) < 0.5f;

    this->X = RandomRangeFloat(
        0.0f, (pParentBG->m_renderRect.right - pParentBG->m_renderRect.left) / this->distance);
    this->X += pParentBG->m_renderRect.left / this->distance;
    this->Y = RandomRangeFloat(
        0.0f,
        ((pParentBG->m_renderRect.bottom - pParentBG->m_renderRect.top) * 0.5f) / this->distance);
    this->Y += pParentBG->m_renderRect.top / this->distance;

    if (moveType == 0)
        this->xMoveSpeed = RandomRangeFloat(-0.05f, -1.0f);
    else if (moveType == 1)
        this->xMoveSpeed = 0; // I don't know either. Maybe something scrapped.

    int r, g, b = 0;
    if (tintType == 0)
    {
        b = (int)(this->distance * 13.0f) + 242;
        g = (int)(this->distance * 40.0f) + 215;
        r = (int)(this->distance * 159.0f) + 96;
    }
    else if (tintType == 1)
    {
        r = (int)(this->distance * 55.0f) + 200;
        g = (int)(this->distance * 255.0f);
        b = g;
    }
    this->color = MAKE_RGBA(r, g, b, 255);
}

Background_Clouds::Background_Clouds(Background* pParentBG, std::string fileName, int moveType,
                                     int tintType)
{
    m_cloudMoveType = moveType;
    m_cloudTintType = tintType;
    m_cloudCount = 50;
    m_pParentRef = pParentBG;
    m_pClouds = 0;

    m_cloudSurf.SetBlendingMode(Surface::BLENDING_PREMULTIPLIED_ALPHA);
    m_cloudSurf.LoadFile(fileName); // This should use GetSmartFile.
    m_cloudSurf.SetupAnim(1, 1);
}

Background_Clouds::~Background_Clouds()
{
    if (m_pClouds != 0)
    {
        delete[] m_pClouds;
    }
}

void Background_Clouds::InitClouds(int cloudCount)
{
    m_cloudCount = cloudCount;
    if (m_pClouds != 0)
    {
        delete[] m_pClouds;
        m_pClouds = 0;
    }
    m_pClouds = new Cloud[m_cloudCount];
    if (m_cloudCount > 0)
    {
        int i = 0;
        while (i < m_cloudCount)
        {
            m_pClouds[i].Init(m_pParentRef, m_cloudMoveType, m_cloudTintType);
            i++;
        }
    }
}

void Background_Clouds::Update()
{
    if (!m_pClouds || m_cloudCount == 0)
        return;
    int i = 0;
    while (i < m_cloudCount)
    {
        Cloud* pCloud = &m_pClouds[i];
        pCloud->X = real::GetApp()->GetDelta() * pCloud->xMoveSpeed + pCloud->X;
        // Define our boundaries
        // For left side, we have to factor in the cloud surface size and the distance its at.
        float boundLeftX =
            (m_pParentRef->m_renderRect.left -
             m_pParentRef->m_scale.x * m_cloudSurf.m_originalWidth * pCloud->distance) /
            pCloud->distance;
        // For right side, even though we never move cloud there, it may get stuck on too far right
        // end or cloud may initialize too far right.
        float boundRightX =
            ((m_pParentRef->m_renderRect.right - m_pParentRef->m_renderRect.left) * 1.4f +
             m_pParentRef->m_renderRect.left) /
            pCloud->distance;
        if (pCloud->X < boundLeftX || pCloud->X > boundRightX)
        {
            // Reset the cloud and it's X coordinate to right side.
            pCloud->Init(m_pParentRef, m_cloudMoveType, m_cloudTintType);
            pCloud->X =
                ((m_pParentRef->m_renderRect.right - m_pParentRef->m_renderRect.left) * 1.2f +
                 m_pParentRef->m_renderRect.left) /
                pCloud->distance;
        }
        i++;
    }
}

void Background_Clouds::Render(CL_Vec2f& vCamera, float graphicDetail, float cloudRangeMin,
                               float cloudRangeMax)
{
    // We don't really render clouds if we have none or are using low detail.
    if (!m_pClouds || m_cloudCount == 0 || graphicDetail < 0.4)
        return;
    int i = -1;
    while (++i < m_cloudCount)
    {
        Cloud* pCloud = &m_pClouds[i];
        if (cloudRangeMin <= pCloud->distance)
        {
            // Game normally does a return; here because it sorts clouds prior.
            if (cloudRangeMax <= pCloud->distance)
                continue;
            // Scale the cloud relative to their distance.
            CL_Vec2f vScale;
            vScale.x = pCloud->distance * m_pParentRef->m_scale.x;
            vScale.y = pCloud->distance * m_pParentRef->m_scale.y;
            // When the game is on high graphic detail, it'll tint the base colour white and apply
            // appropriate opacity.
            if (0.8 < graphicDetail)
                pCloud->color = -0x100 - (int)(pCloud->distance * -255.0);
            m_cloudSurf.BlitScaledAnim((pCloud->distance * pCloud->X) - vCamera.x,
                                        (pCloud->distance * pCloud->Y) - vCamera.y, 0, 0, &vScale,
                                        0, pCloud->color, 0, 0, pCloud->flipX);
        }
    }
}