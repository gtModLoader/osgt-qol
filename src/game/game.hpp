#pragma once
#include "game/struct/graphics/background.hpp"
#include "game/struct/variant.hpp"
#include <map>
#include <stdexcept>
#include <string>
#include <vector>
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
    namespace patched                                                                              \
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
    void resolveSharedSigs();

    // Toggles the loading screen
    void toggleLoadScreen();

    // Resizes the loading screen
    void resizeLoadScreen();

    // Search game memory for a specific byte pattern. Returns a pointer to the first occurrence.
    // Throws std::invalid_argument on invalid pattern or std::runtime_error if failed to find.
    template <typename T = void*> T findMemoryPattern(const std::string& pattern);

    // Detours function located at target address to function located at detour address. Optionally,
    // a pointer might be passed in order to store the original (undetoured) function address.
    // Throws std::runtime_error on failure.
    template <typename F>
    void hookFunction(F target, F detour, F* original, bool hasPriority = false);

    // Works exactly as hookFunction, but uses an IDA-style pattern string to locate the target
    // function address. Throws std::runtime_error on failure.
    template <typename F>
    void hookFunctionPatternDirect(const std::string& pattern, F detour, F* original,
                                   bool hasPriority = false);

    // Works exactly as hookFunctionPatternDirect, but resolves a function call at the target
    // address in order to locate the target function address. Throws std::runtime_error on failure.
    template <typename F>
    void hookFunctionPatternCall(const std::string& pattern, F detour, F* original,
                                 bool hasPriority = false);

    // Changes the game window title to the specified string.
    void setWindowTitle(const std::string& title);

    HWND getWindowHandle() const { return window; }

    void updateWindowHandle();

    void setWindowModdedIcon();

    // Sets the game window visibility.
    inline void setWindowVisible(bool visible) const
    {
        ShowWindow(window, visible ? SW_SHOW : SW_HIDE);
    }

    // Enables all created function hooks
    void finalizeInitialization();

    GameHarness(GameHarness const&) = delete;
    void operator=(GameHarness const&) = delete;

  private:
    // Base address (where the game image is loaded  in memory).
    HMODULE baseAddr;

    // Image size (size of the game image in memory).
    size_t imageSize;

    // Main game window handle.
    HWND window;

    // Last saved game title
    std::string lastSavedTitle = "Growtopia [OSGT-QOL]";

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
        OPTION_CHECKBOX,
        OPTION_MULTICHOICE,
        OPTION_MULTICHOICE_DUALBUTTONS
    };
    struct GameOption
    {
        OptionsManager::GameOptionType type;
        std::string varName;
        std::string displayName;
        std::string extraInfo;
        std::vector<std::string>* displayOptions;
        float vModSizeX = 0;
        void* signal;
    };
    struct OptionsPage
    {
        std::string fancyName;
        std::map<std::string, std::vector<GameOption>> sections;
    };
    // Get OptionsManager instance
    static OptionsManager& get();

    // Initialize OptionsManager. This has to be invoked after GameHarness has finished
    // initialization and the game window has been hidden for patching.
    // This will resolve and hook all the functions needed to provide an API for patches to create
    // their own options with.
    void initialize();

    // Creates an option page with fancy name
    void addOptionPage(std::string ID, std::string displayName)
    {
        optionPages[ID].fancyName = displayName;
    }

    // Adds a slider option to end of GameOptions list.
    // varName is points to a variable in save.dat.
    // displayName is the string visible in middle of a slider option.
    void addSliderOption(std::string varName, std::string displayName, VariantCallback pCallback,
                         std::string hintStr = "")
    {
        GameOption option;
        option.type = OPTION_SLIDER;
        option.varName = varName;
        option.displayName = displayName;
        option.signal = (void*)pCallback;
        option.extraInfo = hintStr;
        rootOptions.push_back(option);
    }

    void addSliderOption(std::string page, std::string section, std::string varName,
                         std::string displayName, VariantCallback pCallback,
                         std::string hintStr = "")
    {
#ifdef DEBUG
        if (optionPages.find(page) == optionPages.end())
        {
            printf("[WARN] Added a slider option to page %s on section %s, but page doesn't exist. "
                   "Using the ID as name.\n",
                   page, section);
        }
#endif
        GameOption option;
        option.type = OPTION_SLIDER;
        option.varName = varName;
        option.displayName = displayName;
        option.signal = (void*)pCallback;
        option.extraInfo = hintStr;
        optionPages[page].sections[section].push_back(option);
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
        rootOptions.push_back(option);
    }
    void addCheckboxOption(std::string page, std::string section, std::string varName,
                           std::string displayName, VariantListCallback pCallback)
    {
#ifdef DEBUG
        if (optionPages.find(page) == optionPages.end())
        {
            printf("[WARN] Added a slider option to page %s on section %s, but page doesn't exist. "
                   "Using the ID as name.\n",
                   page, section);
        }
#endif
        GameOption option;
        option.type = OPTION_CHECKBOX;
        option.varName = varName;
        option.displayName = displayName;
        option.signal = (void*)pCallback;
        optionPages[page].sections[section].push_back(option);
    }

    // Adds a multi-choice option to end of GameOptions list.
    // varName points to a variable in save.dat.
    // displayName is the string visible above the multichoice option.
    // displayOptions is the "pretty name" corresponding to the numeric variable.
    // vModSizeX is optional parameter if you need larger size than vSizeX 180
    // Draws an option that looks roughly:
    // displayName
    // [<] [ displayOptions[var] ] [>]
    void addMultiChoiceOption(std::string varName, std::string displayName,
                              std::vector<std::string>& displayOptions,
                              VariantListCallback pCallback, float vModSizeX = 0,
                              std::string hintStr = "")
    {
        GameOption option;
        option.type = OPTION_MULTICHOICE;
        option.varName = varName;
        option.displayName = displayName;
        option.displayOptions = &displayOptions;
        option.signal = (void*)pCallback;
        option.vModSizeX = vModSizeX;
        option.extraInfo = hintStr;
        rootOptions.push_back(option);
    }
    void addMultiChoiceOption(std::string page, std::string section, std::string varName,
                              std::string displayName, std::vector<std::string>& displayOptions,
                              VariantListCallback pCallback, float vModSizeX = 0,
                              std::string hintStr = "")
    {
#ifdef DEBUG
        if (optionPages.find(page) == optionPages.end())
        {
            printf("[WARN] Added a slider option to page %s on section %s, but page doesn't exist. "
                   "Using the ID as name.\n",
                   page, section);
        }
        else
        {
            if (optionPages[page].sections.find(section) == optionPages[page].sections.end())
            {
                printf("[WARN] Added a slider option to page %s on section %s, but section doesn't "
                       "exist. Using the ID as name.\n",
                       page, section);
            }
        }
#endif
        GameOption option;
        option.type = OPTION_MULTICHOICE;
        option.varName = varName;
        option.displayName = displayName;
        option.displayOptions = &displayOptions;
        option.signal = (void*)pCallback;
        option.vModSizeX = vModSizeX;
        option.extraInfo = hintStr;
        optionPages[page].sections[section].push_back(option);
    }

    void addMultiChoiceOptionDoubleButtons(std::string page, std::string section,
                                           std::string varName, std::string displayName,
                                           std::vector<std::string>& displayOptions,
                                           VariantListCallback pCallback, float vModSizeX = 0,
                                           std::string hintStr = "")
    {
#ifdef DEBUG
        if (optionPages.find(page) == optionPages.end())
        {
            printf("[WARN] Added a slider option to page %s on section %s, but page doesn't exist. "
                   "Using the ID as name.\n",
                   page, section);
        }
        else
        {
            if (optionPages[page].sections.find(section) == optionPages[page].sections.end())
            {
                printf("[WARN] Added a slider option to page %s on section %s, but section doesn't "
                       "exist. Using the ID as name.\n",
                       page, section);
            }
        }
#endif
        GameOption option;
        option.type = OPTION_MULTICHOICE_DUALBUTTONS;
        option.varName = varName;
        option.displayName = displayName;
        option.displayOptions = &displayOptions;
        option.signal = (void*)pCallback;
        option.vModSizeX = vModSizeX;
        option.extraInfo = hintStr;
        optionPages[page].sections[section].push_back(option);
    }

    // All of the custom options we have made are stored here.
    std::vector<GameOption> rootOptions;
    std::map<std::string, OptionsPage> optionPages;

  private:
    // Helper functions called during the hook to render our options.
    static void renderSlider(OptionsManager::GameOption& optionDef, void* pEntPtr, float vPosX,
                             float& vPosY);
    static void renderCheckbox(OptionsManager::GameOption& optionDef, void* pEntPtr, float vPosX,
                               float& vPosY);
    static void renderMultiChoice(OptionsManager::GameOption& optionDef, void* pEntPtr, float vPosX,
                                  float& vPosY);
    static void HandleOptionPageButton(VariantList* pVL);
    static void HandleOptionPageScrollButton(VariantList* pVL);

    // Fastcalls used in hooks
    static void __fastcall OptionsMenuAddContent(void* pEnt, void* unk2, void* unk3, void* unk4);
    static void __fastcall OptionsMenuOnSelect(void* pVList);
    static void __fastcall OnMenuButtonPressed(VariantList*);
    static void __fastcall OnGemButtonPressed(VariantList*);
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
    struct CustomWeatherEvent
    {
        CustomWeather* m_pCustWeather;
        std::string m_prettyName;
    };
    // Get WeatherManager instance
    static WeatherManager& get();

    // Initialize WeatherManager. This has to be invoked after GameHarness has finished
    // initialization and the game window has been hidden for patching.
    // This will resolve and hook all the functions needed to provide an API for weathers
    void initialize();

    static void refreshItemDB();

    // Registers a weather under WeatherManager. Defaults to unmapped weather ID (-1).
    // The weather ID is assigned by the server by setting extra file path as
    // "loader/weather/pretty_name".
    void registerWeather(std::string prettyName, WeatherCallback pCallback, int weatherID = -1);

    std::map<std::string, CustomWeather> weathers;
    bool mappedWeathers = false;

    boost::signal<void(game::WeatherManager::CustomWeatherEvent*)> m_sig_eventSubscribe;

  private:
    static void __thiscall WorldRendererForceBackground(uint8_t* this_, int WeatherID, void* unk3,
                                                        void* unk4);
};

// For now this just serves as a way to get signalled on events.
class EventsAPI
{
  public:
    static EventsAPI& get();

    void initialize();

    boost::signal<void(void)> m_sig_loadFromMem;

    // NOTE: This also fires when dialogs or options is open, mods have to do their own checks if
    // they care.
    boost::signal<void(void)> m_sig_addWasdKeys;
    boost::signal<void(VariantList*)> m_sig_onArcadeInput;
    boost::signal<void(void*, int, bool)> m_sig_netControllerInput;
    boost::signal<void(void*, __int64, __int64, __int64)> m_sig_onMapLoaded;
    boost::signal<void(void)> m_sig_postInitVideo;

    int acquireKeycode();

  private:
    static void __thiscall ItemInfoManagerLoadFromMem(void* this_, char* pBytes, bool arg3);
    static void __fastcall OnArcadeInput(VariantList* pVL);
    static void __fastcall NetControllerLocalOnArcadeInput(void* this_, int keyCode,
                                                           bool bKeyFired);
    static void __fastcall AddWASDKeys();
    static void __fastcall OnMapLoaded(void* this_, __int64 p1, __int64 p2, __int64 p3);

    int m_lastKeycode;
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

template <typename F>
void game::GameHarness::hookFunction(F target, F detour, F* original, bool hasPriority)
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
    if (hasPriority)
    {
        status = MH_EnableHook(target);
        if (status != MH_OK)
        {
            throw std::runtime_error("Failed to enable hook");
        }
    }
}

template <typename F>
void game::GameHarness::hookFunctionPatternDirect(const std::string& pattern, F detour, F* original,
                                                  bool hasPriority)
{
    auto addr = findMemoryPattern<F>(pattern);
    if (addr == nullptr)
        throw std::runtime_error("Failed to find pattern '" + pattern + "'.");
    hookFunction<F>(addr, detour, original, hasPriority);
}

template <typename F>
void game::GameHarness::hookFunctionPatternCall(const std::string& pattern, F detour, F* original,
                                                bool hasPriority)
{
    auto addr = findMemoryPattern<F>(pattern);
    if (addr == nullptr)
        throw std::runtime_error("Failed to find pattern '" + pattern + "'.");
    auto call = utils::resolveRelativeCall<F>(addr);
    if (call == nullptr)
        throw std::runtime_error("Failed to resolve call");
    hookFunction<F>(call, detour, original, hasPriority);
}