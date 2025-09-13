#pragma once
#include "SharedData.h"
#include <iostream>

class SharedMemoryManager {
private:
    HANDLE hMapFile;
    SharedData* pSharedData;

    bool prev_aimbot_enabled;
    bool prev_esp_enabled;
    bool prev_esp_show_names;
    bool prev_esp_show_box;
    bool prev_esp_show_bones;
    bool prev_esp_show_distance;
	bool prev_esp_show_tracer;
    bool isFirstUpdate;
	f32 prev_fov_size;
public:
    SharedMemoryManager() : hMapFile(nullptr), pSharedData(nullptr),
        prev_aimbot_enabled(false), prev_esp_enabled(false),
        prev_esp_show_names(false), prev_esp_show_box(false),
        prev_esp_show_bones(false), prev_esp_show_distance(false), prev_esp_show_tracer(false), prev_fov_size(150.0f),
        isFirstUpdate(true) {
    }

    bool Initialize() {
        hMapFile = CreateFileMapping(
            INVALID_HANDLE_VALUE,
            NULL,
            PAGE_READWRITE,
            0,
            sizeof(SharedData),
            "RobloxExternalSharedMemory"
        );

        if (hMapFile == NULL) {
            debug_print("Failed to create shared memory", 1);
            return false;
        }

        pSharedData = (SharedData*)MapViewOfFile(
            hMapFile,
            FILE_MAP_ALL_ACCESS,
            0, 0,
            sizeof(SharedData)
        );

        if (pSharedData == NULL) {
            debug_print("Failed to map view of shared memory", 1);
            CloseHandle(hMapFile);
            return false;
        }

        // 초기값 설정
        memset(pSharedData, 0, sizeof(SharedData));
		pSharedData->fov_size = 150.0f;
        debug_print("Shared memory initialized successfully", 0);

        return true;
    }

    void UpdateToShared() {
        if (!pSharedData) return;

        pSharedData->aimbot_enabled = Aimbot_Enabled;
        pSharedData->esp_enabled = Esp_Enabled;
        pSharedData->esp_show_names = esp_show_names;
        pSharedData->esp_show_box = esp_show_box;
        pSharedData->esp_show_bones = esp_show_bones;
        pSharedData->esp_show_distance = esp_show_distance;
		pSharedData->esp_show_tracer = esp_show_tracer;
		pSharedData->fov_size = fov_size;
        pSharedData->last_update = GetTickCount64();
        pSharedData->process_running = true;
    }

    void ReadFromShared() {
        if (!pSharedData) {
            std::cout << "[ERROR] pSharedData is null!" << std::endl;
            return;
        }

        // 디버그: 현재 공유 메모리 값들 출력
        static int debugCounter = 0;
        //if (debugCounter % 300 == 0) { // 5초마다 한 번씩 출력 (60fps * 5초)
        //    std::cout << "[DEBUG] Shared Memory Values:" << std::endl;
        //    std::cout << "  Aimbot: " << (pSharedData->aimbot_enabled ? "true" : "false") << std::endl;
        //    std::cout << "  ESP: " << (pSharedData->esp_enabled ? "true" : "false") << std::endl;
        //    std::cout << "  Last Update: " << pSharedData->last_update << std::endl;
        //    std::cout << "  Process Running: " << (pSharedData->process_running ? "true" : "false") << std::endl;
        //}
        debugCounter++;

        // 변경사항 감지 및 출력
        //CheckAndUpdateChanges();

        // 실제 변수 업데이트
        Aimbot_Enabled = pSharedData->aimbot_enabled;
        Esp_Enabled = pSharedData->esp_enabled;
        esp_show_names = pSharedData->esp_show_names;
        esp_show_box = pSharedData->esp_show_box;
        esp_show_bones = pSharedData->esp_show_bones;
        esp_show_distance = pSharedData->esp_show_distance;
		esp_show_tracer = pSharedData->esp_show_tracer;
        fov_size = pSharedData->fov_size;

    }
    void CheckAndUpdateChanges() {
        if (!pSharedData) return;

        bool hasChanges = false;

        // 디버그: 현재 공유 메모리의 실제 값들 출력
        std::cout << "[DEBUG] Reading from shared memory:" << std::endl;
        std::cout << "  pSharedData->aimbot_enabled = " << (pSharedData->aimbot_enabled ? "true" : "false") << std::endl;
        std::cout << "  pSharedData->esp_enabled = " << (pSharedData->esp_enabled ? "true" : "false") << std::endl;
        std::cout << "  prev_aimbot_enabled = " << (prev_aimbot_enabled ? "true" : "false") << std::endl;
        std::cout << "  prev_esp_enabled = " << (prev_esp_enabled ? "true" : "false") << std::endl;

        // Aimbot 상태 변경 확인
        if (prev_aimbot_enabled != pSharedData->aimbot_enabled) {
            std::cout << "[SHARED] Aimbot: " << (pSharedData->aimbot_enabled ? "ENABLED" : "DISABLED")
                << " (UI -> External)" << std::endl;
            prev_aimbot_enabled = pSharedData->aimbot_enabled;
            hasChanges = true;
        }

        // ESP 상태 변경 확인
        if (prev_esp_enabled != pSharedData->esp_enabled) {
            std::cout << "[SHARED] ESP: " << (pSharedData->esp_enabled ? "ENABLED" : "DISABLED")
                << " (UI -> External)" << std::endl;
            prev_esp_enabled = pSharedData->esp_enabled;
            hasChanges = true;
        }

        // ESP Names 상태 변경 확인
        if (prev_esp_show_names != pSharedData->esp_show_names) {
            std::cout << "[SHARED] ESP Names: " << (pSharedData->esp_show_names ? "ON" : "OFF")
                << " (UI -> External)" << std::endl;
            prev_esp_show_names = pSharedData->esp_show_names;
            hasChanges = true;
        }

        // ESP Box 상태 변경 확인
        if (prev_esp_show_box != pSharedData->esp_show_box) {
            std::cout << "[SHARED] ESP Box: " << (pSharedData->esp_show_box ? "ON" : "OFF")
                << " (UI -> External)" << std::endl;
            prev_esp_show_box = pSharedData->esp_show_box;
            hasChanges = true;
        }

        // ESP Bones 상태 변경 확인
        if (prev_esp_show_bones != pSharedData->esp_show_bones) {
            std::cout << "[SHARED] ESP Bones: " << (pSharedData->esp_show_bones ? "ON" : "OFF")
                << " (UI -> External)" << std::endl;
            prev_esp_show_bones = pSharedData->esp_show_bones;
            hasChanges = true;
        }

        // ESP Distance 상태 변경 확인
        if (prev_esp_show_distance != pSharedData->esp_show_distance) {
            std::cout << "[SHARED] ESP Distance: " << (pSharedData->esp_show_distance ? "ON" : "OFF")
                << " (UI -> External)" << std::endl;
            prev_esp_show_distance = pSharedData->esp_show_distance;
            hasChanges = true;
        }
        if(prev_esp_show_tracer != pSharedData->esp_show_tracer) {
            std::cout << "[SHARED] ESP Tracer: " << (pSharedData->esp_show_tracer ? "ON" : "OFF")
                << " (UI -> External)" << std::endl;
            prev_esp_show_tracer = pSharedData->esp_show_tracer;
            hasChanges = true;
		}
        if (abs(prev_fov_size - pSharedData->fov_size) > 0.01f) {
            std::cout << "[SHARED] FOV Size: " << pSharedData->fov_size
                << " (UI -> External)" << std::endl;
            prev_fov_size = pSharedData->fov_size;
            hasChanges = true;
        }
        // 변경사항이 있을 때만 상태 요약 출력
        if (hasChanges) {
            PrintCurrentStatus();
        }

        isFirstUpdate = false;
    }
    void PrintCurrentStatus() {
        if (!pSharedData) return;

        std::cout << "[STATUS] Current Settings:" << std::endl;
        std::cout << "  ├─ Aimbot: " << (pSharedData->aimbot_enabled ? "✓ ENABLED" : "✗ DISABLED") << std::endl;
        std::cout << "  ├─ ESP: " << (pSharedData->esp_enabled ? "✓ ENABLED" : "✗ DISABLED") << std::endl;

        if (pSharedData->esp_enabled) {
            std::cout << "  │  ├─ Names: " << (pSharedData->esp_show_names ? "✓ ON" : "✗ OFF") << std::endl;
            std::cout << "  │  ├─ Box: " << (pSharedData->esp_show_box ? "✓ ON" : "✗ OFF") << std::endl;
            std::cout << "  │  ├─ Bones: " << (pSharedData->esp_show_bones ? "✓ ON" : "✗ OFF") << std::endl;
            std::cout << "  │  └─ Distance: " << (pSharedData->esp_show_distance ? "✓ ON" : "✗ OFF") << std::endl;
        }

        std::cout << "  └─ Last Update: " << pSharedData->last_update << std::endl;

        // 실제 inline 변수들의 현재 상태도 출력
        std::cout << "[INLINE] Actual Variables:" << std::endl;
        std::cout << "  ├─ Aimbot_Enabled: " << (Aimbot_Enabled ? "true" : "false") << std::endl;
        std::cout << "  ├─ Esp_Enabled: " << (Esp_Enabled ? "true" : "false") << std::endl;
        std::cout << "  ├─ esp_show_names: " << (esp_show_names ? "true" : "false") << std::endl;
        std::cout << "  ├─ esp_show_box: " << (esp_show_box ? "true" : "false") << std::endl;
        std::cout << "  ├─ esp_show_bones: " << (esp_show_bones ? "true" : "false") << std::endl;
		std::cout << "  ├─ esp_show_tracer: " << (esp_show_tracer ? "true" : "false") << std::endl;
        std::cout << "  └─ esp_show_distance: " << (esp_show_distance ? "true" : "false") << std::endl;
        std::cout << std::endl;
    }

    // UI 연결 상태 확인
    bool IsUIConnected() {
        if (!pSharedData) return false;

        ULONGLONG currentTime = GetTickCount64();
        return (currentTime - pSharedData->last_update) < 5000; // 5초 이내 업데이트
    }

    // 디버그 정보 출력
    void PrintDebugInfo() {
        if (!pSharedData) {
            std::cout << "[DEBUG] Shared memory not initialized" << std::endl;
            return;
        }

        std::cout << "[DEBUG] Shared Memory Info:" << std::endl;
        std::cout << "  ├─ Memory Address: 0x" << std::hex << (uintptr_t)pSharedData << std::dec << std::endl;
        std::cout << "  ├─ Structure Size: " << sizeof(SharedData) << " bytes" << std::endl;
        std::cout << "  ├─ Process Running: " << (pSharedData->process_running ? "Yes" : "No") << std::endl;
        std::cout << "  ├─ UI Connected: " << (IsUIConnected() ? "Yes" : "No") << std::endl;
        std::cout << "  └─ Last Update: " << pSharedData->last_update << std::endl;
    }

    ~SharedMemoryManager() {
        if (pSharedData) {
            pSharedData->process_running = false;
            UnmapViewOfFile(pSharedData);
        }
        if (hMapFile) {
            CloseHandle(hMapFile);
        }
    }
};