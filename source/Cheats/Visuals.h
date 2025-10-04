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
#include <chrono>
#include <queue>
#include <condition_variable>

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

struct WallcheckResult {
    std::string playerName;
    bool isVisible = false;
    std::chrono::steady_clock::time_point lastUpdate;
    bool isValid = false;

    WallcheckResult() : lastUpdate(std::chrono::steady_clock::now()) {}
};

class WallcheckThreadPool {
private:
    static std::vector<std::thread> workers;
    static std::atomic<bool> shouldStop;
    static std::mutex resultMutex;
    static std::unordered_map<std::string, WallcheckResult> wallcheckResults;
    static std::queue<std::function<void()>> taskQueue;
    static std::mutex queueMutex;
    static std::condition_variable condition;
    static constexpr size_t WALLCHECK_THREAD_COUNT = 4;
    static constexpr std::chrono::milliseconds WALLCHECK_CACHE_DURATION{ 100 };

public:
    static void Initialize() {
        if (!workers.empty()) return;

        shouldStop.store(false);
        workers.reserve(WALLCHECK_THREAD_COUNT);

        for (size_t i = 0; i < WALLCHECK_THREAD_COUNT; ++i) {
            workers.emplace_back([]() {
                while (!shouldStop.load()) {
                    std::function<void()> task;

                    {
                        std::unique_lock<std::mutex> lock(queueMutex);
                        condition.wait(lock, []() { return shouldStop.load() || !taskQueue.empty(); });

                        if (shouldStop.load()) break;
                        if (taskQueue.empty()) continue;

                        task = std::move(taskQueue.front());
                        taskQueue.pop();
                    }

                    try {
                        task();
                    }
                    catch (...) {

                    }
                }
                });
        }
    }

    static void Shutdown() {
        shouldStop.store(true);
        condition.notify_all();

        for (auto& worker : workers) {
            if (worker.joinable()) {
                worker.join();
            }
        }
        workers.clear();

        {
            std::lock_guard<std::mutex> lock(resultMutex);
            wallcheckResults.clear();
        }
    }

    static void SubmitTask(std::function<void()> task) {
        {
            std::lock_guard<std::mutex> lock(queueMutex);
            taskQueue.push(std::move(task));
        }
        condition.notify_one();
    }

    static void SubmitWallcheckTask(const std::string& playerName, const CVector& myPos, const CVector& targetPos) {
        {
            std::lock_guard<std::mutex> lock(resultMutex);
            auto it = wallcheckResults.find(playerName);
            if (it != wallcheckResults.end()) {
                auto timeSinceUpdate = std::chrono::steady_clock::now() - it->second.lastUpdate;
                if (timeSinceUpdate < WALLCHECK_CACHE_DURATION && it->second.isValid) {
                    return;
                }
            }
        }
        {
            std::lock_guard<std::mutex> lock(queueMutex);
            taskQueue.emplace([playerName, myPos, targetPos]() {
                PerformWallcheck(playerName, myPos, targetPos);
                });
        }
        condition.notify_one();
    }

    static bool GetWallcheckResult(const std::string& playerName, bool& isVisible) {
        std::lock_guard<std::mutex> lock(resultMutex);
        auto it = wallcheckResults.find(playerName);
        if (it != wallcheckResults.end()) {
            auto timeSinceUpdate = std::chrono::steady_clock::now() - it->second.lastUpdate;
            if (timeSinceUpdate < WALLCHECK_CACHE_DURATION && it->second.isValid) {
                isVisible = it->second.isVisible;
                return true;
            }
            else if (it->second.isValid) {
                isVisible = it->second.isVisible;
                return true;
            }
        }
        return false;
    }

private:
    static void PerformWallcheck(const std::string& playerName, const CVector& myPos, const CVector& targetPos) {
        try {
            CVector direction = targetPos - myPos;
            float distance = direction.magnitude();
            if (distance == 0) return;
            std::vector<Roblox::PartInfo> obstacles = getValidObstaclesForRaycast(
                myPos.toVector3(), targetPos.toVector3()
            );

            Roblox::RaycastResult result = Roblox::Ray::raycastRotation(
                myPos, direction, distance, std::move(obstacles));

            {
                std::lock_guard<std::mutex> lock(resultMutex);
                auto& wallcheckResult = wallcheckResults[playerName];
                wallcheckResult.playerName = playerName;
                wallcheckResult.isVisible = !result.hit;
                wallcheckResult.lastUpdate = std::chrono::steady_clock::now();
                wallcheckResult.isValid = true;
            }
        }
        catch (...) {
            std::lock_guard<std::mutex> lock(resultMutex);
            auto& wallcheckResult = wallcheckResults[playerName];
            wallcheckResult.isVisible = false;
            wallcheckResult.isValid = false;
        }
    }
};
inline void drawCursor() {
    POINT mousePos;
    GetCursorPos(&mousePos);

    ImVec2 viewportPos = ImGui::GetMainViewport()->Pos;
    ImVec2 mouseImGui = ImVec2(mousePos.x - viewportPos.x, mousePos.y - viewportPos.y);

    ImDrawList* drawList = ImGui::GetForegroundDrawList();
    static auto startTime = std::chrono::steady_clock::now();
    auto currentTime = std::chrono::steady_clock::now();
    f32 timeElapsed = std::chrono::duration<f32>(currentTime - startTime).count();

    const f32 CYCLE_TIME = 3.0f;
    f32 normalizedTime = fmodf(timeElapsed, CYCLE_TIME) / CYCLE_TIME;

    f32 easedTime = easeInOutSineG(normalizedTime);
    f32 rotation = easedTime * 2.0f * M_PI;

    f32 baseSize = 27.0f;
    f32 maxSize = 46.0f;
    f32 sizeSpeed = 2.5f;
    f32 sizeAmplitude = (maxSize - baseSize) * 0.5f;
    f32 crossSize = baseSize + sizeAmplitude + sizeAmplitude * sinf(timeElapsed * sizeSpeed);

    f32 crossThickness = 2.5f;
    f32 gapSize = 13.0f;

    f32 outlineSize = crossSize + 0.5f;
    f32 outlineThickness = crossThickness + 2.0f;
    f32 outlineGapSize = gapSize - 1.0f;

    ImU32 hitmarkerColor = getRainbowColor(timeElapsed, 1.0f);
    ImU32 outlineColor = IM_COL32(0, 0, 0, 180);

    f32 cosR = cosf(rotation);
    f32 sinR = sinf(rotation);
    auto rotatePoint = [&mouseImGui, cosR, sinR](f32 x, f32 y) -> ImVec2 {
        return ImVec2(
            mouseImGui.x + (x * cosR - y * sinR),
            mouseImGui.y + (x * sinR + y * cosR)
        );
        };

    //outline
    ImVec2 outline_top1 = rotatePoint(0, -outlineSize);
    ImVec2 outline_top2 = rotatePoint(0, -outlineGapSize);
    ImVec2 outline_bottom1 = rotatePoint(0, outlineGapSize);
    ImVec2 outline_bottom2 = rotatePoint(0, outlineSize);
    ImVec2 outline_left1 = rotatePoint(-outlineSize, 0);
    ImVec2 outline_left2 = rotatePoint(-outlineGapSize, 0);
    ImVec2 outline_right1 = rotatePoint(outlineGapSize, 0);
    ImVec2 outline_right2 = rotatePoint(outlineSize, 0);

    drawList->AddLine(outline_top1, outline_top2, outlineColor, outlineThickness);
    drawList->AddLine(outline_bottom1, outline_bottom2, outlineColor, outlineThickness);
    drawList->AddLine(outline_left1, outline_left2, outlineColor, outlineThickness);
    drawList->AddLine(outline_right1, outline_right2, outlineColor, outlineThickness);

    // main cursor
    ImVec2 top1 = rotatePoint(0, -crossSize);
    ImVec2 top2 = rotatePoint(0, -gapSize);
    ImVec2 bottom1 = rotatePoint(0, gapSize);
    ImVec2 bottom2 = rotatePoint(0, crossSize);
    ImVec2 left1 = rotatePoint(-crossSize, 0);
    ImVec2 left2 = rotatePoint(-gapSize, 0);
    ImVec2 right1 = rotatePoint(gapSize, 0);
    ImVec2 right2 = rotatePoint(crossSize, 0);

    drawList->AddLine(top1, top2, hitmarkerColor, crossThickness);
    drawList->AddLine(bottom1, bottom2, hitmarkerColor, crossThickness);
    drawList->AddLine(left1, left2, hitmarkerColor, crossThickness);
    drawList->AddLine(right1, right2, hitmarkerColor, crossThickness);

    const char* text = "Untitled External";
    ImVec2 textPos = ImVec2(mouseImGui.x - 55, mouseImGui.y + 55);
    ImU32 textOutlineColor = IM_COL32(0, 0, 0, 200);
    f32 textOutlineThickness = 1.0f;
    // text outline
    for (i32 dx = -1; dx <= 1; dx++) {
        for (i32 dy = -1; dy <= 1; dy++) {
            if (dx == 0 && dy == 0) continue;
            drawList->AddText(
                ImVec2(textPos.x + dx * textOutlineThickness, textPos.y + dy * textOutlineThickness),
                textOutlineColor,
                text
            );
        }
    }
    drawList->AddText(textPos, hitmarkerColor, text);
}
std::vector<std::thread> WallcheckThreadPool::workers;
std::atomic<bool> WallcheckThreadPool::shouldStop{ false };
std::mutex WallcheckThreadPool::resultMutex;
std::unordered_map<std::string, WallcheckResult> WallcheckThreadPool::wallcheckResults;
std::queue<std::function<void()>> WallcheckThreadPool::taskQueue;
std::mutex WallcheckThreadPool::queueMutex;
std::condition_variable WallcheckThreadPool::condition;

inline ImU32 calculateChamsColor(float distance3D) {
    float alpha = 120.0f + (80.0f * (1.0f - std::min(distance3D / 400.0f, 1.0f)));
    int alphaInt = static_cast<int>(alpha);
    return IM_COL32(255, 100, 100, alphaInt);
}

inline ImU32 calculateChamsColorWithVisibility(float distance3D, bool isVisible) {
    float alpha = 120.0f + (80.0f * (1.0f - std::min(distance3D / 400.0f, 1.0f)));
    int alphaInt = static_cast<int>(alpha);

    if (isVisible) {
        return IM_COL32(255, 100, 100, alphaInt);
    }
    else {
        return IM_COL32(100, 100, 255, alphaInt);
    }
}

inline ImU32 getChamsColorForPlayer(float distance3D, const std::string& playerName,
    const CVector& myPos3D, const CVector& headPos3D, bool hasValidMyPos) {

    static std::unordered_map<std::string, ImU32> lastColorCache;
    static std::mutex colorCacheMutex;

    float alpha = 120.0f + (80.0f * (1.0f - std::min(distance3D / 400.0f, 1.0f)));
    int alphaInt = static_cast<int>(alpha);

    if (!hasValidMyPos) {
        return IM_COL32(255, 100, 100, alphaInt);
    }
    bool isVisible = false;
    ImU32 newColor;
    WallcheckThreadPool::SubmitWallcheckTask(playerName, myPos3D, headPos3D);
    if (WallcheckThreadPool::GetWallcheckResult(playerName, isVisible)) {
        newColor = isVisible ? IM_COL32(255, 100, 100, alphaInt) : IM_COL32(100, 100, 255, alphaInt);
        std::lock_guard<std::mutex> lock(colorCacheMutex);
        lastColorCache[playerName] = newColor;
        return newColor;
    }
    else {
        std::lock_guard<std::mutex> lock(colorCacheMutex);
        auto it = lastColorCache.find(playerName);
        if (it != lastColorCache.end()) {
            return it->second;
        }
        else {
            ImU32 defaultColor = IM_COL32(200, 150, 100, alphaInt);
            lastColorCache[playerName] = defaultColor;
            return defaultColor;
        }
    }
}
inline void renderChamsBox(ImDrawList* drawList, const CFrame& cframe, const CVector& size, ImU32 color, bool isVisible = true, float timeElapsed = 0.0f) {
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
    bool bVerticesOnScreen[8] = { false };

    for (int i = 0; i < 8; i++) {
        worldVertices[i] = cframe.pointToWorldSpace(localVertices[i]);
        vertices2D[i] = visualEngine->worldToScreen(worldVertices[i].toVector3());
        if (vertices2D[i].x != 0 || vertices2D[i].y != 0) {
            bVerticesOnScreen[i] = true;
        }
    }

    const int faces[6][4] = {
        {4, 7, 6, 5}, // +Z
        {1, 2, 3, 0}, // -Z
        {3, 2, 6, 7}, // +Y
        {0, 4, 5, 1}, // -Y
        {1, 5, 6, 2}, // +X
        {0, 3, 7, 4}  // -X
    };

    ImU32 finalColor = color;
    if (g_Rainbow) {
        finalColor = getRainbowColor(timeElapsed, 1.0f);
        int originalAlpha = (color >> 24) & 0xFF;
        finalColor = (finalColor & 0x00FFFFFF) | (originalAlpha << 24);
    }

    for (int faceIdx = 0; faceIdx < 6; faceIdx++) {
        const int* face = faces[faceIdx];

        ImVec2 points[4] = {
            ToImVec2(vertices2D[face[0]]),
            ToImVec2(vertices2D[face[1]]),
            ToImVec2(vertices2D[face[2]]),
            ToImVec2(vertices2D[face[3]])
        };

        //back face culling
        float signedArea = (points[1].x - points[0].x) * (points[2].y - points[0].y) - (points[1].y - points[0].y) * (points[2].x - points[0].x);
        if (signedArea < 0) {
            continue;
        }

        drawList->AddConvexPolyFilled(points, 4, finalColor);
    }

    if (g_Rainbow) {
        const int edges[12][2] = {
            {0, 1}, {1, 2}, {2, 3}, {3, 0},
            {4, 5}, {5, 6}, {6, 7}, {7, 4},
            {0, 4}, {1, 5}, {2, 6}, {3, 7}
        };

        ImU32 outlineColor;

        if (isVisible) {
            outlineColor = finalColor;
            int r = (outlineColor >> 0) & 0xFF;
            int g = (outlineColor >> 8) & 0xFF;
            int b = (outlineColor >> 16) & 0xFF;
            int a = (outlineColor >> 24) & 0xFF;
            r = static_cast<int>(r * 0.5f);
            g = static_cast<int>(g * 0.5f);
            b = static_cast<int>(b * 0.5f);
            outlineColor = IM_COL32(r, g, b, a); // rainbow outline
        }
        else {
            outlineColor = finalColor;
            int r = (outlineColor >> 0) & 0xFF;
            int g = (outlineColor >> 8) & 0xFF;
            int b = (outlineColor >> 16) & 0xFF;
            int a = (outlineColor >> 24) & 0xFF;
            r = static_cast<int>(r * 0.5f);
            g = static_cast<int>(g * 0.5f);
            b = static_cast<int>(b * 0.5f);
            outlineColor = IM_COL32(r, g, b, a);
        }

        for (int i = 0; i < 12; i++) {
            const int* edge = edges[i];
            if (!bVerticesOnScreen[edge[0]] || !bVerticesOnScreen[edge[1]]) {
                continue;
            }
            const Vector2& v1 = vertices2D[edge[0]];
            const Vector2& v2 = vertices2D[edge[1]];

            drawList->AddLine(ImVec2(v1.x, v1.y), ImVec2(v2.x, v2.y), outlineColor, 2.0f);
        }
    }
}

inline void renderPlayerChamsWithVisibility(ImDrawList* drawList, RBX::Player& player, float distance3D,
    const CVector& myPos3D, bool hasValidMyPos) {
    RBX::ModelInstance character = player.getModelInstance();
    if (!character.getAddress()) return;

    if (distance3D > 400.0f) return;

    ImU32 chamsColor;
    bool playerIsVisible = true;

    if (hasValidMyPos) {
        auto headPart = character.findFirstChild("Head");
        if (headPart.getAddress()) {
            CVector headPos3D = CVector(headPart.getPrimitive().getPartPosition());
            chamsColor = getChamsColorForPlayer(distance3D, player.getName(), myPos3D, headPos3D, hasValidMyPos);

            WallcheckThreadPool::SubmitWallcheckTask(player.getName(), myPos3D, headPos3D);
            WallcheckThreadPool::GetWallcheckResult(player.getName(), playerIsVisible);
        }
        else {
            chamsColor = calculateChamsColor(distance3D);
        }
    }
    else {
        chamsColor = calculateChamsColor(distance3D);
    }

    static auto startTime = std::chrono::steady_clock::now();
    auto currentTime = std::chrono::steady_clock::now();
    float timeElapsed = std::chrono::duration<float>(currentTime - startTime).count();

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

        renderChamsBox(drawList, cframe, sizeVec, chamsColor, playerIsVisible, timeElapsed);

        processedParts++;
    }
}

inline void renderPlayerChams(ImDrawList* drawList, RBX::Player& player, float distance3D) {
    RBX::ModelInstance character = player.getModelInstance();
    if (!character.getAddress()) return;

    if (distance3D > 400.0f) return;

    ImU32 chamsColor = calculateChamsColor(distance3D);

    static auto startTime = std::chrono::steady_clock::now();
    auto currentTime = std::chrono::steady_clock::now();
    float timeElapsed = std::chrono::duration<float>(currentTime - startTime).count();

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

        renderChamsBox(drawList, cframe, sizeVec, chamsColor, true, timeElapsed);

        processedParts++;
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



inline void drawOBBVisualization(ImDrawList* drawList, const CFrame& cframe, const CVector& size, ImU32 color) {
    CVector halfSize = CVector(size.x * 0.5f, size.y * 0.5f, size.z * 0.5f);
    CVector position = cframe.Position;

    float distanceToCamera = position.magnitude();

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
    bool validVertices[8];
    int validCount = 0;

    for (int i = 0; i < 8; i++) {
        worldVertices[i] = cframe.pointToWorldSpace(localVertices[i]);
        vertices2D[i] = visualEngine->worldToScreen(worldVertices[i].toVector3());

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

    const int edges[12][2] = {
        {0, 1}, {1, 2}, {2, 3}, {3, 0},
        {4, 5}, {5, 6}, {6, 7}, {7, 4},
        {0, 4}, {1, 5}, {2, 6}, {3, 7}
    };

    for (int i = 0; i < 12; i++) {
        if (!validVertices[edges[i][0]] || !validVertices[edges[i][1]]) continue;

        const Vector2& v1 = vertices2D[edges[i][0]];
        const Vector2& v2 = vertices2D[edges[i][1]];

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

        std::vector<Roblox::PartInfo> obstacles = getValidObstaclesForRaycast(myPos3D.toVector3(), headPos3D.toVector3());
        Roblox::RaycastResult result = Roblox::Ray::raycastRotation(myPos3D, dir, distance, obstacles);

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

            float maxDim = std::max({ partInfo.size.x, partInfo.size.y, partInfo.size.z });

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

    CVector headPos3D = CVector(headPart.getPrimitive().getPartPosition());
    Vector2 headPos2D = visualEngine->worldToScreen(headPos3D.toVector3());
    if (headPos2D.x == 0 && headPos2D.y == 0) return;

    CVector dir = headPos3D - myPos3D;
    float distance3D = dir.magnitude();
    if (distance3D == 0) return;

    std::vector<Roblox::PartInfo> obstacles = getValidObstaclesForRaycast(myPos3D.toVector3(), headPos3D.toVector3());
    Roblox::RaycastResult result = Roblox::Ray::raycastRotation(myPos3D, dir, distance3D, obstacles);

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
    Vector2 boundingBoxBottomCenter;
};

static std::vector<std::thread> g_WorkerThreads;
static std::atomic<bool> g_ThreadPoolActive{ false };
static std::mutex g_RenderDataMutex;
static std::vector<PlayerRenderData> g_RenderDataCache;
static constexpr size_t THREAD_POOL_SIZE = 4;

inline void InitESPThreadPool() {
    if (g_ThreadPoolActive.load()) return;

    g_ThreadPoolActive.store(true);
    g_WorkerThreads.reserve(THREAD_POOL_SIZE);

    WallcheckThreadPool::Initialize();
}

inline void ShutdownESPThreadPool() {
    g_ThreadPoolActive.store(false);

    for (auto& thread : g_WorkerThreads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    g_WorkerThreads.clear();

    WallcheckThreadPool::Shutdown();
}

inline PlayerRenderData ProcessPlayerData(RBX::Player& player, const CVector& myPos3D, bool hasValidMyPos) {
    PlayerRenderData data{};
    data.player = player;
    data.isValid = false;

    try {
        try {
            data.playerName = player.getName();
        }
        catch (...) {
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
        }
        else {
            data.distance3D = 0.0f;
            data.lineColor = IM_COL32(255, 0, 0, 255);
        }

        // 스켈레톤 데이터 준비
        data.isR15 = character.findFirstChild("UpperTorso").getAddress() != 0;
        const auto& boneMap = data.isR15 ? boneMapR15 : boneMapR6;

        std::vector<RBX::Instance> children = character.getChildren();
        for (RBX::Instance& child : children) {
            try {
                str childName = child.getName();
                auto it = boneMap.find(childName);
                if (it != boneMap.end()) {
                    int boneIndex = it->second;
                    auto primitive = child.getPrimitive();
                    Vector3 pos3D = primitive.getPartPosition();
                    data.bonePos2D[boneIndex] = visualEngine->worldToScreen(pos3D);
                }
            }
            catch (...) {
                continue;
            }
        }

        data.isValid = true;
    }
    catch (...) {
        data.isValid = false;
    }

    return data;
}

inline void drawCornerBoundingBox(ImDrawList* drawList, const std::array<Vector2, R15_BoneCount>& bonePositions,
    bool isR15, ImU32 color, float thickness = 3.0f, float cornerLength = 15.0f, float distance3D = 0.0f) {
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
        f32 padding = 1.0f;
        left -= padding;
        top -= padding;
        right += padding;
        bottom += padding;
        f32 boxWidth = right - left;
        f32 boxHeight = bottom - top;
        f32 boxDiagonal = sqrtf(boxWidth * boxWidth + boxHeight * boxHeight);
        f32 adaptiveCornerLength = cornerLength;
        if (distance3D > 0.0f) {
            f32 distanceScale = std::max(0.3f, std::min(1.0f, 200.0f / distance3D));
            adaptiveCornerLength = cornerLength * distanceScale;
        }

        f32 maxCornerLength = std::min(boxWidth, boxHeight) * 0.4f;
        adaptiveCornerLength = std::min(adaptiveCornerLength, maxCornerLength);

        adaptiveCornerLength = std::max(adaptiveCornerLength, 3.0f);
        ImU32 outlineColor = IM_COL32(0, 0, 0, 180);
        f32 outlineThickness = thickness + 2.0f;

        drawList->AddLine(ImVec2(left, top), ImVec2(left + adaptiveCornerLength, top), outlineColor, outlineThickness);
        drawList->AddLine(ImVec2(left, top), ImVec2(left, top + adaptiveCornerLength), outlineColor, outlineThickness);
        drawList->AddLine(ImVec2(right - adaptiveCornerLength, top), ImVec2(right, top), outlineColor, outlineThickness);
        drawList->AddLine(ImVec2(right, top), ImVec2(right, top + adaptiveCornerLength), outlineColor, outlineThickness);
        drawList->AddLine(ImVec2(left, bottom - adaptiveCornerLength), ImVec2(left, bottom), outlineColor, outlineThickness);
        drawList->AddLine(ImVec2(left, bottom), ImVec2(left + adaptiveCornerLength, bottom), outlineColor, outlineThickness);
        drawList->AddLine(ImVec2(right, bottom - adaptiveCornerLength), ImVec2(right, bottom), outlineColor, outlineThickness);
        drawList->AddLine(ImVec2(right - adaptiveCornerLength, bottom), ImVec2(right, bottom), outlineColor, outlineThickness);
        drawList->AddLine(ImVec2(left, top), ImVec2(left + adaptiveCornerLength, top), color, thickness);
        drawList->AddLine(ImVec2(left, top), ImVec2(left, top + adaptiveCornerLength), color, thickness);
        drawList->AddLine(ImVec2(right - adaptiveCornerLength, top), ImVec2(right, top), color, thickness);
        drawList->AddLine(ImVec2(right, top), ImVec2(right, top + adaptiveCornerLength), color, thickness);
        drawList->AddLine(ImVec2(left, bottom - adaptiveCornerLength), ImVec2(left, bottom), color, thickness);
        drawList->AddLine(ImVec2(left, bottom), ImVec2(left + adaptiveCornerLength, bottom), color, thickness);
        drawList->AddLine(ImVec2(right, bottom - adaptiveCornerLength), ImVec2(right, bottom), color, thickness);
        drawList->AddLine(ImVec2(right - adaptiveCornerLength, bottom), ImVec2(right, bottom), color, thickness);
    }
}
inline void drawAngleBoundingBox(ImDrawList* drawList, const std::array<Vector2, R15_BoneCount>& bonePositions,
    bool isR15, ImU32 color, float thickness = 3.0f, bool cornersOnly = false, float distance3D = 0.0f) {
    if (cornersOnly) {
        drawCornerBoundingBox(drawList, bonePositions, isR15, color, thickness, 15.0f, distance3D);
        return;
    }

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

        ImU32 outlineColor = IM_COL32(0, 0, 0, 180);
        float outlineThickness = thickness + 2.0f;

        drawList->AddRect(ImVec2(left, top), ImVec2(right, bottom), outlineColor, 0.0f, 0, outlineThickness);
        drawList->AddRect(ImVec2(left, top), ImVec2(right, bottom), color, 0.0f, 0, thickness);
    }
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

    std::vector<PlayerRenderData> allRenderData;
    std::mutex allRenderDataMutex;
    std::atomic<size_t> tasksCompleted = 0;
    size_t tasksSubmitted = 0;

    for (RBX::Player& player : g_PlayerList) {
        if (player.getName() != g_LocalPlayer.getName() &&
            !(player.getTeamName() == g_LocalPlayer.getTeamName() &&
                player.getTeamName() != "No Team" &&
                player.getTeamName() != "Players")) {
            tasksSubmitted++;

            RBX::Player playerCopy = player;

            WallcheckThreadPool::SubmitTask([&allRenderData, &allRenderDataMutex, &tasksCompleted, myPos3D, hasValidMyPos, playerCopy]() mutable {
                PlayerRenderData data = ProcessPlayerData(playerCopy, myPos3D, hasValidMyPos);
                if (data.isValid) {
                    std::lock_guard<std::mutex> lock(allRenderDataMutex);
                    allRenderData.push_back(std::move(data));
                }
                tasksCompleted++;
                });
        }
    }

    while (tasksCompleted < tasksSubmitted) {
        std::this_thread::yield();
    }

    constexpr float ESP_LINE_THICKNESS = 1.5f;
    constexpr float ESP_BOX_THICKNESS = 2.0f;
    constexpr float OUTLINE_THICKNESS_ADDITION = 2.0f;
    static auto startTime = std::chrono::steady_clock::now();
    auto currentTime = std::chrono::steady_clock::now();
    float timeElapsed = std::chrono::duration<float>(currentTime - startTime).count();

    for (const auto& data : allRenderData) {
        if (esp_show_tracer) {
            ImU32 tracerColor;
            ImU32 outlineColor = IM_COL32(0, 0, 0, 180);

            if (g_Rainbow) {
                tracerColor = getRainbowColor(timeElapsed, 1.0f);
                tracerColor = (tracerColor & 0x00FFFFFF) | (150 << 24);
            }
            else {
                tracerColor = IM_COL32(
                    (data.lineColor >> 24) & 0xFF,
                    (data.lineColor >> 16) & 0xFF,
                    (data.lineColor >> 8) & 0xFF,
                    150
                );
            }
            Vector2 tracerTarget = (data.boundingBoxBottomCenter.x != 0 || data.boundingBoxBottomCenter.y != 0)
                ? data.boundingBoxBottomCenter
                : data.headPos2D;
            drawList->AddLine(
                screenBottomCenter,
                ImVec2(tracerTarget.x, tracerTarget.y),
                outlineColor,
                ESP_LINE_THICKNESS + OUTLINE_THICKNESS_ADDITION
            );

            drawList->AddLine(
                screenBottomCenter,
                ImVec2(tracerTarget.x, tracerTarget.y),
                tracerColor,
                ESP_LINE_THICKNESS
            );
        }

        if (esp_chams) {
            renderPlayerChamsWithVisibility(drawList, const_cast<RBX::Player&>(data.player), data.distance3D, myPos3D, hasValidMyPos);
        }

        if (esp_show_names) {
            ImVec2 textSize = ImGui::CalcTextSize(data.playerName.c_str());
            float nameY = data.headPos2D.y - textSize.y - 5;
            drawList->AddText(ImVec2(data.headPos2D.x - textSize.x / 2, nameY),
                IM_COL32(255, 255, 255, 255), data.playerName.c_str());
        }

        if (esp_show_distance && hasValidMyPos) {
            char distText[32];
            snprintf(distText, sizeof(distText), "%.1fm", data.distance3D / 10.0f);
            ImVec2 distTextSize = ImGui::CalcTextSize(distText);
            float distY = data.headPos2D.y - 35;
            drawList->AddText(ImVec2(data.headPos2D.x - distTextSize.x / 2, distY),
                IM_COL32(0, 0, 255, 255), distText);
        }

        if (esp_show_bones) {
            const BoneLine* lines = data.isR15 ? linesR15 : linesR6;
            const size_t lineCount = data.isR15 ? lineCountR15 : lineCountR6;

            ImU32 skeletonColor;
            ImU32 boneOutlineColor = IM_COL32(0, 0, 0, 180);

            if (g_Rainbow) {
                skeletonColor = getRainbowColor(timeElapsed, 1.0f);
            }
            else {
                skeletonColor = data.lineColor;
            }

            for (size_t i = 0; i < lineCount; ++i) {
                const Vector2& a = data.bonePos2D[lines[i].a];
                const Vector2& b = data.bonePos2D[lines[i].b];
                if ((a.x != 0 || a.y != 0) && (b.x != 0 || b.y != 0)) {
                    drawList->AddLine(
                        ToImVec2(a),
                        ToImVec2(b),
                        boneOutlineColor,
                        ESP_LINE_THICKNESS + OUTLINE_THICKNESS_ADDITION
                    );
                    drawList->AddLine(
                        ToImVec2(a),
                        ToImVec2(b),
                        skeletonColor,
                        ESP_LINE_THICKNESS
                    );
                }
            }
        }

        if (esp_show_box) {
            ImU32 boxColor;
            if (g_Rainbow) {
                boxColor = getRainbowColor(timeElapsed, 1.0f);
            }
            else {
                boxColor = data.lineColor;
            }



            if (useCornerBox) {
                drawCornerBoundingBox(drawList, data.bonePos2D, data.isR15, boxColor, ESP_BOX_THICKNESS, 15.0f, data.distance3D);
            }
            else {
                drawAngleBoundingBox(drawList, data.bonePos2D, data.isR15, boxColor, ESP_BOX_THICKNESS, false, data.distance3D);
            }
        }
    }
    drawList->AddCircleFilled(screenBottomCenter, 5.0f, IM_COL32(255, 255, 255, 150));
}
inline void espLoop() {

    static bool threadPoolInitialized = false;
    if (!threadPoolInitialized) {
        InitESPThreadPool();
        threadPoolInitialized = true;
    }
    drawCursor();
    espLoopThreaded();
}