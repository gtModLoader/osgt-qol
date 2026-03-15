#pragma once
#include "game/struct/renderdata/avatarrenderdata.hpp"

class Tile;

// Incomplete.
class AvatarRenderer;

// Move this to own header later.
struct CL_Mat3f
{
    float matrix[9];
};

// Namings matched.
class AvatarRenderObject
{
  public:
    AvatarRenderObject();
    // V3.02 has 43 vfunctions for AVROs.
    virtual ~AvatarRenderObject(){};
    virtual void Init() {};
    virtual void Update() {};
    virtual void RenderGUIStuff(NetAvatar* me, CL_Vec2f localScale) {};
    virtual void RenderInBed(float offset_x, float offset_y, unsigned int forceColor) {};
    virtual void RenderPortrait(AvatarRenderData* avData, float offset_x, float offset_y, unsigned int) {};
    virtual void RenderSitting(float offset_x, float offset_y, unsigned int forceColor, int) {};
    virtual void RenderSittingInTheater(float offset_x, float offset_y, unsigned int forceColor, int) {};
    virtual void RenderInRobot(float offset_x, float offset_y, unsigned int forceColor) {};
    virtual void RenderInPineapple(float offset_x, float offset_y, unsigned int forceColor) {};
    virtual void RenderPresentGoblin(float offset_x, float offset_y, unsigned int forceColor) {};
    virtual void RenderPet() {};
    virtual void RenderBackpack(float cx, float cy, int itemID, unsigned int tint, bool faceLeft) {};
    virtual void RenderFrontpack(float cx, float cy, int itemID, unsigned int tint, bool faceLeft) {};
    virtual void RenderBackArm(float cx, float cy, int tint, int otherTint, int itemID, float angle, bool faceLeft) {};
    virtual void RenderBody(float cx, float cy, int skinTint, bool) {};
    virtual void RenderFace(float cx, float cy, int skinTint, int, bool, int) {};
    virtual void RenderShoes(float cx, float cy, int itemID, float horseY, int skinTint, int otherTint, bool faceLeft,
                             bool) {};
    virtual void RenderShoeWings(float cx, float cy, int itemID, float horseY, int skinTint, int otherTint,
                                 bool faceLeft, bool) {};
    virtual void RenderPants(float cx, float cy, int itemID, int otherTint, int forceColor, bool faceLeft, int horsey) {
    };
    virtual void RenderShirt(float cx, float cy, int itemID, int skinTint, int otherTint, bool faceLeft) {};
    virtual void RenderChestItem(float cx, float cy, int itemID, int otherTint, bool faceLeft, bool topHalfOnly,
                                 bool backSide) {};
    virtual void RenderFaceItem(float cx, float cy, int itemID, int otherTint, bool faceLeft) {};
    virtual void RenderHair(float cx, float cy, int itemID, int otherTint, bool faceLeft) {};
    virtual void RenderHat(float cx, float cy, int itemID, int otherTint, bool faceLeft) {};
    virtual void RenderAccoutrements(float cx, float cy, int otherTint, bool faceLeft) {
    }; // No idea what this is, no xrefs.
    virtual void RenderFrontArm(float cx, float cy, float arm2Angle, int itemID, int skinTint, int othertint,
                                bool faceLeft, int forceColor) {};
    virtual void RenderAura(float cx, float cy, unsigned int forceColor) {};
    virtual void RenderInFront(float cx, float cy, unsigned int forceColor) {};
    virtual void OnAvatarRenderDataUpdate()
    {
        if (m_pAvatarRenderer)
        {
            CL_Vec2f offset;
            GetCharacterOffset(&offset);
            offset *= m_pAvatarRenderData->m_scaleAdjust;
            if (offset.length() > 0.0)
                m_pAvatarRenderData->m_offset += offset;
        }
    };
    virtual void OnDeath(AvatarRenderData* pAvatarRenderData) { m_pAvatarRenderData = pAvatarRenderData; };
    virtual void OnKilled(AvatarRenderData* pAvatarRenderData, float cx, float cy) {};
    virtual void OnRespawned(AvatarRenderData* pAvatarRenderData) {};
    virtual void AddClothingParticles(AvatarRenderData* pAvatarRenderData, bool iStink, bool fastStink, bool slowStink,
                                      bool crazyStink) {};
    virtual void SwitchOffClothingParticles(AvatarRenderData* pAvatarRenderData) {};
    virtual void EndClothingParticles(AvatarRenderData* pAvatarRenderData) {};
    virtual CL_Vec2f* GetCharacterOffset(CL_Vec2f* result)
    {
        if (m_pAvatarRenderData)
            *result = *GetCharacterOffsetSpecified(result, m_pAvatarRenderData);
        else
            *result = 0;
        return result;
    }
    virtual CL_Vec2f* GetCharacterOffsetSpecified(CL_Vec2f* result, AvatarRenderData* pData)
    {
        *result = 0;
        return result;
    }
    virtual bool NeedRender() { return 1; };
    virtual bool NeedRenderOverride() { return 1; };
    virtual bool NeedRenderPet() { return 0; };
    virtual bool NeedParticleEmitterPositionOverride() { return 0; }
    virtual void OnClothingModified() {};

    AvatarRenderer* m_pAvatarRenderer;
    AvatarRenderData* m_pAvatarRenderData;
    int m_type;
    int m_id;
    ItemInfo* iteminfo;
    CL_Mat3f m_transformLocal;
    CL_Mat3f m_transformWorld;
    SurfaceAnim* m_pSurface;
    CL_Vec2f m_localScale;
    unsigned int m_tint = -1;
    bool m_bForceColor;
    unsigned int m_flags;
    CL_Vec2f particleEmitterPosition;
    int clothingParticleType;
};
static_assert(sizeof(AvatarRenderObject) == 152, "AvatarRenderObject class size mismatch.");

// Namings matched. Implementation incomplete.
class GenericAvatarRenderObject : public AvatarRenderObject
{
  public:
    // GenericAvatarRenderObject overrides VF #2, #3, #13, #14, #15, #20, #21, #22, #23, #24, #25, #27, #35 and adds
    // #44.
    virtual void Init();
    virtual void Update();
    virtual void RenderBackpack(float cx, float cy, int itemID, unsigned int tint, bool faceLeft);
    virtual void RenderFrontpack(float cx, float cy, int itemID, unsigned int tint, bool faceLeft);
    virtual void RenderBackArm(float cx, float cy, int tint, int otherTint, int itemID, float angle, bool faceLeft);
    virtual void RenderPants(float cx, float cy, int itemID, int otherTint, int forceColor, bool faceLeft, int horsey);
    virtual void RenderShirt(float cx, float cy, int itemID, int skinTint, int otherTint, bool faceLeft);
    virtual void RenderChestItem(float cx, float cy, int itemID, int otherTint, bool faceLeft, bool topHalfOnly,
                                 bool backSide);
    virtual void RenderFaceItem(float cx, float cy, int itemID, int otherTint, bool faceLeft);
    virtual void RenderHair(float cx, float cy, int itemID, int otherTint, bool faceLeft);
    virtual void RenderHat(float cx, float cy, int itemID, int otherTint, bool faceLeft);
    virtual void RenderFrontArm(float cx, float cy, float arm2Angle, int itemID, int skinTint, int othertint,
                                bool faceLeft, int forceColor);
    virtual void SwitchOffClothingParticles(AvatarRenderData* pAvatarRenderData);
    virtual void SetAnimFrames(std::string* frames) {};

    void RenderBackpackBySpecifiedFrame(float cx, float cy, int itemID, unsigned int tint, bool faceLeft, int frameX,
                                        int frameY);

    std::vector<CL_Vec2i> m_animFrames;
    std::vector<CL_Vec2i> m_anim2Frames;
    int m_currentFrame;
    int m_currentFrame2;
    int m_animLength;
    CL_Vec2f m_offset;
    unsigned int m_fxFlags;
    unsigned int m_tint1;
    unsigned int m_tint2;
    int m_animRateMS;
    bool m_bReverseAnim;
};
static_assert(sizeof(GenericAvatarRenderObject) == 240, "GenericAvatarRenderObject class size mismatch.");
