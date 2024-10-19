#include "game/game.hpp"
#include "patch/patch.hpp"

#include "game/struct/entity.hpp"
#include "utils/utils.hpp"

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

// GetFontAndScaleToFitThisLinesPerScreenY
// Params: &fontID, &fontScale, Lines
REGISTER_GAME_FUNCTION(GetFontAndScaleToFitThisLinesPerScreenY,
                       "48 89 5C 24 08 57 48 83 EC 50 0F 29 74 24 40 48 8B DA", __fastcall, void,
                       uint32_t&, float&, float);

// CreateInputTextEntity
// NOTE: Need to investigate the final 3 strings. They're not there originally in Proton SDK.
// Returns: Created Entity
// Params: Parent Entity, Entity Name, X, Y, Text, sizeX, sizeY, ??, ??, ??
REGISTER_GAME_FUNCTION(CreateInputTextEntity, "48 8B C4 55 41 54 41 55 41 56 41 57 48 8D 68 D1",
                       __fastcall, Entity*, Entity*, std::string, float, float, std::string, float,
                       float, std::string, std::string, std::string);

// CreateTextLabelEntity
// Returns: Created Entity
// Params: Parent Entity, Entity Name, X, Y, Text, sizeX, sizeY, ??, ??, ??
REGISTER_GAME_FUNCTION(CreateTextLabelEntity, "48 8B C4 55 57 41 56 48 8D 68 A9 48 81 EC D0 00",
                       __fastcall, Entity*, Entity*, std::string, float, float, std::string);

// SetupTextEntity
// Params: Target Entity, FontID, fontScale
REGISTER_GAME_FUNCTION(SetupTextEntity,
                       "48 8B C4 55 57 41 54 41 56 41 57 48 8D 68 A1 48 81 EC E0 00 00 00 48 C7 44",
                       __fastcall, void, Entity*, uint32_t, float);

// SlideScreen
// Params: Target Entity, bool (Slide in or out), Transition Time (MS), DelayMS
REGISTER_GAME_FUNCTION(SlideScreen, "48 8B C4 55 56 57 41 54 41 55 41 56 41 57 48 8D 68 98 48 81",
                       __fastcall, void, Entity*, bool, int, int);

static std::string serverOverride = "osgt1.cernodile.com";
class ServerSwitcher : public patch::BasePatch
{
  public:
    void apply() const override
    {
        auto& game = game::GameHarness::get();
        // Resolve UI functions - we will need these to properly construct our UI in OnlineMenu
        real::GetFontAndScaleToFitThisLinesPerScreenY =
            game.findMemoryPattern<GetFontAndScaleToFitThisLinesPerScreenY_t>(
                pattern::GetFontAndScaleToFitThisLinesPerScreenY);
        real::CreateInputTextEntity =
            game.findMemoryPattern<CreateInputTextEntity_t>(pattern::CreateInputTextEntity);
        real::CreateTextLabelEntity =
            game.findMemoryPattern<CreateTextLabelEntity_t>(pattern::CreateTextLabelEntity);
        real::SetupTextEntity = game.findMemoryPattern<SetupTextEntity_t>(pattern::SetupTextEntity);
        real::SlideScreen = game.findMemoryPattern<SlideScreen_t>(pattern::SlideScreen);

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
        // This function calls HTTPComponent with a dead link, causing it to conflict with InitConnection.
        // Safer to create a empty detour instead to completely dissolve this issue.
        game.hookFunctionPatternDirect<RequestIAPPricesFromHouston_t>(
            pattern::RequestIAPPricesFromHouston, RequestIAPPricesFromHouston,
            &real::RequestIAPPricesFromHouston);
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
        float marginY = pOnlineMenu->GetEntityByName("tankid_name_label")
                            ->GetShared()
                            ->GetVar("pos2d")
                            ->GetVector2()
                            .y;
        float marginX =
            pOnlineMenu->GetEntityByName("text")->GetShared()->GetVar("pos2d")->GetVector2().x;

        // Create our very own label
        Entity* pServerLabel = real::CreateTextLabelEntity(
            pOnlineMenu, "osgt_qol_server_label", marginX, marginY, "Server Data Location");
        // We need to call SetupTextEntity so it scales and lines up with the rest of UI.
        real::SetupTextEntity(pServerLabel, fontID, fontScale);

        // ..and an accompanying textbox
        // We will append the height of our label to be directly below it for marginY.
        // We will also make the textbox as wide as the label itself.
        Entity* pServerInput = real::CreateInputTextEntity(
            pOnlineMenu, "osgt_qol_server_input", marginX,
            marginY + pServerLabel->GetShared()->GetVar("size2d")->GetVector2().y, serverOverride,
            pServerLabel->GetShared()->GetVar("size2d")->GetVector2().x, 0.0, "", "", "");

        // Make it a bit neater by setting a max length and disallowing going through bounds.
        pServerInput->GetComponentByName("InputTextRender")->GetVar("inputLengthMax")->Set(32U);
        pServerInput->GetComponentByName("InputTextRender")
            ->GetVar("truncateTextIfNeeded")
            ->Set(1U);
        // InputTextEntity is still an TextEntity, we have to scale it to rest of UI with the helper
        // function.
        real::SetupTextEntity(pServerInput, fontID, fontScale);

        // We nopped the slidescreen earlier, put it back in place for the smooth transition.
        real::SlideScreen(pOnlineMenu, true, 500, 0);
        return pOnlineMenu;
    }

    static __int64 __fastcall OnlineMenuOnConnect(Entity* pEnt)
    {
        // pEnt is the "Connect" button. We can ask for "osgt_qol_server_input" here.
        Entity* pServerInput = pEnt->GetParent()->GetEntityByName("osgt_qol_server_input");
        if (pServerInput != nullptr)
        {
            // If the patch takes too long to load, the element may not even exist.
            std::string userInput =
                pServerInput->GetComponentByName("InputTextRender")->GetVar("text")->GetString();
            if (userInput.size() != 0)
            {
                serverOverride = userInput;
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
            pVList->Get(0).Set(serverOverride);
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