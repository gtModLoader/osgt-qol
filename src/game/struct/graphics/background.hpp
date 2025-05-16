#pragma once

#include "boost/signals/trackable.hpp"
#include "game/struct/loopingsound.hpp"
#include "game/struct/variant.hpp"
#include "game/struct/vec.hpp"

class Background : public boost::signals::trackable
{
  public:
    Background();
    virtual ~Background();

    virtual void Render(Vec2f& vScreenPos, float graphicDetail);
    virtual void Init(bool);
    virtual void Update();
    virtual void RenderForeground(Vec2f&, float);
    virtual void SetScale(Vec2f&);
    virtual void UpdateSFXVolume(float vol);

    void OnEnterForeground(VariantList*);
    void OnEnterBackground(VariantList*);

    void OnAudioDisabled();
    void OnAudioEnabled();

    // Seems to be used to signify if the weather has snow or other kind of falling particle
    bool m_bParticles = false;

    // Used for the weather change animation/fade
    float m_fadeProgress = 1.0f;

    // These two are primarily used in parallax scrolling
    Vec2f m_screenSize;
    Vec2f m_scale = Vec2f(1.0f, 1.0f);

    Rectf m_renderRect;
    LoopingSound m_loopingSound;
    float m_loopingSoundVol = 0.25f;
};
static_assert(sizeof(Background) == 264, "Background class size mismatch.");