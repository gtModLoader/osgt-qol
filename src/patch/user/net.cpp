#include "game/game.hpp"
#include "game/signatures.hpp"
#include "game/struct/component.hpp"
#include "game/struct/components/gamelogic.hpp"
#include "patch/patch.hpp"

#include "game/struct/entity.hpp"
#include "game/struct/miscutils.hpp"
#include "utils/utils.hpp"
#include <string>

// OnlineMenuCreate
// Returns: Online Menu entity
// Params: GUI Entity
REGISTER_GAME_FUNCTION(
    OnlineMenuCreate,
    "48 8B C4 55 41 56 41 57 48 8D A8 98 FD FF FF 48 81 EC 50 03 00 00 48 C7 45 E0 FE FF FF FF 48 "
    "89 58 10 48 89 70 18 48 89 78 20 0F 29 70 D8 0F 29 78 C8 44 0F 29 40",
    __fastcall, Entity*, Entity*);

// OnlineMenuOnConnect
// Returns: A pointer returned by InitConnection
// Params: Clicked Entity (button)
REGISTER_GAME_FUNCTION(OnlineMenuOnConnect,
                       "48 8B C4 57 48 81 EC 80 00 00 00 48 C7 40 A8 FE FF FF FF 48 89 58 10 48 89 "
                       "70 18 48 8B 05 0E 7B 2D 00 48 33 C4 48 89 44 24 78 FF",
                       __fastcall, __int64, Entity*);

// HTTPComponent::InitAndStart
// Params: this, VariantList* pVList
REGISTER_GAME_FUNCTION(HTTPComponentInitAndStart,
                       "48 8B C4 57 48 81 EC 80 00 00 00 48 C7 40 A8 FE FF FF FF 48 89 58 18 48 89 "
                       "70 20 48 8B DA 48 8B",
                       __fastcall, void, void*, VariantList*);

// RequestIAPPricesFromHouston
// This seems to be a legacy holdover from 3.xx clients. This interferes with client ability to
// request server data properly the first time around.
// Params: Unknown
REGISTER_GAME_FUNCTION(
    RequestIAPPricesFromHouston,
    "48 8B C4 55 41 54 41 55 41 56 41 57 48 8D A8 18 FB FF FF 48 81 EC C0 05 00 00 48 C7 44",
    __fastcall, void, void*);

// CreateInputTextEntity
// NOTE: Need to investigate the final 3 strings. They're not there originally in Proton SDK.
// Returns: Created Entity
// Params: Parent Entity, Entity Name, X, Y, Text, sizeX, sizeY, ??, ??, ??
REGISTER_GAME_FUNCTION(CreateInputTextEntity, "48 8B C4 55 41 54 41 55 41 56 41 57 48 8D 68 D1",
                       __fastcall, Entity*, Entity*, std::string, float, float, std::string, float,
                       float, std::string, std::string, std::string);

// GetAppCachePath
// Returns: Game directory string
REGISTER_GAME_FUNCTION(GetAppCachePath, "40 53 48 83 EC 30 33 C0 48 C7 41 18 0F 00 00 00 48",
                       __fastcall, std::string);

// LogToConsole
// Params: Text to log
REGISTER_GAME_FUNCTION(LogToConsole,
                       "48 89 4C 24 08 48 89 54 24 10 4C 89 44 24 18 4C 89 4C 24 20 53 57 B8 88 10 "
                       "00 00 ? ? ? ? ? 48 2B E0 48 8B 05 86 E4 2D",
                       __fastcall, void, const char*);

REGISTER_GAME_FUNCTION(
    GameLogicComponentOnLogonAccepted,
    "48 8B C4 55 56 57 41 54 41 55 41 56 41 57 48 8D 68 A8 48 81 EC 20 01 00 00 48 C7 44 24 38 FE",
    __fastcall, void, GameLogicComponent*, VariantList*);

REGISTER_GAME_FUNCTION(
    GameLogicComponentOnAdd,
    "48 8B C4 55 57 41 56 48 8D A8 68 F9 FF FF 48 81 EC 80 07 00 00 48 C7 44 24 50 FE FF FF FF",
    __fastcall, void, GameLogicComponent*);

REGISTER_GAME_FUNCTION(GetRegionString,
                       "48 8B C4 55 48 8D 68 A1 48 81 EC E0 00 00 00 48 C7 45 9F FE FF FF FF 48 89 "
                       "58 10 48 89 78 18 48 8B ? ? ? ? ? 48 33 C4 48 89 45 47 48 8B D9",
                       __fastcall, std::string);

class ServerSwitcher : public patch::BasePatch
{
  public:
    void apply() const override
    {
        auto& game = game::GameHarness::get();
        // Resolve UI functions - we will need these to properly construct our UI in OnlineMenu
        real::CreateInputTextEntity =
            game.findMemoryPattern<CreateInputTextEntity_t>(pattern::CreateInputTextEntity);

        // We want to also inform the user which server they are using.
        real::LogToConsole = game.findMemoryPattern<LogToConsole_t>(pattern::LogToConsole);

        // Nop the original slide-screen so the original function does not start sliding our scene
        // too early. We will call it ourselves later in the hook when we have injected our own
        // entities in.
        auto addr = game.findMemoryPattern<uint8_t*>(
            "E8 ? ? ? ? 45 33 C9 41 B8 F4 01 00 00 B2 01 48 8B CF E8 ? ? ? ? 48 8D");
        utils::nopMemory(addr + 5, 19);

        // Hook
        game.hookFunctionPatternDirect<OnlineMenuCreate_t>(
            pattern::OnlineMenuCreate, OnlineMenuCreate, &real::OnlineMenuCreate);
        game.hookFunctionPatternDirect<OnlineMenuOnConnect_t>(
            pattern::OnlineMenuOnConnect, OnlineMenuOnConnect, &real::OnlineMenuOnConnect);

        // NOTE: We prefer HTTPComponent::InitAndStart over App::GetServerInfo here solely because
        // the game has a hard-coded literal for growtopia2.com inside InitConnection it will try to
        // use if first connect fails.
        game.hookFunctionPatternDirect<HTTPComponentInitAndStart_t>(
            pattern::HTTPComponentInitAndStart, HTTPComponentInitAndStart,
            &real::HTTPComponentInitAndStart);
        // This function calls HTTPComponent with a dead link, causing it to conflict with
        // InitConnection. Safer to create a empty detour instead to completely dissolve this issue.
        game.hookFunctionPatternDirect<RequestIAPPricesFromHouston_t>(
            pattern::RequestIAPPricesFromHouston, RequestIAPPricesFromHouston,
            &real::RequestIAPPricesFromHouston);

        // Use any cached value
        std::string Hostname = real::GetApp()->GetVar("osgt_qol_server_hostname")->GetString();
        if (Hostname.size() == 0)
        {
            real::GetApp()->GetVar("osgt_qol_server_hostname")->Set("osgt1.cernodile.com");
        }
    }

    static Entity* __fastcall OnlineMenuCreate(Entity* pGUI)
    {
        // Let game construct online menu itself ready for us
        Entity* pOnlineMenu = real::OnlineMenuCreate(pGUI);

        // OnlineMenu uses this scale for its UI elements
        uint32_t fontID;
        float fontScale;
        real::GetFontAndScaleToFitThisLinesPerScreenY(fontID, fontScale, 18);

        // Take a few elements as anchors to get margins down.
        // We are namely interested in "GrowID: " label for Y coordinate and
        // "text" label for X coordinate.
        float marginY =
            pOnlineMenu->GetEntityByName("tankid_name_label")->GetVar("pos2d")->GetVector2().y;
        float marginX = pOnlineMenu->GetEntityByName("text")->GetVar("pos2d")->GetVector2().x;

        // Create our very own label
        Entity* pServerLabel = real::CreateTextLabelEntity(
            pOnlineMenu, "osgt_qol_server_label", marginX, marginY, "Server Data Location");
        // We need to call SetupTextEntity so it scales and lines up with the rest of UI.
        real::SetupTextEntity(pServerLabel, fontID, fontScale);

        // ..and an accompanying textbox
        // We will append the height of our label to be directly below it for marginY.
        // We will also make the textbox stretch to same length as non-ID name field.
        // The larger InputTextEntity is, the more characters it can fit within its bounds.
        float vSizeX = pOnlineMenu->GetEntityByName("name")->GetVar("size2d")->GetVector2().x;
        vSizeX +=
            pOnlineMenu->GetEntityByName("name_input_box_online")->GetVar("size2d")->GetVector2().x;
        Entity* pServerInput = real::CreateInputTextEntity(
            pOnlineMenu, "osgt_qol_server_input", marginX,
            marginY + pServerLabel->GetVar("size2d")->GetVector2().y,
            real::GetApp()->GetVar("osgt_qol_server_hostname")->GetString(), vSizeX, 0.0, "", "",
            "");

        // Make it a bit neater by setting a max length and disallowing going through bounds.
        EntityComponent* pTextRenderComp = pServerInput->GetComponentByName("InputTextRender");
        pTextRenderComp->GetVar("inputLengthMax")->Set(32U);
        pTextRenderComp->GetVar("truncateTextIfNeeded")->Set(1U);
        // InputTextEntity is still an TextEntity, we have to scale it to rest of UI with the helper
        // function.
        real::SetupTextEntity(pServerInput, fontID, fontScale);

        // We nopped the slidescreen earlier, put it back in place for the smooth transition.
        real::SlideScreen(pOnlineMenu, true, 500, 0);
        return pOnlineMenu;
    }

    static __int64 __fastcall OnlineMenuOnConnect(Entity* pEnt)
    {
        // pEnt is normally "Connect" button, but if you enter the game via Enter hotkey, then it
        // will be null.
        Entity* pServerInput = nullptr;
        if (pEnt)
            pServerInput = pEnt->GetParent()->GetEntityByName("osgt_qol_server_input");
        else
        {
            // We have to call "ByNameRecursively" here because when you launch the game, OnlineMenu
            // goes under "GUI", but when you change resolutions *before* entering the game, it
            // suddenly changes whose the parent and causes game to crash.. kinda weird.
            pServerInput = real::GetApp()
                               ->m_entityRoot->GetEntityByNameRecursively("OnlineMenu")
                               ->GetEntityByName("osgt_qol_server_input");
        }

        if (pServerInput != nullptr)
        {
            // If the patch takes too long to load, the element may not even exist.
            std::string userInput =
                pServerInput->GetComponentByName("InputTextRender")->GetVar("text")->GetString();
            if (userInput.size() != 0)
            {
                real::GetApp()->GetVar("osgt_qol_server_hostname")->Set(userInput);
            }
        }
        return real::OnlineMenuOnConnect(pEnt);
    }

    static void __fastcall HTTPComponentInitAndStart(void* this_, VariantList* pVList)
    {
        std::string URI = pVList->Get(2).GetString();
        // We only want to modify something accessing the server data.
        if (URI == "growtopia/server_data.php")
        {
            pVList->Get(0).Set(real::GetApp()->GetVar("osgt_qol_server_hostname")->GetString());
            real::LogToConsole(
                std::string("Using `w" +
                            real::GetApp()->GetVar("osgt_qol_server_hostname")->GetString() +
                            "`` as the server data provider...")
                    .c_str());
        }
        real::HTTPComponentInitAndStart(this_, pVList);
    }

    static void __fastcall RequestIAPPricesFromHouston(void* unk)
    {
        // Stub out as it normally will attempt to do first HTTPComponent connection here
        // which will fail first server data fetch instead.
        return;
    }
};
REGISTER_USER_GAME_PATCH(ServerSwitcher, server_data_switcher);

static std::string cachePath = "";
class CacheLocationFixer : public patch::BasePatch
{
  public:
    void apply() const override
    {
        auto& game = game::GameHarness::get();

        // Get our current directory
        TCHAR lpBuffer[MAX_PATH];
        DWORD dwRet;
        dwRet = GetCurrentDirectory(MAX_PATH, lpBuffer);
        if (dwRet == 0)
        {
            // If we can't determine the current directory, this patch can't do anything
            throw std::runtime_error("Failed to get current directory, cannot apply patch safely.");
            return;
        }
        else
        {
            // Assign the new cache path value to both App and our static variable
            real::GetApp()->m_cachePath = std::string(lpBuffer) + "\\cache/";
            // Windows doesn't give us a trailing slash. We want to have one in our path variable
            cachePath.assign(lpBuffer);
            cachePath.append("\\");
            // We need to create the directory ourselves or the game will freak out about "no
            // storage space" when retrieving items.dat
            CreateDirectoryA(real::GetApp()->m_cachePath.c_str(), 0);
        }

        // Hook GetAppCachePath to resolve any directory issues when downloading files to cache
        game.hookFunctionPatternDirect<GetAppCachePath_t>(pattern::GetAppCachePath, GetAppCachePath,
                                                          &real::GetAppCachePath);
    }

    static std::string __fastcall GetAppCachePath() { return cachePath; }
};
REGISTER_USER_GAME_PATCH(CacheLocationFixer, cache_location_fixer);

class AllowReleasingMutex : public patch::BasePatch
{
  public:
    void apply() const override
    {
        // Patch out Internal Memory Error 49
        auto& game = game::GameHarness::get();
        auto addr = game.findMemoryPattern<uint8_t*>(
            "E8 ? ? ? ? 48 8B C8 33 D2 E8 ? ? ? ? E8 ? ? ? ? 4C 8B C0 48 8D ? ? ? ? ? 45 33 C9 33");
        utils::nopMemory(addr, 47);
    }
};
REGISTER_USER_GAME_PATCH(AllowReleasingMutex, allow_releasing_mutex);

class AcceptOlderLogonIdentifier : public patch::BasePatch
{
  public:
    void apply() const override
    {
        auto& game = game::GameHarness::get();
        real::GameLogicComponentOnLogonAccepted =
            game.findMemoryPattern<GameLogicComponentOnLogonAccepted_t>(
                pattern::GameLogicComponentOnLogonAccepted);
        game.hookFunctionPatternDirect<GameLogicComponentOnAdd_t>(pattern::GameLogicComponentOnAdd,
                                                                  GameLogicComponentOnAdd,
                                                                  &real::GameLogicComponentOnAdd);
    }

    static void __fastcall GameLogicComponentOnAdd(GameLogicComponent* this_)
    {
        real::GameLogicComponentOnAdd(this_);
        std::vector<std::string> logonFunctions = {"OnSuperMainStartAcceptLogonFB211131ddf",
                                                   "OnSuperMainStartAcceptLogonFB211131dd",
                                                   "OnSuperMainStartAcceptLogonFB211131d",
                                                   "OnSuperMainStartAcceptLogonFB211131",
                                                   "OnSuperMainStartAcceptLogonFB21113",
                                                   "OnSuperMainStartAcceptLogonFB2111",
                                                   "OnSuperMainStartAcceptLogonFB211",
                                                   "OnSuperMainStartAcceptLogonFB21",
                                                   "OnSuperMainStartAcceptLogonFB2",
                                                   "OnSuperMainStartAcceptLogonFB",
                                                   "OnSuperMainStartAcceptLogonF",
                                                   "OnSuperMainStartAcceptLogon",
                                                   "OnAcceptLogon",
                                                   "OnFirstLogonAccepted",
                                                   "OnLogonAccepted",
                                                   "OnInitialLogonAccepted"};

        for (auto logonFunction : logonFunctions)
        {
            this_->GetShared()
                ->GetFunction(logonFunction)
                ->sig_function.connect(1, boost::bind(real::GameLogicComponentOnLogonAccepted,
                                                      real::GetApp()->GetGameLogic(), _1));
        }
    }
};
REGISTER_USER_GAME_PATCH(AcceptOlderLogonIdentifier, accept_old_onsuperlogon);

// Valid 2-letter non-developer flags for V3.02 vanilla cache, sorted alphabetically.
static std::vector<std::string> validRegions = {
    "ad", "ae", "af", "ag", "ai", "al", "am", "an", "ao", "ar", "as", "at", "au", "aw", "ax", "az",
    "ba", "bb", "bd", "be", "bf", "bg", "bh", "bi", "bj", "bm", "bn", "bo", "br", "bs", "bt", "bv",
    "bw", "by", "bz", "ca", "cc", "cd", "cf", "cg", "ch", "ci", "ck", "cl", "cm", "cn", "co", "cr",
    "cs", "cu", "cv", "cx", "cy", "cz", "de", "dj", "dk", "dm", "do", "dz", "ec", "ee", "eg", "eh",
    "er", "es", "et", "fi", "fj", "fk", "fm", "fo", "fr", "ga", "gb", "gd", "ge", "gf", "gh", "gi",
    "gl", "gm", "gn", "gp", "gq", "gr", "gs", "gt", "gu", "gw", "gy", "hk", "hm", "hn", "hr", "ht",
    "hu", "id", "ie", "il", "in", "io", "iq", "ir", "is", "it", "jm", "jo", "jp", "ke", "kg", "kh",
    "ki", "km", "kn", "kp", "kr", "kw", "ky", "kz", "la", "lb", "lc", "lg", "li", "lk", "lr", "ls",
    "lt", "lu", "lv", "ly", "ma", "mc", "md", "me", "mg", "mh", "mk", "ml", "mm", "mn", "mo", "mp",
    "mq", "mr", "ms", "mt", "mu", "mv", "mw", "mx", "my", "mz", "na", "nc", "ne", "nf", "ng", "ni",
    "nl", "no", "np", "nr", "nu", "nz", "om", "pa", "pe", "pf", "pg", "ph", "pk", "pl", "pm", "pn",
    "pr", "ps", "pt", "pw", "py", "qa", "re", "ro", "rs", "ru", "rw", "sa", "sb", "sc", "sd", "se",
    "sg", "sh", "si", "sj", "sk", "sl", "sm", "sn", "so", "sr", "st", "sv", "sy", "sz", "tc", "td",
    "tf", "tg", "th", "tj", "tk", "tl", "tm", "tn", "to", "tr", "tt", "tv", "tw", "tz", "ua", "ug",
    "um", "us", "uy", "uz", "va", "vc", "ve", "vg", "vi", "vn", "vu", "wf", "ws", "ye", "yt", "za",
    "zm", "zw"};

class LocaleSwitcher : public patch::BasePatch
{
  public:
    void apply() const override
    {
        auto& game = game::GameHarness::get();

        game.hookFunctionPatternDirect<GetRegionString_t>(pattern::GetRegionString, GetRegionString,
                                                          &real::GetRegionString);

        Variant* pVariant = real::GetApp()->GetVar("osgt_qol_localeswitch_pref");
        if (pVariant->GetType() != Variant::TYPE_UINT32)
        {
            std::string currentRegion = real::GetRegionString();
            if (currentRegion.length() == 5)
                currentRegion = ToLowerCaseString(currentRegion.substr(3, 2));
            auto it = std::find(validRegions.begin(), validRegions.end(), currentRegion);
            if (it != validRegions.end())
                pVariant->Set(uint32_t(std::distance(validRegions.begin(), it)));
            else
                pVariant->Set(225U);
        }
        auto& optionsMgr = game::OptionsManager::get();
        optionsMgr.addMultiChoiceOptionDoubleButtons(
            "qol", "System", "osgt_qol_localeswitch_pref", "Locale for flag", validRegions,
            &LocaleSwitcherOnSelect, 80.0f, "`5(needs re-login to apply)``");
    }

    static void LocaleSwitcherOnSelect(VariantList* pVariant)
    {
        // Update the multichoice index
        Entity* pClickedEnt = pVariant->Get(1).GetEntity();
        Variant* pOptVar = real::GetApp()->GetVar("osgt_qol_localeswitch_pref");
        uint32_t idx = pOptVar->GetUINT32();
        std::string entName = pClickedEnt->GetName();
        // We are using double-buttons optiondef, we'll use "lower" values to decrement/increment by
        // one and the normal values to skip by first character of country code.
        if (entName == "back_lower")
        {
            if (idx == 0)
                idx = (uint32_t)validRegions.size() - 1;
            else
                idx--;
        }
        else if (entName == "next_lower")
        {
            if (idx >= validRegions.size() - 1)
                idx = 0;
            else
                idx++;
        }
        else if (entName == "back")
        {
            char group = validRegions[idx][0];
            while (true)
            {
                if (idx == 0)
                    idx = (uint32_t)validRegions.size() - 1;
                else
                    idx--;
                if (validRegions[idx][0] != group)
                {
                    break;
                }
            }
        }
        else if (entName == "next")
        {
            char group = validRegions[idx][0];
            while (true)
            {
                if (idx >= validRegions.size() - 1)
                    idx = 0;
                else
                    idx++;
                if (validRegions[idx][0] != group)
                {
                    break;
                }
            }
        }
        pOptVar->Set(idx);
        // Update the option label
        Entity* pTextLabel = pClickedEnt->GetParent()->GetEntityByName("txt");
        real::SetTextEntity(pTextLabel, validRegions[idx]);
    }

    static std::string __fastcall GetRegionString()
    {
        Variant* pVariant = real::GetApp()->GetVar("osgt_qol_localeswitch_pref");
        if (pVariant->GetUINT32() < validRegions.size())
        {
            return "en_" + validRegions[pVariant->GetUINT32()];
        }
        return real::GetRegionString();
    }
};
REGISTER_USER_GAME_PATCH(LocaleSwitcher, locale_switcher);