set(SOURCES
    dllmain.cpp
    dinput8.def
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
    patch/core/version.cpp
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

# Display version format for visual elements
if (PROJECT_VERSION_PATCH EQUAL 0)
    set(PROJECT_DISPLAY_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}")
else()
    set(PROJECT_DISPLAY_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")
endif()

configure_file(version.h.in "${CMAKE_CURRENT_BINARY_DIR}/version.h" @ONLY)

target_include_directories(${PROJECT_NAME} PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)

# /W4 is too noisy
target_compile_options(${PROJECT_NAME} PRIVATE /W3)

target_link_libraries(${PROJECT_NAME} PRIVATE
    minhook
    boost_signals
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    $<$<BOOL:${DEVELOPMENT_BUILD}>:OSGT_QOL_DEVELOPMENT>
    _SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS
    _CRT_SECURE_NO_WARNINGS
    WIN32_LEAN_AND_MEAN
    NOMINMAX
)
