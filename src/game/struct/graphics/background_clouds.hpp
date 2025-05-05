#pragma once
#include "surface.hpp"

class Background;

struct Cloud
{
    float X;
    float Y;
    float xMoveSpeed;
    float yMoveSpeed; // Unused.
    float distance;
    bool flipX;
    unsigned int color;

    void Init(Background* pParentBG, int moveType, int tintType);
};

// NOTE: This is not a derivative of Background, but a standalone component.
class Background_Clouds
{
  public:
    Background_Clouds(Background* pParentBG, std::string fileName, int moveType, int tintType);
    virtual ~Background_Clouds();
    virtual void Render(CL_Vec2f&, float, float, float);
    virtual void Update();

    void InitClouds(int cloudCount);

    int m_cloudMoveType;
    int m_cloudTintType;
    SurfaceAnim m_cloudSurf;
    int m_cloudCount = 50;
    Cloud* m_pClouds = nullptr;
    Background* m_pParentRef;
};
