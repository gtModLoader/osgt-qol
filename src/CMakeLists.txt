set(SOURCES
    dllmain.cpp
    res/icon.rc
    game/game.cpp
    game/input.cpp
    game/itemapi.cpp
    game/drawing.cpp
    game/options.cpp
    game/weathermanager.cpp
    game/struct/entityutils.cpp
    game/struct/miscutils.cpp
    game/struct/renderutils.cpp
    game/struct/components/gamelogic.cpp
    game/struct/graphics/background.cpp
    game/struct/graphics/background_blank.cpp
    game/struct/graphics/background_clouds.cpp
    game/struct/graphics/background_default.cpp
    game/struct/graphics/background_blood.cpp
    game/struct/graphics/surface.cpp
    patch/patch.cpp
    utils/utils.cpp
)

set(SOURCES_USER_PATCHES
    patch/user/audio.cpp
    patch/user/chat.cpp
    patch/user/framerate.cpp
    patch/user/input.cpp
    patch/user/items.cpp
    patch/user/net.cpp
    patch/user/render.cpp
    patch/user/playmodoverlay.cpp
)

set (SOURCES_CORE_PATCHES
    patch/core/config.cpp
    patch/core/about.cpp
    patch/core/integrity.cpp
)

add_library(${PROJECT_NAME} SHARED 
    ${SOURCES}
    ${SOURCES_CORE_PATCHES} 
    ${SOURCES_USER_PATCHES}
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_NAME "dinput8"
    SUFFIX ".dll"
)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# /W4 is too noisy
target_compile_options(${PROJECT_NAME} PRIVATE /W3)

target_link_libraries(${PROJECT_NAME} PRIVATE
    minhook
    boost_signals
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    $<$<BOOL:${DEVELOPMENT_BUILD}>:DEVELOPMENT>
    _SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS
    _CRT_SECURE_NO_WARNINGS
    WIN32_LEAN_AND_MEAN
    NOMINMAX
)
