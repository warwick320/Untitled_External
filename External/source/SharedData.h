#pragma once
#include <Windows.h>

// �����Ϸ� �е��� �����Ͽ� �ϰ��� �޸� ���̾ƿ� ����
#pragma pack(push, 1)
struct SharedData {
    // bool ��� BYTE ��� (ũ�Ⱑ 1����Ʈ�� ��Ȯ�ϰ� ���ǵ�)
    BYTE aimbot_enabled;
    BYTE esp_enabled;
    BYTE esp_show_names;
    BYTE esp_show_box;
    BYTE esp_show_bones;
    BYTE esp_show_distance;
    BYTE esp_show_tracer;
    BYTE triggerbot;      // ���� ����ü�� ������ ����
    BYTE esp_chams;       // �̸� ����! (charms �� chams)
    
    // �ε��Ҽ��� ���� ������ ���� �е�
    BYTE padding[3];      // 4����Ʈ ��迡 ���߱� ����
    
    // �ε��Ҽ��� �� (���� ����ü�� ������ ����)
    float fov_size;       // float�� ��������� ���
    float targetSpeed;
    float jumpPower;
    
    // ���� Ȯ�� �ʵ�
    ULONGLONG last_update;
    BYTE process_running;
    BYTE reserved[7];     // 8����Ʈ ��迡 ���߱� ����
};
#pragma pack(pop)