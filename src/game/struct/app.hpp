#pragma once
#include "baseapp.hpp"
#include "iteminfomanager.hpp"
#include "variantdb.hpp"

class GameLogicComponent;

// Expected size: 4976 bytes
class App : public BaseApp
{
  public:
    GameLogicComponent* GetGameLogic() { return m_pGameLogic; }
    ItemInfoManager* GetItemInfoManager() { return &m_itemInfoManager; }
    void* GetTileSheetManager() { return &m_tileSheetManager; }
    void* GetAdManager() { return &m_adManager; }
    void* GetIAPManager() { return &m_iapManager; }
    void* GetAudioCache() { return &m_audioCache; }
    void* GetConsole() { return &m_console; }
    void* GetClient() { return m_pClient; }
    Variant* GetVar(std::string varName) { return m_sharedDB.GetVar(varName); }

    void* dtor;

    GameLogicComponent* m_pGameLogic;
    void* m_pTrackHandler; // TrackHandlerComponent*
    ItemInfoManager m_itemInfoManager;
    uint8_t m_tileSheetManager[32];
    uint8_t m_adManager[288];
    void* m_pUnk;
    uint8_t m_iapManager[416];
    uint8_t pad1[88]; // Unidentified
    uint8_t m_audioCache[88];
    void* m_pUnk2;
    void* m_pClient; // ENetClient*
    void* m_pUnk3;
    uint8_t m_console[176];
    bool m_bDidPostInit;
    uint8_t pad3[7]; // Probably compiler padding
    VariantDB m_sharedDB;
    uint8_t pad4[440]; // Unidentified
    std::string m_cachePath;
    uint8_t pad5[100]; // Unidentified
    int m_playerGDPRState;
    int m_playerAge;
    // +4840 - Seems to be some bool as well
    uint8_t pad6[140]; // Unidentified
    // +4873 - Unidentified bool
};