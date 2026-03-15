#include "surface.hpp"
#include "game/signatures.hpp"

#include <string>

// We pass the original ctor here. Original ctor sets the original vtable as well.
// When our own stuff calls ~Surface(), we pass it through as well to actual dtor.
// We don't really need to do that for SurfaceAnim though.
Surface::Surface() { real::SurfaceCtor(this); }
Surface::~Surface() { real::SurfaceDtor(this); }

SurfaceAnim::SurfaceAnim() { real::SurfaceAnimCtor(this); }

bool Surface::LoadFile(std::string name, bool bAddBasePath) { return real::SurfaceLoadFile(this, name, bAddBasePath); }
void Surface::BlitScaled(float x, float y, CL_Vec2f& vScale, int alignment, unsigned int rgba, float rotation,
                         void* pRenderBatcher, bool flipX, bool flipY)
{
    real::SurfaceBlitScaled(this, x, y, vScale, alignment, rgba, rotation, pRenderBatcher, flipX, flipY);
}
void SurfaceAnim::SetupAnim(int framesX, int framesY)
{
    m_framesX = framesX;
    m_framesY = framesY;

    m_frameWidth = (float)(int(GetWidth() / framesX));
    m_frameHeight = (float)(int(GetHeight() / framesY));
}
void SurfaceAnim::BlitScaledAnim(float x, float y, int frameX, int frameY, CL_Vec2f* vScale, int alignment,
                                 unsigned int rgba, float rotation, CL_Vec2f vRotationPt, bool flipX, bool flipY,
                                 void* pBatcher, int padding)
{
    real::SurfaceAnimBlitScaledAnim(this, x, y, frameX, frameY, vScale, alignment, rgba, rotation, &vRotationPt, flipX,
                                    flipY, pBatcher, padding);
}

void Surface::Kill()
{
    real::SurfaceKill(this);
}