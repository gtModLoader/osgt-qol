#include "game/game.hpp"
#include "game/signatures.hpp"
#include "game/struct/net/gameupdatepacket.hpp"
#include "game/struct/netavatar/netavatar.hpp"
#include "patch/patch.hpp"
#include "utils/utils.hpp"

REGISTER_GAME_FUNCTION(NetAvatarSetCharacterState,
                       "40 57 48 83 EC 40 8B 42 14 48 8B F9 89 81 50 02 00 00 8B 42 0C 48 89 74 24 58", __fastcall,
                       void, NetAvatar*, GameUpdatePacket*);
REGISTER_GAME_FUNCTION(
    OnStoreBuyConfirm,
    "48 8B C4 55 41 56 41 57 48 8D 68 88 48 81 EC 60 01 00 00 48 C7 44 24 38 FE FF FF FF 48 89 58 08", __fastcall,
    void);
REGISTER_GAME_FUNCTION(GetChosenStoreItemEntity,
                       "40 57 48 83 EC 50 48 C7 44 24 20 FE FF FF FF 48 89 5C 24 60 48 8B ? ? ? ? ? 48 33 C4 48 89 44 "
                       "24 48 48 83 3D ? ? ? ? ? 75 0E",
                       __fastcall, Entity*);

static uint8_t* createAccSecurityAddr;
class PauseMenuNoAAP : public patch::BasePatch
{
  public:
    void apply() const override
    {
        // Removes the "Secure my Account" button when desired from Pause Menu to restore visual
        // identity of the client.
        // This patch can be "unpatched" and "repatched" on the fly due to how simple the changes
        // done are.
        auto& game = game::GameHarness::get();

        // Default it to enabled.
        Variant* pVariant = real::GetApp()->GetVar("osgt_qol_hide_aap_button");
        if (pVariant->GetType() == Variant::TYPE_UNUSED)
            pVariant->Set(1U);

        auto& optionsMgr = game::OptionsManager::get();
        optionsMgr.addCheckboxOption("qol", "UI", "osgt_qol_hide_aap_button",
                                     "Hide \"Secure my Account\" text from Pause Menu", &OnCheckboxCallback);

        createAccSecurityAddr =
            game.findMemoryPattern<uint8_t*>("0F 84 ? ? ? ? E8 ? ? ? ? 80 B8 15 02 00 00 00 0F 85 ? ? ? ? E8");

        patchIfNeeded();
    }

    static void OnCheckboxCallback(VariantList* pVariant)
    {
        Entity* pCheckbox = pVariant->Get(1).GetEntity();
        bool bChecked = pCheckbox->GetVar("checked")->GetUINT32() != 0;
        real::GetApp()->GetVar("osgt_qol_hide_aap_button")->Set(uint32_t(bChecked));
        patchIfNeeded();
    }

    static void patchIfNeeded()
    {
        // Rewrite a JZ to JMP to skip "Secure My Account" block from ever executing.
        if (real::GetApp()->GetVar("osgt_qol_hide_aap_button")->GetUINT32() == 1)
            utils::writeMemoryPattern(createAccSecurityAddr, "90 E9");
        else
            utils::writeMemoryPattern(createAccSecurityAddr, "0F 84");
    }
};
REGISTER_USER_GAME_PATCH(PauseMenuNoAAP, pause_menu_no_aap);

class DoubleJumpStateFixer : public patch::BasePatch
{
  public:
    void apply() const override
    {
        // By default, the game will force your double jump available state to be off whenever you receive a state
        // packet. This was likely done to prevent some weird high-jump + double-jump combos back in the day, but the
        // way this is implemented is inherently flawed and also obsolete by 2016-2017 state of the game. We will
        // workaround this by restoring double jump available flag if double jump effect was never removed in first
        // place and user still had a free jump remaining.
        auto& game = game::GameHarness::get();
        game.hookFunctionPatternDirect<NetAvatarSetCharacterState_t>(
            pattern::NetAvatarSetCharacterState, NetAvatarSetCharacterState, &real::NetAvatarSetCharacterState);
    }

    static void NetAvatarSetCharacterState(NetAvatar* this_, GameUpdatePacket* packet)
    {
        bool bCurrentJumpState = this_->m_bDoubleJumpAvailable;
        real::NetAvatarSetCharacterState(this_, packet);
        // Double jump flag wasn't removed and we had a double jump available? Restore it.
        if (bCurrentJumpState && (this_->m_flags & 0x02))
            this_->m_bDoubleJumpAvailable = true;
    }
};
REGISTER_USER_GAME_PATCH(DoubleJumpStateFixer, double_jump_state_fixer);

class SkipStoreConfirm : public patch::BasePatch
{
  public:
    void apply() const override
    {
        // This patch skips the server confirmation screen ("Please wait...") when buying packs from the gem store. This
        // negates low-ping advantage when buying bulk packs at expense of not seeing what you bought in the GUI.
        // You still have to click both "Buy" and "Confirm", it just won't wait for the server side confirmation
        // anymore.
        auto& game = game::GameHarness::get();
        game.hookFunctionPatternDirect<OnStoreBuyConfirm_t>(pattern::OnStoreBuyConfirm, OnStoreBuyConfirm,
                                                            &real::OnStoreBuyConfirm);
        real::GetChosenStoreItemEntity =
            game::GameHarness::get().findMemoryPattern<GetChosenStoreItemEntity_t>(pattern::GetChosenStoreItemEntity);

        // Default it to disabled.
        Variant* pVariant = real::GetApp()->GetVar("osgt_qol_skip_store_confirm");
        if (pVariant->GetType() == Variant::TYPE_UNUSED)
            pVariant->Set(0U);
        m_bSkipConfirm = pVariant->GetUINT32() == 1;

        auto& optionsMgr = game::OptionsManager::get();
        optionsMgr.addCheckboxOption("qol", "Input", "osgt_qol_skip_store_confirm",
                                     "Skip store purchase confirmation for packs", &OnCheckboxCallback);
    }

    static void OnCheckboxCallback(VariantList* pVariant)
    {
        Entity* pCheckbox = pVariant->Get(1).GetEntity();
        bool bChecked = pCheckbox->GetVar("checked")->GetUINT32() != 0;
        real::GetApp()->GetVar("osgt_qol_skip_store_confirm")->Set(uint32_t(bChecked));
        m_bSkipConfirm = bChecked;
    }

    static void OnStoreBuyConfirm()
    {
        if (real::GetApp()->m_serverProtocol >= 92 || !m_bSkipConfirm)
        {
            real::OnStoreBuyConfirm();
            return;
        }
        Entity* pChosen = real::GetChosenStoreItemEntity();
        if (pChosen)
            real::SendPacket(2, "action|buy\nitem|" + pChosen->GetName(), real::GetApp()->m_pClient->m_pEnetPeer);
    }

  private:
    static bool m_bSkipConfirm;
};
bool SkipStoreConfirm::m_bSkipConfirm = false;
REGISTER_USER_GAME_PATCH(SkipStoreConfirm, skip_store_confirm);