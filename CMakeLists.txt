cmake_minimum_required(VERSION 3.25)
project(osgt-qol LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Must use /MD regardless of build type to match the game's runtime
set(CMAKE_MSVC_RUNTIME_LIBRARY MultiThreadedDLL)

if(NOT MSVC OR NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
    message(FATAL_ERROR "${PROJECT_NAME} requires MSVC and a 64-bit target")
endif()

add_subdirectory(vendor)
add_subdirectory(src)
