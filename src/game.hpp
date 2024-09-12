#pragma once
#include <format>
#include <stdexcept>
#include <string>
#include <unordered_map>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "minhook/minhook.h"
#include "utils.hpp"

// Utility macro for easy declaration of game functions. This will create a global function pointer
// alias called <name>_t, and create an instance of said function pointer in the real namespace.
// The pattern will be accessible in the pattern namespace. (e.g. pattern::name). Do not use this
// macro in header files to avoid namespace pollution.
#define REGISTER_GAME_FUNCTION(name, patt, conv, ret, ...)                                         \
    using name##_t = ret(conv*)(__VA_ARGS__);                                                      \
    namespace real                                                                                 \
    {                                                                                              \
    inline name##_t name = nullptr;                                                                \
    }                                                                                              \
    namespace pattern                                                                              \
    {                                                                                              \
    inline std::string name = patt;                                                                \
    }

namespace game
{

// Barebones abstract base patch class. All game patches must derive from this class.
class BasePatch
{
  public:
    virtual void apply() const = 0;
    virtual ~BasePatch() = default;
};

// Responsible for interacting with internal game mechanics and memory.
class GameHarness
{
  public:
    // Get GameHarness singleton instance.
    static GameHarness& get();

    // Initialization function. This cannot be performed at construct-time due to the fact the game
    // itself is not fully initialized at that point (the constructor may be called during static
    // initialization of other objects).
    void initialize();

    // Search game memory for a specific byte pattern. Returns a pointer to the first occurrence.
    // Throws std::invalid_argument on invalid pattern or std::runtime_error if failed to find.
    template <typename T = void*> T findMemoryPattern(const std::string& pattern);

    // Detours function located at target address to function located at detour address. Optionally,
    // a pointer might be passed in order to store the original (undetoured) function address.
    // Throws std::runtime_error on failure.
    template <typename F> void hookFunction(F target, F detour, F* original);

    // Works exactly as hookFunction, but uses an IDA-style pattern string to locate the target
    // function address. Throws std::runtime_error on failure.
    template <typename F>
    void hookFunctionPatternDirect(const std::string& pattern, F detour, F* original);

    // Works exactly as hookFunctionPatternDirect, but resolves a function call at the target
    // address in order to locate the target function address. Throws std::runtime_error on failure.
    template <typename F>
    void hookFunctionPatternCall(const std::string& pattern, F detour, F* original);

    // Register a new game patch.
    void registerPatch(const std::string& name, BasePatch* patch);

    // Applies a specific patch by name. Throws a std::runtime_error if the patch is not found,
    // or if an error occurs while applying the patch.
    void applyPatch(const std::string& name);

    // Applies all patches registered in the patch map. Throws a std::runtime_error if an error
    // occurs while applying a patch.
    void applyAllPatches();

    // Updates game window title to the OSGT-QOL specific format. Usually you want to call this
    // after the patch applying loop logic.
    void updateWindowTitle();

    GameHarness(GameHarness const&) = delete;
    void operator=(GameHarness const&) = delete;

  private:
    // Base address (where the game image is loaded  in memory).
    HMODULE baseAddr;

    // Image size (size of the game image in memory).
    size_t imageSize;

    // Main game window handle.
    HWND window;

    // Map of registered game patches.
    std::unordered_map<std::string, BasePatch*> patches;

    GameHarness() = default;
};

// Utility class made for automatic/static patch registration within GameHarness. Ideally, this
// shouldn't be used directly. Use the REGISTER_GAME_PATCH macro instead.
template <class T> class RegisterPatch
{
    static_assert(std::is_base_of_v<BasePatch, T>, "T must derive from BasePatch.");

  public:
    T instance;
    // Add pointer to instance to the GameHarness patch map.
    inline RegisterPatch(const std::string& name)
    {
        GameHarness::get().registerPatch(name, &instance);
    }
};

// Utility macro for easy registration of game patches. Creates an instanc of RegisterPatch<type>
// within the patch namespace.
#define REGISTER_GAME_PATCH(type, name)                                                            \
    namespace patch                                                                                \
    {                                                                                              \
    game::RegisterPatch<type> patch##name(#name);                                                  \
    }

}; // namespace game

///////////////////////////////////
// TEMPLATE FUNCTION DEFINITIONS //
///////////////////////////////////

template <typename T> T game::GameHarness::findMemoryPattern(const std::string& pattern)
{
    static_assert(std::is_pointer_v<T>, "T must be a pointer type.");

    auto bytes = utils::parsePattern(pattern);
    uint8_t* begin = reinterpret_cast<uint8_t*>(baseAddr);
    uint8_t* end = reinterpret_cast<uint8_t*>(baseAddr) + imageSize;
    while (begin < end)
    {
        bool match = true;
        for (size_t i = 0; i < bytes.size(); i++)
        {
            if (!bytes[i].has_value())
                continue;
            if (bytes[i].value() != begin[i])
            {
                match = false;
                break;
            }
        }
        if (match)
            return reinterpret_cast<T>(begin);
        begin++;
    }
    throw std::runtime_error(std::format("Failed to find pattern '{}'.", pattern));
}

template <typename F> void game::GameHarness::hookFunction(F target, F detour, F* original)
{
    MH_STATUS status = MH_CreateHook(target, detour, reinterpret_cast<void**>(original));
    if (status != MH_OK)
    {
        auto msg = std::format("Failed to create hook at 0x{:p} ({})", (void*)target,
                               MH_StatusToString(status));
        throw std::runtime_error(msg);
    }
    if ((status = MH_EnableHook(target)) != MH_OK)
    {
        auto msg = std::format("Failed to enable hook at 0x{:p} ({})", (void*)target,
                               MH_StatusToString(status));
        throw std::runtime_error(msg);
    }
}

template <typename F>
void game::GameHarness::hookFunctionPatternDirect(const std::string& pattern, F detour, F* original)
{
    auto addr = findMemoryPattern<F>(pattern);
    if (addr == nullptr)
        throw std::runtime_error(std::format("Failed to find pattern '{}'.", pattern));
    hookFunction<F>(addr, detour, original);
}

template <typename F>
void hookFunctionPatternCall(const std::string& pattern, F detour, F* original)
{
    auto addr = findMemoryPattern<F>(pattern);
    if (addr == nullptr)
        throw std::runtime_error(std::format("Failed to find pattern '{}'.", pattern));
    auto call = utils::resolveRelativeCall<F>(addr);
    if (call == nullptr)
        throw std::runtime_error(std::format("Failed to resolve call at 0x{:p}.", (void*)addr));
    hookFunction<F>(addr, detour, original);
}