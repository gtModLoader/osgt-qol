#include "game/game.hpp"
#include "game/signatures.hpp"
#include "game/struct/entity.hpp"
#include "patch/patch.hpp"
#include <string>
#include <version.h>

REGISTER_GAME_FUNCTION(AboutMenuAddScrollContent,
                       "48 8B C4 55 41 56 41 57 48 8D A8 F8 FB FF FF 48 81 EC F0 04", __fastcall,
                       void, Entity*);

class AboutMenuAttribution : public patch::BasePatch
{
  public:
    void apply() const override
    {
        auto& game = game::GameHarness::get();
        // Hook AboutMenuAddScrollContent.
        game.hookFunctionPatternDirect<AboutMenuAddScrollContent_t>(
            pattern::AboutMenuAddScrollContent, AboutMenuAddScrollContent,
            &real::AboutMenuAddScrollContent);
    }

    static void __fastcall AboutMenuAddScrollContent(Entity* pScrollChild)
    {
        // Let about menu construct itself normally, we can't really do a pre-hook
        // here since the function deletes all children by default.
        real::AboutMenuAddScrollContent(pScrollChild);

        // Patch the uncentered buttons located after the blurbs.
        int i = 0;
        for (const auto& ent : *pScrollChild->GetChildren())
        {
            // Bit lazy, but I don't want to do 6 string comparisons every loop cycle
            // or iterate over the list 6 separate times.
            if (i > 0 || ent->GetName() == "privacy")
            {
                // Fix the alignment manually. The position just isn't multiplied proper.
                CL_Vec2f pos2d = ent->GetVar("pos2d")->GetVector2();
                pos2d.x *= 2;
                ent->GetVar("pos2d")->Set(pos2d);
                if (++i >= 6)
                    break;
            }
        }

        // We take over TextBox2 (the last Entity under children) and insert our own
        // attribution logic to it.
        Entity* pTextBox2 = pScrollChild->GetChildren()->back();
        EntityComponent* pTextComponent = pTextBox2->GetComponentByName("TextBoxRender");
        if (pTextComponent != nullptr)
        {
            Variant* pTextVariant = pTextComponent->GetShared()->GetVarIfExists("text");
            if (pTextVariant != nullptr)
            {
                // Set our attribution.
                real::SetTextEntity(pTextBox2, pTextVariant->GetString() + GetAttributionText());
                // Signal proton to resize the menu for our text to be visible.
                VariantList vl(pScrollChild->GetParent()->GetParent());
                real::ResizeScrollBounds(&vl);
            }
        }
    }

    // Returns the attribution text to be appended to the about menu. Also includes a list of
    // applied user patches.
    static std::string GetAttributionText()
    {
        auto patches = patch::PatchManager::get().getAppliedUserPatchList();

        std::string firstLine = "\nOSGT-QOL " OSGT_QOL_DISPLAY_VERSION;
#ifdef OSGT_QOL_DEVELOPMENT
        firstLine += " (Development Build)\n";
#else
        firstLine += "\n";
#endif

        std::string text = firstLine +
                           "`6Created by `wCernodile`` and `whouz``."
                           "\nThere are currently `w" +
                           std::to_string(patches.size()) + "`` patches applied: ";

        // Create comma-separated list of patches.
        for (size_t i = 0; i < patches.size(); i++)
        {
            text += "`w" + patches[i] + "``";
            if (i < patches.size() - 1)
                text += ", ";
        }
        return text;
    }
};
REGISTER_CORE_GAME_PATCH(AboutMenuAttribution, about_menu_attrib);