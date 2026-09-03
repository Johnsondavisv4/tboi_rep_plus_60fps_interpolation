# The Binding of Isaac: Repentance+ 60 FPS Animation Interpolation

An isolated, standalone implementation of the **60 FPS Animation Interpolation** feature from [REPENTOGON](https://github.com/TeamREPENTOGON/REPENTOGON) for vanilla **The Binding of Isaac: Repentance+ (v1.9.7.15 / J374)**.

Because this is a native proxy DLL (`dinput8.dll`) and not a Lua mod, it does not disable achievements and works during **Daily Runs** (use at your own discretion regarding any leaderboards or anti-cheat policies).

---

## Installation

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
cmake -B build -A Win32
cmake --build build --config Release
```
The compiled DLL will be in `build\Release\dinput8.dll`.

---

## Credits

* **[REPENTOGON](https://github.com/TeamREPENTOGON/REPENTOGON)** - Original `InterpolV2` logic & concept.
* **[MinHook](https://github.com/TsudaKageyu/minhook)** - API Hooking Library.