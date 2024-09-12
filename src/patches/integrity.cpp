#include "game.hpp"
#include "utils.hpp"

class IntegrityPatch : public game::BasePatch
{
  public:
    void apply() const override
    {
        auto& game = game::GameHarness::get();
        // Patch out CRC integrity check.
        auto addr = game.findMemoryPattern("00 3B C1 75 ? 85 C9");
        if (addr == nullptr)
            throw std::runtime_error("Failed to find integrity check pattern.");
        if (!utils::nopMemory(reinterpret_cast<uint8_t*>(addr) + 1, 6))
            throw std::runtime_error("Failed to patch integrity check.");
    }
};
REGISTER_GAME_PATCH(IntegrityPatch, integrity_bypass);