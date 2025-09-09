#pragma once
#include <driver/communication.h>

#include <memory>
#include <Render/Render.h>
#include <SDK/Classes/instance.h>
#include <SDK/Classes/DataModel/DataModel.h>
#include <SDK/Classes/VisualEngine/VisualEngine.h>
#include <SDK/Offset.h>

inline std::unique_ptr<communication> comms = std::make_unique<communication>();

inline std::unique_ptr<Render> render;

inline std::unique_ptr<RBX::DataModel> dataModel;
inline std::unique_ptr<RBX::VisualEngine> visualEngine;

inline bool Esp_Enabled = false;
inline bool Aimbot_Enabled = false;
inline bool esp_show_names = false;
inline bool esp_show_box= false;
inline bool esp_show_bones = false;
inline bool esp_show_distance = false;
inline float fov_size = 150.0f;
inline float smoothMultiplier = 1; // 기본 스무딩 배율
inline bool toggePF = false;
// 이징 함수 정의
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
inline auto linear = [](float t) { return t; };
inline auto easeIn = [](float t) { return t * t; };
inline auto easeOut = [](float t) { return 1.0f - (1.0f - t) * (1.0f - t); };
//inline auto easeInOut = [](float t) { return t < 0.5f ? 2.0f * t * t : 1.0f - powf(-2.0f * t + 2.0f, 2) / 2.0f; };
inline auto easeInOutSineG = [](float t) { return -(cosf(M_PI * t) - 1.0f) / 2.0f; }; // sine
inline auto easeInOut = [](float t) {return easeInOutSineG(t); };
inline auto easeInOutCubic = [](float b, float c, float t) {
    if (t < 0.5f) {
        return c / 2.0f * 4.0f * t * t * t + b;
    }
    float f = (t - 1.0f);
    return c / 2.0f * (4.0f * f * f * f + 1.0f) + b;
};