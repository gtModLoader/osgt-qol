#pragma once
#include "game/struct/vec.hpp"
#include <list>

struct TextItem
{
    std::string m_text;
    unsigned int m_creationTimeMs;
    unsigned int m_endTimeMS;
    int m_type;
};

struct TextObject
{
    std::list<TextItem> m_items;
    int m_netID;
    int m_tileIndex;
    Rectf m_boundingRect;
    std::string m_finalText;
    float m_fontScale;
    unsigned int m_startTimeOfOldestText;
    unsigned int m_endTimeOfLastText;
    CL_Vec2f m_vWorldPos;
    unsigned int m_color;
    unsigned int m_colorBG;
    bool m_bIsBillboard;
    bool m_bDisablesOnDemand;
};

class SurfaceAnim;
class TextManager
{
  public:
    virtual ~TextManager();
    SurfaceAnim* m_pRectSurf;
    SurfaceAnim* m_pWhiteRectSurf;
    std::list<TextObject> m_texts;
};