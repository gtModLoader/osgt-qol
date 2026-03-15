#pragma once
#include "baseapp.hpp"
#include "enetclient.hpp"
#include "iteminfomanager.hpp"
#include "tilesheetmanager.hpp"
#include "variantdb.hpp"


class GameLogicComponent;

// Expected size: 4976 bytes
class App : public BaseApp
{
  public:
    virtual ~App();
    virtual void Init();
    virtual void Kill();
    virtual void OnPreInitVideo();
    virtual void Draw();
    virtual void Update();
    virtual void OnEnterBackground();
    virtual void OnEnterForeground();
    virtual void OnScreenSizeChange();
    virtual void LoadVarDB();
    GameLogicComponent* GetGameLogic() { return m_pGameLogic; }
    ItemInfoManager* GetItemInfoManager() { return &m_itemInfoManager; }
    TileSheetManager* GetTileSheetManager() { return &m_tileSheetManager; }
    void* GetAdManager() { return &m_adManager; }
    void* GetIAPManager() { return &m_iapManager; }
    void* GetAudioCache() { return &m_audioCache; }
    void* GetConsole() { return &m_console; }
    void* GetClient() { return m_pClient; }
    Variant* GetVar(std::string varName) { return m_sharedDB.GetVar(varName); }

    GameLogicComponent* m_pGameLogic;
    void* m_pTrackHandler; // TrackHandlerComponent*
    ItemInfoManager m_itemInfoManager;
    TileSheetManager m_tileSheetManager;
    uint8_t m_adManager[288];
    void* m_pUnk;
    uint8_t m_iapManager[416];
    uint8_t pad1[88]; // Unidentified
    uint8_t m_audioCache[88];
    void* m_pUnk2;
    ENetClient* m_pClient;
    void* m_pUnk3;
    Console m_logConsole;
    bool m_bDidPostInit;
    uint8_t pad3[7]; // Probably compiler padding
    VariantDB m_sharedDB;
    uint8_t pad4[440]; // Unidentified
    std::string m_cachePath;
    uint8_t pad5[56]; // Unidentified
    int m_serverProtocol;
    uint8_t pad6[40];
    int m_playerGDPRState;
    int m_playerAge;
    // +4840 - Seems to be some bool as well
    uint8_t pad7[140]; // Unidentified
    // +4873 - Unidentified bool
};
static_assert(sizeof(App) == 4976, "App class size mismatch.");