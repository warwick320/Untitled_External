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
    f32 fov_size;

    // ���� Ȯ�ο�
    ULONGLONG last_update;
    bool process_running;
};