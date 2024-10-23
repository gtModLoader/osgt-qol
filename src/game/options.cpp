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

// OptionsMenuOnSelect
// Params: VariantList with GUI event details
REGISTER_GAME_FUNCTION(OptionsMenuOnSelect,
                       "48 8B C4 55 41 54 41 55 41 56 41 57 48 8D A8 58 FC FF FF 48 81 EC 80 04 00 "
                       "00 48 C7 45 48 FE FF FF FF 48 89 58 10 48 89 70 18 48 89 78 20 48 8B 05 8B "
                       "FF 2C 00 48 33 C4 48 89 85 70 03 00 00 45",
                       __fastcall, void, void* pVList);

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
    real::BoostSigFire = game.findMemoryPattern<BoostSigFire_t>(pattern::BoostSigFire);
    real::GetApp =
        utils::resolveRelativeCall<GetApp_t>(game.findMemoryPattern<uint8_t*>(pattern::GetApp) + 4);
    real::GetFontAndScaleToFitThisLinesPerScreenY =
        game.findMemoryPattern<GetFontAndScaleToFitThisLinesPerScreenY_t>(
            pattern::GetFontAndScaleToFitThisLinesPerScreenY);
    real::CreateTextLabelEntity =
        game.findMemoryPattern<CreateTextLabelEntity_t>(pattern::CreateTextLabelEntity);
    real::SetupTextEntity = game.findMemoryPattern<SetupTextEntity_t>(pattern::SetupTextEntity);
    real::iPadMapX = game.findMemoryPattern<iPadMapX_t>(pattern::iPadMapX);
    real::iPadMapY = game.findMemoryPattern<iPadMapY_t>(pattern::iPadMapY);
    real::iPhoneMapX = game.findMemoryPattern<iPhoneMapX_t>(pattern::iPhoneMapX);
    real::iPhoneMapY = game.findMemoryPattern<iPhoneMapY_t>(pattern::iPhoneMapY);

    // Hook
    game.hookFunctionPatternDirect<OptionsMenuAddContent_t>(
        pattern::OptionsMenuAddContent, OptionsMenuAddContent, &real::OptionsMenuAddContent);
    game.hookFunctionPatternDirect<OptionsMenuOnSelect_t>(
        pattern::OptionsMenuOnSelect, OptionsMenuOnSelect, &real::OptionsMenuOnSelect);
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
    // NOTE: This can be removed when there's functional boost signals1 support.
    pSliderComp->GetParent()->GetVarWithDefault("osgt_setting", Variant(optionDef.varName));
    // Move the slider button according to variable in SharedDB.
    pSliderComp->GetVar("progress")->Set(real::GetApp()->GetVar(optionDef.varName)->GetFloat());
    // Fire m_pSig_onChanged.
    real::BoostSigFire(pSliderComp->GetVar("progress")->m_pSig_onChanged, 0);

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

    // Adjust margin for next option.
    vPosY += pCheckbox->GetVar("size2d")->GetVector2().y;
}

void OptionsManager::OptionsMenuAddContent(void* pEnt, void* unk2, void* unk3, void* unk4)
{
    // Let the game construct options menu for us.
    real::OptionsMenuAddContent(pEnt, unk2, unk3, unk4);
    Entity* pScrollChild = reinterpret_cast<Entity*>(pEnt);

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

void OptionsManager::OptionsMenuOnSelect(void* pVListPtr)
{
    VariantList* pVList = reinterpret_cast<VariantList*>(pVListPtr);
    Entity* pEnt = pVList->Get(1).GetEntity();
    // Are we exiting the Options menu? If so, lets save our custom settings.
    if (pEnt->GetName() == "Back")
    {
        // The Entity structure is something like to this:
        // OptionsMenu
        //   BGRectBack
        //   scroll
        //     scroll_child
        //       [...]
        //   Back
        Entity* pScrollChild =
            pEnt->GetParent()->GetEntityByName("scroll")->GetEntityByName("scroll_child");
        // Lets save our checkboxes
        auto& optionsMgr = game::OptionsManager::get();
        for (const auto& opt : optionsMgr.options)
        {
            if (opt.type != OptionsManager::OPTION_CHECKBOX)
                continue;
            // Checkboxes can be found easily with their name.
            // Boost should ideally direct it to a dedicated "handler" function though.
            // In the future, a patch should pass on a function of its own for checkboxes on
            // what to do after a state change, if needed.
            Entity* pCheckbox = pScrollChild->GetEntityByName(opt.varName);
            Variant* pVariant = real::GetApp()->GetVar(opt.varName);
            pVariant->Set(pCheckbox->GetVar("checked")->GetUINT32());
        }
        // Lets save our sliders
        // NOTE: Rework when we can use boost signals1.
        std::vector<Entity*> pEnts;
        // We don't need to recurse too deep. SliderEnts are already direct children of
        // scroll_child
        pScrollChild->GetEntitiesByName(&pEnts, "SliderEnt", 1);
        // Iterate over them and seek their "osgt_setting" value
        for (const auto& ent : pEnts)
        {
            Variant* pVariant = ent->GetShared()->GetVarIfExists("osgt_setting");
            if (pVariant == nullptr)
                continue;
            std::string key = pVariant->GetString();
            pVariant = real::GetApp()->GetVar(key);
            pVariant->Set(ent->GetComponentByName("Slider")->GetVar("progress")->GetFloat());
        }
    }
    real::OptionsMenuOnSelect(pVList);
    return;
}
}; // namespace game