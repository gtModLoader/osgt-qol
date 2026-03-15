#include "game/struct/tilesheetmanager.hpp"
#include "game/signatures.hpp"
#include "game/struct/iteminfomanager.hpp"
#include "miscutils.hpp"

void TileSheetManager::LoadTileSheetByID(unsigned short itemID)
{
    ItemInfo* pItemInfo = real::GetApp()->GetItemInfoManager()->GetItemByIDSafe(itemID);
    if (!pItemInfo)
    {
        printf("Error, can't locate tile %d data\n", itemID);
        return;
    }
    if (!pItemInfo->m_fileName.size())
    {
        printf("Error, tile(%d) haven't texture file", itemID);
        return;
    }
    std::string textureName = "game/" + pItemInfo->m_fileName;
    std::string fName;
    real::AppGetSmartFileName(real::GetApp(), fName, textureName);
    SurfaceAnim* pSurf = real::GetApp()->m_resourceManager.GetSurfaceAnim(fName, false);
    m_tileSheets[itemID] = pSurf;
    if (!pSurf)
    {
        real::LogToConsole("Can't load tilesheet %s... using temp tilesheet for item %d", fName.c_str(), 0);
        textureName = "game/tiles_page1.rttex";
        real::AppGetSmartFileName(real::GetApp(), fName, textureName);
        m_tileSheets[itemID] = real::GetApp()->m_resourceManager.GetSurfaceAnim(fName, false);
        m_tileSheets[itemID]->m_bSmoothing = false;
        pItemInfo->m_imageStorage = 1;
        pItemInfo->m_frameX = 0;
        pItemInfo->m_frameY = 3;
        return;
    }
    pSurf->m_bSmoothing = false;
    switch (pItemInfo->m_imageStorage)
    {
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
    {
        if (itemID == 5020 || itemID == 8306 || (pItemInfo->m_flags2 & 0x10000))
            pSurf->SetupAnim(64, 64);
        else
            pSurf->SetupAnim(32, 32);
        break;
    }
    default:
        break;
    }
}

void TileSheetManager::LoadTileSheetByIDIcon(unsigned short itemID)
{
    ItemInfo* pItemInfo = real::GetApp()->GetItemInfoManager()->GetItemByIDSafe(itemID);
    if (!pItemInfo)
    {
        printf("Error, can't locate tile %d data\n", itemID);
        return;
    }
    if (!pItemInfo->m_fileName.size())
    {
        printf("Error, tile(%d) haven't texture file", itemID);
        return;
    }
    std::string ext = GetFileExtension(pItemInfo->m_fileName);
    if (!ext.size())
        return;
    std::string fileWithoutExtension = pItemInfo->m_fileName.substr(0, pItemInfo->m_fileName.find(ext) - 1);
    std::string reconstructedName = fileWithoutExtension + "_icon." + ext;
    std::string fName;
    real::AppGetSmartFileName(real::GetApp(), fName, reconstructedName);

    SurfaceAnim* pSurf = real::GetApp()->m_resourceManager.GetSurfaceAnim(fName, false);
    if (pSurf)
    {
        pSurf->m_bSmoothing = false;
        switch (pItemInfo->m_imageStorage)
        {
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
        {
            pSurf->SetupAnim(32, 32);
            break;
        }
        default:
            break;
        }
    }
    m_tileSheets[itemID + 1] = pSurf ? pSurf : m_tileSheets[itemID];
}