#pragma once
#include <Windows.h>

// 컴파일러 패딩을 제어하여 일관된 메모리 레이아웃 보장
#pragma pack(push, 1)
struct SharedData {
    // bool 대신 BYTE 사용 (크기가 1바이트로 명확하게 정의됨)
    BYTE aimbot_enabled;
    BYTE esp_enabled;
    BYTE esp_show_names;
    BYTE esp_show_box;
    BYTE esp_show_bones;
    BYTE esp_show_distance;
    BYTE esp_show_tracer;
    BYTE triggerbot;      // 상대방 구조체와 동일한 순서
    BYTE esp_chams;       // 이름 수정! (charms → chams)
    
    // 부동소수점 값의 정렬을 위한 패딩
    BYTE padding[3];      // 4바이트 경계에 맞추기 위함
    
    // 부동소수점 값 (상대방 구조체와 동일한 순서)
    float fov_size;       // float을 명시적으로 사용
    float targetSpeed;
    float jumpPower;
    
    // 상태 확인 필드
    ULONGLONG last_update;
    BYTE process_running;
    BYTE reserved[7];     // 8바이트 경계에 맞추기 위함
};
#pragma pack(pop)