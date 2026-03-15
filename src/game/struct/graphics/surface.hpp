#pragma once
#include "game/struct/vec.hpp"
#include <string>

// We try replicate the classes here, but we really are going to just pass it on to game's actual
// ctor.
class SoftSurface;
class Surface
{
  public:
    enum eTextureType
    {
        /// Linear filtering, wrap mode is repeat, uses mipmaps.
        TYPE_DEFAULT,
        /// Linear filtering, wrap mode is clamp to edge, no mipmaps.
        TYPE_GUI,
        /// The texture is owned by somebody else and is responsible for setting the appropriate
        /// parameters.
        TYPE_NOT_OWNER,
        /// Nearest pixel filtering, wrap mode is clamp to edge, no mipmaps.
        TYPE_NO_SMOOTHING
    };

    enum eBlendingMode
    {
        BLENDING_NORMAL,
        BLENDING_ADDITIVE,
        BLENDING_PREMULTIPLIED_ALPHA,
        BLENDING_MULTIPLY,
        BLENDING_DARKEN
    };
    Surface();
    virtual ~Surface();
    //void* vftable;
    void* sig_pad1;
    void* sig_pad2;
    void* sig_pad3;
    // uint8_t trackSignal[24];

    bool LoadFile(std::string fName, bool bAddBasePath = true);

    void BlitScaled(float x, float y, CL_Vec2f& vScale, int alignment = 1, unsigned int rgba = 0xFFFFFFFF,
                    float rotation = 0, void* pRenderBatcher = NULL, bool flipX = false, bool flipY = false);
    void SetBlendingMode(eBlendingMode blendMode) { m_blendingMode = blendMode; }
    int GetWidth() { return m_originalWidth; }
    int GetHeight() { return m_originalHeight; }
    void Kill();

    enum eTextureCreationMethod
    {
        TEXTURE_CREATION_NONE,   // we haven't made a texture yet
        TEXTURE_CREATION_FILE,   // we'll reload automatically
        TEXTURE_CREATION_MEMORY, // we'll lose it, but not restore it
        TEXTURE_CREATION_BLANK   // we'll reinitialize the texture as blank, up to you to redraw it
    };

    unsigned short m_texture = -1; // bgfx::TextureHandle
    int m_texWidth, m_texHeight;
    int m_originalWidth, m_originalHeight;
    bool m_bUsesAlpha;
    eTextureType m_texType;
    eBlendingMode m_blendingMode;
    int m_mipMapCount;
    int m_memUsed;
    std::string m_textureLoaded;
    eTextureCreationMethod m_textureCreationMethod;
    bool m_bSmoothing = true;
    bool m_bAddBasePath;
};

class SurfaceAnim : public Surface
{
  public:
    SurfaceAnim();
    int m_framesX;
    int m_framesY;
    float m_frameWidth;
    float m_frameHeight;

    void BlitScaledAnim(float x, float y, int frameX, int frameY, CL_Vec2f* vScale, int alignment = 1,
                        unsigned int rgba = 0xFFFFFFFF, float rotation = 0,
                        CL_Vec2f vRotationPtScreenCoords = CL_Vec2f(0, 0), bool flipX = false, bool flipY = false,
                        void* pBatcher = NULL, int padding = 0);
    void SetupAnim(int framesX, int framesY);
    int GetFramesX() { return m_framesX; }
    int GetFramesY() { return m_framesY; }
    float GetFrameWidth() { return m_frameWidth; }
    float GetFrameHeight() { return m_frameHeight; }
};

static_assert(sizeof(Surface) == 112, "Surface class size mismatch.");
static_assert(sizeof(SurfaceAnim) == 128, "SurfaceAnim class size mismatch.");