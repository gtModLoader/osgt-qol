#include "game.hpp"
#include "game/struct/entity.hpp"
#include "signatures.hpp"
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
    real::iPadMapX = game.findMemoryPattern<iPadMapX_t>(pattern::iPadMapX);
    real::iPadMapY = game.findMemoryPattern<iPadMapY_t>(pattern::iPadMapY);
    real::iPhoneMapX = game.findMemoryPattern<iPhoneMapX_t>(pattern::iPhoneMapX);
    real::iPhoneMapY = game.findMemoryPattern<iPhoneMapY_t>(pattern::iPhoneMapY);
    real::ResizeScrollBounds =
        game.findMemoryPattern<ResizeScrollBounds_t>(pattern::ResizeScrollBounds);

    // Hook
    game.hookFunctionPatternDirect<OptionsMenuAddContent_t>(
        pattern::OptionsMenuAddContent, OptionsMenuAddContent, &real::OptionsMenuAddContent);
}

void OptionsManager::renderSlider(OptionsManager::GameOption& optionDef, void* pEntityPtr,
                                  float vPosX, float& vPosY)
{
    Entity* pEnt = reinterpret_cast<Entity*>(pEntityPtr);

    // The client uses this for padding, lets do the same for consistency.
    vPosY += real::iPhoneMapY(20.0) * 2;

    // The final 4 string args aren't there in Proton, but they are in client. Currently they don't
    // seem to have much use.
    EntityComponent* pSliderComp = real::CreateSlider(pEnt, vPosX, vPosY, real::iPhoneMapX(360.0f),
                                                      "interface/slider_button.rttex", "Min",
                                                      optionDef.displayName, "Max", "", "", "", "");

    // Lets assign parent SliderEnt a name we can identify later.
    pSliderComp->GetParent()->GetVarWithDefault("osgt_setting", Variant(optionDef.varName));

    // Move the slider button according to variable in SharedDB.
    pSliderComp->GetVar("progress")->Set(real::GetApp()->GetVar(optionDef.varName)->GetFloat());
    if (optionDef.signal != nullptr)
        pSliderComp->GetVar("progress")
            ->GetSigOnChanged()
            ->connect(reinterpret_cast<VariantCallback>(optionDef.signal));

    // Adjust margin for next option.
    vPosY += pSliderComp->GetParent()->GetVar("size2d")->GetVector2().y;
}

void OptionsManager::renderCheckbox(OptionsManager::GameOption& optionDef, void* pEntityPtr,
                                    float vPosX, float& vPosY)
{
    Entity* pEnt = reinterpret_cast<Entity*>(pEntityPtr);

    // The client uses this for padding, lets do the same for consistency.
    vPosY += real::iPhoneMapY(20.0);

    // Retrieve fontscale
    uint32_t fontID;
    float fontScale;
    real::GetFontAndScaleToFitThisLinesPerScreenY(fontID, fontScale, 20);

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
            // Fix the alignment manually. The position just isn't multiplied proper.
            CL_Vec2f pos2d = ent->GetVar("pos2d")->GetVector2();
            pos2d.x *= 2;
            ent->GetVar("pos2d")->Set(pos2d);
            if (++i >= 5)
                break;
        }
    }

    // Use final element of the menu for anchoring our elements to.
    Entity* pLastEntity = pScrollChild->GetChildren()->back();
    // I know, iPhoneMapX used on a Y pos, but that's apparently what the client does.
    float vPosY = real::iPhoneMapX(5.0);
    vPosY += pLastEntity->GetVar("pos2d")->GetVector2().y;
    vPosY += pLastEntity->GetVar("size2d")->GetVector2().y;
    float vPosX = real::iPhoneMapX(5.0);

    // Create scaling for our label
    uint32_t fontID;
    float fontScale;
    // 11 lines on desktop, 9 on mobile platforms.
    real::GetFontAndScaleToFitThisLinesPerScreenY(fontID, fontScale, 11);

    // Create our very own label.
    Entity* pOptionsLabel = real::CreateTextLabelEntity(pScrollChild, "osgt_qol_options", vPosX,
                                                        vPosY, "OSGT-QOL Options");
    // Set scaling for label.
    real::SetupTextEntity(pOptionsLabel, fontID, fontScale);

    // Move cursor further down by label size.
    vPosY += pOptionsLabel->GetVar("size2d")->GetVector2().y;

    // Lets start building our options.
    auto& optionsMgr = game::OptionsManager::get();
    for (auto& option : optionsMgr.options)
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
        default:
            break;
        }
    }

    // Finally resize.
    VariantList vl(pScrollChild->GetParent()->GetParent());
    real::ResizeScrollBounds(&vl);
    return;
}

}; // namespace game