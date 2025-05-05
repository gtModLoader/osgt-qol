# Build the main binary.
set (SOURCES
    dllmain.cpp
    game/game.cpp
    game/drawing.cpp
    game/options.cpp
    game/weathermanager.cpp
    game/struct/graphics/background.cpp
    game/struct/graphics/background_clouds.cpp
    game/struct/graphics/background_default.cpp
    game/struct/graphics/background_blood.cpp
    game/struct/graphics/surface.cpp
    patch/patch.cpp
    utils/utils.cpp
)
set (SOURCES_USER_PATCHES
    patch/user/audio.cpp
    patch/user/chat.cpp
    patch/user/framerate.cpp
    patch/user/input.cpp
    patch/user/net.cpp
    patch/user/render.cpp
)
set (SOURCES_CORE_PATCHES
    patch/core/about.cpp
    patch/core/integrity.cpp
)
add_library(dinput8 SHARED ${SOURCES} ${SOURCES_CORE_PATCHES} ${SOURCES_USER_PATCHES})
target_include_directories(dinput8 PRIVATE ${CMAKE_SOURCE_DIR}/src)

# Don't use /W4, it's too noisy. Use D_SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS to allow use of stdext::hash_map.
set (OPTIONS 
    /W3
    /D_SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS
)
target_compile_options(dinput8 PRIVATE ${OPTIONS})

set (LIBRARIES 
    minhook
    libboost_signals-vc120-mt-1_55
    winmm.lib
)
target_link_libraries(dinput8 PRIVATE ${LIBRARIES})
