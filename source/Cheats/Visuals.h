#pragma once

#include <Cheats/Players.h>
#include <SDK/Classes/World/Primtive.h>
#include <SDK/Classes/Math/CFrame.h>
#include <array>
#include <unordered_map>
#include <SDK/Offset.h>
#include <Render/Render.h>
#include <Windows.h>
#include <cmath>
#include <algorithm>
#include <Cheats/RayCast.h>
#include <Cheats/Object.h>
#include <Cheats/Aimbot.h>
#include <vector>
#include <thread>
#include <mutex>
#include <future>
#include <atomic>

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



inline ImU32 getChamsColor(float distance3D) {
    float alpha = 120.0f + (80.0f * (1.0f - std::min(distance3D / 200.0f, 1.0f)));
    int alphaInt = static_cast<int>(alpha);
    return IM_COL32(255, 100, 100, alphaInt);
}

inline ImU32 getChamsBorderColor(float distance3D) {
    float alpha = 180.0f + (75.0f * (1.0f - std::min(distance3D / 200.0f, 1.0f)));
    int alphaInt = static_cast<int>(alpha);
    return IM_COL32(255, 80, 80, alphaInt);
}


inline void drawOBBChams(ImDrawList* drawList, const CFrame& cframe, const CVector& size, ImU32 color) {
    CVector halfSize = CVector(size.x * 0.5f, size.y * 0.5f, size.z * 0.5f);

    CVector localVertices[8] = {
        {-halfSize.x, -halfSize.y, -halfSize.z},
        {+halfSize.x, -halfSize.y, -halfSize.z}, 
        {+halfSize.x, +halfSize.y, -halfSize.z},
        {-halfSize.x, +halfSize.y, -halfSize.z},
        {-halfSize.x, -halfSize.y, +halfSize.z},
        {+halfSize.x, -halfSize.y, +halfSize.z},
        {+halfSize.x, +halfSize.y, +halfSize.z}, 
        {-halfSize.x, +halfSize.y, +halfSize.z}  
    };

    Vector2 vertices2D[8];
    CVector worldVertices[8];

    for (int i = 0; i < 8; i++) {
        worldVertices[i] = cframe.pointToWorldSpace(localVertices[i]);
        vertices2D[i] = visualEngine->worldToScreen(worldVertices[i].toVector3());
    }

    const int faces[6][4] = {
        {4, 7, 6, 5},
        {1, 2, 3, 0},
        {3, 2, 6, 7},
        {0, 4, 5, 1},
        {1, 5, 6, 2},
        {0, 3, 7, 4}
    };

    for (int faceIdx = 0; faceIdx < 6; faceIdx++) {
        ImVec2 p1 = ImVec2(vertices2D[faces[faceIdx][0]].x, vertices2D[faces[faceIdx][0]].y);
        ImVec2 p2 = ImVec2(vertices2D[faces[faceIdx][1]].x, vertices2D[faces[faceIdx][1]].y);
        ImVec2 p3 = ImVec2(vertices2D[faces[faceIdx][2]].x, vertices2D[faces[faceIdx][2]].y);
        ImVec2 p4 = ImVec2(vertices2D[faces[faceIdx][3]].x, vertices2D[faces[faceIdx][3]].y);

        drawList->AddQuadFilled(p1, p2, p3, p4, color);
    }
}

inline void drawAngleBoundingBox(ImDrawList* drawList, const std::array<Vector2, R15_BoneCount>& bonePositions,
    bool isR15, ImU32 color, float thickness = 3.0f) {
    if (bonePositions.empty()) return;

    const auto& boneMap = isR15 ? boneMapR15 : boneMapR6;
    const int boneCount = isR15 ? R15_BoneCount : R6_BoneCount;

    float left = 0, top = 0, right = 0, bottom = 0;
    bool boundsInitialized = false;
    int validBones = 0;

    for (int i = 0; i < boneCount; ++i) {
        const Vector2& pos = bonePositions[i];
        if (pos.x != 0 || pos.y != 0) {
            if (!boundsInitialized) {
                left = right = pos.x;
                top = bottom = pos.y;
                boundsInitialized = true;
            }
            else {
                if (pos.x < left) left = pos.x;
                if (pos.y < top) top = pos.y;
                if (pos.x > right) right = pos.x;
                if (pos.y > bottom) bottom = pos.y;
            }
            validBones++;
        }
    }

    if (validBones > 0 && boundsInitialized) {
        float padding = 1.0f;
        left -= padding;
        top -= padding;
        right += padding;
        bottom += padding;

        drawList->AddRect(ImVec2(left, top), ImVec2(right, bottom), color, 0.0f, 0, thickness);
    }
}

inline void drawAABBVisualization(ImDrawList* drawList, const CVector& center, const CVector& size, ImU32 color) {
    CVector halfSize = CVector(size.x * 0.5f, size.y * 0.5f, size.z * 0.5f);

    CVector vertices[8] = {
        {center.x - halfSize.x, center.y - halfSize.y, center.z - halfSize.z}, // 0: 뒤쪽 아래 왼쪽
        {center.x + halfSize.x, center.y - halfSize.y, center.z - halfSize.z}, // 1: 뒤쪽 아래 오른쪽
        {center.x + halfSize.x, center.y + halfSize.y, center.z - halfSize.z}, // 2: 뒤쪽 위 오른쪽
        {center.x - halfSize.x, center.y + halfSize.y, center.z - halfSize.z}, // 3: 뒤쪽 위 왼쪽
        {center.x - halfSize.x, center.y - halfSize.y, center.z + halfSize.z}, // 4: 앞쪽 아래 왼쪽
        {center.x + halfSize.x, center.y - halfSize.y, center.z + halfSize.z}, // 5: 앞쪽 아래 오른쪽
        {center.x + halfSize.x, center.y + halfSize.y, center.z + halfSize.z}, // 6: 앞쪽 위 오른쪽
        {center.x - halfSize.x, center.y + halfSize.y, center.z + halfSize.z}  // 7: 앞쪽 위 왼쪽
    };

    // 2D로 변환
    Vector2 vertices2D[8];
    for (int i = 0; i < 8; i++) {
        vertices2D[i] = visualEngine->worldToScreen(vertices[i].toVector3());  // CVector -> Vector3 변환
    }


    const int edges[12][2] = {

        {0, 1}, {1, 2}, {2, 3}, {3, 0},

        {4, 5}, {5, 6}, {6, 7}, {7, 4},

        {0, 4}, {1, 5}, {2, 6}, {3, 7}
    };

    for (int i = 0; i < 12; i++) {
        const Vector2& v1 = vertices2D[edges[i][0]];
        const Vector2& v2 = vertices2D[edges[i][1]];


        if ((v1.x != 0 || v1.y != 0) && (v2.x != 0 || v2.y != 0)) {
            drawList->AddLine(ImVec2(v1.x, v1.y), ImVec2(v2.x, v2.y), color, 1.5f);
        }
    }
}


inline void drawOBBVisualization(ImDrawList* drawList, const CFrame& cframe, const CVector& size, ImU32 color) {
    CVector halfSize = CVector(size.x * 0.5f, size.y * 0.5f, size.z * 0.5f);
    CVector position = cframe.Position;

    // 박스 중심점과 카메라 간의 거리 계산
    float distanceToCamera = position.magnitude();


    //if (distanceToCamera > 500.0f) {
    //    drawAABBVisualization(drawList, cframe.Position, size, color);
    //    return;
    //}


    CVector localVertices[8] = {
        {-halfSize.x, -halfSize.y, -halfSize.z}, // 0: 뒤쪽 아래 왼쪽
        {+halfSize.x, -halfSize.y, -halfSize.z}, // 1: 뒤쪽 아래 오른쪽
        {+halfSize.x, +halfSize.y, -halfSize.z}, // 2: 뒤쪽 위 오른쪽
        {-halfSize.x, +halfSize.y, -halfSize.z}, // 3: 뒤쪽 위 왼쪽
        {-halfSize.x, -halfSize.y, +halfSize.z}, // 4: 앞쪽 아래 왼쪽
        {+halfSize.x, -halfSize.y, +halfSize.z}, // 5: 앞쪽 아래 오른쪽
        {+halfSize.x, +halfSize.y, +halfSize.z}, // 6: 앞쪽 위 오른쪽
        {-halfSize.x, +halfSize.y, +halfSize.z}  // 7: 앞쪽 위 왼쪽
    };

    Vector2 vertices2D[8];
    CVector worldVertices[8];
    bool validVertices[8];
    int validCount = 0;

    for (int i = 0; i < 8; i++) {
        worldVertices[i] = cframe.pointToWorldSpace(localVertices[i]);
        vertices2D[i] = visualEngine->worldToScreen(worldVertices[i].toVector3());

        // 거리 기반 유효성 검사
        bool isOnScreen = !(vertices2D[i].x == 0 && vertices2D[i].y == 0);
        bool isInBounds = vertices2D[i].x > -5000 && vertices2D[i].x < 5000 &&
            vertices2D[i].y > -5000 && vertices2D[i].y < 5000;

        if (distanceToCamera > 300.0f) {
            isInBounds = vertices2D[i].x > -2000 && vertices2D[i].x < 2000 &&
                vertices2D[i].y > -2000 && vertices2D[i].y < 2000;
        }

        validVertices[i] = isOnScreen && isInBounds;
        if (validVertices[i]) validCount++;
    }


  //  if (validCount < 6) {
  //      //drawOBBVisualization(drawList, cframe.Position, size, color);
		//drawAABBVisualization(drawList, cframe.Position, size, color);
  //      return;
  //  }


    const int edges[12][2] = {
        {0, 1}, {1, 2}, {2, 3}, {3, 0},
        {4, 5}, {5, 6}, {6, 7}, {7, 4},
        {0, 4}, {1, 5}, {2, 6}, {3, 7}
    };

    for (int i = 0; i < 12; i++) {
        if (!validVertices[edges[i][0]] || !validVertices[edges[i][1]]) continue;

        const Vector2& v1 = vertices2D[edges[i][0]];
        const Vector2& v2 = vertices2D[edges[i][1]];

        // 모서리 길이 검사
        float edgeLength = sqrtf(powf(v2.x - v1.x, 2) + powf(v2.y - v1.y, 2));
        float maxAllowedEdgeLength = 200.0f;
        if (distanceToCamera > 200.0f) {
            maxAllowedEdgeLength = 100.0f;
        }
        if (distanceToCamera > 400.0f) {
            maxAllowedEdgeLength = 50.0f;
        }

        if (edgeLength < maxAllowedEdgeLength) {
            drawList->AddLine(ImVec2(v1.x, v1.y), ImVec2(v2.x, v2.y), color, 2.0f);
        }
    }
}

inline void pf_Esp() {
    ImDrawList* drawList = ImGui::GetBackgroundDrawList();
    for (RBX::Instance& part : pf_PlayerPartList) {
        RBX::Primitive prim = part.getPrimitive();
        Vector3 PartPos3D = prim.getPartPosition();
        Vector2 PartPos2D = visualEngine->worldToScreen(PartPos3D);
        drawList->AddCircleFilled(ImVec2(PartPos2D.x, PartPos2D.y), 5.0f, IM_COL32(0, 255, 0, 255));
    }
}


inline void drawPlayerChams(ImDrawList* drawList, RBX::Player& player, float distance3D) {
    RBX::ModelInstance character = player.getModelInstance();
    if (!character.getAddress()) return;

    if (distance3D > 200.0f) return;

    ImU32 chamsColor = getChamsColor(distance3D);
    //ImU32 borderColor = getChamsBorderColor(distance3D);

    auto children = character.getChildren();

    constexpr size_t MAX_PARTS_TO_PROCESS = 15;
    size_t processedParts = 0;

    for (RBX::Instance& child : children) {
        if (processedParts >= MAX_PARTS_TO_PROCESS) break;

        if (!child.getAddress()) continue;

        const std::string& partName = child.getName();
        if (!isBodyPart(partName)) continue;

        RBX::Primitive primitive = child.getPrimitive();
        if (!primitive.getAddress()) continue;

        Vector3 position = primitive.getPartPosition();
        if (!visualEngine->isOnScreen(position)) continue;

        Vector3 size = primitive.getPartSize();
        if (size.x <= 0.1f || size.y <= 0.1f || size.z <= 0.1f ||
            size.x > 20.0f || size.y > 20.0f || size.z > 20.0f) {
            continue;
        }

        CFrame cframe = primitive.getPartCFrame();
        CVector sizeVec(size);

        drawOBBChams(drawList, cframe, sizeVec, chamsColor);
        //drawOBBVisualization(drawList, cframe, sizeVec, borderColor);

        processedParts++;
    }
}
inline void drawRaycastVisualization(ImDrawList* drawList, const CVector& myPos3D, bool hasValidMyPos) {
    if (!hasValidMyPos) return;

    for (RBX::Player& player : g_PlayerList) {
        if (player.getName() == g_LocalPlayer.getName() ||
            (player.getTeamName() == g_LocalPlayer.getTeamName() && player.getTeamName() != "No Team" && player.getTeamName() != "Players")) {
            continue;
        }

        RBX::ModelInstance character = player.getModelInstance();
        if (!character.getAddress()) continue;

        auto headPart = character.findFirstChild("Head");
        if (!headPart.getAddress()) continue;

        CVector headPos3D = CVector(headPart.getPrimitive().getPartPosition());
        if (!visualEngine->isOnScreen(headPos3D.toVector3())) continue;

        Vector2 myPos2D = visualEngine->worldToScreen(myPos3D.toVector3());
        Vector2 headPos2D = visualEngine->worldToScreen(headPos3D.toVector3());

        if ((myPos2D.x == 0 && myPos2D.y == 0) || (headPos2D.x == 0 && headPos2D.y == 0)) continue;

        CVector dir = headPos3D - myPos3D;
        float distance = dir.magnitude();
        if (distance == 0) continue;

        std::vector<Roblox::PartInfo> obstacles = getRelevantObstaclesWithSize(myPos3D.toVector3(), headPos3D.toVector3());

        Roblox::RaycastResult result = Roblox::Ray::cast_ray_with_rotation(myPos3D, dir, distance, obstacles);


        ImU32 rayColor = result.hit ? IM_COL32(255, 100, 100, 200) : IM_COL32(100, 255, 100, 200);
        drawList->AddLine(
            ImVec2(myPos2D.x, myPos2D.y),
            ImVec2(headPos2D.x, headPos2D.y),
            rayColor,
            3.0f
        );

        if (result.hit) {
            Vector2 hitPos2D = visualEngine->worldToScreen(result.hitPosition.toVector3());
            if (!(hitPos2D.x == 0 && hitPos2D.y == 0)) {
                drawList->AddCircleFilled(ImVec2(hitPos2D.x, hitPos2D.y), 8.0f, IM_COL32(255, 255, 0, 255));
                drawList->AddCircle(ImVec2(hitPos2D.x, hitPos2D.y), 10.0f, IM_COL32(255, 0, 0, 255), 0, 2.0f);

                char hitText[64];
                snprintf(hitText, sizeof(hitText), "OPTIMIZED HIT %.1fm", result.distance / 10.0f);
                drawList->AddText(ImVec2(hitPos2D.x + 15, hitPos2D.y - 10), IM_COL32(255, 255, 255, 255), hitText);
            }
        }

        const std::vector<Roblox::PartInfo>& partsToShow = result.hit ? result.partsUpToHit : obstacles;

        for (const auto& partInfo : partsToShow) {
            Vector2 partPos2D = visualEngine->worldToScreen(partInfo.position.toVector3());
            if (partPos2D.x == 0 && partPos2D.y == 0) continue;

            float maxDim = partInfo.size.x;
            if (partInfo.size.y > maxDim) maxDim = partInfo.size.y;
            if (partInfo.size.z > maxDim) maxDim = partInfo.size.z;

            ImU32 partColor;
            if (result.hit && &partInfo == &result.hitPart) {
                partColor = IM_COL32(255, 50, 50, 200);
            }
            else if (maxDim < 5.0f) {
                partColor = IM_COL32(100, 150, 255, 150);
            }
            else if (maxDim < 15.0f) {
                partColor = IM_COL32(255, 200, 50, 150);
            }
            else {
                partColor = IM_COL32(255, 150, 50, 150);
            }


            drawOBBVisualization(drawList, partInfo.cframe, partInfo.size, partColor);

            drawList->AddCircleFilled(ImVec2(partPos2D.x, partPos2D.y), 2.0f, IM_COL32(255, 255, 255, 100));

            float distToPart = (partInfo.position - myPos3D).magnitude();
            if (distToPart < 100.0f) {
                char sizeText[128];
                snprintf(sizeText, sizeof(sizeText), "%s\nOBB: %.1f×%.1f×%.1f %s",
                    partInfo.className.c_str(),
                    partInfo.size.x, partInfo.size.y, partInfo.size.z,
                    (result.hit && &partInfo == &result.hitPart) ? "(HIT!)" : "");


                ImVec2 textPos = ImVec2(partPos2D.x + 10, partPos2D.y - 30);
                ImVec2 textSize = ImGui::CalcTextSize(sizeText);
                drawList->AddRectFilled(
                    ImVec2(textPos.x - 2, textPos.y - 2),
                    ImVec2(textPos.x + textSize.x + 2, textPos.y + textSize.y + 2),
                    IM_COL32(0, 0, 0, 180),
                    3.0f
                );

                drawList->AddText(textPos, IM_COL32(255, 255, 255, 255), sizeText);
            }
        }

        char playerInfo[128];
        snprintf(playerInfo, sizeof(playerInfo), "%s\nDistance: %.1fm\nVisible Parts: %zu/%zu\nStatus: %s (OPTIMIZED)",
            player.getName().c_str(),
            distance / 10.0f,
            partsToShow.size(),
            obstacles.size(),
            result.hit ? "BLOCKED" : "CLEAR"
        );

        ImVec2 infoPos = ImVec2(headPos2D.x + 15, headPos2D.y - 40);
        ImVec2 infoSize = ImGui::CalcTextSize(playerInfo);

        drawList->AddRectFilled(
            ImVec2(infoPos.x - 3, infoPos.y - 3),
            ImVec2(infoPos.x + infoSize.x + 3, infoPos.y + infoSize.y + 3),
            IM_COL32(0, 0, 0, 200),
            4.0f
        );

        drawList->AddText(infoPos, IM_COL32(255, 255, 255, 255), playerInfo);

        break;
    }
}
inline void drawHitIndicator(ImDrawList* drawList, RBX::Player& player, const CVector& myPos3D, bool hasValidMyPos) {
    if (!hasValidMyPos) return;

    RBX::ModelInstance character = player.getModelInstance();
    if (!character.getAddress()) return;

    auto headPart = character.findFirstChild("Head");
    if (!headPart.getAddress()) return;

    CVector headPos3D = CVector(headPart.getPrimitive().getPartPosition());  // Vector3 -> CVector 변환
    Vector2 headPos2D = visualEngine->worldToScreen(headPos3D.toVector3());  // CVector -> Vector3 변환
    if (headPos2D.x == 0 && headPos2D.y == 0) return;

    CVector dir = headPos3D - myPos3D;
    float distance3D = dir.magnitude();
    if (distance3D == 0) return;

    std::vector<Roblox::PartInfo> obstacles = getRelevantObstaclesWithSize(myPos3D.toVector3(), headPos3D.toVector3());  // CVector -> Vector3 변환
    Roblox::RaycastResult result = Roblox::Ray::cast_ray_with_rotation(myPos3D, dir, distance3D, obstacles);

    if (!result.hit) {
        const char* hitText = "OBB CLEAR";
        ImVec2 textSize = ImGui::CalcTextSize(hitText);
        float hitTextY = headPos2D.y - textSize.y - 50;

        ImVec2 bgMin = ImVec2(headPos2D.x - textSize.x / 2 - 2, hitTextY - 2);
        ImVec2 bgMax = ImVec2(headPos2D.x + textSize.x / 2 + 2, hitTextY + textSize.y + 2);
        drawList->AddRectFilled(bgMin, bgMax, IM_COL32(0, 0, 0, 180), 2.0f);

        drawList->AddText(
            ImVec2(headPos2D.x - textSize.x / 2, hitTextY),
            IM_COL32(0, 255, 0, 255),
            hitText
        );
    }
}


struct PlayerRenderData {
    RBX::Player player;
    std::string playerName; 
    CVector headPos3D;
    Vector2 headPos2D;
    float distance3D;
    ImU32 lineColor;
    std::array<Vector2, R15_BoneCount> bonePos2D;
    bool isValid;
    bool isR15;
};

static std::vector<std::thread> g_WorkerThreads;
static std::atomic<bool> g_ThreadPoolActive{false};
static std::mutex g_RenderDataMutex;
static std::vector<PlayerRenderData> g_RenderDataCache;
static constexpr size_t THREAD_POOL_SIZE = 100; // cpu core 수에 맞게


inline void InitESPThreadPool() {
    if (g_ThreadPoolActive.load()) return;
    
    g_ThreadPoolActive.store(true);
    g_WorkerThreads.reserve(THREAD_POOL_SIZE);
}

inline void ShutdownESPThreadPool() {
    g_ThreadPoolActive.store(false);
    
    for (auto& thread : g_WorkerThreads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    g_WorkerThreads.clear();
}

inline PlayerRenderData ProcessPlayerData(RBX::Player& player, const CVector& myPos3D, bool hasValidMyPos) {
    PlayerRenderData data{};
    data.player = player;
    data.isValid = false;
    
    try {
        try {
            data.playerName = player.getName();
        } catch (...) {
            data.playerName = "Unknown";
        }
        
        RBX::ModelInstance character = player.getModelInstance();
        if (!character.getAddress()) return data;
        
        auto headPart = character.findFirstChild("Head");
        if (!headPart.getAddress()) return data;
        
        data.headPos3D = CVector(headPart.getPrimitive().getPartPosition());
        if (!visualEngine->isOnScreen(data.headPos3D.toVector3())) return data;
        
        data.headPos2D = visualEngine->worldToScreen(data.headPos3D.toVector3());
        if (data.headPos2D.x == 0 && data.headPos2D.y == 0) return data;
        
        // 3D 거리 계산
        if (hasValidMyPos) {
            float dx = data.headPos3D.x - myPos3D.x;
            float dy = data.headPos3D.y - myPos3D.y;
            float dz = data.headPos3D.z - myPos3D.z;
            data.distance3D = sqrtf(dx * dx + dy * dy + dz * dz);
            
            // 거리 기반 색상 계산
            float normalizedDist = data.distance3D / 200.0f;
            if (normalizedDist > 1.0f) normalizedDist = 1.0f;
            const int nearRed = 255;
            const int farRed = 90;
            int red = static_cast<int>(nearRed + (farRed - nearRed) * normalizedDist);
            data.lineColor = IM_COL32(red, 0, 0, 255);
        } else {
            data.distance3D = 0.0f;
            data.lineColor = IM_COL32(255, 0, 0, 255);
        }
        
        // 스켈레톤 데이터 준비
        data.isR15 = character.findFirstChild("UpperTorso").getAddress() != 0;
        const auto& boneMap = data.isR15 ? boneMapR15 : boneMapR6;
        
        std::vector<RBX::Instance> children = character.getChildren();
        for (RBX::Instance& child : children) {
            try {
                std::string childName = child.getName();  // str -> std::string
                auto it = boneMap.find(childName);
                if (it != boneMap.end()) {
                    int boneIndex = it->second;
                    auto primitive = child.getPrimitive();
                    Vector3 pos3D = primitive.getPartPosition();
                    data.bonePos2D[boneIndex] = visualEngine->worldToScreen(pos3D);
                }
            } catch (...) {
                continue;
            }
        }
        
        data.isValid = true;
    } catch (...) {
        data.isValid = false;
    }
    
    return data;
}

inline void espLoopThreaded() {
    if (toggePF) {
        pf_Esp();
        return;
    }
    
    ImDrawList* drawList = ImGui::GetBackgroundDrawList();
    ImVec2 viewportPos = ImGui::GetMainViewport()->Pos;
    ImVec2 viewportSize = ImGui::GetMainViewport()->Size;
    ImVec2 screenBottomCenter = ImVec2(
        viewportPos.x + viewportSize.x / 2,
        viewportPos.y + viewportSize.y - 5
    );
    
    CVector myPos3D = { 0, 0, 0 };
    bool hasValidMyPos = false;
    
    if (g_LocalPlayer.getAddress()) {
        RBX::Player localPlayerInstance(g_LocalPlayer.getAddress());
        RBX::ModelInstance myCharacter = localPlayerInstance.getModelInstance();
        if (myCharacter.getAddress()) {
            auto myHead = myCharacter.findFirstChild("Head");
            if (myHead.getAddress()) {
                myPos3D = CVector(myHead.getPrimitive().getPartPosition());
                hasValidMyPos = true;
            }
        }
    }

    const size_t playerCount = g_PlayerList.size();
    if (playerCount == 0) return;
    
    const size_t chunkSize = std::max(size_t(1), playerCount / THREAD_POOL_SIZE);
    std::vector<std::future<std::vector<PlayerRenderData>>> futures;

    for (size_t i = 0; i < playerCount; i += chunkSize) {
        size_t endIdx = std::min(i + chunkSize, playerCount);
        
        auto future = std::async(std::launch::async, [i, endIdx, &myPos3D, hasValidMyPos]() {
            std::vector<PlayerRenderData> chunkData;
            chunkData.reserve(endIdx - i);
            
            for (size_t j = i; j < endIdx; ++j) {
                RBX::Player& player = g_PlayerList[j];

                if (player.getName() == g_LocalPlayer.getName() ||
                    (player.getTeamName() == g_LocalPlayer.getTeamName() && 
                     player.getTeamName() != "No Team" && 
                     player.getTeamName() != "Players")) {
                    continue;
                }
                
                PlayerRenderData data = ProcessPlayerData(player, myPos3D, hasValidMyPos);
                if (data.isValid) {
                    chunkData.push_back(std::move(data));
                }
            }
            
            return chunkData;
        });
        
        futures.push_back(std::move(future));
    }

    std::vector<PlayerRenderData> allRenderData;
    for (auto& future : futures) {
        try {
            auto chunkData = future.get();
            allRenderData.insert(allRenderData.end(), 
                                std::make_move_iterator(chunkData.begin()),
                                std::make_move_iterator(chunkData.end()));
        } catch (...) {

        }
    }
    

    constexpr float ESP_LINE_THICKNESS = 1.5f;
    constexpr float ESP_BOX_THICKNESS = 2.0f;
    
    for (const auto& data : allRenderData) {
        // Tracer 그리기
        if (esp_show_tracer) {
            ImU32 tracerColor = IM_COL32(
                (data.lineColor >> 24) & 0xFF,  // R
                (data.lineColor >> 16) & 0xFF,  // G  
                (data.lineColor >> 8) & 0xFF,   // B
                150                              // A
            );
            drawList->AddLine(
                screenBottomCenter,
                ImVec2(data.headPos2D.x, data.headPos2D.y),
                tracerColor,
                ESP_LINE_THICKNESS
            );
        }
        
        // Chams 그리기
        if (esp_chams) {
            drawPlayerChams(drawList, const_cast<RBX::Player&>(data.player), data.distance3D);
        }
        
        // 이름 표시
        if (esp_show_names) {

            ImVec2 textSize = ImGui::CalcTextSize(data.playerName.c_str());
            float nameY = data.headPos2D.y - textSize.y - 5;
            drawList->AddText(ImVec2(data.headPos2D.x - textSize.x / 2, nameY),
                IM_COL32(255, 255, 255, 255), data.playerName.c_str());
        }
        // 거리 표시
        if (esp_show_distance && hasValidMyPos) {
            char distText[32];
            snprintf(distText, sizeof(distText), "%.1fm", data.distance3D / 10.0f);
            ImVec2 distTextSize = ImGui::CalcTextSize(distText);
            float distY = data.headPos2D.y - 35;
            drawList->AddText(ImVec2(data.headPos2D.x - distTextSize.x / 2, distY), 
                            IM_COL32(0, 0, 255, 255), distText);
        }
        
        // 스켈레톤 그리기
        if (esp_show_bones) {
            const BoneLine* lines = data.isR15 ? linesR15 : linesR6;
            const size_t lineCount = data.isR15 ? lineCountR15 : lineCountR6;
            
            for (size_t i = 0; i < lineCount; ++i) {
                const Vector2& a = data.bonePos2D[lines[i].a];
                const Vector2& b = data.bonePos2D[lines[i].b];
                if ((a.x != 0 || a.y != 0) && (b.x != 0 || b.y != 0)) {
                    drawList->AddLine(ToImVec2(a), ToImVec2(b), data.lineColor, ESP_LINE_THICKNESS);
                }
            }
        }
        
        // 박스 그리기
        if (esp_show_box) {
            drawAngleBoundingBox(drawList, data.bonePos2D, data.isR15, data.lineColor, ESP_BOX_THICKNESS);
        }
    }
    
    // 크로스헤어 그리기
    drawList->AddCircleFilled(screenBottomCenter, 5.0f, IM_COL32(255, 255, 255, 150));
}


inline void espLoop() {
    static bool threadPoolInitialized = false;
    if (!threadPoolInitialized) {
        InitESPThreadPool();
        threadPoolInitialized = true;
    }
    
    espLoopThreaded();
}
