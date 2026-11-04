# The Binding of Isaac: Repentance+ 60 FPS Animation Interpolation

An isolated, standalone implementation of the **60 FPS Animation Interpolation** feature from [REPENTOGON](https://github.com/TeamREPENTOGON/REPENTOGON) for vanilla **The Binding of Isaac: Repentance+ (v1.9.7.15 / J374 and v1.9.7.17 / J460)**.

Because this is a native proxy DLL (`dinput8.dll`) and not a Lua mod, it does not disable achievements and works during **Daily Runs** (use at your own discretion regarding any leaderboards or anti-cheat policies).

---

## Installation

Choose the DLL built for your exact game version. Each DLL supports only its selected version; there is no automatic version detection.

Copy `dinput8.dll` and `interpol.ini` into your game directory (next to `isaac-ng.exe`):
```
C:\Program Files (x86)\Steam\steamapps\common\The Binding of Isaac Rebirth\
```

---

## Configuration (`interpol.ini`)

```ini
[Interpolation]
Enabled=1
```
* `Enabled=1`: 60 FPS interpolation on.
* `Enabled=0`: Vanilla 30 FPS animations.

---

## Building

```powershell
cmake -B build/1.9.7.15 -A Win32 "-DGAME_VERSION=1.9.7.15"
cmake --build build/1.9.7.15 --config Release

cmake -B build/1.9.7.17 -A Win32 "-DGAME_VERSION=1.9.7.17"
cmake --build build/1.9.7.17 --config Release
```
The DLLs will be in `build/1.9.7.15/Release/dinput8.dll` and `build/1.9.7.17/Release/dinput8.dll`. Keep them in separate versioned packages with `interpol.ini`; both must retain the name `dinput8.dll` when installed.

`GAME_VERSION` defaults to `1.9.7.17`. Unsupported values are rejected by CMake. Shared interpolation code lives in `src/InterpolationHook.cpp`; version-specific addresses and patterns live in `src/GameVersion.h`.

---

## Releases (GitHub Actions)

Every push to `main` automatically builds **all versions in `game-versions.txt`** and publishes a complete GitHub release. No manual tag creation or publication is required. Releases use an automatically generated `build-<run-id>` tag pointing to the source commit. You can also run the workflow manually from `main`.

Each release includes one ZIP per supported game version and `SHA256SUMS.txt`. Each ZIP contains `dinput8.dll`, `interpol.ini`, the README, and a game-version notice. Publication happens only after every version builds and all packages are uploaded. Previous releases remain available.

To support a new game version:

1. Add its addresses and patterns to `src/GameVersion.h`, keeping existing profiles.
2. Append its version to `game-versions.txt` (oldest to newest).
3. Commit and push to `main`.

The workflow recompiles and packages the entire list, including all older versions. Existing profiles continue to use their original values; rebuilding does not guarantee byte-identical DLLs if shared code or the compiler changes. CMake defaults to the last listed version for a fresh build directory.

---
## Credits

* **[REPENTOGON](https://github.com/TeamREPENTOGON/REPENTOGON)** - Original `InterpolV2` logic & concept.
* **[MinHook](https://github.com/TsudaKageyu/minhook)** - API Hooking Library.
