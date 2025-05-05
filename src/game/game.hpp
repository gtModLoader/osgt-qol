#pragma once
#include "game/struct/graphics/background.hpp"
#include "game/struct/variant.hpp"
#include <stdexcept>
#include <string>


#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "utils/utils.hpp"
#include <minhook.h>

// Utility macro for easy declaration of game functions. This will create a global function pointer
// alias called <name>_t, and create an instance of said function pointer in the real namespace.
// The pattern will be accessible in the pattern namespace. (e.g. pattern::name). Do not use this
// macro in header files to avoid namespace pollution.
#define REGISTER_GAME_FUNCTION(name, patt, conv, ret, ...)                                         \
    using name##_t = ret(conv*)(__VA_ARGS__);                                                      \
    namespace real                                                                                 \
    {                                                                                              \
    name##_t name = nullptr;                                                                       \
    }                                                                                              \
    namespace pattern                                                                              \
    {                                                                                              \
    std::string name = patt;                                                                       \
    }

namespace game
{

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

    // Temporarily used to store various function signature resolves that are used in
    // structs/classes. This should be probably reworked when an API-consumer model is created.
    void resolveRenderSigs();

    // Toggles the loading screen
    void toggleLoadScreen();

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

    // Changes the game window title to the specified string.
    void setWindowTitle(const std::string& title);

    // Toggles the game's audio through Windows Multimedia API
    void toggleGameAudio();

    // Sets the game window visibility.
    inline void setWindowVisible(bool visible) const
    {
        ShowWindow(window, visible ? SW_SHOW : SW_HIDE);
    }

    GameHarness(GameHarness const&) = delete;
    void operator=(GameHarness const&) = delete;

  private:
    // Base address (where the game image is loaded  in memory).
    HMODULE baseAddr;

    // Image size (size of the game image in memory).
    size_t imageSize;

    // Main game window handle.
    HWND window;

    // Audio mute state
    bool audioMuted = false;

    // Last audio level state pre-mute
    DWORD audioLevel;

    GameHarness() = default;
};

// Responsible for providing a stable API for patches to add in-game options with
typedef void (*VariantCallback)(Variant*);
typedef void (*VariantListCallback)(VariantList*);
class OptionsManager
{
  public:
    enum GameOptionType : uint8_t
    {
        OPTION_SLIDER,
        OPTION_CHECKBOX
    };
    struct GameOption
    {
        OptionsManager::GameOptionType type;
        std::string varName;
        std::string displayName;
        void* signal;
    };
    // Get OptionsManager instance
    static OptionsManager& get();

    // Initialize OptionsManager. This has to be invoked after GameHarness has finished
    // initialization and the game window has been hidden for patching.
    // This will resolve and hook all the functions needed to provide an API for patches to create
    // their own options with.
    void initialize();

    // Adds a slider option to end of GameOptions list.
    // varName is points to a variable in save.dat.
    // displayName is the string visible in middle of a slider option.
    void addSliderOption(std::string varName, std::string displayName, VariantCallback pCallback)
    {
        GameOption option;
        option.type = OPTION_SLIDER;
        option.varName = varName;
        option.displayName = displayName;
        option.signal = (void*)pCallback;
        options.push_back(option);
    }

    // Adds a checkbox option to end of GameOptions list.
    // varName is points to a variable in save.dat.
    // displayName is the string visibile directly next to the checkbox.
    void addCheckboxOption(std::string varName, std::string displayName,
                           VariantListCallback pCallback)
    {
        GameOption option;
        option.type = OPTION_CHECKBOX;
        option.varName = varName;
        option.displayName = displayName;
        option.signal = (void*)pCallback;
        options.push_back(option);
    }

    // All of the custom options we have made are stored here.
    std::vector<GameOption> options;

  private:
    // Helper functions called during the hook to render our options.
    static void renderSlider(OptionsManager::GameOption& optionDef, void* pEntPtr, float vPosX,
                             float& vPosY);
    static void renderCheckbox(OptionsManager::GameOption& optionDef, void* pEntPtr, float vPosX,
                               float& vPosY);

    // Fastcalls used in hooks
    static void __fastcall OptionsMenuAddContent(void* pEnt, void* unk2, void* unk3, void* unk4);
    static void __fastcall OptionsMenuOnSelect(void* pVList);
};

// Responsible for providing a standard way to register custom weathers
typedef Background* (*WeatherCallback)();
class WeatherManager
{
  public:
    struct CustomWeather
    {
        int mappedID;
        WeatherCallback callback;
    };
    // Get WeatherManager instance
    static WeatherManager& get();

    // Initialize WeatherManager. This has to be invoked after GameHarness has finished
    // initialization and the game window has been hidden for patching.
    // This will resolve and hook all the functions needed to provide an API for weathers
    void initialize();

    void refreshIDs();

    void registerWeather(std::string prettyName, WeatherCallback pCallback);
    std::map<std::string, CustomWeather> weathers;
    bool mappedWeathers = false;

  private:
    static void __thiscall WorldRendererForceBackground(uint8_t* this_, int WeatherID, void* unk3,
                                                        void* unk4);
};
} // namespace game

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
            if (bytes[i] == UINT16_MAX)
                continue;
            if (bytes[i] != begin[i])
            {
                match = false;
                break;
            }
        }
        if (match)
            return reinterpret_cast<T>(begin);
        begin++;
    }
    throw std::runtime_error("Failed to find pattern '" + pattern + "'.");
}

template <typename F> void game::GameHarness::hookFunction(F target, F detour, F* original)
{
    MH_STATUS status = MH_CreateHook(target, detour, reinterpret_cast<void**>(original));
    if (status != MH_OK)
    {
        // auto msg = std::format("Failed to create hook at 0x{:p} ({})", (void*)target,
        //                        MH_StatusToString(status));
        // throw std::runtime_error(msg);
        // TODO: Fix the messages to be like they were above
        throw std::runtime_error("Failed to create hook.");
    }
    if ((status = MH_EnableHook(target)) != MH_OK)
    {
        // auto msg = std::format("Failed to enable hook at 0x{:p} ({})", (void*)target,
        //                        MH_StatusToString(status));
        // throw std::runtime_error(msg);
        // TODO: Fix the messages to be like they were above
        throw std::runtime_error("Failed to enable hook.");
    }
}

template <typename F>
void game::GameHarness::hookFunctionPatternDirect(const std::string& pattern, F detour, F* original)
{
    auto addr = findMemoryPattern<F>(pattern);
    if (addr == nullptr)
        throw std::runtime_error("Failed to find pattern '" + pattern + "'.");
    hookFunction<F>(addr, detour, original);
}

template <typename F>
void game::GameHarness::hookFunctionPatternCall(const std::string& pattern, F detour, F* original)
{
    auto addr = findMemoryPattern<F>(pattern);
    if (addr == nullptr)
        throw std::runtime_error("Failed to find pattern '" + pattern + "'.");
    auto call = utils::resolveRelativeCall<F>(addr);
    if (call == nullptr)
        throw std::runtime_error("Failed to resolve call at " + std::to_string((uintptr_t*)addr));
    hookFunction<F>(call, detour, original);
}