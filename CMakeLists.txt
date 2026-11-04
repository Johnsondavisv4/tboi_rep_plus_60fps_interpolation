cmake_minimum_required(VERSION 3.15)
project(InterPolation_vanilla C CXX)

file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/game-versions.txt" GAME_VERSIONS REGEX "^[0-9]+\\.[0-9]+\\.[0-9]+\\.[0-9]+$")
if(NOT GAME_VERSIONS)
    message(FATAL_ERROR "game-versions.txt must contain supported game versions")
endif()
list(GET GAME_VERSIONS -1 LATEST_GAME_VERSION)
set(GAME_VERSION "${LATEST_GAME_VERSION}" CACHE STRING "Target Repentance+ version")
set_property(CACHE GAME_VERSION PROPERTY STRINGS ${GAME_VERSIONS})
if(NOT GAME_VERSION IN_LIST GAME_VERSIONS)
    message(FATAL_ERROR "Unsupported GAME_VERSION '${GAME_VERSION}'. Choose from: ${GAME_VERSIONS}")
endif()
string(REPLACE "." "_" GAME_VERSION_SUFFIX "${GAME_VERSION}")
set(GAME_VERSION_DEFINE "GAME_VERSION_${GAME_VERSION_SUFFIX}")
message(STATUS "Target Repentance+ version: ${GAME_VERSION}")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

# MinHook
add_subdirectory(vendor/minhook)

set(SOURCE_FILES
    src/main.cpp
    src/Config.cpp
    src/InterpolationHook.cpp
    src/dinput8.def
    src/resource.rc
)

add_library(dinput8 SHARED ${SOURCE_FILES})
target_compile_definitions(dinput8 PRIVATE ${GAME_VERSION_DEFINE})

target_include_directories(dinput8 PRIVATE
    src
    vendor/minhook/include
)

target_link_libraries(dinput8 PRIVATE
    minhook
    psapi
)

# Output name must be dinput8.dll with standard security flags
set_target_properties(dinput8 PROPERTIES
    OUTPUT_NAME "dinput8"
    PREFIX ""
    SUFFIX ".dll"
    LINK_FLAGS "/DYNAMICBASE /NXCOMPAT"
)
