#pragma once
#include <windows.h>

struct SharedData {

    bool aimbot_enabled;
    bool esp_enabled;
    bool esp_show_names;
    bool esp_show_box;
    bool esp_show_bones;
    bool esp_show_distance;
    bool esp_show_tracer;

    // Aimbot 설정
    float fov_size;
    float smooth_multiplier;

    // 상태 확인용
    ULONGLONG last_update;
    bool process_running;
};