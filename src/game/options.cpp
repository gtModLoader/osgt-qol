#include "game.hpp"
#include "game/struct/entity.hpp"
#include "game/struct/entityutils.hpp"
#include "game/struct/renderutils.hpp"
#include "signatures.hpp"
#include "struct/rtrect.hpp"
#include "struct/variant.hpp"

// OptionsMenuAddContent
REGISTER_GAME_FUNCTION(OptionsMenuAddContent,
                       "48 8B C4 55 56 57 41 54 41 55 41 56 41 57 48 8D A8 38 FB FF FF 48 81 EC 90 "
                       "05 00 00 48 C7 45 08 FE FF FF FF 48 89 58 10 0F 29 70 B8 0F 29 78 A8 44 0F "
                       "29 40 98 44 0F 29 48 88 44 0F 29 90 78 FF FF FF 44 0F 29 98 68 FF FF FF 44 "
                       "0F 29 A0 58 FF FF FF 44 0F 29 A8 48 FF FF FF 48 8B 05 0F",
                       __fastcall, void, void* pScrollChild, void*, void*, void*);

// CreateSlider
// NOTE: Need to investigate the final strings. They're not there originally in Proton SDK.
REGISTER_GAME_FUNCTION(
    CreateSlider,
    "48 8B C4 55 53 56 57 41 54 41 55 41 56 41 57 48 8D A8 78 FE FF FF 48 81 EC 48 02 00 00 48 C7 "
    "45 30 FE FF FF FF 0F 29 70 A8 0F 29 78 98 44 0F 29 40 88 44 0F 29 88 78 FF FF FF 44 0F 29 A0",
    __fastcall, EntityComponent*, Entity* pBG, float x, float y, float sizeX,
    std::string buttonFileName, std::string left, std::string middle, std::string right,
    std::string, std::string, std::string, std::string);

// CreateCheckbox
// Again with trailing strings that aren't there in Proton.
REGISTER_GAME_FUNCTION(
    CreateCheckBox,
    "48 8B C4 55 53 56 57 41 54 41 55 41 56 41 57 48 8D A8 78 FE FF FF 48 81 EC 48 02 00 00 48 C7 "
    "45 30 FE FF FF FF 0F 29 70 A8 0F 29 78 98 44 0F 29 40 88 44 0F 29 88 78 FF FF FF 48 8B",
    __fastcall, Entity*, Entity* pBG, std::string name, std::string text, float x, float y,
    bool bChecked, uint32_t fontID, float fontScale, bool unclickable, std::string unk10,
    std::string unk11, std::string unk12)

// CreateTextButtonEntity
REGISTER_GAME_FUNCTION(CreateTextButtonEntity,
                       "48 8B C4 55 53 56 57 41 54 41 55 41 56 41 57 48 8D 68 B8 48 81 EC 08 01 00 "
                       "00 48 C7 45 A0 FE FF FF FF",
                       __fastcall, Entity*, Entity* pParentEnt, std::string name, float x, float y,
                       std::string text, bool bUnderline, int unk7 /*=0*/,
                       std::string controlGroup /*=""*/, bool unk9 /*=false*/,
                       std::string unk10 /*=""*/, bool unk11 /*=true*/, bool unk12 /*=false*/)

// ResizeScrollBounds
// Params: VariantList with Entity containing "Scroll" and "scroll_child"
REGISTER_GAME_FUNCTION(ResizeScrollBounds,
                       "48 8B C4 55 48 8D 68 A1 48 81 EC D0 00 00 00 48 C7 45 A7 FE FF FF FF 48 89 "
                       "58 10 48 89 70 18 48 89 78",
                       __fastcall, void, VariantList* pVList);

// iPadMapX
REGISTER_GAME_FUNCTION(iPadMapX, "66 0F 6E 0D 9C 76 4B 00 0F 5B C9 F3 0F 59 C8 F3 0F", __fastcall,
                       float, float);

// iPadMapY
REGISTER_GAME_FUNCTION(
    iPadMapY,
    "66 0F 6E 0D 68 76 4B 00 0F 5B C9 F3 0F 59 C8 F3 0F 2C C1 66 0F 6E C0 0F 5B C0 F3 0F 5E",
    __fastcall, float, float);

// iPhoneMapX
REGISTER_GAME_FUNCTION(iPhoneMapX, "66 0F 6E 0D 7C 75 4B 00 0F 5B C9 F3 0F 59 C8 F3 0F 2C C1",
                       __fastcall, float, float);

// iPhoneMapY
REGISTER_GAME_FUNCTION(iPhoneMapY,
                       "66 0F 6E 0D 08 75 4B 00 0F 5B C9 F3 0F 59 C8 F3 0F 2C C1 66 0F 6E C0 0F 5B "
                       "C0 F3 0F 5E 05 AE 28",
                       __fastcall, float, float);

REGISTER_GAME_FUNCTION(AddFocusIfNeeded,
                       "40 55 56 57 41 54 41 55 41 56 41 57 48 8D 6C 24 D9 48 81 EC A0 00 00 00",
                       __fastcall, void, Entity*, bool, int);

// SlideScreen
// Params: Target Entity, bool (Slide in or out), Transition Time (MS), DelayMS
REGISTER_GAME_FUNCTION(SlideScreen,
                       "48 8B C4 55 56 57 41 54 41 55 41 56 41 57 48 8D 68 98 48 81 EC 30 01 00 00 "
                       "48 C7 44 24 50 FE FF FF FF",
                       __fastcall, void, Entity*, bool, int, int);

REGISTER_GAME_FUNCTION(
    TouchHandlerComponent,
    "48 89 4C 24 08 53 48 83 EC 50 48 C7 44 24 20 FE FF FF FF 48 8B D9 E8 ? ? ? ? 90 48 8D ? ? ? ? "
    "? 48 89 03 33 C9 48 89 8B 10 01 00 00 48 89 8B 08 01 00 00 48 C7 44 24 40 0F",
    __fastcall, EntityComponent*, void*);
REGISTER_GAME_FUNCTION(
    TouchHandlerArcadeComponent,
    "48 89 4C 24 08 53 48 83 EC 50 48 C7 44 24 20 FE FF FF FF 48 8B D9 E8 ? ? ? ? 90 48 8D ? ? ? ? "
    "? 48 89 03 33 C9 48 89 8B 18 01 00 00 48 89 8B 10 01 00 00",
    __fastcall, EntityComponent*, void*);
REGISTER_GAME_FUNCTION(FilterInputComponent,
                       "48 89 4C 24 08 53 48 83 EC 50 48 C7 44 24 20 FE FF FF FF 48 8B D9 E8 ? ? ? "
                       "? 90 48 8D ? ? ? ? ? 48 89 03 33 C9 48 89 8B D8 00 00 00 48 89 8B E0 00 00 "
                       "00 66 89 8B E8 00 00 00 48 89 8B EC 00 00 00",
                       __fastcall, EntityComponent*, void*);

REGISTER_GAME_FUNCTION(MessageManagerCallEntityFunction,
                       "E8 ? ? ? ? 90 48 8D 4D B8 E8 ? ? ? ? 48 8B 8E 18 01 00 00 B2 01",
                       __fastcall, void, void*, Entity*, int, std::string, VariantList*, int);

REGISTER_GAME_FUNCTION(GetMessageManager,
                       "E8 ? ? ? ? 48 8B C8 89 5C 24 28 48 8D 85 00 05 00 00 48 89 44 24 20 4C 8D "
                       "8D E0 03 00 00 41 B8 A4 06 00 00 48 8B D6",
                       __fastcall, void*);
REGISTER_GAME_FUNCTION(DisableAllButtonsEntity,
                       "40 55 56 57 48 8D 6C 24 B9 48 81 EC 90 00 00 00 48 C7 45 D7 FE FF FF FF 48 "
                       "89 9C 24 C0 00 00 00",
                       __fastcall, void, Entity*, bool);
REGISTER_GAME_FUNCTION(OptionsMenuCreate,
                       "48 8B C4 55 57 41 54 41 56 41 57 48 8D A8 78 FC FF FF 48 81 EC 60 04 00 00 "
                       "48 C7 45 78 FE FF FF FF 48 89 58 10 48 89 70 18 0F 29 70 C8 0F 29 78 B8",
                       __fastcall, void, Entity*, bool);
REGISTER_GAME_FUNCTION(AddHotkeyToButton,
                       "40 57 48 83 EC 60 48 C7 44 24 20 FE FF FF FF 48 89 5C 24 78 48 8B ? ? ? ? "
                       "? 48 33 C4 48 89 44 24 50 8B FA 48 8B D9 48 85 C9 75 07 33 C0 E9 ? ? ? ? "
                       "B9 F0 00 00 00 E8 ? ? ? ? 48 89 44 24 28 48 85 C0 74 09",
                       __fastcall, void, Entity*, int);
REGISTER_GAME_FUNCTION(SetButtonStyleEntity,
                       "E8 ? ? ? ? 45 33 C9 41 B8 F4 01 00 00 B2 01 48 8B CE E8 ? ? ? ? 90 48 8D "
                       "8D 20 01 00 00 E8 ? ? ? ? 90 48 8B 55 98",
                       __fastcall, void, Entity*, int);
struct ScrollSettings
{
    CL_Vec2f vPos = {0, 0};
    CL_Vec2f vSize = {0, 0};
    char m_unk = 0;
    char m_unk2 = 0;
    uint8_t m_unk3[6];
    Entity* m_renderScissor = nullptr;
    bool m_fingerTracking = true;
    bool m_scrollWidth = false;
};
REGISTER_GAME_FUNCTION(ScrollScroll,
                       "40 57 48 83 EC 50 48 C7 44 24 20 FE FF FF FF 48 89 5C 24 70 48 8B ? ? ? ? "
                       "? 48 33 C4 48 89 44 24 48 48 8B DA 48 8B F9 48 C7 44 24 40 0F 00 00 00 48 "
                       "C7 44 24 38 00 00 00 00 C6 44 24 28 00 41 B8 06 00 00 00 48 8D ? ? ? ? ? "
                       "48 8D 4C 24 28 E8 ? ? ? ? 90 4C 8B C3 48 8D 54 24 28 48 8B CF E8",
                       __fastcall, Entity*, void* pMem, ScrollSettings*);
REGISTER_GAME_FUNCTION(SetScrollProgressEntity,
                       "48 8B C4 55 48 8D A8 78 FE FF FF 48 81 EC 80 02 00 00 48 C7 44 24 20 FE FF "
                       "FF FF 48 89 58 18 48 89 78 20 48 8B",
                       __fastcall, void, Entity*, CL_Vec2f*);
REGISTER_GAME_FUNCTION(OnMenuButtonPressed,
                       "40 53 41 56 41 57 48 83 EC 50 48 8B ? ? ? ? ? 48 33 C4 48 89 44 24 40",
                       __fastcall, void, VariantList*);
REGISTER_GAME_FUNCTION(OnGemButtonPressed,
                       "40 53 48 83 EC 50 48 C7 44 24 20 FE FF FF FF 48 8B ? ? ? ? ? 48 33 C4 48 "
                       "89 44 24 48 48 8D ? ? ? ? ? E8 ? ? ? ? E8",
                       __fastcall, void, VariantList*);
namespace game
{

OptionsManager& OptionsManager::get()
{
    static OptionsManager instance;
    return instance;
}

void game::OptionsManager::initialize()
{
    auto& game = game::GameHarness::get();

    // Resolve our needed functions
    real::CreateSlider = game.findMemoryPattern<CreateSlider_t>(pattern::CreateSlider);
    real::CreateCheckBox = game.findMemoryPattern<CreateCheckBox_t>(pattern::CreateCheckBox);
    real::CreateTextButtonEntity =
        game.findMemoryPattern<CreateTextButtonEntity_t>(pattern::CreateTextButtonEntity);
    real::iPadMapX = game.findMemoryPattern<iPadMapX_t>(pattern::iPadMapX);
    real::iPadMapY = game.findMemoryPattern<iPadMapY_t>(pattern::iPadMapY);
    real::iPhoneMapX = game.findMemoryPattern<iPhoneMapX_t>(pattern::iPhoneMapX);
    real::iPhoneMapY = game.findMemoryPattern<iPhoneMapY_t>(pattern::iPhoneMapY);
    real::ResizeScrollBounds =
        game.findMemoryPattern<ResizeScrollBounds_t>(pattern::ResizeScrollBounds);
    real::AddFocusIfNeeded = game.findMemoryPattern<AddFocusIfNeeded_t>(pattern::AddFocusIfNeeded);
    real::SlideScreen = game.findMemoryPattern<SlideScreen_t>(pattern::SlideScreen);
    real::TouchHandlerComponent =
        game.findMemoryPattern<TouchHandlerComponent_t>(pattern::TouchHandlerComponent);
    real::TouchHandlerArcadeComponent =
        game.findMemoryPattern<TouchHandlerArcadeComponent_t>(pattern::TouchHandlerArcadeComponent);
    real::FilterInputComponent =
        game.findMemoryPattern<FilterInputComponent_t>(pattern::FilterInputComponent);
    real::ScrollScroll = game.findMemoryPattern<ScrollScroll_t>(pattern::ScrollScroll);
    real::SetScrollProgressEntity =
        game.findMemoryPattern<SetScrollProgressEntity_t>(pattern::SetScrollProgressEntity);
    real::DisableAllButtonsEntity =
        game.findMemoryPattern<DisableAllButtonsEntity_t>(pattern::DisableAllButtonsEntity);
    real::OptionsMenuCreate =
        game.findMemoryPattern<OptionsMenuCreate_t>(pattern::OptionsMenuCreate);
    real::AddHotkeyToButton =
        game.findMemoryPattern<AddHotkeyToButton_t>(pattern::AddHotkeyToButton);

    real::GetMessageManager = utils::resolveRelativeCall<GetMessageManager_t>(
        game.findMemoryPattern<uint8_t*>(pattern::GetMessageManager));
    real::MessageManagerCallEntityFunction =
        utils::resolveRelativeCall<MessageManagerCallEntityFunction_t>(
            game.findMemoryPattern<uint8_t*>(pattern::MessageManagerCallEntityFunction));
    real::SetButtonStyleEntity = utils::resolveRelativeCall<SetButtonStyleEntity_t>(
        game.findMemoryPattern<uint8_t*>(pattern::SetButtonStyleEntity));
    // Hook
    game.hookFunctionPatternDirect<OptionsMenuAddContent_t>(
        pattern::OptionsMenuAddContent, OptionsMenuAddContent, &real::OptionsMenuAddContent);
    game.hookFunctionPatternDirect<OnMenuButtonPressed_t>(
        pattern::OnMenuButtonPressed, OnMenuButtonPressed, &real::OnMenuButtonPressed);
    game.hookFunctionPatternDirect<OnGemButtonPressed_t>(
        pattern::OnGemButtonPressed, OnGemButtonPressed, &real::OnGemButtonPressed);
}

void OptionsManager::renderSlider(OptionsManager::GameOption& optionDef, void* pEntityPtr,
                                  float vPosX, float& vPosY)
{
    Entity* pEnt = reinterpret_cast<Entity*>(pEntityPtr);

    // The client uses this real::iPhoneMapY(20.0) * 2 in main page, but since it's not really
    // scaled in height, we'll opt to use a static padding.
    vPosY += 100.0f;

    // We really don't want it to be smaller than vanilla 1024x768 iPhoneMapX(360.0f), but we want
    // it to be leaner in high res.
    float vSixeX = real::iPhoneMapX(260.0f);
    if (vSixeX < 768.80f)
        vSixeX = 768.80f;

    // The final 4 string args aren't there in Proton, but they are in client. Currently they don't
    // seem to have much use.
    EntityComponent* pSliderComp =
        real::CreateSlider(pEnt, vPosX, vPosY, vSixeX, "interface/slider_button.rttex", "Min",
                           optionDef.displayName, "Max", "", "", "", "");

    CL_Vec2f vSliderSize = pSliderComp->GetParent()->GetVar("size2d")->GetVector2();
    // If we have a hint string assigned, we'll display it below the slider, helps with centering
    // and width issues.
    if (optionDef.extraInfo.size() > 0)
    {
        Entity* pTxtEnt = real::CreateTextLabelEntity(
            pEnt, "txt", vPosX + (vSixeX / 2), vPosY + vSliderSize.y + 25.0f, optionDef.extraInfo);
        real::SetupTextEntity(pTxtEnt, 0, 0);
        pTxtEnt->GetVar("alignment")->Set(5U);

        vPosY += pTxtEnt->GetVar("size2d")->GetVector2().y + 25.0f;
    }

    // Move the slider button according to variable in SharedDB.
    pSliderComp->GetVar("progress")->Set(real::GetApp()->GetVar(optionDef.varName)->GetFloat());
    if (optionDef.signal != nullptr)
        pSliderComp->GetVar("progress")
            ->GetSigOnChanged()
            ->connect(reinterpret_cast<VariantCallback>(optionDef.signal));

    // Adjust margin for next option.
    vPosY += vSliderSize.y;
}

void OptionsManager::renderCheckbox(OptionsManager::GameOption& optionDef, void* pEntityPtr,
                                    float vPosX, float& vPosY)
{
    Entity* pEnt = reinterpret_cast<Entity*>(pEntityPtr);

    // Pad before next checkbox, client originally uses real::iPhoneMapY(20.0) in main page, but on
    // large screens it's kinda wack.
    vPosY += 30.0f;

    uint32_t fontID = 0;
    float fontScale = 1.0f;

    // Retrieve our variant as we need to set checkbox value as its created
    Variant* pVariant = real::GetApp()->GetVar(optionDef.varName);

    // The final 4 string args aren't there in Proton, but they are in client. Currently they don't
    // seem to have much use.
    Entity* pCheckbox =
        real::CreateCheckBox(pEnt, optionDef.varName, optionDef.displayName, vPosX, vPosY,
                             pVariant->GetUINT32() == 1, fontID, fontScale, false, "", "", "");

    if (optionDef.signal != nullptr)
        pCheckbox->GetFunction("OnButtonSelected")
            ->sig_function.connect(reinterpret_cast<VariantListCallback>(optionDef.signal));

    // Adjust margin for next option.
    vPosY += pCheckbox->GetVar("size2d")->GetVector2().y;
}

void OptionsManager::renderMultiChoice(OptionsManager::GameOption& optionDef, void* pEntityPtr,
                                       float vPosX, float& vPosY)
{
    if (optionDef.displayOptions->size() == 0)
        return;

    float vSizeX = real::iPhoneMapX(180.0f) + real::iPhoneMapX(optionDef.vModSizeX);

    Entity* pEnt = reinterpret_cast<Entity*>(pEntityPtr);

    // Namespace it for multichoice callbacks to find label easier
    Entity* pMCEnt = new Entity("opt_mc_container");
    pEnt->AddEntity(pMCEnt);

    // Pad it similarly to rest of options
    vPosY += 30.0f;

    // Retrieve fontscale
    uint32_t fontID;
    float fontScale;
    real::GetFontAndScaleToFitThisLinesPerScreenY(fontID, fontScale, 28);

    // Create the option label
    Entity* pOptionsLabel = real::CreateTextLabelEntity(pMCEnt, "optLabel", vPosX + (vSizeX / 2),
                                                        vPosY, optionDef.displayName);
    real::SetupTextEntity(pOptionsLabel, 0, 0);
    pOptionsLabel->GetVar("alignment")->Set(ALIGNMENT_UPPER_CENTER);

    vPosY += pOptionsLabel->GetVar("size2d")->GetVector2().y + real::iPhoneMapY(5.0);

    // Retrieve our variant as we need to set the shown option label with it.
    uint32_t idx = real::GetApp()->GetVar(optionDef.varName)->GetUINT32();
    if (idx > optionDef.displayOptions->size())
    {
        real::GetApp()->GetVar(optionDef.varName)->Set(0U);
        idx = 0;
    }

    // Re-scale for the multichoice modal itself.
    Rectf screenRect;
    real::GetScreenRect(screenRect);
    if (screenRect.bottom <= 1080)
        real::GetFontAndScaleToFitThisLinesPerScreenY(fontID, fontScale, 24);
    else
        real::GetFontAndScaleToFitThisLinesPerScreenY(fontID, fontScale, 30);

    // They added some wack trailing args we don't care about to end of TextButtonEntity.
    Entity* pBackButton = real::CreateTextButtonEntity(pMCEnt, "back", vPosX, vPosY, " <<  ", false,
                                                       0, "", 0, "", 1, 0);
    real::SetupTextEntity(pBackButton, fontID, fontScale);
    real::AddBMPRectAroundEntity(pBackButton, 0xccb887ff, 0xccb887ff, real::iPadMapY(5.0), true,
                                 fontScale, fontID, false);
    SetTextShadowColor(pBackButton, 150);

    if (optionDef.type == OPTION_MULTICHOICE_DUALBUTTONS)
    {
        Entity* pBackButtonLower = real::CreateTextButtonEntity(
            pMCEnt, "back_lower",
            vPosX + pBackButton->GetVar("size2d")->GetVector2().x + real::iPadMapY(10.0), vPosY,
            " <  ", false, 0, "", 0, "", 1, 0);
        real::SetupTextEntity(pBackButtonLower, fontID, fontScale);
        real::AddBMPRectAroundEntity(pBackButtonLower, 0xccb887ff, 0xccb887ff, real::iPadMapY(5.0),
                                     true, fontScale, fontID, false);
        SetTextShadowColor(pBackButtonLower, 150);
        if (optionDef.signal != nullptr)
            pBackButtonLower->GetFunction("OnButtonSelected")
                ->sig_function.connect(reinterpret_cast<VariantListCallback>(optionDef.signal));
    }

    Entity* pTextLabel =
        real::CreateTextLabelEntity(pMCEnt, "txt", vPosX + (vSizeX / 2),
                                    vPosY + (pBackButton->GetVar("size2d")->GetVector2().y / 2),
                                    (*optionDef.displayOptions)[idx]);
    real::SetupTextEntity(pTextLabel, 0, 0);
    pTextLabel->GetVar("alignment")->Set(ALIGNMENT_CENTER);

    Entity* pNextButton = real::CreateTextButtonEntity(pMCEnt, "next", vPosX + vSizeX, vPosY,
                                                       " >> ", false, 0, "", 0, "", 1, 0);
    pNextButton->GetVar("alignment")->Set(ALIGNMENT_UPPER_RIGHT);
    real::SetupTextEntity(pNextButton, fontID, fontScale);
    real::AddBMPRectAroundEntity(pNextButton, 0xccb887ff, 0xccb887ff, real::iPadMapY(5.0), true,
                                 fontScale, fontID, false);
    SetTextShadowColor(pNextButton, 150);

    if (optionDef.type == OPTION_MULTICHOICE_DUALBUTTONS)
    {
        Entity* pNextButtonLower = real::CreateTextButtonEntity(
            pMCEnt, "next_lower",
            vPosX + vSizeX - pNextButton->GetVar("size2d")->GetVector2().x - real::iPadMapY(10.0),
            vPosY, " >  ", false, 0, "", 0, "", 1, 0);
        pNextButtonLower->GetVar("alignment")->Set(ALIGNMENT_UPPER_RIGHT);
        real::SetupTextEntity(pNextButtonLower, fontID, fontScale);
        real::AddBMPRectAroundEntity(pNextButtonLower, 0xccb887ff, 0xccb887ff, real::iPadMapY(5.0),
                                     true, fontScale, fontID, false);
        SetTextShadowColor(pNextButtonLower, 150);
        if (optionDef.signal != nullptr)
            pNextButtonLower->GetFunction("OnButtonSelected")
                ->sig_function.connect(reinterpret_cast<VariantListCallback>(optionDef.signal));
    }

    if (optionDef.signal != nullptr)
    {
        pBackButton->GetFunction("OnButtonSelected")
            ->sig_function.connect(reinterpret_cast<VariantListCallback>(optionDef.signal));
        pNextButton->GetFunction("OnButtonSelected")
            ->sig_function.connect(reinterpret_cast<VariantListCallback>(optionDef.signal));
    }

    CL_Vec2f vButtonSize = pNextButton->GetVar("size2d")->GetVector2();
    // If we have a hint string assigned, we'll display it below the option, helps with centering
    // and width issues.
    if (optionDef.extraInfo.size() > 0)
    {
        Entity* pTxtEnt = real::CreateTextLabelEntity(
            pEnt, "txt", vPosX + (vSizeX / 2), vPosY + vButtonSize.y + 25.0f, optionDef.extraInfo);
        real::SetupTextEntity(pTxtEnt, 0, 0);
        pTxtEnt->GetVar("alignment")->Set(ALIGNMENT_UPPER_CENTER);

        vPosY += pTxtEnt->GetVar("size2d")->GetVector2().y + 25.0f;
    }

    // Adjust margin for next option.
    vPosY += vButtonSize.y;
}

void OptionPageOnSelect(VariantList* pVL)
{
    if (pVL->Get(1).GetEntity()->GetName() == "Back")
    {
        // Kill the hotkey from repeat-firing, otherwise spamming esc can cause some weirdness.
        pVL->Get(1).GetEntity()->RemoveComponentByName("SelectButtonWithCustomInput");

        // Kill rest of the menu and recreate original options.
        Entity* pParentEnt = pVL->Get(1).GetEntity()->GetParent();
        real::SlideScreen(pParentEnt, 0, 500, 0);
        real::MessageManagerCallEntityFunction(real::GetMessageManager(), pParentEnt, 500,
                                               "OnDelete", 0, 1);
        real::OptionsMenuCreate(real::GetApp()->m_entityRoot->GetEntityByName("GUI"),
                                pParentEnt->GetVar("FromMainMenu")->GetUINT32() == 1);
        return;
    }
}

void OptionsManager::HandleOptionPageButton(VariantList* pVL)
{
    // Kill the main options menu so we don't click-through it.
    Entity* pParentEnt = pVL->Get(1).GetEntity()->GetParent()->GetParent()->GetParent();
    uint32_t bFromMainMenu = pParentEnt->GetVar("FromMainMenu")->GetUINT32();

    // Kill the hotkey from repeat-firing, otherwise spamming esc can cause some weirdness.
    Entity* pParentBackBtn = pParentEnt->GetEntityByName("Back");
    pParentBackBtn->RemoveComponentByName("SelectButtonWithCustomInput");

    real::DisableAllButtonsEntity(pParentEnt, true);
    real::SlideScreen(pParentEnt, 0, 500, 0);
    real::MessageManagerCallEntityFunction(real::GetMessageManager(), pParentEnt, 500, "OnDelete",
                                           0, 1);

    // Construct our own OptionsPage Entity
    Entity* pClickedEnt = pVL->Get(1).GetEntity();
    auto& optionsMgr = game::OptionsManager::get();
    if (optionsMgr.optionPages.find(pClickedEnt->GetName()) == optionsMgr.optionPages.end())
        return;
    Entity* pOverEnt =
        real::CreateOverlayEntity(real::GetApp()->m_entityRoot->GetEntityByName("GUI"),
                                  "OptionsPage", "interface/large/generic_menu.rttex", 0, 0);
    real::AddFocusIfNeeded(pOverEnt, true, 500);

    // This is necessary for Options mainly to know when we re-create it later.
    pOverEnt->GetVar("FromMainMenu")->Set(bFromMainMenu);

    // We will size it to match our screen.
    Rectf screenRect;
    real::GetScreenRect(screenRect);
    CL_Vec2f vScreenSize(screenRect.right, screenRect.bottom);
    real::EntitySetScaleBySize(pOverEnt, vScreenSize, 0, 0);

    // Setup the dimensions of where the scroll area will go
    CL_Vec2f vTextAreaPos = CL_Vec2f((float)(int)real::iPhoneMapX(2), real::iPhoneMapY(10));
    float offsetFromBottom = real::iPhoneMapY(48);
    float offsetFromRight = real::iPhoneMapY(0);
    CL_Vec2f vTextAreaBounds =
        (vScreenSize - CL_Vec2f(offsetFromRight, offsetFromBottom)) - vTextAreaPos;

    // Create scroll itself - using vanilla Proton components won't work as Growtopia modified them
    // heavily. They will function, but you can't use sliders properly with vanilla ones.
    ScrollSettings scrollSet;
    scrollSet.vPos = vTextAreaPos;
    scrollSet.vSize = vTextAreaBounds;
    Entity* pScroll = real::ScrollScroll(operator new(0x1b8), &scrollSet);
    pOverEnt->AddEntity(pScroll);
    Entity* pScrollChild = pScroll->GetEntityByName("scroll_child");

    OptionsManager::OptionsPage& page = optionsMgr.optionPages[pClickedEnt->GetName()];
    float vPosX = real::iPhoneMapX(5.0);
    float vPosY = 0;

    uint32_t fontID;
    float fontScale;
    real::GetFontAndScaleToFitThisLinesPerScreenY(fontID, fontScale, 11);

    // Blit the page title
    Entity* pPageLabel = real::CreateTextLabelEntity(
        pScrollChild, "title", vPosX, vPosY,
        page.fancyName.size() == 0 ? pClickedEnt->GetName() : page.fancyName);
    real::SetupTextEntity(pPageLabel, fontID, 0);
    vPosY += pPageLabel->GetVar("size2d")->GetVector2().y;

    for (auto& section : optionsMgr.optionPages[pClickedEnt->GetName()].sections)
    {
        Entity* pSectionLabel =
            real::CreateTextLabelEntity(pScrollChild, "section", vPosX, vPosY, section.first);
        real::SetupTextEntity(pSectionLabel, fontID, 0);
        vPosY += pSectionLabel->GetVar("size2d")->GetVector2().y;
        for (auto& option : section.second)
        {
            switch (option.type)
            {
            case game::OptionsManager::OPTION_SLIDER:
            {
                renderSlider(option, (void*)pScrollChild, vPosX, vPosY);
                break;
            }
            case game::OptionsManager::OPTION_CHECKBOX:
            {
                renderCheckbox(option, (void*)pScrollChild, vPosX, vPosY);
                break;
            }
            case game::OptionsManager::OPTION_MULTICHOICE:
            case game::OptionsManager::OPTION_MULTICHOICE_DUALBUTTONS:
            {
                renderMultiChoice(option, (void*)pScrollChild, vPosX, vPosY);
                break;
            }
            default:
                break;
            }
        }
        vPosY += real::iPhoneMapY(20.0);
    }

    // Resize scroll bounds before sending.
    VariantList vl(pScrollChild->GetParent()->GetParent());
    real::ResizeScrollBounds(&vl);

    // and blit a Back button.
    real::GetFontAndScaleToFitThisLinesPerScreenY(fontID, fontScale, 13);
    Entity* pBackButton = real::CreateTextButtonEntity(pOverEnt, "Back", real::iPhoneMapX(25.0),
                                                       screenRect.bottom - real::iPhoneMapY(40.0),
                                                       "Back", false, 0, "", 0, "", 1, 0);
    pBackButton->GetFunction("OnButtonSelected")->sig_function.connect(&OptionPageOnSelect);
    real::SetupTextEntity(pBackButton, fontID, fontScale);
    real::AddBMPRectAroundEntity(pBackButton, 0xccb887ff, 0xccb887ff, real::iPadMapY(20.0), true,
                                 fontScale, fontID, false);
    SetTextShadowColor(pBackButton, 150);
    real::AddHotkeyToButton(pBackButton, 500000);
    real::SetButtonStyleEntity(pBackButton, 1);

    real::SlideScreen(pOverEnt, 1, 500, 0);
}

void OptionsManager::HandleOptionPageScrollButton(VariantList* pVL)
{
    CL_Vec2f progress = {0.0f, 1.0f};
    real::SetScrollProgressEntity(pVL->Get(1).GetEntity()->GetParent()->GetParent(), &progress);
}

void OptionsManager::OptionsMenuAddContent(void* pEnt, void* unk2, void* unk3, void* unk4)
{
    // Let the game construct options menu for us.
    real::OptionsMenuAddContent(pEnt, unk2, unk3, unk4);
    Entity* pScrollChild = reinterpret_cast<Entity*>(pEnt);

    // Patch the uncentered buttons located after vanilla checkboxes.
    int i = 0;
    for (const auto& ent : *pScrollChild->GetChildren())
    {
        // Bit lazy, but I don't want to do 5 string comparisons every loop cycle
        // or iterate over the list 5 separate times.
        if (i > 0 || ent->GetName() == "support")
        {
            // Fix the alignment manually. It's not exactly "perfect", but it beats vanilla
            // alignment.
            CL_Vec2f pos2d = ent->GetVar("pos2d")->GetVector2();
            pos2d.x += real::iPadMapY(10.0f);
            ent->GetVar("pos2d")->Set(pos2d);
            if (++i >= 5)
                break;
        }
    }

    // Use final or penultimate element of the menu for anchoring our elements to.
    Entity* pLastEntity = pScrollChild->GetChildren()->back();
    if (pLastEntity->GetName() == "SkinSelected")
        pLastEntity = *std::prev(pScrollChild->GetChildren()->end(), 2);
    // I know, iPhoneMapX used on a Y pos, but that's apparently what the client does.
    float vPosY = real::iPhoneMapX(5.0);
    vPosY += pLastEntity->GetVar("pos2d")->GetVector2().y;
    vPosY += pLastEntity->GetVar("size2d")->GetVector2().y;
    float vPosX = real::iPhoneMapX(5.0);

    // Create scaling for our label
    uint32_t fontID;
    float fontScale;
    real::GetFontAndScaleToFitThisLinesPerScreenY(fontID, fontScale, 16);

    // Create a quick-jump to bottom button next to parental settings
    Entity* pParentalSettings = pScrollChild->GetEntityByName("Parental");
    CL_Vec2f vParentalSize = pParentalSettings->GetVar("size2d")->GetVector2();
    CL_Vec2f vParentalPos = pParentalSettings->GetVar("pos2d")->GetVector2();
    Entity* pButtonEnt = real::CreateTextButtonEntity(
        pScrollChild, "JumpButton", vPosX + vParentalSize.x + real::iPadMapX(40.0f),
        vParentalPos.y + real::iPadMapY(10.0f), "Jump to Bottom", false, 0, "", 0, "", 1, 0);
    real::SetupTextEntity(pButtonEnt, fontID, fontScale);
    real::AddBMPRectAroundEntity(pButtonEnt, 0xfbe3a5ff, 0xfbe3a5ff, real::iPadMapX(5.0), true,
                                 fontScale, fontID, false);
    SetTextShadowColor(pButtonEnt, 150);
    pButtonEnt->GetFunction("OnButtonSelected")->sig_function.connect(HandleOptionPageScrollButton);

    // Create our very own label for modded options.
    // 11 lines on desktop, 9 on mobile platforms.
    real::GetFontAndScaleToFitThisLinesPerScreenY(fontID, fontScale, 11);
    Entity* pOptionsLabel = real::CreateTextLabelEntity(pScrollChild, "osgt_qol_options", vPosX,
                                                        vPosY, "Modded Options:");
    // Set scaling for label.
    real::SetupTextEntity(pOptionsLabel, fontID, 0);

    // Move cursor further down by label size.
    vPosY += pOptionsLabel->GetVar("size2d")->GetVector2().y + real::iPhoneMapY(10.0);

    // Lets start building our options.
    auto& optionsMgr = game::OptionsManager::get();

    real::GetFontAndScaleToFitThisLinesPerScreenY(fontID, fontScale, 20);
    for (auto& page : optionsMgr.optionPages)
    {
        Entity* pButtonEnt = real::CreateTextButtonEntity(
            pScrollChild, page.first, vPosX, vPosY,
            page.second.fancyName.size() > 0 ? page.second.fancyName : page.first, false, 0, "", 0,
            "", 1, 0);
        real::SetupTextEntity(pButtonEnt, fontID, fontScale);
        real::AddBMPRectAroundEntity(pButtonEnt, 0xccb887ff, 0xccb887ff, real::iPadMapY(20.0), true,
                                     fontScale, fontID, false);
        SetTextShadowColor(pButtonEnt, 150);
        CL_Vec2f pos2d = pButtonEnt->GetVar("pos2d")->GetVector2();
        pos2d.x += real::iPadMapY(10.0f);
        pButtonEnt->GetVar("pos2d")->Set(pos2d);
        pButtonEnt->GetFunction("OnButtonSelected")->sig_function.connect(HandleOptionPageButton);
        vPosY += real::iPadMapY(20.0f) + real::iPhoneMapY(20.0f) + real::iPhoneMapX(5.0f);
    }
    for (auto& option : optionsMgr.rootOptions)
    {
        switch (option.type)
        {
        case game::OptionsManager::OPTION_SLIDER:
        {
            renderSlider(option, (void*)pScrollChild, vPosX, vPosY);
            break;
        }
        case game::OptionsManager::OPTION_CHECKBOX:
        {
            renderCheckbox(option, (void*)pScrollChild, vPosX, vPosY);
            break;
        }
        case game::OptionsManager::OPTION_MULTICHOICE:
        case game::OptionsManager::OPTION_MULTICHOICE_DUALBUTTONS:
        {
            renderMultiChoice(option, (void*)pScrollChild, vPosX, vPosY);
            break;
        }
        default:
            break;
        }
    }

    // Finally resize.
    VariantList vl(pScrollChild->GetParent()->GetParent());
    real::ResizeScrollBounds(&vl);
    return;
}

void OptionsManager::OnMenuButtonPressed(VariantList* pVL)
{
    // Do we care if anyone else needs OnMenuButtonPressed...? I guess they can create a signal for
    // it if they need it.
    if (real::GetApp()->m_entityRoot->GetEntityByNameRecursively("OptionsMenu") ||
        real::GetApp()->m_entityRoot->GetEntityByNameRecursively("OptionsPage"))
        return;
    real::OnMenuButtonPressed(pVL);
}

void OptionsManager::OnGemButtonPressed(VariantList* pVL)
{
    // Same comment as for MenuButtonPressed.
    if (real::GetApp()->m_entityRoot->GetEntityByNameRecursively("OptionsMenu") ||
        real::GetApp()->m_entityRoot->GetEntityByNameRecursively("OptionsPage"))
        return;
    real::OnGemButtonPressed(pVL);
}

}; // namespace game