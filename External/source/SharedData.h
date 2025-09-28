#pragma once
#include <Windows.h>

#pragma pack(push, 1)
struct SharedData {
    // flag bytes
    BYTE aimbot_enabled;
    BYTE esp_enabled;
    BYTE esp_show_names;
    BYTE esp_show_box;
    BYTE esp_show_bones;
    BYTE esp_show_distance;
    BYTE esp_show_tracer;
    BYTE triggerbot;
    BYTE esp_chams;

    // Aimbot Type (0: Legit, 1: Sticky, 2: Custom)
    BYTE aimbot_type;

    BYTE padding[2];

    // floats
    float fov_size;
    float smooth;
    float targetSpeed;
    float jumpPower;

    // 키바인드 문자열 (최대 63자 + null terminator)
    char aimbot_keybind[64];

    // 상태
    ULONGLONG last_update;
    BYTE process_running;
    BYTE reserved[7];
};
#pragma pack(pop)