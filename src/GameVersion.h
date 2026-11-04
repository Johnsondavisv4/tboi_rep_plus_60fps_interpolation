#pragma once
#include <cstdint>

// Build-time profiles taken from the corresponding version branches.
namespace GameVersion {
#if defined(GAME_VERSION_1_9_7_15) && defined(GAME_VERSION_1_9_7_17)
#error Select only one game version
#elif defined(GAME_VERSION_1_9_7_15)
inline constexpr uintptr_t ManagerRva = 0x80C09C; // J374
inline constexpr uintptr_t FrameCountOffset = 0x4ABA8;
inline constexpr char RenderFramePattern[] = "53 8B DC 83 EC 08 83 E4 F8 83 C4 04 55 8B 6B ?? 89 6C 24 ?? 8B EC 6A FF 68 ?? ?? ?? ?? 64 A1 ?? ?? ?? ?? 50 53 81 EC 98 01 00 00 A1 ?? ?? ?? ?? 33 C5 89 45 ?? 56 57 50";
#elif defined(GAME_VERSION_1_9_7_17)
inline constexpr uintptr_t ManagerRva = 0x87169C; // J460
inline constexpr uintptr_t FrameCountOffset = 0x4ABBC;
inline constexpr char RenderFramePattern[] = "53 8B DC 83 EC 08 83 E4 F8 83 C4 04 55 8B 6B ?? 89 6C 24 ?? 8B EC 6A FF 68 ?? ?? ?? ?? 64 A1 ?? ?? ?? ?? 50 53 81 EC E8 01 00 00 A1 ?? ?? ?? ?? 33 C5 89 45 ?? 56 57 50 8D 45 ?? 64 A3 ?? ?? ?? ?? 80 79 0C 00";
#else
#error Select a supported GAME_VERSION through CMake
#endif
}
