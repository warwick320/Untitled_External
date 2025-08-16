#pragma once

#include <Cheats/Players.h>
#include <SDK/Classes/World/Primtive.h>
#include <array>
#include <unordered_map>
#include <SDK/Offset.h>
#include <Render/Render.h>
#include <Windows.h>
#include <cmath>
#include <algorithm>
inline ImVec2 ToImVec2(const Vector2& v) {
    return ImVec2(v.x, v.y);
}

enum BoneR15 {
    R15_Head, R15_UpperTorso, R15_LowerTorso,
    R15_LeftUpperArm, R15_LeftLowerArm, R15_LeftHand,
    R15_RightUpperArm, R15_RightLowerArm, R15_RightHand,
    R15_LeftUpperLeg, R15_LeftLowerLeg, R15_LeftFoot,
    R15_RightUpperLeg, R15_RightLowerLeg, R15_RightFoot,
    R15_BoneCount
};
enum BoneR6 {
    R6_Head, R6_Torso,
    R6_LeftArm, R6_RightArm,
    R6_LeftLeg, R6_RightLeg,
    R6_BoneCount
};

constexpr const char* bonesR15[] = {
    "Head", "UpperTorso", "LowerTorso", "LeftUpperArm", "LeftLowerArm", "LeftHand",
    "RightUpperArm", "RightLowerArm", "RightHand", "LeftUpperLeg", "LeftLowerLeg", "LeftFoot",
    "RightUpperLeg", "RightLowerLeg", "RightFoot"
};
constexpr const char* bonesR6[] = {
    "Head", "Torso", "Left Arm", "Right Arm", "Left Leg", "Right Leg"
};

struct BoneLine {
    int a, b;
};

constexpr BoneLine linesR15[] = {
    {R15_Head, R15_UpperTorso}, {R15_UpperTorso, R15_LowerTorso},
    {R15_UpperTorso, R15_LeftUpperArm}, {R15_LeftUpperArm, R15_LeftLowerArm}, {R15_LeftLowerArm, R15_LeftHand},
    {R15_UpperTorso, R15_RightUpperArm}, {R15_RightUpperArm, R15_RightLowerArm}, {R15_RightLowerArm, R15_RightHand},
    {R15_LowerTorso, R15_LeftUpperLeg}, {R15_LeftUpperLeg, R15_LeftLowerLeg}, {R15_LeftLowerLeg, R15_LeftFoot},
    {R15_LowerTorso, R15_RightUpperLeg}, {R15_RightUpperLeg, R15_RightLowerLeg}, {R15_RightLowerLeg, R15_RightFoot}
};
constexpr size_t lineCountR15 = sizeof(linesR15) / sizeof(BoneLine);

constexpr BoneLine linesR6[] = {
    {R6_Head, R6_Torso}, {R6_Torso, R6_LeftArm}, {R6_Torso, R6_RightArm}, {R6_Torso, R6_LeftLeg}, {R6_Torso, R6_RightLeg}
};
constexpr size_t lineCountR6 = sizeof(linesR6) / sizeof(BoneLine);

static const auto boneMapR15 = [] {
    std::unordered_map<std::string, int> map;
    for (int i = 0; i < R15_BoneCount; ++i) {
        map[bonesR15[i]] = i;
    }
    return map;
    }();

static const auto boneMapR6 = [] {
    std::unordered_map<std::string, int> map;
    for (int i = 0; i < R6_BoneCount; ++i) {
        map[bonesR6[i]] = i;
    }
    return map;
    }();


 
inline void drawAngleBoundingBox(ImDrawList* drawList, const std::array<Vector2, R15_BoneCount>& bonePositions,
    bool isR15, ImU32 color) {
    if (bonePositions.empty()) return;

    float minX = 99999.0f, maxX = -99999.0f;
    float minY = 99999.0f, maxY = -99999.0f;

    // 모든 유효한 본 위치를 확인하여 경계 계산
    const auto& boneMap = isR15 ? boneMapR15 : boneMapR6;
    const int boneCount = isR15 ? R15_BoneCount : R6_BoneCount;

    int validBones = 0;
    for (int i = 0; i < boneCount; ++i) {
        const Vector2& pos = bonePositions[i];
        if (pos.x != 0 || pos.y != 0) {
            minX = (pos.x < minX) ? pos.x : minX;
            maxX = (pos.x > maxX) ? pos.x : maxX;
            minY = (pos.y < minY) ? pos.y : minY;
            maxY = (pos.y > maxY) ? pos.y : maxY;
            validBones++;
        }
    }

    if (validBones > 0) {
        float padding = 1.0f;
        minX -= padding;
        maxX += padding;
        minY -= padding;
        maxY += padding;

        drawList->AddRect(ImVec2(minX, minY), ImVec2(maxX, maxY), color, 0.0f, 0, 2.0f);
    }
}

inline void espLoop() {
    ImDrawList* drawList = ImGui::GetBackgroundDrawList();

    ImVec2 viewportPos = ImGui::GetMainViewport()->Pos;
    ImVec2 viewportSize = ImGui::GetMainViewport()->Size;
    ImVec2 screenBottomCenter = ImVec2(
        viewportPos.x + viewportSize.x / 2,
        viewportPos.y + viewportSize.y - 5
    );

    Vector3 myPos3D = { 0, 0, 0 };
    bool hasValidMyPos = false;

    if (g_LocalPlayer.getAddress()) {
        RBX::Player localPlayerInstance(g_LocalPlayer.getAddress());
        RBX::ModelInstance myCharacter = localPlayerInstance.getModelInstance();
        if (myCharacter.getAddress()) {
            auto myHead = myCharacter.findFirstChild("Head");
            if (myHead.getAddress()) {
                myPos3D = myHead.getPrimitive().getPartPosition();
                hasValidMyPos = true;
            }
        }
    }

    for (RBX::Player& player : g_PlayerList) {
        RBX::ModelInstance character = player.getModelInstance();

        if (!character.getAddress() || player.getName() == g_LocalPlayer.getName() ||
            (player.getTeamName() == g_LocalPlayer.getTeamName() && player.getTeamName() != "No Team" && player.getTeamName() != "Players")) {
            continue;
        }
		

        auto headPart = character.findFirstChild("Head");
        if (!headPart.getAddress()) {
			printf("Player %s has no head part\n", player.getName().c_str());
            continue;
        }
        Vector3 headPos3D = headPart.getPrimitive().getPartPosition();
        if (visualEngine->isOnScreen(headPos3D) == false) continue;
        Vector2 headPos2D = visualEngine->worldToScreen(headPos3D);
        if (headPos2D.x == 0 && headPos2D.y == 0) continue;

        // 3D 거리 계산
        float distance3D = 0.0f;
        if (hasValidMyPos) {
            float dx = headPos3D.x - myPos3D.x;
            float dy = headPos3D.y - myPos3D.y;
            float dz = headPos3D.z - myPos3D.z;
            distance3D = sqrtf(dx * dx + dy * dy + dz * dz);
        }

        ImU32 lineColor;
        if (hasValidMyPos) {
            float normalizedDist = ((distance3D / 200.0f) < 1.0f ? (distance3D / 200.0f) : 1.0f);
            int red = static_cast<int>(255 * normalizedDist);
            int green = static_cast<int>(255 * (1.0f - normalizedDist));
            lineColor = IM_COL32(red, green, 0,1000); // 반투명
        }
        else {
            lineColor = IM_COL32(255, 255, 255, 200); // 흰색 기본
        }

        // 이름 표시 (머리 위)
        std::string name = player.getName();
        ImVec2 textSize = ImGui::CalcTextSize(name.c_str());
        float nameY = headPos2D.y - textSize.y - 5;
		if (esp_show_names) {
			drawList->AddText(ImVec2(headPos2D.x - textSize.x / 2, nameY), IM_COL32(255, 255, 255, 255), name.c_str());
		}

        // 거리 표시 (이름 위)
        char distText[32];
        if (hasValidMyPos) {
            snprintf(distText, sizeof(distText), "%.1fm", distance3D / 10.0f);
        }
        else {
            snprintf(distText, sizeof(distText), "N/A");
        }
        ImVec2 distTextSize = ImGui::CalcTextSize(distText);
        float distY = headPos2D.y - 35;
        if (esp_show_distance) {
            drawList->AddText(ImVec2(headPos2D.x - distTextSize.x / 2, distY), IM_COL32(0, 255, 0, 255), distText);
        }

        // 스켈레톤 그리기 (색상 동기화)
        const bool isR15 = character.findFirstChild("UpperTorso").getAddress() != 0;
        const auto& boneMap = isR15 ? boneMapR15 : boneMapR6;
        const BoneLine* lines = isR15 ? linesR15 : linesR6;
        const size_t lineCount = isR15 ? lineCountR15 : lineCountR6;

        std::array<Vector2, R15_BoneCount> bonePos2D{};
        std::vector<RBX::Instance> children = character.getChildern();
        for (RBX::Instance& child : children) {
            std::string childName = child.getName();
            auto it = boneMap.find(childName);
            if (it != boneMap.end()) {
                int boneIndex = it->second;
                auto primitive = child.getPrimitive();
                Vector3 pos3D = primitive.getPartPosition();
                bonePos2D[boneIndex] = visualEngine->worldToScreen(pos3D);
            }
        }
        if (esp_show_bones) {
            for (size_t i = 0; i < lineCount; ++i) {
                const Vector2& a = bonePos2D[lines[i].a];
                const Vector2& b = bonePos2D[lines[i].b];
                if ((a.x != 0 || a.y != 0) && (b.x != 0 || b.y != 0)) {
                    drawList->AddLine(ToImVec2(a), ToImVec2(b), lineColor); // 색상 동기화
                }
            }
        }


		if (esp_show_box) {
			// 각도 기반 박스 그리기
			drawAngleBoundingBox(drawList, bonePos2D, isR15, lineColor);
		}
        //drawAngleBoundingBox(drawList, bonePos2D, isR15, lineColor);
    }

    drawList->AddCircleFilled(screenBottomCenter, 5.0f, IM_COL32(255, 255, 255, 150));
    //drawRadar(drawList, myPos3D, hasValidMyPos);
}


float FOV_RADIUS = fov_size;
constexpr float MAX_TARGET_DIST = 650.0f;
inline void aimbotLoop()
{

    POINT mousePos;
    GetCursorPos(&mousePos);

    ImVec2 viewportPos = ImGui::GetMainViewport()->Pos;
    ImVec2 mouseImGui = ImVec2(mousePos.x - viewportPos.x, mousePos.y - viewportPos.y);

    ImDrawList* drawList = ImGui::GetBackgroundDrawList();
    drawList->AddCircle(mouseImGui, fov_size, IM_COL32(255, 0, 0, 180), 64, 2.0f);
    if (render->isVisible) return;
    if (!(GetAsyncKeyState(VK_LBUTTON) & 0x8000))
        return;

    Vector3 myPos = { 0, 0, 0 };
    if (g_LocalPlayer.getAddress()) {
        RBX::Player localPlayerInstance(g_LocalPlayer.getAddress());
        RBX::ModelInstance myCharacter = localPlayerInstance.getModelInstance();
        if (myCharacter.getAddress()) {
            auto myHead = myCharacter.findFirstChild("Head");
            if (myHead.getAddress())
                myPos = myHead.getPrimitive().getPartPosition();
        }
    }

    static RBX::Player* lastTarget = nullptr;
    Vector2 bestTarget2D{ 0, 0 };
    float minDist2D = FLT_MAX;
    float minDist3D = FLT_MAX;

    for (RBX::Player& player : g_PlayerList) {
        RBX::ModelInstance character = player.getModelInstance();

        if (!character.getAddress() || player.getName() == g_LocalPlayer.getName() ||
            (player.getTeamName() == g_LocalPlayer.getTeamName() && player.getTeamName() != "No Team" && player.getTeamName() != "Players")) {
            continue;
        }

        auto headPart = character.findFirstChild("Head");
        if (!headPart.getAddress()) continue;
        Vector3 headPos3D = headPart.getPrimitive().getPartPosition();
        Vector2 headPos2D = visualEngine->worldToScreen(headPos3D);
        if (headPos2D.x == 0 && headPos2D.y == 0) continue;

        float dist3D = sqrtf(
            (headPos3D.x - myPos.x) * (headPos3D.x - myPos.x) +
            (headPos3D.y - myPos.y) * (headPos3D.y - myPos.y) +
            (headPos3D.z - myPos.z) * (headPos3D.z - myPos.z)
        );
        if (dist3D > MAX_TARGET_DIST) continue;

        float dx = headPos2D.x - mouseImGui.x;
        float dy = headPos2D.y - mouseImGui.y;
        float dist2D = sqrtf(dx * dx + dy * dy);
        if (dist2D > fov_size) continue;

        // 2D 거리 차이가 56px 이상이면 2D 거리 우선, 아니면 3D 거리 우선
        if (dist2D < minDist2D - 56.0f) {
            minDist2D = dist2D;
            minDist3D = dist3D;
            bestTarget2D = headPos2D;
            lastTarget = &player;
        }
        else if (fabs(dist2D - minDist2D) <= 70.0f) {
            if (dist3D < minDist3D) {
                minDist2D = dist2D;
                minDist3D = dist3D;
                bestTarget2D = headPos2D;
                lastTarget = &player;
            }
        }
    }

    if (bestTarget2D.x != 0 && bestTarget2D.y != 0) {
        int deltaX = static_cast<int>(bestTarget2D.x - mouseImGui.x);
        int deltaY = static_cast<int>(bestTarget2D.y - mouseImGui.y);
        float dist = sqrtf(deltaX * deltaX + deltaY * deltaY);

        float smooth;
		
        if (dist <= 20.0f) {
            smooth = 1.2f * smoothMultiplier;
        }
        else if (dist <= 50.0f) {
            smooth = 1.8f * smoothMultiplier;
        }
        else if (dist <= 100.0f) {
            smooth = 2.5f * smoothMultiplier;
        }
        else {
            smooth = 3.5f * smoothMultiplier;
        }

        deltaX = static_cast<int>(deltaX / smooth);
        deltaY = static_cast<int>(deltaY / smooth);
        
        if (abs(deltaX) < 1 && abs(deltaY) < 1) {
            if (deltaX != 0) deltaX = (deltaX > 0) ? 1 : -1;
            if (deltaY != 0) deltaY = (deltaY > 0) ? 1 : -1;
        }

        INPUT input = { 0 };
        input.type = INPUT_MOUSE;
        input.mi.dx = deltaX;
        input.mi.dy = deltaY;
        input.mi.dwFlags = MOUSEEVENTF_MOVE;
        SendInput(1, &input, sizeof(INPUT));
    }
}
