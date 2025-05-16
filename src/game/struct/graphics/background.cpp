#include "background.hpp"
#include "game/signatures.hpp"
#include <string>


#include "boost/bind/bind.hpp"
#include "game/struct/renderutils.hpp"
#include "game/struct/variant.hpp"
#include "game/struct/vec.hpp"


Background::Background()
{
    real::GetApp()->m_sig_enterforeground.connect(
        1, boost::bind(&Background::OnEnterForeground, this, _1));
    real::GetApp()->m_sig_enterbackground.connect(
        1, boost::bind(&Background::OnEnterBackground, this, _1));
}

Background::~Background() {}

void Background::Render(Vec2f& vScreenPos, float graphicDetail)
{
    // Need to find out what this vec is for. Looks like it's related to some bgfx rotation matrix?
    Vec2f unk4(0.0, 0.0);
    // Get our screen size
    Rectf screenRect;
    real::GetScreenRect(screenRect);
    // Draw the default backdrop (same as Background_Default)
    real::DrawFilledRect(screenRect, MAKE_RGBA(96, 215, 242, 255), 0.0f, &unk4);
    // Draw the weather change fade effect.
    if (graphicDetail > 0.1 && m_fadeProgress > 0.0f)
    {
        real::DrawFilledRect(screenRect, -0x100 - (int)(m_fadeProgress * -255.0), 0.0f, &unk4);
        m_fadeProgress = m_fadeProgress - (real::GetApp()->GetDeltaTick() / 1000.0f);
    }
}

void Background::Init(bool bInWorld) { return; }

void Background::Update() { return; }

void Background::RenderForeground(Vec2f&, float) { return; }

void Background::SetScale(Vec2f& vScale)
{
    if (m_bParticles)
    {
        m_scale.x = vScale.x;
        m_scale.y = vScale.y;
    }
}

void Background::UpdateSFXVolume(float vol)
{
    vol = real::GetApp()->GetVar("music_vol")->GetFloat();
    // m_loopingSound.SetVolume(vol);
}

void Background::OnEnterForeground(VariantList* pVList)
{
    // m_loopingSound.SetDisabled(0);
}

void Background::OnEnterBackground(VariantList* pVList)
{
    // m_loopingSound.SetDisabled(1);
}

void Background::OnAudioEnabled()
{
    // m_loopingSound.SetDisabled(0);
}

void Background::OnAudioDisabled()
{
    // m_loopingSound.SetDisabled(1);
}