#pragma once
#include <Windows.h>

#pragma pack(push, 1)
struct SharedData {
    // flag bytes (16 bytes total)
    BYTE aimbot_enabled;      // offset 0
    BYTE esp_enabled;         // offset 1
    BYTE esp_show_names;      // offset 2
    BYTE esp_show_box;        // offset 3
    BYTE esp_show_bones;      // offset 4
    BYTE esp_show_distance;   // offset 5
    BYTE esp_show_tracer;     // offset 6
    BYTE triggerbot;          // offset 7
    BYTE esp_chams;           // offset 8
    BYTE RaycastAim;          // offset 9
    BYTE aimbot_type;         // offset 10
    BYTE useCornerBox;        // offset 11
    BYTE g_Rainbow;           // offset 12
    BYTE process_running;     // offset 13
    BYTE reserved_flags[2];   // offset 14-15

    // float values (16 bytes)
    f32 fov_size;             // offset 16
    f32 smooth;               // offset 20
    f32 targetSpeed;          // offset 24
    f32 jumpPower;            // offset 28

    // keybind string (64 bytes)
    char aimbot_keybind[64];  // offset 32

    // timestamp and reserved (16 bytes)
    ULONGLONG last_update;    // offset 96
    BYTE reserved[8];         // offset 104
};
#pragma pack(pop)

// 구조체 크기 검증
static_assert(sizeof(SharedData) == 112, "SharedData size must be 112 bytes");