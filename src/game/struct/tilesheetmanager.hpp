#pragma once
#include "game/struct/graphics/surface.hpp"
#include <vector>

// Namings matched.
class TileSheetManager
{
  public:
    virtual ~TileSheetManager(){};
    std::vector<SurfaceAnim*> m_tileSheets;

    SurfaceAnim* GetTileSheetByID(unsigned short itemID, bool iconVersion)
    {
        int ID = itemID - 1;
        if ((itemID & 1) == 0)
            ID = itemID;
        if (ID > m_tileSheets.size())
            m_tileSheets.resize(ID + 2);
        if (!m_tileSheets[ID])
            LoadTileSheetByID(ID);
        if (!iconVersion)
            return m_tileSheets[ID];
        if (!m_tileSheets[ID + 1])
            LoadTileSheetByIDIcon(ID);
        return m_tileSheets[ID + 1];
    }
    void LoadTileSheetByID(unsigned short);
    void LoadTileSheetByIDIcon(unsigned short);
    void UnloadAll()
    {
        for (auto it = m_tileSheets.begin(); it != m_tileSheets.end(); it++)
        {
            if (*it)
                (*it)->Kill();
        }
        m_tileSheets.clear();
    }
};