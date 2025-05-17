#pragma once
#include "background.hpp"
#include "surface.hpp"

class Background_Blank : public Background
{
  public:
    Background_Blank();
    virtual ~Background_Blank();

    virtual void Render(Vec2f& screenSize, float graphicDetail);
};
