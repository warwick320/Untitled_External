#pragma once
#define NOMINMAX
//#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <winsock2.h>
#define CURL_STATICLIB 
#define WIN32_LEAN_AND_MEAN
#ifdef _WIN32
#pragma comment(lib, "vender/libcurl_a.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "wldap32.lib")
#pragma comment(lib, "crypt32.lib")
#pragma comment(lib, "normaliz.lib")
#endif
#include <driver/communication.h>
#include <atomic>
#include "curl.h"
#include <memory>
#include <nlohmann/json.hpp>
#include <Render/Render.h>
#include <SDK/Classes/instance.h>
#include <SDK/Classes/DataModel/DataModel.h>
#include <SDK/Classes/VisualEngine/VisualEngine.h>
#include <SDK/Offset.h>
#include <mutex>
inline std::unique_ptr<communication> comms = std::make_unique<communication>();
inline std::mutex global_lock;
inline std::unique_ptr<Render> render;

inline std::unique_ptr<RBX::DataModel> dataModel;
inline std::unique_ptr<RBX::VisualEngine> visualEngine;
inline std::atomic<bool> runningThread{ true };
inline std::atomic<bool> paused{ false };
inline bool triggerbot = false;
inline bool esp_chams = false;
inline bool Esp_Enabled = false;
inline bool Aimbot_Enabled = false;
inline bool esp_show_names = false;
inline bool esp_show_box= false;
inline bool esp_show_tracer = false;
inline bool esp_show_bones = false;
inline bool esp_show_distance = false;
inline f32 fov_size = 150.0f;
inline f32 g_Smooth = 1.0f;
inline bool toggePF = false;
inline str aimbot_keybind = "LMB";
inline i32 aimbot_type = 0;
inline bool useCornerBox = false;
inline bool RaycastAim = false;

inline void debug_print(str text, i32 lev) {
    str log = (lev == 0) ? "[+]" : (lev == 1) ? "[-]" : "[?]";
    std::cout << std::format("{} {}\n", log, text) << std::endl;
}

// 이징 함수 정의
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
inline auto linear = [](f32 t) { return t; };
inline auto easeIn = [](f32 t) { return t * t; };
inline auto easeOut = [](f32 t) { return 1.0f - (1.0f - t) * (1.0f - t); };
inline auto easeInOutSineG = [](f32 t) { return -(cosf(M_PI * t) - 1.0f) / 2.0f; }; // sine
inline auto easeInOut = [](f32 t) {return easeInOutSineG(t); };
inline auto easeInOutCubic = [](f32 b, f32 c, f32 t) {
    if (t < 0.5f) {
        return c / 2.0f * 4.0f * t * t * t + b;
    }
    f32 f = (t - 1.0f);
    return c / 2.0f * (4.0f * f * f * f + 1.0f) + b;
};
inline u64 DataModelOld = 0;
inline u64 VisualEngineOld = 0;
inline u64 fakeDataModel;
inline f32 targetSpeed = 16.0f;
inline f32 jumpPower = 50.0f;
inline bool g_Rainbow = false;