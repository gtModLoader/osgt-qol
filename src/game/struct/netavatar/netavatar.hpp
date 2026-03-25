#pragma once
#include "game/struct/component.hpp"
#include "game/struct/lockfloat.hpp"
#include "game/struct/renderdata/avatarrenderdata.hpp"
#include "game/struct/timer.hpp"
#include "game/struct/vec.hpp"

// Namings matched.
class NetBase
{
  public:
    virtual ~NetBase(){};
    CL_Vec2f m_vPos;
    CL_Vec2f m_vSize;
    CL_Vec2f m_movementVerify;
    CL_Vec2f m_sizeVerify;
    std::string m_name;
    int m_netID;
    bool m_deleteFlag;
    EntityComponent m_component;
};
static_assert(sizeof(NetBase) == 296, "NetBase class size mismatch.");

// Namings matched.
class NetMoving : public NetBase
{
  public:
    virtual ~NetMoving(){};
    CL_Vec2f m_origin;
    bool m_collidable;
    bool m_bFacingLeft;
    float _moveX;
    float _moveY;

    Rectf GetCollisionRectWorld()
    {
        Rectf res;
        res.left = m_vPos.x;
        res.top = m_vPos.y;
        res.right = m_vPos.x + m_vSize.x;
        res.bottom = m_vPos.y + m_vSize.y;
        return res;
    }
};
static_assert(sizeof(NetMoving) == 320, "NetMoving class size mismatch.");

enum GuildRole : unsigned int
{
    GUILD_ROLE_MEMBER,
    GUILD_ROLE_ELDER,
    GUILD_ROLE_CO_LEADER,
    GUILD_ROLE_LEADER
};
struct GuildClientModel
{
    int guildID;
    int worldID;
    int guildEmblem;
    GuildRole role;
};

class SurfaceAnim;
struct Tile;

// Needs to be verified, should be 90% it though.
class NetAvatar : public NetMoving
{
  public:
    virtual ~NetAvatar(){};
    unsigned int m_raceTimeStart;
    int m_countDownScore;
    float m_countDownTime;
    unsigned int m_inWaterTime;
    LockFloat m_punchReload;
    unsigned int m_pantsColor;
    GuildClientModel GuildClientModelObj;
    std::string m_OnlineID;
    float m_bJumpKeydownTime;
    int worldEntityID;
    int m_jumpsSinceLastGround;
    int m_maxJumpsSinceLastGround;
    bool m_bOnSolid;
    bool m_bOnIce;
    bool m_bOnMud;
    bool m_bHandItemActive;
    bool m_bBackItemActive;
    bool m_bDoubleJumpActive;
    LockFloat m_speedX;
    LockFloat m_speedY;
    bool m_bDidGravity;
    bool m_bRequestedJump;
    bool m_isCommunicationDisabled;
    unsigned int m_lastHackSendTimer;
    int m_emotion;
    float m_emotionTime;
    void* m_pController;     // NetControllerBase*
    void* m_pPacketReceiver; // AvatarPacketReceiver*
    void* m_pPacketSender;   // AvatarPacketSender*
    bool m_bJumpKeyDown;
    bool m_bTouchedLavaOnLastBounce;
    unsigned int m_flagsToAddToNextPacketSent;
    int m_freezeState;
    unsigned int m_playerID;
    bool m_bAllowJumpForOneFrameExtra;
    uint8_t pad[3];
    CL_Vec2f m_predicitionChange;
    std::string m_country;
    SurfaceAnim* m_pFlagSurf;
    bool m_bInvisible;
    bool m_bIsMod;
    bool m_bIsSuperMod;
    int m_nextPacketTileID;
    unsigned int m_audioTimer;
    int m_iconState;
    unsigned int m_flags;
    unsigned int m_flags2;
    unsigned int m_flagsMay2019;
    unsigned int m_hairDye;
    unsigned int m_eyeColor;
    unsigned int m_pupilColor;
    bool m_bDownKeyDown;
    bool m_bUnderwater;
    bool m_bDontsendMovementUntilUnfrozen;
    unsigned int m_punchType;
    LockFloat m_playerGravity;
    LockFloat m_baseAccel;
    LockFloat m_maxSpeedX;
    float m_maxSpeedWaterX;
    bool m_bDoubleJumpAvailable;
    LockFloat m_knockBackPower;
    std::string m_forceWeaponSfx;
    std::string m_forceWepPainSfx;
    std::string m_forceDeathScream;
    float m_health;
    float m_visHealth;
    bool m_bCheated;
    int m_lastPortal;
    int m_hCheck;
    int m_respawnIndex;
    int m_faction;
    int m_bountyLevel;
    int m_bountyScore;
    bool m_FactionWarActive;
    bool m_battleTeam;
    bool m_battleFlagInHand;
    int m_battleScore;
    int m_onSpikeCheck;
    int m_onLavaCheck;
    int m_balloonHitFromFaction;
    int m_balloonWetModActive;
    CL_Vec2f m_toolTouchPosCenterScreen;
    short transmutatedClothingItems[9];
    short clothingItems[9];
    int artifactItem;
    int equippedItemCount;
    uint8_t m_adventure[64];
    uint8_t m_avatarData[48];
    AvatarRenderData m_renderData;
    int m_checkItemsForHackTimers;
    Tile* m_pLastTileOver;
    Tile* m_pLastTileUnderUs;
    int m_activeBattlePet;
    int m_petBattleID;
    bool inittedAvatarRenderData;
    Tile* m_challengeStartFlag;
    Tile* m_challengeEndFlag;
    CL_Vec2f m_jumpOrigin;
    bool m_bHitMomentumTrampoline;
    float m_momentumHitSpeed;
    LockFloat m_punchPowerTimer;
    LockFloat m_doingPunchPowerUp;
    LockFloat m_doingPowerUpWithButton;
    LockFloat m_punchPowerLevel;
    LockFloat m_raymanEquippedKey1;
    LockFloat m_raymanEquippedKey2;
    bool m_bInGuild;
    bool m_bDisplayGuildMascot;
    bool m_bIsMaxLevel;
    int m_roleIcon;
    Tile* pDisguisedTile;
    int disguiseTileID;
    short disguiseTileBGID;
    int disguiseParticleEmitterIndex;
    std::shared_ptr<Timer> disguiseParticleTimer;
    int m_PreviousSolidFloorMapIndex;
    bool m_simFloatHack;
    bool m_simOneHitHack;
    SurfaceAnim* m_pXLogoSurf;
    bool m_isXBLAccount;
    bool isShowingTitleIcon;
    std::map<int, int> m_TransmutatedLinkPairs;
    std::map<int, int> m_TemporalTransmutatedLinkPairs;
};
static_assert(sizeof(NetAvatar) == 7864, "NetAvatar class size mismatch.");