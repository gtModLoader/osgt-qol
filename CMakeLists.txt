cmake_minimum_required(VERSION 3.12)
project(osgt-qol LANGUAGES CXX C)

if(NOT MSVC OR NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
    message(FATAL_ERROR "Only 64-bit MSVC is supported.")
endif()

set (SOURCES
    src/setup.cpp
    src/dinput8/dinput8.cpp
    src/dinput8/dinput8.def
    src/utils/pattern.cpp
    src/utils/memory.cpp
    src/game/hook/SetFPSLimit.cpp
    src/game/hook/PetRenderDataUpdate.cpp
    src/game/hook/AudioManagerFMODPreload.cpp
    src/game/hook/CreateLogOverlay.cpp
)

set (SOURCES_MH
    src/minhook/buffer.c
    src/minhook/hook.c
    src/minhook/trampoline.c
    src/minhook/hde/hde64.c
)

add_library(dinput8 SHARED ${SOURCES} ${SOURCES_MH})
# Don't use /W4, it's too noisy.
target_compile_options(dinput8 PRIVATE /W3)
target_include_directories(dinput8 PRIVATE ${CMAKE_SOURCE_DIR}/src)
