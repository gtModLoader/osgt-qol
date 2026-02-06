#pragma once

#include "game/struct/vec.hpp"

class WorldRenderer;

class WorldCamera
{
  public:
    WorldCamera();
    virtual ~WorldCamera();

    WorldRenderer* m_pWorldRenderer;
    CL_Vec2f m_position;       // Current camera position, what the player actually sees.
    CL_Vec2f m_visualPosition; // NetAvatar's position
    CL_Vec2f m_zoomLevel;
    CL_Vec2f m_screenPosOffset;
    CL_Vec2f m_screenSize;       // Raw screen size
    CL_Vec2f m_zoomedScreenSize; // Screen size with zoom level multiplied ontop
};