#pragma once
#include <Cheats/Players.h>
#include <SDK/Classes/World/Primtive.h>
#include <SDK/Classes/Math/CFrame.h>
#include <SDK/Offset.h>
#include <Render/Render.h>
#include <Windows.h>
#include <cmath>
#include <algorithm>

inline bool isBodyPart(const std::string& partName) {

    if (partName == "Head" || partName == "UpperTorso" || partName == "LowerTorso" ||
        partName == "LeftUpperArm" || partName == "LeftLowerArm" || partName == "LeftHand" ||
        partName == "RightUpperArm" || partName == "RightLowerArm" || partName == "RightHand" ||
        partName == "LeftUpperLeg" || partName == "LeftLowerLeg" || partName == "LeftFoot" ||
        partName == "RightUpperLeg" || partName == "RightLowerLeg" || partName == "RightFoot") {
        return true;
    }


    if (partName == "Torso" || partName == "Left Arm" || partName == "Right Arm" ||
        partName == "Left Leg" || partName == "Right Leg") {
        return true;
    }

    return false;
}
float FOV_RADIUS = fov_size;
constexpr float MAX_TARGET_DIST = 650.0f;


inline void triggerBot() {
    f32 TRIGGER_FOV = 10.0f;
    // -- hold �ɼ�
    //static bool isHolding = false;
    //static DWORD lastClickTime = 0;  
    //constexpr DWORD CLICK_COOLDOWN = 50;  
    static i32 tick = 0;
    POINT mousePos;
    GetCursorPos(&mousePos);

    ImVec2 viewportPos = ImGui::GetMainViewport()->Pos;
    ImVec2 mouseImGui = ImVec2(mousePos.x - viewportPos.x, mousePos.y - viewportPos.y);

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

    //if (!hasValidMyPos) {
    //    if (isHolding) {
    //        INPUT input = { 0 };
    //        input.type = INPUT_MOUSE;
    //        input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
    //        SendInput(1, &input, sizeof(INPUT));
    //        isHolding = false;

    //    }
    //    return;
    //}

    RBX::Player* bestTarget = nullptr;
    float bestDistance2D = FLT_MAX;
    CVector bestTargetPos3D;
    bool targetFound = false;

    for (RBX::Player& player : g_PlayerList) {
        RBX::ModelInstance character = player.getModelInstance();

        if (!character.getAddress() || player.getName() == g_LocalPlayer.getName() ||
            (player.getTeamName() == g_LocalPlayer.getTeamName() &&
                player.getTeamName() != "No Team" && player.getTeamName() != "Play")) {
            continue;
        }

        std::vector<RBX::Instance> children = character.getChildren();

        auto headPart = character.findFirstChild("Head");
        if (headPart.getAddress()) {
            try {
                RBX::Primitive primitive = headPart.getPrimitive();
                if (primitive.getAddress()) {
                    CVector partPos3D = CVector(primitive.getPartPosition());
                    Vector2 partPos2D = visualEngine->worldToScreen(partPos3D.toVector3());

                    if (!(partPos2D.x == 0 && partPos2D.y == 0)) {

                        float dx = partPos2D.x - mouseImGui.x;
                        float dy = partPos2D.y - mouseImGui.y;
                        float distance2D = sqrtf(dx * dx + dy * dy);

                        if (distance2D <= TRIGGER_FOV && distance2D < bestDistance2D) {
                            if (TRIGGER_FOV < 30.0f) TRIGGER_FOV = TRIGGER_FOV + tick;
                            //printf("TRIGGER_FOV: %f\n", TRIGGER_FOV);
                            bestDistance2D = distance2D;
                            bestTarget = &player;
                            bestTargetPos3D = partPos3D;
                            targetFound = true;
                        }
                    }
                }
            }
            catch (...) {}
        }


        if (!targetFound) {
            for (RBX::Instance& child : children) {
                if (!child.getAddress()) continue;

                try {
                    std::string partName = child.getName();


                    if (!isBodyPart(partName) || partName == "Head") { // Head�� �̹� Ȯ������
                        continue;
                    }

                    RBX::Primitive primitive = child.getPrimitive();
                    if (!primitive.getAddress()) continue;

                    CVector partPos3D = CVector(primitive.getPartPosition());
                    Vector2 partPos2D = visualEngine->worldToScreen(partPos3D.toVector3());

                    if (partPos2D.x == 0 && partPos2D.y == 0) continue;

                    float dx = partPos2D.x - mouseImGui.x;
                    float dy = partPos2D.y - mouseImGui.y;
                    float centerDistance = sqrtf(dx * dx + dy * dy);

                    if (centerDistance <= TRIGGER_FOV) {
                        if (centerDistance < bestDistance2D) {
                            bestDistance2D = centerDistance;
                            bestTarget = &player;
                            bestTargetPos3D = partPos3D;
                            targetFound = true;
                        }
                        continue; // ���� ��Ʈ��
                    }

                    Vector3 partSize = primitive.getPartSize();
                    CFrame partCFrame = primitive.getPartCFrame();
                    CVector halfSize = CVector(partSize.x * 0.5f, partSize.y * 0.5f, partSize.z * 0.5f);

                    CVector keyVertices[4] = {
                        {-halfSize.x, -halfSize.y, -halfSize.z}, // ���� �Ʒ� ����
                        {+halfSize.x, +halfSize.y, -halfSize.z}, // ���� �� ������  
                        {-halfSize.x, +halfSize.y, +halfSize.z}, // ���� �� ����
                        {+halfSize.x, -halfSize.y, +halfSize.z}  // ���� �Ʒ� ������
                    };

                    bool partInFOV = false;
                    float minDistance2D = FLT_MAX;

                    for (int i = 0; i < 4; i++) {
                        CVector worldVertex = partCFrame.pointToWorldSpace(keyVertices[i]);
                        Vector2 vertex2D = visualEngine->worldToScreen(worldVertex.toVector3());

                        if (vertex2D.x == 0 && vertex2D.y == 0) continue;

                        float vdx = vertex2D.x - mouseImGui.x;
                        float vdy = vertex2D.y - mouseImGui.y;
                        float distance2D = sqrtf(vdx * vdx + vdy * vdy);

                        if (distance2D <= TRIGGER_FOV) {
                            partInFOV = true;
                            if (distance2D < minDistance2D) {
                                minDistance2D = distance2D;
                            }
                        }
                    }

                    if (partInFOV && minDistance2D < bestDistance2D) {
                        bestDistance2D = minDistance2D;
                        bestTarget = &player;
                        bestTargetPos3D = partPos3D;
                        targetFound = true;
                    }
                }
                catch (...) {
                    continue;
                }
            }
        }

        if (targetFound && bestTarget == &player) {
            break;
        }
    }

    DWORD currentTime = GetTickCount();

    if (bestTarget && targetFound) {
        CVector direction = bestTargetPos3D - myPos3D;
        float distance3D = direction.magnitude();

        if (distance3D > 0) {

            std::vector<Roblox::PartInfo> obstacles = getRelevantObstaclesWithSize(
                myPos3D.toVector3(), bestTargetPos3D.toVector3()
            );

            Roblox::RaycastResult result = Roblox::Ray::cast_ray_with_rotation(
                myPos3D, direction, distance3D, obstacles
            );

            if (!result.hit) {
				printf("TriggerBot: Shooting at %s\n", bestTarget->getName().c_str());
                INPUT input = { 0 };
                input.type = INPUT_MOUSE;
                input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
                SendInput(1, &input, sizeof(INPUT));
                sleep_ms(10);
                input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
                SendInput(1, &input, sizeof(INPUT));
            }
            //if (!isHolding && (currentTime - lastClickTime > CLICK_COOLDOWN)) {


            //    tick += 5;
            //    INPUT input = { 0 };
            //    input.type = INPUT_MOUSE;
            //    input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
            //    SendInput(1, &input, sizeof(INPUT));
            //    isHolding = true;
            //    lastClickTime = currentTime;

            //}

        //else {
        //    if (isHolding) {
        //        INPUT input = { 0 };
        //        input.type = INPUT_MOUSE;
        //        input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
        //        SendInput(1, &input, sizeof(INPUT));
        //        isHolding = false;
        //        tick = 0;
        //        

        //    }
        //}
        }
    }
    //else {

    //    if (isHolding) {
    //        INPUT input = { 0 };
    //        input.type = INPUT_MOUSE;
    //        input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
    //        SendInput(1, &input, sizeof(INPUT));
    //        isHolding = false;


    //    }
    //}
}
inline void triggerBotThread() {
    while (true) {
        if (triggerbot) {
            triggerBot();
        }
        sleep_ms(1);
    }
}   
inline void pf_Aimbot() {
    POINT mousePos;
    GetCursorPos(&mousePos);

    ImVec2 viewportPos = ImGui::GetMainViewport()->Pos;
    ImVec2 mouseImGui = ImVec2(mousePos.x - viewportPos.x, mousePos.y - viewportPos.y);

    ImDrawList* drawList = ImGui::GetBackgroundDrawList();
    drawList->AddCircle(mouseImGui, fov_size, IM_COL32(255, 0, 0, 180), 64, 2.0f);
    if (render->isVisible) return;
    if (!(GetAsyncKeyState(VK_LBUTTON) & 0x8000))
        return;

    float bestDist = FOV_RADIUS;
    int bestDeltaX = 0, bestDeltaY = 0;
    bool found = false;

    for (RBX::Instance& part : pf_PlayerPartList) {
        RBX::Primitive prim = part.getPrimitive();
        Vector3 PartPos3D = prim.getPartPosition();
        Vector2 PartPos2D = visualEngine->worldToScreen(PartPos3D);
        float deltaX = PartPos2D.x - mouseImGui.x;
        float deltaY = PartPos2D.y - mouseImGui.y;
        float dist = sqrtf(deltaX * deltaX + deltaY * deltaY);
        if (dist > fov_size) continue;
        if (!found || dist < bestDist) {
            bestDist = dist;
            bestDeltaX = static_cast<int>(deltaX);
            bestDeltaY = static_cast<int>(deltaY);
            found = true;
        }
    }
    if (found) {
        int smooth = 3;
        float moveX = bestDeltaX / smooth;
        float moveY = bestDeltaY / smooth;
        if (abs(moveX) < 1 && abs(moveY) < 1) {
            if (moveX != 0) moveX = (moveX > 0) ? 1 : -1;
            if (moveY != 0) moveY = (moveY > 0) ? 1 : -1;
        }
        INPUT input = { 0 };
        input.type = INPUT_MOUSE;
        input.mi.dx = static_cast<int>(moveX);
        input.mi.dy = static_cast<int>(moveY);
        input.mi.dwFlags = MOUSEEVENTF_MOVE;
        SendInput(1, &input, sizeof(INPUT));
    }
}
inline u64 getKey() {
    // 마우스 버튼
    if (aimbot_keybind == "LMB") return VK_LBUTTON;
    if (aimbot_keybind == "RMB") return VK_RBUTTON;

    // 문자키 (A-Z)
    if (aimbot_keybind == "A") return 'A';
    if (aimbot_keybind == "B") return 'B';
    if (aimbot_keybind == "C") return 'C';
    if (aimbot_keybind == "D") return 'D';
    if (aimbot_keybind == "E") return 'E';
    if (aimbot_keybind == "F") return 'F';
    if (aimbot_keybind == "G") return 'G';
    if (aimbot_keybind == "H") return 'H';
    if (aimbot_keybind == "I") return 'I';
    if (aimbot_keybind == "J") return 'J';
    if (aimbot_keybind == "K") return 'K';
    if (aimbot_keybind == "L") return 'L';
    if (aimbot_keybind == "M") return 'M';
    if (aimbot_keybind == "N") return 'N';
    if (aimbot_keybind == "O") return 'O';
    if (aimbot_keybind == "P") return 'P';
    if (aimbot_keybind == "Q") return 'Q';
    if (aimbot_keybind == "R") return 'R';
    if (aimbot_keybind == "S") return 'S';
    if (aimbot_keybind == "T") return 'T';
    if (aimbot_keybind == "U") return 'U';
    if (aimbot_keybind == "V") return 'V';
    if (aimbot_keybind == "W") return 'W';
    if (aimbot_keybind == "X") return 'X';
    if (aimbot_keybind == "Y") return 'Y';
    if (aimbot_keybind == "Z") return 'Z';

    // 숫자키 (0-9)
    if (aimbot_keybind == "0") return '0';
    if (aimbot_keybind == "1") return '1';
    if (aimbot_keybind == "2") return '2';
    if (aimbot_keybind == "3") return '3';
    if (aimbot_keybind == "4") return '4';
    if (aimbot_keybind == "5") return '5';
    if (aimbot_keybind == "6") return '6';
    if (aimbot_keybind == "7") return '7';
    if (aimbot_keybind == "8") return '8';
    if (aimbot_keybind == "9") return '9';

    // 기능키 (F1-F12)
    if (aimbot_keybind == "F1") return VK_F1;
    if (aimbot_keybind == "F2") return VK_F2;
    if (aimbot_keybind == "F3") return VK_F3;
    if (aimbot_keybind == "F4") return VK_F4;
    if (aimbot_keybind == "F5") return VK_F5;
    if (aimbot_keybind == "F6") return VK_F6;
    if (aimbot_keybind == "F7") return VK_F7;
    if (aimbot_keybind == "F8") return VK_F8;
    if (aimbot_keybind == "F9") return VK_F9;
    if (aimbot_keybind == "F10") return VK_F10;
    if (aimbot_keybind == "F11") return VK_F11;
    if (aimbot_keybind == "F12") return VK_F12;

    // 특수키
    if (aimbot_keybind == "Space") return VK_SPACE;
    if (aimbot_keybind == "Enter") return VK_RETURN;
    if (aimbot_keybind == "Return") return VK_RETURN;
    if (aimbot_keybind == "Tab") return VK_TAB;
    if (aimbot_keybind == "Backspace") return VK_BACK;
    if (aimbot_keybind == "Delete") return VK_DELETE;
    if (aimbot_keybind == "Insert") return VK_INSERT;
    if (aimbot_keybind == "Home") return VK_HOME;
    if (aimbot_keybind == "End") return VK_END;
    if (aimbot_keybind == "PageUp") return VK_PRIOR;
    if (aimbot_keybind == "PageDown") return VK_NEXT;

    // 방향키
    if (aimbot_keybind == "↑" || aimbot_keybind == "Up") return VK_UP;
    if (aimbot_keybind == "↓" || aimbot_keybind == "Down") return VK_DOWN;
    if (aimbot_keybind == "←" || aimbot_keybind == "Left") return VK_LEFT;
    if (aimbot_keybind == "→" || aimbot_keybind == "Right") return VK_RIGHT;

     if(aimbot_keybind == "Ctrl") return VK_CONTROL;
     if(aimbot_keybind == "Shift") return VK_SHIFT;
     if(aimbot_keybind == "Alt") return VK_MENU;

    return VK_LBUTTON;
}
struct SmoothingRange {
    float distance;
    float smoothValue;
};

inline float getSmoothValue(float dist) {
    static const SmoothingRange smoothingRanges[] = {
        { 10.0f, 1.05f },
        { 20.0f, 1.4f },
        { 40.0f, 1.8f },
        { 60.0f, 2.2f },
        { FLT_MAX, 2.4f }  // default for distances > 60
    };

    constexpr size_t rangeCount = sizeof(smoothingRanges) / sizeof(SmoothingRange);

    for (size_t i = 0; i < rangeCount; ++i) {
        if (dist < smoothingRanges[i].distance) {
            return smoothingRanges[i].smoothValue;
        }
    }

    return 2.4f; // fallback
}
inline void moveMouse(float deltaX, float deltaY, float smooth) {
    float moveX = deltaX / smooth;
    float moveY = deltaY / smooth;

    if (abs(moveX) < 1 && abs(moveY) < 1) {
        if (moveX != 0) moveX = (moveX > 0) ? 1 : -1;
        if (moveY != 0) moveY = (moveY > 0) ? 1 : -1;
    }

    INPUT input = { 0 };
    input.type = INPUT_MOUSE;
    input.mi.dx = static_cast<i32>(moveX);
    input.mi.dy = static_cast<i32>(moveY);
    input.mi.dwFlags = MOUSEEVENTF_MOVE;
    SendInput(1, &input, sizeof(INPUT));
}

inline void moveMouseEased(float deltaX, float deltaY, float dist) {
    float smooth = getSmoothValue(dist);
    float t = 1.0f / smooth;
    float easedX = easeInOutCubic(0.0f, deltaX, t);
    float easedY = easeInOutCubic(0.0f, deltaY, t);

    INPUT input = { 0 };
    input.type = INPUT_MOUSE;
    input.mi.dx = static_cast<i32>(easedX);
    input.mi.dy = static_cast<i32>(easedY);
    input.mi.dwFlags = MOUSEEVENTF_MOVE;
    SendInput(1, &input, sizeof(INPUT));
}

inline void aimbotLoop() {
    static DWORD lastTargetDeathTime = 0;
    static bool waitAfterDeath = false;

    if (toggePF) {
        pf_Aimbot();
    }
    else {
        POINT mousePos;
        GetCursorPos(&mousePos);

        ImVec2 viewportPos = ImGui::GetMainViewport()->Pos;
        ImVec2 mouseImGui = ImVec2(mousePos.x - viewportPos.x, mousePos.y - viewportPos.y);

        ImDrawList* drawList = ImGui::GetBackgroundDrawList();
        drawList->AddCircle(mouseImGui, fov_size, IM_COL32(255, 0, 0, 180), 64, 2.0f);
        if (render->isVisible) return;
        if (!(GetAsyncKeyState(getKey()) & 0x8000))return; // key

        CVector myPos = { 0, 0, 0 };
        if (g_LocalPlayer.getAddress()) {
            RBX::Player localPlayerInstance(g_LocalPlayer.getAddress());
            RBX::ModelInstance myCharacter = localPlayerInstance.getModelInstance();
            if (myCharacter.getAddress()) {
                auto myHead = myCharacter.findFirstChild("Head");
                if (myHead.getAddress())
                    myPos = CVector(myHead.getPrimitive().getPartPosition());
            }
        }

        static std::string lastTargetName = "";
        Vector2 bestTarget2D{ 0, 0 };
        float minDist2D = FLT_MAX;
        float minDist3D = FLT_MAX;
        RBX::Player* currentTarget = nullptr;

        for (RBX::Player& player : g_PlayerList) {
            RBX::ModelInstance character = player.getModelInstance();

            if (!character.getAddress() || player.getName() == g_LocalPlayer.getName() ||
                (player.getTeamName() == g_LocalPlayer.getTeamName() && player.getTeamName() != "No Team" && player.getTeamName() != "Players")) {
                continue;
            }

            auto headPart = character.findFirstChild("Head");
            if (!headPart.getAddress()) continue;
            CVector headPos3D = CVector(headPart.getPrimitive().getPartPosition());
            Vector2 headPos2D = visualEngine->worldToScreen(headPos3D.toVector3());
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

            if (dist2D < minDist2D - 56.0f) {
                minDist2D = dist2D;
                minDist3D = dist3D;
                bestTarget2D = headPos2D;
                currentTarget = &player;
            }
            else if (fabs(dist2D - minDist2D) <= 70.0f) {
                if (dist3D < minDist3D) {
                    minDist2D = dist2D;
                    minDist3D = dist3D;
                    bestTarget2D = headPos2D;
                    currentTarget = &player;
                }
            }
        }

        if (currentTarget) {
            std::string currentTargetName = currentTarget->getName();
            if (!lastTargetName.empty() && lastTargetName != currentTargetName) {
                lastTargetDeathTime = GetTickCount();
                waitAfterDeath = true;
            }
            lastTargetName = currentTargetName;
        }
        else {
            if (!lastTargetName.empty()) {
                lastTargetDeathTime = GetTickCount();
                waitAfterDeath = true;
            }
            lastTargetName = "";
        }

        if (waitAfterDeath) {
            if (GetTickCount() - lastTargetDeathTime < 50) {
                return;
            }
            waitAfterDeath = false;
        }

        if (bestTarget2D.x != 0 && bestTarget2D.y != 0) {
            float deltaX = bestTarget2D.x - mouseImGui.x;
            float deltaY = bestTarget2D.y - mouseImGui.y;
            float dist = sqrtf(deltaX * deltaX + deltaY * deltaY);


            switch (aimbot_type) {
                case 0: 
                    moveMouseEased(deltaX, deltaY, dist);
                    break;
                case 1: 
                    moveMouse(deltaX, deltaY, 3.0f);
                    break;
                case 2: 
                    moveMouse(deltaX, deltaY, g_Smooth);
                    break;
                default:
                    break;
            }
        }
    }
}
