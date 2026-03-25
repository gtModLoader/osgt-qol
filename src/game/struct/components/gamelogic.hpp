#pragma once
#include "game/struct/battlestatus.hpp"
#include "game/struct/component.hpp"
#include "game/struct/playeritems.hpp"
#include "game/struct/textmanager.hpp"
#include "game/struct/world/world.hpp"
#include "game/struct/world/worldrenderer.hpp"

struct ProgressUIHelper
{
    int m_Value;
    int m_Cap;
    int m_IconItemID;
    bool m_IsSet;
    int m_LastCheckTick;
    int m_isShow;
};
class NetAvatar;
// Expected size: 880 bytes
class GameLogicComponent : public EntityComponent
{
  public:
    WorldTileMap* GetTileMap() { return &m_pWorld->m_tiles; }

    int GetTileWidth() { return GetTileMap()->m_sizeX; }
    int GetTileHeight() { return GetTileMap()->m_sizeY; }

    bool IsDialogOpened();

    boost::signal<void(VariantList*)> m_sig_onPlayerClicked;
    int CurrentArtifact;
    int CurrentPlanter;
    int CurrentPlanterTileX;
    int CurrentPlanterTileY;
    int m_MagicCompassTrackingItemID;
    int m_userId;
    bool m_drop_lavacheck_faster;
    bool m_bItemPickerActive;
    int bumpTimer;
    int m_tradeItemID; // Might be wrong? Seems to be a boolean in 3.02.
    bool m_bIsInLocalMode;
    CL_Vec2f* m_pPos2d;
    World* m_pWorld;
    WorldRenderer* m_pWorldRenderer;
    CL_Vec2f* m_pScale2d;
    CL_Vec2f* m_pSize2d;
    uint8_t m_netObjManager[48];
    int m_fakeNetID;
    NetAvatar* m_pLocalPlayer; // NetAvatar*
    TextManager m_textManager;
    PlayerItems m_playerItems;
    int m_activeItemID;
    bool m_bHasGrowID;
    bool m_bHasAccountSecured;
    int m_gems;
    int m_sessionGems;
    bool m_bShowingNetworkProblem;
    unsigned int m_networkProblemTimer;
    uint8_t m_effectManager[24];
    bool m_bIsGameSupporter;
    bool m_bIsSGameSupporter;
    unsigned int m_timeOfLastSendMS;
    bool m_bForcedMusicMode;
    int m_punchRangeMod;
    int m_buildRangeMod;
    bool m_bSendIAPOnNextPing;
    void* m_pTradeMenu;
    int m_baseTimeSecondsFromServer;
    unsigned int m_timeThatTimeWasUpdated;
    BattleStatus m_battleStatus;
    uint8_t m_packetWatcher[16];
    int m_serverTimeMs;
    int m_serverTimeUpdateTick;
    int m_currentServerTimeMs;
    int m_activeHoliday;
    unsigned int m_server_tick;
    unsigned int m_ClientTickWhenReceavingServerTick;
    bool m_ClashEventIsActive;
    int m_uiFlowType;
    bool m_Paw2018SkinColor1Ability;
    bool m_Paw2018SkinColor2Ability;
    bool m_availableRoleSkins[6];
    bool m_availableRoleIcons[6];
    std::map<int, int> m_gamePacketCounter;
    std::map<int, int> m_messageTypeCounter;
    std::map<std::string, int> m_functionPacketCounter;
    int m_lastCheckTime;
    int m_lastDialogTime;
    unsigned int CONST_ProgressUIAnimMS;
    unsigned int m_ProgressUIAnimTimer;
    ProgressUIHelper m_ProgressUIHelper;
    std::map<std::string, int> dialogCheckBoxMap;
};
static_assert(sizeof(GameLogicComponent) == 880, "GameLogicComponent class size mismatch.");