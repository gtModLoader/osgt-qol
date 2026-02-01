#include "game/game.hpp"
#include "game/signatures.hpp"
#include "patch/patch.hpp"
#include <version.h>

class ReportVersionToServer : public patch::BasePatch
{
  public:
    void apply() const override
    {
        // We will let the server transparently know that we are a modded user and also give them
        // our version.
        auto& events = game::EventsAPI::get();
        events.m_sig_loadFromMem.connect(&ReportVersionToServer::refreshItemDB);
    }

    static void refreshItemDB()
    {
        real::SendPacket(2, "action|report_loader\nname|osgt-qol\nversion|" OSGT_QOL_VERSION "\n",
                         real::GetApp()->m_pClient->m_pEnetPeer);
    }
};
REGISTER_CORE_GAME_PATCH(ReportVersionToServer, report_version_to_server);