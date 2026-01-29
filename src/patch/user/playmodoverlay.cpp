#include "game/game.hpp"
#include "game/signatures.hpp"
#include "game/struct/graphics/surface.hpp"
#include "patch/patch.hpp"
#include "utils/utils.hpp"

#include "game/struct/component.hpp"
#include "game/struct/components/gamelogic.hpp"
#include "game/struct/components/mapbg.hpp"
#include "game/struct/entity.hpp"
#include "game/struct/entityutils.hpp"
#include "game/struct/renderutils.hpp"
#include "game/struct/rtrect.hpp"
#include "game/struct/variant.hpp"

#include "game/struct/custom/playmodoverlay.hpp"
#include "game/struct/net/enet.hpp"

REGISTER_GAME_FUNCTION(OnMapLoaded,
                       "40 53 48 83 EC 20 F3 0F 10 ? ? ? ? ? 48 8B D9 C6 81 D4 00 00 00 00",
                       __fastcall, void, void*, __int64, __int64, __int64);
REGISTER_GAME_FUNCTION(
    GetMessageTypeFromPacket,
    "48 83 EC 28 48 83 79 18 04 73 13 48 8D ? ? ? ? ? E8 ? ? ? ? 33 C0 48 83 C4 28 C3", __fastcall,
    int, void*);
REGISTER_GAME_FUNCTION(
    OnLogGrabBarChanged,
    "48 8B C4 55 41 56 41 57 48 8D 68 A1 48 81 EC E0 00 00 00 48 C7 45 AF FE FF FF FF 48 89 58 08 "
    "48 89 70 10 48 89 78 18 4C 89 60 20 0F 29 70 D8 44 0F 29 40 C8 48 8B ? ? ? ? ? 48 33 C4 48 89 "
    "45 17 E8 ? ? ? ? 48 85 C0 0F 84 ? ? ? ? 48 C7 45 0F 0F 00 00 00",
    __fastcall, void, Variant*);
REGISTER_GAME_FUNCTION(GameLogicComponentKillWorld,
                       "48 89 5C 24 08 48 89 74 24 10 57 48 83 EC 20 0F B6 FA 48 8B D9 E8",
                       __fastcall, void, GameLogicComponent*, bool);

static std::vector<ActiveMod> g_activeMods;
static std::map<short, short> g_modIcons;
static time_t g_lastPlaymodTimerRecycle = 0;
static float g_playmodIconHeight = 0.0f;
static float g_lastOverlayHeight = 150.0f;
static bool g_bRecalculateIconHeight = true;
static bool g_IsOverlayHidden = true;

class PlaymodTimersOverlay : public patch::BasePatch
{
  public:
    void apply() const override
    {
        // /!\ PROOF OF CONCEPT MOD - A lot of this needs to be rewritten with proper APIs down the
        // line.
        //
        // Playmod Timers Overlay does what it says it does. It will create a "MMORPG-buff-bar"-like
        // overlay below the chat window showing the duration of your active playmods (with duration
        // of <=2h)
        //
        // We achieve this by following flow:
        // Advertise to server we are using the mod using SendPacket
        // Server sends us a list of playmod IDs and respective icons, the mod stores them in
        // g_modIcons.
        // Server also sends us a list of our own active playmods, the mod stores them in
        // g_activeMods
        // ---
        // When the player enters the world and OnMapLoaded is fired, we can be fairly sure that
        // GameMenu has been initialized and we can add our own GUI logic, so we create a new
        // "TimerOverlay" dummy entity*, iterate over our active mods and create sub-entities inside
        // the TimerOverlay for them and in the end attach a "recycler loop" to OnUpdate for
        // TimerOverlay.
        // The recycler loop checks if any playmods need to be removed, repositioned or brought
        // visible.
        // * - the dummy entity is also used to help position the child entities without expensive
        // positional maths. It's more-so a container entity.
        // ---
        // Earlier mentioned sub-entity is called "PlaymodTimerEntity". Each active playmod gets
        // one. PlaymodTimerEntity gets 2 children - an icon, which is created by help of
        // SetupEntityIconFromItem function from the game, and text for the playmod duration.
        // PlaymodTimerEntity is self-updating, it will decrement time and mark itself as dead when
        // the time runs out.
        // ---
        // The server can also add/remove playmods at will - if it's removed, the info about playmod
        // is updated in active list then asked to update on overlay - if it's added, it gets added
        // to the loop and immediate reconstruction of the overlay is done.

        auto& game = game::GameHarness::get();
        // Used for constructing the overlay after we got signal that the map is loaded.
        game.hookFunctionPatternDirect<OnMapLoaded_t>(pattern::OnMapLoaded, OnMapLoaded,
                                                      &real::OnMapLoaded);
        // We need to receive modded packets from server. We COULD hook into ENetClient functions
        // instead.
        game.hookFunctionPatternDirect<GetMessageTypeFromPacket_t>(
            pattern::GetMessageTypeFromPacket, GetMessageTypeFromPacket,
            &real::GetMessageTypeFromPacket);

        // We anchor into OnLogGrabBarChanged so we can position ourselves against it.
        game.hookFunctionPatternDirect<OnLogGrabBarChanged_t>(
            pattern::OnLogGrabBarChanged, OnLogGrabBarChanged, &real::OnLogGrabBarChanged);

        // We should cleanup when player leaves world as well, so we need to know an event for it.
        game.hookFunctionPatternDirect<GameLogicComponentKillWorld_t>(
            pattern::GameLogicComponentKillWorld, GameLogicComponentKillWorld,
            &real::GameLogicComponentKillWorld);

        // LoadFromMem is a good enough signal that we should ask for playmod data.
        auto& itemapi = game::ItemAPI::get();
        itemapi.m_sig_loadFromMem.connect(&PlaymodTimersOverlay::refreshItemDB);
        // TODO: Clear UI on server exit.

        // In-game way of toggling overlay visibility
        g_IsOverlayHidden =
            real::GetApp()->GetVar("osgt_qol_hide_playmod_overlay")->GetUINT32() != 0;
        auto& optionsMgr = game::OptionsManager::get();
        optionsMgr.addCheckboxOption("qol", "UI", "osgt_qol_hide_playmod_overlay",
                                     "Hide playmod overlay\n", &OnOverlayHideCallback);
    }

    static bool DoesTimerOverlayExist()
    {
        Entity* pWorldSpecificGUI =
            real::GetApp()->m_entityRoot->GetEntityByName("GUI")->GetEntityByName(
                "WorldSpecificGUI");
        if (!pWorldSpecificGUI)
            return false;
        return pWorldSpecificGUI->GetEntityByName("TimerOverlay") != nullptr;
    }

    static void __fastcall GameLogicComponentKillWorld(GameLogicComponent* this_, bool bUnk)
    {
        // Clear any old ui
        Entity* pWorldSpecificGUI =
            real::GetApp()->m_entityRoot->GetEntityByName("GUI")->GetEntityByName(
                "WorldSpecificGUI");
        if (pWorldSpecificGUI)
            pWorldSpecificGUI->RemoveEntityByAddress(
                pWorldSpecificGUI->GetEntityByName("TimerOverlay"));
        real::GameLogicComponentKillWorld(this_, bUnk);
    }

    static void refreshItemDB()
    {
        g_activeMods.clear();
        // We signal the server we want playmod data and that we are modded user.
        real::SendPacket(2, "action|enable_mod\nname|playmod_overlay\nproto|1\n",
                         real::GetApp()->m_pClient->m_pEnetPeer);
    }

    static void OnLogGrabBarChanged(Variant* pV)
    {
        real::OnLogGrabBarChanged(pV);
        // Anchor TimerOverlay below the chat window.
        Entity* pConsoleLogParent =
            real::GetApp()->m_entityRoot->GetEntityByName("ConsoleLogParent");
        Entity* pLogEnt = pConsoleLogParent->GetEntityByName("ConsoleGrab");
        float yLevel = 0.0;
        if (!pLogEnt)
        {
            pLogEnt = pConsoleLogParent->GetEntityByName("LogRectBG");
            if (!pLogEnt)
                return;
            else
                yLevel = pLogEnt->GetVar("size2d")->GetVector2().y + 16;
        }
        else
            yLevel = pLogEnt->GetVar("pos2d")->GetVector2().y + 16;
        Entity* pGUI = real::GetApp()->m_entityRoot->GetEntityByName("GUI");
        Entity* pWorldSpecificGUI = pGUI->GetEntityByName("WorldSpecificGUI");
        if (!pWorldSpecificGUI)
            return;
        Entity* pOverlayEnt = pWorldSpecificGUI->GetEntityByName("TimerOverlay");
        if (!pOverlayEnt)
            return;
        pOverlayEnt->GetVar("pos2d")->Set(CL_Vec2f(16, yLevel));
        g_lastOverlayHeight = yLevel;
    }

    static int __fastcall GetMessageTypeFromPacket(void* _packet)
    {
        int res = real::GetMessageTypeFromPacket(_packet);
        ENetPacket* packet = reinterpret_cast<ENetPacket*>(_packet);
        if (packet->dataLength < 10)
            return res;
        // Check for mod packet type - this is a placeholder, proper protocol needs to be
        // envisioned.
        if (memcmp("MODPMOL", packet->data, 7) != 0)
            return res;
        PlaymodPacketAction action = *(PlaymodPacketAction*)(packet->data + 7);
        short ToParse = *(short*)(packet->data + 8);
        switch (action)
        {
        case PLAYMOD_PROTOCOL_ADD_OR_REMOVE:
        {
            if (packet->dataLength < 16)
            {
                printf("[PlaymodOverlays] invaid add/del mod payload.\n");
                break;
            }
            int offset = 10;
            PlaymodPacketDataPlaymodInfo* modData =
                (PlaymodPacketDataPlaymodInfo*)(packet->data + offset);
            bool bNewTarget = true;
            for (auto it = g_activeMods.begin(); it != g_activeMods.end();)
            {
                if (it->m_playmodID == modData->PlaymodID)
                {
                    // We can't update the overlay if it doesn't exist, so treat it as a new target.
                    time_t oldKillstamp = it->m_killstamp;
                    bNewTarget = false;
                    it->m_killstamp = modData->Duration + time(0);
                    if (!DoesTimerOverlayExist())
                    {
                        if (oldKillstamp <= time(0) || it->m_killstamp <= time(0) ||
                            modData->Duration <= 0)
                        {
                            printf("[PlaymodOverlays] killing offscreen\n");
                            it = g_activeMods.erase(it);
                            bNewTarget = true;
                            break;
                        }
                    }
                    // Hide >2hr mods from showing up at first.
                    if (it->m_killstamp - time(0) > 7200)
                        it->m_bDelayLoad = true;
                    UpdateOverlayTimer(modData->PlaymodID, it->m_killstamp);
                    printf("[PlaymodOverlays] upd kstamp=%lld, oldstamp=%lld, dur=%d, pmod=%d\n",
                           it->m_killstamp, oldKillstamp, modData->Duration, modData->PlaymodID);
                    if (it->m_killstamp <= time(0) || modData->Duration <= 0)
                    {
                        it = g_activeMods.erase(it);
                    }
                    break;
                }
                it++;
            }
            if (bNewTarget && modData->Duration > 0)
            {
                printf("[PlaymodOverlays] new pmod target\n");
                ActiveMod mod;
                mod.m_playmodID = modData->PlaymodID;
                mod.m_killstamp = time(0) + (time_t)modData->Duration;
                // Hide >2hr mods from showing up at first.
                if (mod.m_killstamp - time(0) > 7200)
                    mod.m_bDelayLoad = true;
                auto pair = g_modIcons.find(mod.m_playmodID);
                if (pair != g_modIcons.end())
                    mod.m_iconID = pair->second;
                g_activeMods.push_back(mod);
                if (DoesTimerOverlayExist())
                    OnRefreshOverlay();
            }
            break;
        }
        case PLAYMOD_PROTOCOL_SEND_BULK:
        {
            if (packet->dataLength < ((ToParse * 6) + 10))
            {
                printf("[PlaymodOverlays] invalid bulk data payload.\n");
                break;
            }
            int offset = 10;
            for (int i = 0; i < ToParse; i++)
            {
                PlaymodPacketDataPlaymodInfo* modData =
                    (PlaymodPacketDataPlaymodInfo*)(packet->data + offset);
                offset += 6;

                if (modData->Duration <= 0)
                    continue;

                ActiveMod mod;
                mod.m_playmodID = modData->PlaymodID;
                mod.m_killstamp = time(0) + (time_t)modData->Duration;
                // Hide >2hr mods from showing up at first.
                if (mod.m_killstamp - time(0) > 7200)
                    mod.m_bDelayLoad = true;
                auto pair = g_modIcons.find(mod.m_playmodID);
                if (pair != g_modIcons.end())
                    mod.m_iconID = pair->second;
                g_activeMods.push_back(mod);
            }
            if (DoesTimerOverlayExist())
                OnRefreshOverlay();
            break;
        }
        case PLAYMOD_PROTOCOL_UPDATE_ICON_DATA:
        {
            // size check
            if (packet->dataLength < ((ToParse * 4) + 10))
            {
                printf("[PlaymodOverlays] invalid icon data payload.\n");
                break;
            }
            int offset = 10;
            for (int i = 0; i < ToParse; i++)
            {
                PlaymodPacketDataIconInfo* iconData =
                    (PlaymodPacketDataIconInfo*)(packet->data + offset);
                offset += 4;
                g_modIcons.insert(std::make_pair(iconData->PlaymodID, iconData->ItemID));
            }
            break;
        }
        default:
            break;
        }
        return res;
    }

    static void __fastcall OnMapLoaded(void* this_, __int64 p1, __int64 p2, __int64 p3)
    {
        real::OnMapLoaded(this_, p1, p2, p3);
        if (!g_IsOverlayHidden)
            ConstructOverlay();
    }

    static void ConstructOverlay()
    {
        Entity* pWorldSpecificGUI =
            real::GetApp()->m_entityRoot->GetEntityByName("GUI")->GetEntityByName(
                "WorldSpecificGUI");
        if (pWorldSpecificGUI->GetEntityByName("TimerOverlay"))
        {
            OnRefreshOverlay();
            return;
        }
        // We position the overlay right below chat window and we store the last height in a global
        // variable for easier retrieval here rather than needing to recalculate it.
        Entity* pOverlayEnt = pWorldSpecificGUI->AddEntityToFront(new Entity("TimerOverlay"));
        pOverlayEnt->GetVar("pos2d")->Set(CL_Vec2f(16, g_lastOverlayHeight));

        CL_Vec2f Bounds(0, 0);
        // std::sort(g_activeMods.begin(), g_activeMods.end());
        for (auto mod : g_activeMods)
        {
            if (mod.m_killstamp <= time(0))
                continue;
            if (mod.m_bDelayLoad)
                continue;
            PlaymodTimerEntity* pTimerEnt = CreateTicker(mod.m_iconID, pOverlayEnt, Bounds);
            // If the server has supplied us with playmod IDs, we can set it as "trackable" meaning
            // if we get told a mod has expired, we can easily look it up and remove it.
            if (mod.m_playmodID != -1)
                pTimerEnt->SetTrackableID(mod.m_playmodID);
            pTimerEnt->SetTimer(mod.m_killstamp - time(0));
        }

        g_lastPlaymodTimerRecycle = time(0);
        pOverlayEnt->GetFunction("OnUpdate")
            ->sig_function.connect(
                1, boost::bind(&PlaymodTimersOverlay::OnOverlayUpdate, pOverlayEnt, _1));
    }

    static void UpdateOverlayTimer(short PlaymodID, time_t killstamp)
    {
        // Called to update existing timer with either expiry or changed expiration date.
        Entity* pWorldSpecificGUI =
            real::GetApp()->m_entityRoot->GetEntityByName("GUI")->GetEntityByName(
                "WorldSpecificGUI");
        if (!pWorldSpecificGUI)
            return;
        Entity* pOverlayEnt = pWorldSpecificGUI->GetEntityByName("TimerOverlay");
        if (!pOverlayEnt)
            return;
        std::vector<Entity*> pTimerEnts;
        pOverlayEnt->GetEntitiesByName(&pTimerEnts, "PlaymodTimerEntity");
        bool bFound = false;
        for (auto it = pTimerEnts.begin(); it != pTimerEnts.end(); it++)
        {
            PlaymodTimerEntity* pTimerEnt = reinterpret_cast<PlaymodTimerEntity*>(*it);
            if (pTimerEnt->m_trackableID == PlaymodID)
            {
                pTimerEnt->m_killstamp = killstamp;
                // Reset killables and force-rerender the timer on match
                pTimerEnt->m_bKillable = false;
                pTimerEnt->m_lastUpdate = 0;
                pTimerEnt->OnUpdate(nullptr);
                bFound = true;
                break;
            }
        }
        if (!bFound)
            OnRefreshOverlay();
    }

    static void OnRefreshOverlay()
    {
        // Just kill the overlay and create it again.
        Entity* pWorldSpecificGUI =
            real::GetApp()->m_entityRoot->GetEntityByName("GUI")->GetEntityByName(
                "WorldSpecificGUI");
        if (!pWorldSpecificGUI)
            return;
        Entity* pOverlayEnt = pWorldSpecificGUI->GetEntityByName("TimerOverlay");
        if (pOverlayEnt)
            pWorldSpecificGUI->RemoveEntityByAddress(pOverlayEnt);
        ConstructOverlay();
    }

    static PlaymodTimerEntity* CreateTicker(int ItemID, Entity* ParentEnt, CL_Vec2f& bounds)
    {
        // "Ticker" is a PlaymodTimerEntity which contains the playmod icon and the timer text.
        PlaymodTimerEntity* pTimerEnt = new PlaymodTimerEntity();
        pTimerEnt->GetVar("pos2d")->Set(bounds);
        ParentEnt->AddEntity(pTimerEnt);
        CL_Vec2f IconBounds(0, 0);
        Entity* pIcon = DrawIcon(ItemID, pTimerEnt, IconBounds);
        Entity* pTextLabel = real::CreateTextLabelEntity(pTimerEnt, "txt", 42.0f, 0, "0:00");
        SetTextShadowColor(pTextLabel, 150);
        if (g_playmodIconHeight == 0.0f || g_bRecalculateIconHeight)
        {
            // Basically icon size + some extra padding.
            g_playmodIconHeight = pIcon->GetVar("size2d")->GetVector2().y * 1.5f;
            g_bRecalculateIconHeight = false;
        }
        bounds.y += g_playmodIconHeight;
        return pTimerEnt;
    }

    static void OnOverlayUpdate(Entity* pEnt, VariantList* pVL)
    {
        if (g_IsOverlayHidden)
            return;
        if (g_lastPlaymodTimerRecycle >= time(0))
            return;
        // Once-a-second recycle loop for overlayed timers. Kill any entities marked as killable.
        bool bReorder = false;
        std::vector<Entity*> pTimerEnts;
        pEnt->GetEntitiesByName(&pTimerEnts, "PlaymodTimerEntity", 1);
        for (auto it = pTimerEnts.begin(); it != pTimerEnts.end();)
        {
            PlaymodTimerEntity* pTimerEnt = reinterpret_cast<PlaymodTimerEntity*>(*it);
            if (pTimerEnt->m_bKillable)
            {
                if (pTimerEnt->m_trackableID != -1)
                {
                    for (auto modIt = g_activeMods.begin(); modIt != g_activeMods.end(); modIt++)
                    {
                        if (modIt->m_playmodID == pTimerEnt->m_trackableID)
                        {
                            g_activeMods.erase(modIt);
                            break;
                        }
                    }
                }
                pEnt->RemoveEntityByAddress(pTimerEnt);
                it = pTimerEnts.erase(it);
                bReorder = true;
            }
            else
                it++;
        }
        // We use the bound for repositioning and also activating rendering of delayed timers.
        CL_Vec2f Bounds(0, 0);
        if (bReorder)
        {
            for (auto ent : pTimerEnts)
            {
                ent->GetVar("pos2d")->Set(Bounds);
                Bounds.y += g_playmodIconHeight;
            }
        }
        else if (pTimerEnts.size() > 0)
            Bounds.y = g_playmodIconHeight + pTimerEnts.back()->GetVar("pos2d")->GetVector2().y;
        for (auto it = g_activeMods.begin(); it != g_activeMods.end(); it++)
        {
            if (it->m_killstamp <= time(0))
                continue;
            if (it->m_bDelayLoad)
            {
                if ((it->m_killstamp - time(0)) <= 7200)
                {
                    it->m_bDelayLoad = false;
                    PlaymodTimerEntity* pTimerEnt = CreateTicker(it->m_iconID, pEnt, Bounds);
                    if (it->m_playmodID != -1)
                        pTimerEnt->SetTrackableID(it->m_playmodID);
                    pTimerEnt->SetTimer(it->m_killstamp - time(0));
                }
                else
                    continue;
            }
        }
        g_lastPlaymodTimerRecycle = time(0);
        return;
    }

    static Entity* DrawIcon(int ItemID, Entity* ParentEnt, CL_Vec2f& bounds)
    {
        if (ItemID == 0)
            ItemID = 1;
        Entity* pIcon = real::SetupEntityIconFromItem(&ItemID, ParentEnt, &bounds, 0, true);
        pIcon->GetVar("scale2d")->Set(CL_Vec2f(1.5f));
        return pIcon;
    }

    static void OnOverlayHideCallback(VariantList* pVariant)
    {
        Entity* pCheckbox = pVariant->Get(1).GetEntity();
        bool bChecked = pCheckbox->GetVar("checked")->GetUINT32() != 0;
        real::GetApp()->GetVar("osgt_qol_hide_playmod_overlay")->Set(uint32_t(bChecked));
        g_IsOverlayHidden = bChecked;

        if (g_IsOverlayHidden)
        {
            // Remove immediately if hiding
            Entity* pWorldSpecificGUI =
                real::GetApp()->m_entityRoot->GetEntityByName("GUI")->GetEntityByName(
                    "WorldSpecificGUI");

            if (pWorldSpecificGUI)
            {
                Entity* pOverlay = pWorldSpecificGUI->GetEntityByName("TimerOverlay");
                if (pOverlay)
                {
                    pWorldSpecificGUI->RemoveEntityByAddress(pOverlay);
                }
            }
        }
        else
        {
            // Re-construct if unhiding
            ConstructOverlay();
        }
    }
};

REGISTER_USER_GAME_PATCH(PlaymodTimersOverlay, playmod_overlay);
