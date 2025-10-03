#pragma once
#include "SharedData.h"
#include <windows.h>
#include <iostream>
#include <thread>
#include <atomic>

class NamedPipeServer {
private:
    HANDLE hPipe;
    SharedData currentData;
    std::thread serverThread;
    std::atomic<bool> isRunning;

    bool prev_aimbot_enabled;
    int prev_aimbot_type;
    bool prev_esp_enabled;
    bool prev_esp_show_names;
    bool prev_esp_show_box;
    bool prev_esp_show_bones;
    bool prev_esp_show_distance;
    bool prev_esp_show_tracer;
    bool prev_esp_chams;
    bool prev_triggerbot;
    float prev_fov_size;
    float prev_smooth;
    float prev_jumpPower;
    float prev_targetSpeed;
    std::string prev_aimbot_keybind;
    bool prev_useCornerBox;
    bool prev_g_Rainbow;
    bool prev_RaycastAim;
    bool isFirstUpdate;

public:
    NamedPipeServer() :
        hPipe(INVALID_HANDLE_VALUE),
        isRunning(false),
        prev_aimbot_enabled(false),
        prev_aimbot_type(0),
        prev_esp_enabled(false),
        prev_esp_show_names(false),
        prev_esp_show_box(false),
        prev_esp_show_bones(false),
        prev_esp_show_distance(false),
        prev_esp_show_tracer(false),
        prev_esp_chams(false),
        prev_fov_size(150.0f),
        prev_smooth(1.0f),
        prev_jumpPower(50.0f),
        prev_targetSpeed(16.0f),
        prev_triggerbot(false),
        prev_aimbot_keybind("LMB"),
        prev_useCornerBox(false),
        prev_g_Rainbow(false),
        prev_RaycastAim(false),
        isFirstUpdate(true)
    {
        memset(&currentData, 0, sizeof(SharedData));

        // 기본값 설정
        currentData.aimbot_enabled = 0;
        currentData.aimbot_type = 0;
        currentData.esp_enabled = 0;
        currentData.fov_size = 150.0f;
        currentData.smooth = 1.0f;
        currentData.jumpPower = 50.0f;
        currentData.targetSpeed = 16.0f;
        currentData.triggerbot = 0;
        currentData.useCornerBox = 0;
        currentData.g_Rainbow = 0;
        currentData.RaycastAim = 0;
        strcpy_s(currentData.aimbot_keybind, sizeof(currentData.aimbot_keybind), "LMB");
    }

    bool Initialize() {
        std::cout << "[PIPE] Initializing Named Pipe Server..." << std::endl;

        isRunning = true;
        serverThread = std::thread(&NamedPipeServer::ServerLoop, this);

        std::cout << "[PIPE] Server initialized successfully" << std::endl;
        return true;
    }

    void ServerLoop() {
        while (isRunning) {
            // Named Pipe
            hPipe = CreateNamedPipe(
                TEXT("\\\\.\\pipe\\RobloxExternalPipe"),
                PIPE_ACCESS_DUPLEX,
                PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
                1,  // 최대 인스턴스 수
                sizeof(SharedData),  // 출력 버퍼 크기
                sizeof(SharedData),  // 입력 버퍼 크기
                0,
                NULL
            );

            if (hPipe == INVALID_HANDLE_VALUE) {
                std::cout << "[ERROR] Failed to create named pipe: " << GetLastError() << std::endl;
                Sleep(1000);
                continue;
            }

            std::cout << "[PIPE] Waiting for client connection..." << std::endl;

            BOOL connected = ConnectNamedPipe(hPipe, NULL) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);

            if (connected) {
                std::cout << "[PIPE] Client connected!" << std::endl;
                HandleClient();
            }
            else {
                std::cout << "[ERROR] Failed to connect client: " << GetLastError() << std::endl;
            }

            CloseHandle(hPipe);
            hPipe = INVALID_HANDLE_VALUE;
        }
    }

    void HandleClient() {
        while (isRunning) {
            SharedData receivedData;
            DWORD bytesRead = 0;

            BOOL success = ReadFile(
                hPipe,
                &receivedData,
                sizeof(SharedData),
                &bytesRead,
                NULL
            );

            if (!success || bytesRead == 0) {
                if (GetLastError() == ERROR_BROKEN_PIPE) {
                    std::cout << "[PIPE] Client disconnected" << std::endl;
                }
                else {
                    std::cout << "[ERROR] ReadFile failed: " << GetLastError() << std::endl;
                }
                break;
            }

            if (bytesRead == sizeof(SharedData)) {
                ProcessReceivedData(receivedData);
            }

            UpdateToShared();
            SendDataToClient();

            Sleep(16);
        }
    }

    void ProcessReceivedData(const SharedData& data) {
        bool hasChanges = false;

        // RaycastAim 수신 데이터 디버그
        std::cout << "[DEBUG] Received RaycastAim raw value: " << static_cast<int>(data.RaycastAim) << std::endl;

        if (prev_aimbot_enabled != data.aimbot_enabled) {
            std::cout << "[PIPE] Aimbot: " << (data.aimbot_enabled ? "ENABLED" : "DISABLED")
                << " (UI -> External)" << std::endl;
            prev_aimbot_enabled = data.aimbot_enabled;
            Aimbot_Enabled = data.aimbot_enabled;
            hasChanges = true;
        }

        if (prev_aimbot_type != data.aimbot_type) {
            std::cout << "[PIPE] Aimbot Type: " << (int)data.aimbot_type
                << " (UI -> External)" << std::endl;
            prev_aimbot_type = data.aimbot_type;
            aimbot_type = data.aimbot_type;
            hasChanges = true;
        }

        if (prev_esp_enabled != data.esp_enabled) {
            std::cout << "[PIPE] ESP: " << (data.esp_enabled ? "ENABLED" : "DISABLED")
                << " (UI -> External)" << std::endl;
            prev_esp_enabled = data.esp_enabled;
            Esp_Enabled = data.esp_enabled;
            hasChanges = true;
        }

        if (prev_esp_show_names != data.esp_show_names) {
            std::cout << "[PIPE] ESP Names: " << (data.esp_show_names ? "ON" : "OFF") << std::endl;
            prev_esp_show_names = data.esp_show_names;
            esp_show_names = data.esp_show_names;
            hasChanges = true;
        }

        if (prev_esp_show_box != data.esp_show_box) {
            std::cout << "[PIPE] ESP Box: " << (data.esp_show_box ? "ON" : "OFF") << std::endl;
            prev_esp_show_box = data.esp_show_box;
            esp_show_box = data.esp_show_box;
            hasChanges = true;
        }

        if (prev_esp_show_bones != data.esp_show_bones) {
            std::cout << "[PIPE] ESP Bones: " << (data.esp_show_bones ? "ON" : "OFF") << std::endl;
            prev_esp_show_bones = data.esp_show_bones;
            esp_show_bones = data.esp_show_bones;
            hasChanges = true;
        }

        if (prev_esp_show_distance != data.esp_show_distance) {
            std::cout << "[PIPE] ESP Distance: " << (data.esp_show_distance ? "ON" : "OFF") << std::endl;
            prev_esp_show_distance = data.esp_show_distance;
            esp_show_distance = data.esp_show_distance;
            hasChanges = true;
        }

        if (prev_esp_show_tracer != data.esp_show_tracer) {
            std::cout << "[PIPE] ESP Tracer: " << (data.esp_show_tracer ? "ON" : "OFF") << std::endl;
            prev_esp_show_tracer = data.esp_show_tracer;
            esp_show_tracer = data.esp_show_tracer;
            hasChanges = true;
        }

        if (prev_esp_chams != data.esp_chams) {
            std::cout << "[PIPE] ESP Chams: " << (data.esp_chams ? "ON" : "OFF") << std::endl;
            prev_esp_chams = data.esp_chams;
            esp_chams = data.esp_chams;
            hasChanges = true;
        }

        if (prev_triggerbot != data.triggerbot) {
            std::cout << "[PIPE] Triggerbot: " << (data.triggerbot ? "ENABLED" : "DISABLED") << std::endl;
            prev_triggerbot = data.triggerbot;
            triggerbot = data.triggerbot;
            hasChanges = true;
        }

        if (abs(prev_fov_size - data.fov_size) > 0.01f) {
            std::cout << "[PIPE] FOV Size: " << data.fov_size << std::endl;
            prev_fov_size = data.fov_size;
            fov_size = data.fov_size;
            hasChanges = true;
        }

        if (abs(prev_smooth - data.smooth) > 0.01f) {
            std::cout << "[PIPE] Smooth: " << data.smooth << std::endl;
            prev_smooth = data.smooth;
            g_Smooth = data.smooth;
            hasChanges = true;
        }

        if (abs(prev_targetSpeed - data.targetSpeed) > 0.01f) {
            std::cout << "[PIPE] Target Speed: " << data.targetSpeed << std::endl;
            prev_targetSpeed = data.targetSpeed;
            targetSpeed = data.targetSpeed;
            hasChanges = true;
        }

        if (abs(prev_jumpPower - data.jumpPower) > 0.01f) {
            std::cout << "[PIPE] Jump Power: " << data.jumpPower << std::endl;
            prev_jumpPower = data.jumpPower;
            jumpPower = data.jumpPower;
            hasChanges = true;
        }

        if (prev_useCornerBox != data.useCornerBox) {
            std::cout << "[PIPE] Use Corner Box: " << (data.useCornerBox ? "ON" : "OFF") << std::endl;
            prev_useCornerBox = data.useCornerBox;
            useCornerBox = data.useCornerBox;
            hasChanges = true;
        }

        if (prev_g_Rainbow != data.g_Rainbow) {
            std::cout << "[PIPE] Rainbow: " << (data.g_Rainbow ? "ON" : "OFF") << std::endl;
            prev_g_Rainbow = data.g_Rainbow;
            g_Rainbow = data.g_Rainbow;
            hasChanges = true;
        }

        if (prev_RaycastAim != data.RaycastAim) {
            std::cout << "[PIPE] Raycast Aim: " << (data.RaycastAim ? "ENABLED" : "DISABLED") << std::endl;
            prev_RaycastAim = data.RaycastAim;
            RaycastAim = data.RaycastAim;
            hasChanges = true;
        }

        std::string current_keybind = std::string(data.aimbot_keybind);
        if (prev_aimbot_keybind != current_keybind) {
            std::cout << "[PIPE] Aimbot Keybind: '" << current_keybind << "'" << std::endl;
            prev_aimbot_keybind = current_keybind;
            aimbot_keybind = current_keybind;
            hasChanges = true;
        }

        currentData.aimbot_enabled = Aimbot_Enabled;
        currentData.aimbot_type = aimbot_type;
        currentData.esp_enabled = Esp_Enabled;
        currentData.esp_show_names = esp_show_names;
        currentData.esp_show_box = esp_show_box;
        currentData.esp_show_bones = esp_show_bones;
        currentData.esp_show_distance = esp_show_distance;
        currentData.esp_show_tracer = esp_show_tracer;
        currentData.esp_chams = esp_chams;
        currentData.triggerbot = triggerbot;
        currentData.fov_size = fov_size;
        currentData.smooth = g_Smooth;
        currentData.targetSpeed = targetSpeed;
        currentData.jumpPower = jumpPower;
        currentData.useCornerBox = useCornerBox;
        currentData.g_Rainbow = g_Rainbow;
        currentData.RaycastAim = RaycastAim;
        strcpy_s(currentData.aimbot_keybind, sizeof(currentData.aimbot_keybind), aimbot_keybind.c_str());

        if (hasChanges) {
            PrintCurrentStatus();
        }

        isFirstUpdate = false;
    }

    void UpdateToShared() {
        currentData.aimbot_enabled = Aimbot_Enabled;
        currentData.esp_enabled = Esp_Enabled;
        currentData.esp_show_names = esp_show_names;
        currentData.esp_show_box = esp_show_box;
        currentData.esp_show_bones = esp_show_bones;
        currentData.esp_show_distance = esp_show_distance;
        currentData.esp_show_tracer = esp_show_tracer;
        currentData.esp_chams = esp_chams;
        currentData.fov_size = fov_size;
        currentData.smooth = g_Smooth;
        currentData.triggerbot = triggerbot;
        currentData.targetSpeed = targetSpeed;
        currentData.jumpPower = jumpPower;
        currentData.useCornerBox = useCornerBox;
        currentData.g_Rainbow = g_Rainbow;
        currentData.RaycastAim = RaycastAim;

        if (aimbot_keybind != currentData.aimbot_keybind) {
            strcpy_s(currentData.aimbot_keybind, sizeof(currentData.aimbot_keybind), aimbot_keybind.c_str());
        }

        currentData.last_update = GetTickCount64();
        currentData.process_running = true;
    }

    void SendDataToClient() {
        if (hPipe == INVALID_HANDLE_VALUE) return;

        DWORD bytesWritten = 0;
        BOOL success = WriteFile(
            hPipe,
            &currentData,
            sizeof(SharedData),
            &bytesWritten,
            NULL
        );

        if (!success || bytesWritten != sizeof(SharedData)) {
            //write failed
        }
    }

    void ReadFromShared() {
        // None 
    }

    void CheckAndUpdateChanges() {
        //auto 
    }

    void PrintCurrentStatus() {
        std::cout << "[STATUS] Current Settings:" << std::endl;
        std::cout << "  ├─ Aimbot: " << (currentData.aimbot_enabled ? "✓ ENABLED" : "✗ DISABLED") << std::endl;
        std::cout << "  │  ├─ Type: " << (int)currentData.aimbot_type << std::endl;
        std::cout << "  │  ├─ Keybind: '" << currentData.aimbot_keybind << "'" << std::endl;
        std::cout << "  │  └─ Raycast Aim: " << (currentData.RaycastAim ? "✓ ON" : "✗ OFF") << std::endl;
        std::cout << "  ├─ ESP: " << (currentData.esp_enabled ? "✓ ENABLED" : "✗ DISABLED") << std::endl;

        if (currentData.esp_enabled) {
            std::cout << "  │  ├─ Names: " << (currentData.esp_show_names ? "✓ ON" : "✗ OFF") << std::endl;
            std::cout << "  │  ├─ Box: " << (currentData.esp_show_box ? "✓ ON" : "✗ OFF") << std::endl;
            std::cout << "  │  ├─ Corner Box: " << (currentData.useCornerBox ? "✓ ON" : "✗ OFF") << std::endl;
            std::cout << "  │  ├─ Bones: " << (currentData.esp_show_bones ? "✓ ON" : "✗ OFF") << std::endl;
            std::cout << "  │  ├─ Tracer: " << (currentData.esp_show_tracer ? "✓ ON" : "✗ OFF") << std::endl;
            std::cout << "  │  ├─ Distance: " << (currentData.esp_show_distance ? "✓ ON" : "✗ OFF") << std::endl;
            std::cout << "  │  └─ Rainbow: " << (currentData.g_Rainbow ? "✓ ON" : "✗ OFF") << std::endl;
        }

        std::cout << "  ├─ Triggerbot: " << (currentData.triggerbot ? "✓ ENABLED" : "✗ DISABLED") << std::endl;
        std::cout << "  ├─ FOV Size: " << currentData.fov_size << std::endl;
        std::cout << "  ├─ Smooth: " << currentData.smooth << std::endl;
        std::cout << "  ├─ Target Speed: " << currentData.targetSpeed << std::endl;
        std::cout << "  ├─ Jump Power: " << currentData.jumpPower << std::endl;
        std::cout << "  └─ Last Update: " << currentData.last_update << std::endl;
        std::cout << std::endl;
    }

    std::string GetAimbotKeybind() {
        return std::string(currentData.aimbot_keybind);
    }

    bool IsUIConnected() {
        ULONGLONG currentTime = GetTickCount64();
        return (hPipe != INVALID_HANDLE_VALUE) && ((currentTime - currentData.last_update) < 5000);
    }

    void PrintDebugInfo() {
        std::cout << "[DEBUG] Named Pipe Server Info:" << std::endl;
        std::cout << "  ├─ Pipe Handle: " << (hPipe != INVALID_HANDLE_VALUE ? "Valid" : "Invalid") << std::endl;
        std::cout << "  ├─ Structure Size: " << sizeof(SharedData) << " bytes" << std::endl;
        std::cout << "  ├─ Process Running: " << (currentData.process_running ? "Yes" : "No") << std::endl;
        std::cout << "  ├─ UI Connected: " << (IsUIConnected() ? "Yes" : "No") << std::endl;
        std::cout << "  ├─ Aimbot Keybind: '" << currentData.aimbot_keybind << "'" << std::endl;
        std::cout << "  └─ Last Update: " << currentData.last_update << std::endl;
    }

    ~NamedPipeServer() {
        isRunning = false;

        if (hPipe != INVALID_HANDLE_VALUE) {
            CloseHandle(hPipe);
            hPipe = INVALID_HANDLE_VALUE;
        }

        if (serverThread.joinable()) {
            serverThread.join();
        }

        std::cout << "[PIPE] Server shut down" << std::endl;
    }
};