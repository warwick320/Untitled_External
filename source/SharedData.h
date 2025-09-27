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
    BYTE padding[3];

    float fov_size;
    float targetSpeed;
    float jumpPower;

    ULONGLONG last_update;
    BYTE process_running;
    BYTE reserved[7];
};
#pragma pack(pop)