#pragma once
#include "SharedData.h"
#include <iostream>
#include <string>

class SharedMemoryManager {
private:
    HANDLE hMapFile;
    SharedData* pSharedData;

    bool prev_aimbot_enabled;
    int prev_aimbot_type;
    bool prev_esp_enabled;
    bool prev_esp_show_names;
    bool prev_esp_show_box;
    bool prev_esp_show_bones;
    bool prev_esp_show_distance;
    bool prev_esp_show_tracer;
    bool prev_esp_chams;
    bool isFirstUpdate;
    bool prev_triggerbot;
    float prev_fov_size;
    float prev_smooth;
    float prev_jumpPower;
    float prev_targetSpeed;
    std::string prev_aimbot_keybind;

public:
    SharedMemoryManager() : hMapFile(nullptr), pSharedData(nullptr),
        prev_aimbot_enabled(false), prev_aimbot_type(0), prev_esp_enabled(false),
        prev_esp_show_names(false), prev_esp_show_box(false),
        prev_esp_show_bones(false), prev_esp_show_distance(false),
        prev_esp_show_tracer(false), prev_esp_chams(false),
        prev_fov_size(150.0f), prev_smooth(1.0f), prev_jumpPower(50.0f), prev_targetSpeed(16.0f),
        prev_triggerbot(false), prev_aimbot_keybind("LMB"), isFirstUpdate(true) {
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

        memset(pSharedData, 0, sizeof(SharedData));
        pSharedData->aimbot_enabled = 0;
        pSharedData->aimbot_type = 0;
        pSharedData->esp_enabled = 0;
        pSharedData->fov_size = 150.0f;
        pSharedData->smooth = 1.0f;
        pSharedData->jumpPower = 50.0f;
        pSharedData->targetSpeed = 16.0f;
        pSharedData->triggerbot = 0;

        strcpy_s(pSharedData->aimbot_keybind, sizeof(pSharedData->aimbot_keybind), "LMB");

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
        pSharedData->esp_chams = esp_chams;
        pSharedData->fov_size = fov_size;
        pSharedData->smooth = g_Smooth; 
        pSharedData->triggerbot = triggerbot;
        pSharedData->targetSpeed = targetSpeed;
        pSharedData->jumpPower = jumpPower;

        if (aimbot_keybind != pSharedData->aimbot_keybind) {
            strcpy_s(pSharedData->aimbot_keybind, sizeof(pSharedData->aimbot_keybind), aimbot_keybind.c_str());
        }

        pSharedData->last_update = GetTickCount64();
        pSharedData->process_running = true;
    }

    void ReadFromShared() {
        if (!pSharedData) {
            std::cout << "[ERROR] pSharedData is null!" << std::endl;
            return;
        }

        static int debugCounter = 0;
        debugCounter++;

        Aimbot_Enabled = pSharedData->aimbot_enabled;
        aimbot_type = pSharedData->aimbot_type;
        Esp_Enabled = pSharedData->esp_enabled;
        esp_show_names = pSharedData->esp_show_names;
        esp_show_box = pSharedData->esp_show_box;
        esp_show_bones = pSharedData->esp_show_bones;
        esp_show_distance = pSharedData->esp_show_distance;
        esp_show_tracer = pSharedData->esp_show_tracer;
        esp_chams = pSharedData->esp_chams;
        fov_size = pSharedData->fov_size;
        g_Smooth = pSharedData->smooth; 
        jumpPower = pSharedData->jumpPower;
        targetSpeed = pSharedData->targetSpeed;
        triggerbot = pSharedData->triggerbot;

        aimbot_keybind = std::string(pSharedData->aimbot_keybind);
    }

    void CheckAndUpdateChanges() {
        if (!pSharedData) return;

        bool hasChanges = false;

        if (prev_aimbot_enabled != pSharedData->aimbot_enabled) {
            std::cout << "[SHARED] Aimbot: " << (pSharedData->aimbot_enabled ? "ENABLED" : "DISABLED")
                << " (UI -> External)" << std::endl;
            prev_aimbot_enabled = pSharedData->aimbot_enabled;
            hasChanges = true;
        }

        if (prev_aimbot_type != pSharedData->aimbot_type) {
            std::cout << "[SHARED] Aimbot Type: " << (int)pSharedData->aimbot_type
                << " (UI -> External)" << std::endl;
            prev_aimbot_type = pSharedData->aimbot_type;
            hasChanges = true;
        }

        if (prev_esp_enabled != pSharedData->esp_enabled) {
            std::cout << "[SHARED] ESP: " << (pSharedData->esp_enabled ? "ENABLED" : "DISABLED")
                << " (UI -> External)" << std::endl;
            prev_esp_enabled = pSharedData->esp_enabled;
            hasChanges = true;
        }

        if (prev_esp_show_names != pSharedData->esp_show_names) {
            std::cout << "[SHARED] ESP Names: " << (pSharedData->esp_show_names ? "ON" : "OFF")
                << " (UI -> External)" << std::endl;
            prev_esp_show_names = pSharedData->esp_show_names;
            hasChanges = true;
        }

        if (prev_esp_show_box != pSharedData->esp_show_box) {
            std::cout << "[SHARED] ESP Box: " << (pSharedData->esp_show_box ? "ON" : "OFF")
                << " (UI -> External)" << std::endl;
            prev_esp_show_box = pSharedData->esp_show_box;
            hasChanges = true;
        }

        if (prev_esp_show_bones != pSharedData->esp_show_bones) {
            std::cout << "[SHARED] ESP Bones: " << (pSharedData->esp_show_bones ? "ON" : "OFF")
                << " (UI -> External)" << std::endl;
            prev_esp_show_bones = pSharedData->esp_show_bones;
            hasChanges = true;
        }

        if (prev_esp_show_distance != pSharedData->esp_show_distance) {
            std::cout << "[SHARED] ESP Distance: " << (pSharedData->esp_show_distance ? "ON" : "OFF")
                << " (UI -> External)" << std::endl;
            prev_esp_show_distance = pSharedData->esp_show_distance;
            hasChanges = true;
        }

        if (prev_esp_show_tracer != pSharedData->esp_show_tracer) {
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


        if (abs(prev_smooth - pSharedData->smooth) > 0.01f) {
            std::cout << "[SHARED] Smooth: " << pSharedData->smooth
                << " (UI -> External)" << std::endl;
            prev_smooth = pSharedData->smooth;
            hasChanges = true;
        }

        if (abs(prev_targetSpeed - pSharedData->targetSpeed) > 0.01f) {
            std::cout << "[SHARED] Target Speed: " << pSharedData->targetSpeed
                << " (UI -> External)" << std::endl;
            prev_targetSpeed = pSharedData->targetSpeed;
            hasChanges = true;
        }

        if (abs(prev_jumpPower - pSharedData->jumpPower) > 0.01f) {
            std::cout << "[SHARED] Jump Power: " << pSharedData->jumpPower
                << " (UI -> External)" << std::endl;
            prev_jumpPower = pSharedData->jumpPower;
            hasChanges = true;
        }

        std::string current_keybind = std::string(pSharedData->aimbot_keybind);
        if (prev_aimbot_keybind != current_keybind) {
            std::cout << "[SHARED] Aimbot Keybind: '" << current_keybind
                << "' (UI -> External)" << std::endl;
            prev_aimbot_keybind = current_keybind;
            hasChanges = true;
        }

        if (hasChanges) {
            PrintCurrentStatus();
        }

        isFirstUpdate = false;
    }

    void PrintCurrentStatus() {
        if (!pSharedData) return;

        std::cout << "[STATUS] Current Settings:" << std::endl;
        std::cout << "  ├─ Aimbot: " << (pSharedData->aimbot_enabled ? "✓ ENABLED" : "✗ DISABLED") << std::endl;
        std::cout << "  │  ├─ Type: " << (int)pSharedData->aimbot_type << std::endl;
        std::cout << "  │  └─ Keybind: '" << pSharedData->aimbot_keybind << "'" << std::endl;
        std::cout << "  ├─ ESP: " << (pSharedData->esp_enabled ? "✓ ENABLED" : "✗ DISABLED") << std::endl;

        if (pSharedData->esp_enabled) {
            std::cout << "  │  ├─ Names: " << (pSharedData->esp_show_names ? "✓ ON" : "✗ OFF") << std::endl;
            std::cout << "  │  ├─ Box: " << (pSharedData->esp_show_box ? "✓ ON" : "✗ OFF") << std::endl;
            std::cout << "  │  ├─ Bones: " << (pSharedData->esp_show_bones ? "✓ ON" : "✗ OFF") << std::endl;
            std::cout << "  │  ├─ Tracer: " << (pSharedData->esp_show_tracer ? "✓ ON" : "✗ OFF") << std::endl;
            std::cout << "  │  └─ Distance: " << (pSharedData->esp_show_distance ? "✓ ON" : "✗ OFF") << std::endl;
        }

        std::cout << "  ├─ Triggerbot: " << (pSharedData->triggerbot ? "✓ ENABLED" : "✗ DISABLED") << std::endl;
        std::cout << "  ├─ FOV Size: " << pSharedData->fov_size << std::endl;
        std::cout << "  ├─ Smooth: " << pSharedData->smooth << std::endl;  // 추가
        std::cout << "  ├─ Target Speed: " << pSharedData->targetSpeed << std::endl;
        std::cout << "  ├─ Jump Power: " << pSharedData->jumpPower << std::endl;
        std::cout << "  └─ Last Update: " << pSharedData->last_update << std::endl;
        std::cout << std::endl;
    }

    std::string GetAimbotKeybind() {
        if (!pSharedData) return "LMB";
        return std::string(pSharedData->aimbot_keybind);
    }

    bool IsUIConnected() {
        if (!pSharedData) return false;
        ULONGLONG currentTime = GetTickCount64();
        return (currentTime - pSharedData->last_update) < 5000;
    }

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
        std::cout << "  ├─ Aimbot Keybind: '" << pSharedData->aimbot_keybind << "'" << std::endl;
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