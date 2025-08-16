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