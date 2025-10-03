#pragma once
#include <Windows.h>


#ifndef f32
typedef float f32;
#endif

#pragma pack(push, 1)
struct SharedData {

    BYTE aimbot_enabled;
    BYTE esp_enabled;
    BYTE esp_show_names;
    BYTE esp_show_box;
    BYTE esp_show_bones;
    BYTE esp_show_distance;
    BYTE esp_show_tracer;
    BYTE triggerbot;
    BYTE esp_chams;


    BYTE aimbot_type;
    BYTE padding[2];

    // aim (8 bytes)
    f32 fov_size;      // offset 12
    f32 smooth;        // offset 16

    // player (8 bytes)
    f32 targetSpeed;   // offset 20
    f32 jumpPower;     // offset 24

    // keybind (64 bytes)
    char aimbot_keybind[64];  // offset 28

    // settings (2 bytes)
    BYTE useCornerBox;   
    BYTE g_Rainbow;   

    ULONGLONG last_update;
    BYTE process_running;
    BYTE reserved[7];
};
#pragma pack(pop)
