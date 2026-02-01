#pragma once
#include <string>
#include <type_traits>
#include <unordered_map>

namespace patch
{

// Barebones abstract base patch class. All game patches must derive from this class.
class BasePatch
{
  public:
    virtual void apply() const = 0;
    virtual ~BasePatch() = default;
};

// Defines the type of game patch.
enum class PatchType
{
    Core, // Unconditionally applied.
    User, // Applying is decided by user.
};

// Responsible for managing & applying game patches.
class PatchManager
{
  public:
    // Get PatchManager singleton instance.
    static PatchManager& get();

    // Register a new game patch.
    template <class T, PatchType P>
    inline void registerPatch(BasePatch* instance, const std::string& name)
    {
        static_assert(std::is_base_of_v<BasePatch, T>, "T must derive from BasePatch.");
        patchMap[name] = PatchInfo{instance, P};
    }

    // Apply all core patches and user patches listed in given file. If the file doesn't exist, all
    // user patches are applied. If a patch fails to apply, a std::runtime_error exception is
    // thrown.
    void applyPatchesFromFile(const std::string& fileName);

    // Returns a list of all applied user patches.
    std::vector<std::string> getAppliedUserPatchList() const;

    // Returns whether a patch is applied.
    bool isPatchApplied(const std::string& patchName);

  private:
    // Internal structure for storing patch information.
    struct PatchInfo
    {
        // Pointer to the patch instance.
        BasePatch* instance;

        // Type of the patch (core or user).
        PatchType type;

        // Whether the patch has been applied.
        bool applied = false;
    };

    // Map of registered game patches.
    std::unordered_map<std::string, PatchInfo> patchMap;

    // Apply a patch by name. Throws std::runtime_error exception if the patch doesn't exist or if
    // it fails to apply correctly.
    void applyPatch(const std::string& name);
};

namespace internal
{

// Utility class made for automatic/static patch registration within GameHarness. Ideally, this
// shouldn't be used directly. Use the REGISTER_CORE_GAME_PATCH or REGISTER_USER_GAME_PATCH macros
// instead.
template <class T, PatchType P> class RegisterPatch
{
    static_assert(std::is_base_of_v<BasePatch, T>, "T must derive from BasePatch.");

  public:
    T instance;
    // Add pointer to instance to the GameHarness patch map.
    inline RegisterPatch(const std::string& name)
    {
        PatchManager::get().registerPatch<T, P>(&instance, name);
    }
};

// Utility macro for easy registration of core game patches. Creates an instance of
// RegisterPatch<type, PatchType::Core> within the patch namespace.
#define REGISTER_CORE_GAME_PATCH(type, name)                                                       \
    namespace patch                                                                                \
    {                                                                                              \
    namespace internal                                                                             \
    {                                                                                              \
    RegisterPatch<type, PatchType::Core> patch##name(#name);                                       \
    }                                                                                              \
    }

// Ditto, but for user patches.
#define REGISTER_USER_GAME_PATCH(type, name)                                                       \
    namespace patch                                                                                \
    {                                                                                              \
    namespace internal                                                                             \
    {                                                                                              \
    RegisterPatch<type, PatchType::User> patch##name(#name);                                       \
    }                                                                                              \
    }

} // namespace internal

} // namespace patch