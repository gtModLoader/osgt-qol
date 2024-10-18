#include "game/game.hpp"
#include "patch/patch.hpp"
#include "utils/utils.hpp"

// DownloadFileComponent::Init
REGISTER_GAME_FUNCTION(DownloadFileComponentInit,
                       "40 55 53 56 57 41 54 41 55 41 56 41 57 48 8D AC 24 28 FE FF FF", __fastcall,
                       void, void*, std::string, void*, void*)

class IntegrityPatch : public patch::BasePatch
{
  public:
    void apply() const override
    {
        auto& game = game::GameHarness::get();
        // Patch out CRC integrity check.
        auto addr = game.findMemoryPattern<uint8_t*>("00 3B C1 75 ? 85 C9");
        utils::nopMemory(addr + 1, 6);
    }
};
REGISTER_CORE_GAME_PATCH(IntegrityPatch, integrity_bypass);

class SecureItemManager : public patch::BasePatch
{
  public:
    void apply() const override
    {
        auto& game = game::GameHarness::get();
        game.hookFunctionPatternDirect<DownloadFileComponentInit_t>(
            pattern::DownloadFileComponentInit, DownloadFileComponentInit,
            &real::DownloadFileComponentInit);
    }

    static void __fastcall DownloadFileComponentInit(void* this_, std::string filePath, void* unk3,
                                                     void* unk4)
    {
        // Prevent path traversal by strictly disallowing usage of ".." in file paths.
        // We do not need to sanitize for / \ : characters as absolute cache path already gets
        // prepended to the file name, so attacking via absolute path isn't going to work.
        // This attack vector is commonly used by malicious servers in order to drop malware to
        // end-user's computer.
        if (filePath.find("..") != std::string::npos)
        {
            auto msg = std::format(
                "The server you were connecting to contained a malicious item definitions file. "
                "Shutting down game to prevent further damages. Offending filename: {}",
                filePath);
            // Hide the game window as sometimes the error may get stuck behind it.
            auto& game = game::GameHarness::get();
            game.setWindowVisible(false);
            // Use a regular message box as throwing an exception did not work, most probably UB for
            // the game client.
            MessageBoxA(nullptr, msg.c_str(), "Error", MB_ICONERROR | MB_OK);
            ExitProcess(EXIT_FAILURE);
        }
        real::DownloadFileComponentInit(this_, filePath, unk3, unk4);
    }
};
REGISTER_CORE_GAME_PATCH(SecureItemManager, secure_item_manager);