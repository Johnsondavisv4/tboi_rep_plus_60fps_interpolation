cmake_minimum_required(VERSION 3.15)
project(InterPolation_vanilla C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# MinHook
add_subdirectory(vendor/minhook)

set(SOURCE_FILES
    src/main.cpp
    src/Config.cpp
    src/InterpolationHook.cpp
    src/dinput8.def
)

add_library(dinput8 SHARED ${SOURCE_FILES})

target_include_directories(dinput8 PRIVATE
    src
    vendor/minhook/include
)

target_link_libraries(dinput8 PRIVATE
    minhook
    psapi
)

# Output name must be dinput8.dll
set_target_properties(dinput8 PROPERTIES
    OUTPUT_NAME "dinput8"
    PREFIX ""
    SUFFIX ".dll"
)
