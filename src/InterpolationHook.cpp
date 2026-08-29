#include "InterpolationHook.h"
#include "IsaacTypes.h"
#include "Config.h"
#include "PatternScanner.h"
#include <MinHook.h>
#include <cmath>

static AnimationState* s_curAnimState = nullptr;
static uintptr_t s_moduleBase = 0;

typedef void(__thiscall* RenderFrame_t)(AnimationLayer* thisPtr, const Vector& position, int unk, const Vector& topLeftClamp, const Vector& bottomRightClamp, ANM2* animation);
typedef void(__thiscall* AnimationStateRender_t)(AnimationState* thisPtr, const Vector& position, const Vector& topLeftClamp, const Vector& bottomRightClamp);

static RenderFrame_t oRenderFrame = nullptr;
static AnimationStateRender_t oAnimationStateRender = nullptr;

static inline unsigned int GetManagerFrameCount() {
    if (!s_moduleBase) {
        s_moduleBase = (uintptr_t)GetModuleHandleA(nullptr);
    }
    // g_Manager pointer at RVA 0x87169C
    void** ppMgr = (void**)(s_moduleBase + 0x87169C);
    if (!ppMgr || !*ppMgr) return 0;
    uintptr_t pMgr = (uintptr_t)*ppMgr;

    // Verified _framecount offset in Manager for Repentance+ J460 is 0x4ABBC
    return *(unsigned int*)(pMgr + 0x4ABBC);
}

// Accurate shortest-path angular difference in degrees
inline float _interpol_short_angle_dis(float from, float to) {
    float diff = fmodf(to - from, 360.0f);
    if (diff > 180.0f) {
        diff -= 360.0f;
    } else if (diff < -180.0f) {
        diff += 360.0f;
    }
    return diff;
}

inline float _interpol_angle_lerp(float from, float to, float perc) {
    return from + _interpol_short_angle_dis(from, to) * perc;
}

void __fastcall Hooked_AnimationStateRender(AnimationState* thisPtr, void* _edx, const Vector& position, const Vector& topLeftClamp, const Vector& bottomRightClamp) {
    AnimationState* prev = s_curAnimState;
    s_curAnimState = thisPtr;
    oAnimationStateRender(thisPtr, position, topLeftClamp, bottomRightClamp);
    s_curAnimState = prev;
}

void __fastcall Hooked_RenderFrame(AnimationLayer* thisPtr, void* _edx, const Vector& position, int unk, const Vector& topLeftClamp, const Vector& bottomRightClamp, ANM2* animation) {
    if (!g_Config.Enabled || !thisPtr || !thisPtr->_animFrames || !s_curAnimState || !s_curAnimState->_isPlaying || unk < 0 || unk >= thisPtr->_numFrames) {
        oRenderFrame(thisPtr, position, unk, topLeftClamp, bottomRightClamp, animation);
        return;
    }

    AnimationFrame* ourframe = &thisPtr->_animFrames[unk];
    if (!ourframe->interpolated) {
        oRenderFrame(thisPtr, position, unk, topLeftClamp, bottomRightClamp, animation);
        return;
    }

    if (unk + 1 >= thisPtr->_numFrames) {
        oRenderFrame(thisPtr, position, unk, topLeftClamp, bottomRightClamp, animation);
        return;
    }

    AnimationFrame* nextframe = &thisPtr->_animFrames[unk + 1];

    // Read real engine frame counter directly from g_Manager (matches REPENTOGON 1:1)
    float lerpappend = 0.5f;
    if (GetManagerFrameCount() % 2 != 0) {
        lerpappend = 0.0f;
    }
    if (animation) {
        lerpappend *= animation->_playbackSpeed;
    }

    float duration = (ourframe->duration > 0) ? (float)ourframe->duration : 1.0f;
    float lerpperc = (lerpappend + s_curAnimState->_animFrame - (float)ourframe->startFrame) / duration;
    if (lerpperc > 1.0f) {
        lerpperc = 1.0f;
    } else if (lerpperc < 0.0f) {
        lerpperc = 0.0f;
    }
    float lerpbegin = (1.0f - lerpperc);

    Vector oldscale = ourframe->scale;
    Vector oldpos = ourframe->pos;
    float oldrot = ourframe->rotation;

    ColorMod oldc;
    for (int i = 0; i < 3; i++) {
        oldc._offset[i] = ourframe->color._offset[i];
        ourframe->color._offset[i] = ourframe->color._offset[i] * lerpbegin + nextframe->color._offset[i] * lerpperc;
    }
    for (int i = 0; i < 4; i++) {
        oldc._tint[i] = ourframe->color._tint[i];
        ourframe->color._tint[i] = ourframe->color._tint[i] * lerpbegin + nextframe->color._tint[i] * lerpperc;
    }

    ourframe->scale.x = ourframe->scale.x * lerpbegin + nextframe->scale.x * lerpperc;
    ourframe->scale.y = ourframe->scale.y * lerpbegin + nextframe->scale.y * lerpperc;
    ourframe->pos.x = ourframe->pos.x * lerpbegin + nextframe->pos.x * lerpperc;
    ourframe->pos.y = ourframe->pos.y * lerpbegin + nextframe->pos.y * lerpperc;
    ourframe->rotation = _interpol_angle_lerp(ourframe->rotation, nextframe->rotation, lerpperc);

    oRenderFrame(thisPtr, position, unk, topLeftClamp, bottomRightClamp, animation);

    for (int i = 0; i < 3; i++) {
        ourframe->color._offset[i] = oldc._offset[i];
    }
    for (int i = 0; i < 4; i++) {
        ourframe->color._tint[i] = oldc._tint[i];
    }

    ourframe->scale = oldscale;
    ourframe->pos = oldpos;
    ourframe->rotation = oldrot;
}

bool InitializeHooks() {
    if (MH_Initialize() != MH_OK) {
        return false;
    }

    s_moduleBase = (uintptr_t)GetModuleHandleA(nullptr);

    // Pattern for AnimationLayer::RenderFrame (0x8520 in isaac-ng.exe)
    uintptr_t pRenderFrame = PatternScanner::FindPattern("53 8B DC 83 EC 08 83 E4 F8 83 C4 04 55 8B 6B ?? 89 6C 24 ?? 8B EC 6A FF 68 ?? ?? ?? ?? 64 A1 ?? ?? ?? ?? 50 53 81 EC E8 01 00 00 A1 ?? ?? ?? ?? 33 C5 89 45 ?? 56 57 50 8D 45 ?? 64 A3 ?? ?? ?? ?? 80 79 0C 00");
    if (pRenderFrame) {
        MH_CreateHook((LPVOID)pRenderFrame, (LPVOID)&Hooked_RenderFrame, (LPVOID*)&oRenderFrame);
    }

    // Pattern for AnimationState::Render (0x9430 in isaac-ng.exe)
    uintptr_t pAnimStateRender = PatternScanner::FindPattern("55 8B EC 83 E4 F8 51 53 56 57 8B F9 8B 57 04 85 D2 74 34 33 F6 39 72 1C");
    if (pAnimStateRender) {
        MH_CreateHook((LPVOID)pAnimStateRender, (LPVOID)&Hooked_AnimationStateRender, (LPVOID*)&oAnimationStateRender);
    }

    MH_EnableHook(MH_ALL_HOOKS);
    return true;
}

void UninitializeHooks() {
    MH_DisableHook(MH_ALL_HOOKS);
    MH_Uninitialize();
}
