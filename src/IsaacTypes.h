#pragma once
#include <cstdint>
#include <cmath>

#pragma pack(push, 1)

struct Vector {
    float x;
    float y;

    Vector() : x(0.0f), y(0.0f) {}
    Vector(float _x, float _y) : x(_x), y(_y) {}
};

struct ColorMod {
    float _tint[4];      // 0x0 - 0x10
    float _colorize[4];  // 0x10 - 0x20
    float _offset[3];    // 0x20 - 0x2C
};

struct AnimationFrame {
    Vector crop;          // 0x0
    float width;          // 0x8
    float height;         // 0xC
    Vector pos;           // 0x10
    Vector scale;         // 0x18
    Vector pivot;         // 0x20
    int duration;         // 0x28
    bool visible;         // 0x2C
    uint8_t _pad0[3];     // 0x2D - 0x2F
    ColorMod color;       // 0x30
    float rotation;       // 0x5C
    bool interpolated;    // 0x60
    uint8_t _pad1[3];     // 0x61 - 0x63
    int startFrame;       // 0x64
    int endFrame;         // 0x68
};
static_assert(sizeof(AnimationFrame) == 0x6C, "AnimationFrame size mismatch");

struct AnimationLayer {
    int _layerID;                 // 0x0
    AnimationFrame* _animFrames;  // 0x4
    int _numFrames;               // 0x8
    bool _visible;                // 0xC
    uint8_t _pad0[3];             // 0xD - 0xF
};
static_assert(sizeof(AnimationLayer) == 0x10, "AnimationLayer size mismatch");

struct AnimationState {
    void* _animation;             // 0x0
    void* _animData;              // 0x4
    int* _layerFrames;            // 0x8
    int* _nullLayerFrames;        // 0xC
    float _animFrame;             // 0x10
    bool _isPlaying;              // 0x14
    uint8_t _pad0[3];             // 0x15 - 0x17
    int _currentlyTriggeredEvents;// 0x18
    int _previouslyTriggeredEvents;// 0x1C
};
static_assert(sizeof(AnimationState) == 0x20, "AnimationState size mismatch");

struct ANM2 {
    uint8_t _pad0[0x104];
    float _playbackSpeed;         // 0x104
};

#pragma pack(pop)
