# Memory Control Source - [Link](https://github.com/obamaruntz/RickOwens00)

# 기능

1. Aimbot
2. Esp

# 애로 사항

1. 게임 접속후 켜야함
2. 기본 캐릭터만 지원 ( + 펜텀포스 대충 지원)

# 작동원리
## 1. Anti Cheat Bypass

- 이 코드가 Roblox의 안티치트를 우회할수있는 이유는 간단하다.
- 로블록스의 안티치트 프로그램은 Usermode 기반이기 때문이다.
- Usermode? usermode는 쉽게 설명하면 운영체제(os)에서 실행되는 일반적인 프로그램의 영역이다.
- 유저 모드는 권한이 제한되어 있기 때문에 메모리,하드웨어를 직접 건드릴수 없고 시스템 콜을 통한 커널 요청을 해야한다.
- 그럼 유저모드를 사용하는 이유가 뭘까? 여러 이유가 있겠지만 대표적으로 안정성이 뛰어나다.
- 예를 들어 유저 모드에서는 메모장이 에러를 이르켜서 크래쉬가 나도 각자 독립된 메모리에서 실행되기 때문에 다른 프로그램에 영향을 미치지 않는다.
- 하지만 커널 모드에서 크래쉬가 난다면 시스템 전체에 문제가 생긴다. 이밖에도 호환성 및 유지보수등 다양한 이유로 대부분의 프로그램이 유저 모드에서 작동한다.
- 이제 앞에서 설명한것 처럼 로블록스의 안티 치트 프로그램도 유저 모드 기반이기 때문에 커널 기반의 드라이버를 이용하면 더 높은 권한으로 안티 치트를 우회해서 메모리에 접근할수있게 된다.
- 따라서 가상드라이버를 만들어 DeviceIoControl() 같은 방식으로 드라이버와 소통을 하면 안티치트를 우회할수있다.
> 아래 코드는 IoCreateDevice()로 커널 드라이버용 오브젝트를 생성하고 IOCTL 요청을 처리하는 코드이다.
```cpp
#include <ntifs.h>
#include "definitions.h"

NTSTATUS driver_initialize(PDRIVER_OBJECT driver_object, PUNICODE_STRING registry_path) {
    UNREFERENCED_PARAMETER(registry_path);

    NTSTATUS status = STATUS_UNSUCCESSFUL;

    UNICODE_STRING device_name = {};
    RtlInitUnicodeString(&device_name, DEVICE_NAME);

    PDEVICE_OBJECT device_object = nullptr;

    status = IoCreateDevice(driver_object, NULL, &device_name, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &device_object);
    if (!NT_SUCCESS(status)) return status;

    UNICODE_STRING symbolic_link = {};
    RtlInitUnicodeString(&symbolic_link, SYMBOLIC_LINK);

    status = IoCreateSymbolicLink(&symbolic_link, &device_name);
    if (!NT_SUCCESS(status)) return status;

    SetFlag(device_object->Flags, DO_BUFFERED_IO);

    driver_object->MajorFunction[IRP_MJ_CREATE] = major_functions::dispatcher;
    driver_object->MajorFunction[IRP_MJ_CLOSE] = major_functions::dispatcher;
    // IOCTL 요청을 처리하는 부분
    driver_object->MajorFunction[IRP_MJ_DEVICE_CONTROL] = major_functions::io_controller;
    // 언로드 x
    driver_object->DriverUnload = nullptr;

    ClearFlag(device_object->Flags, DO_DIRECT_IO);
    ClearFlag(device_object->Flags, DO_DEVICE_INITIALIZING);

    return STATUS_SUCCESS;
}

NTSTATUS DriverEntry() {
    UNICODE_STRING driver_name = {};
    RtlInitUnicodeString(&driver_name, DRIVER_NAME);
    // 드라이버 생성
    return IoCreateDriver(&driver_name, &driver_initialize);
}
```
- 이제 이렇게 만들어진 가상 드라이버는 Kdmapper라는 커널 드라이버 mapper로 윈도우 커널에 강제로 로드한다.
> 여기서 강제로 로드하는 이유는 윈도우 정책이 바뀌면서 드라이버가 반드시 MS 인증을 받아야 로드 되도록 바뀌었기 때문이다.

- 로드 후 아래 코드처럼 핸들을 열면 이 핸들을 통해 IOCTL 호출을 할수있게되어 최종적으로 안티치트를 우회해 메모리 값을 가져올수 있는 사전 준비를 끝내게 된다.
```cpp
communication::communication() {
    // 여기서 RickOwens00은 드라이버의 심볼릭 링크 이름이다.
    driver_handle = CreateFileA("\\\\.\\RickOwens00", GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
}
```

## 2. Roblox 메모리 접근
- 이제 위에서 만들었던 가상 드라이버 핸들을 통해 CreateToolhelp32Snapshot 함수로 현재 실행중인 프로세스를 가져오고
- 가져온 프로세스의 목록을 Process32First(snapshot, &process_entry) 및 Process32Next(snapshot, &process_entry)로 순회 하며 원하는 이름과 일치하는 프로세스를 찾고
- 이를 통해 PID를 찾는다.
- 이렇게 얻은 PID로 아래 있는 코드로 base address(타겟 프로세스의 메인 모듈)을 찾는다.
> 쉽게 생각하면 프로세스가 로드된 EXE 시작 주소를 찾아서 이를 통해 메모리 오프셋에 접근을 하는것.
```cpp
u64 communication::find_image() {
    u64 image_address = 0;
    _BA arguments = {}; // _BA = Base Address 구조체
    arguments.security_code = SECURITY_CODE; //드라이버 호출 인증 0x94c9e4bc3
    arguments.process_id = process_id; // 어느 프로세스에서 가져올지
    arguments.address = &image_address; // 결과값 포인터
    //_BA arguments를 전달을 통해서 Base Address를 image_address에 기록
    // 여기서 driver_handle은 아까 연 드라이버 핸들
    DeviceIoControl(driver_handle, BA_CODE, &arguments, sizeof(arguments), nullptr, NULL, NULL, NULL);

    communication::image_address = image_address;

    return image_address;
}
```
- 이제 이렇게 베이스 주소를 통해 [오프셋](https://offsets.ntgetwritewatch.workers.dev/offsets.hpp)에 접근 할수있게 되었다.
> ex)
```cpp
	visualEngine = std::make_unique<RBX::VisualEngine>(comms->read<u64>(comms->image_address + Offsets::VisualEngine));
```
- 이제 로블록스의 여러 Instance와 Player등을 다루기위해 DataModel을 불러와야한다.
> 최상위 컨테이너의 역할
```cpp
u64 fakeDataModel = comms->read<u64>(comms->image_address + Offsets::FakeDataModel);
dataModel = std::make_unique<RBX::DataModel>(comms->read<u64>(fakeDataModel + Offsets::RealDataModel));
```
- ex) 이런식으로 데이터 모델의 주소에서 Players라는 자식 인스턴스의 주소를 찾을수 있다
```cpp
RBX::Instance playerService = dataModel->findFirstChild("Players");
```
- 또한 Esp등 화면에 그릴때 필요한 좌표 변환을 위해 앞서 예시로 보여줬던 visualEngine을 불러와야한다.
```cpp
	visualEngine = std::make_unique<RBX::VisualEngine>(comms->read<u64>(comms->image_address + Offsets::VisualEngine));
```
- 이제 Aimbot과Esp를 만들 기초적인 로직은 끝났다.

## 3. Esp 구현
- 먼저 esp를 구현하려면 화면에 그래픽을 그려야 하기때문에 Imgui라는 라이브러리를 사용할것이다.
> ImGui에 대한 설명은 생략하도록 하겠습니다.
- 그 다음으론 우선 플레이어들의 정보를 가져와야 한다.
- 플레이어의 정보는 앞서 설명했던 dataModel을 이용해서 찾으면된다.
- 찾는 방법은 dataModel의 주소로 Childern offset을 활용해
```cpp
std::vector<RBX::Instance> RBX::Instance::getChildern() {
	std::vector<RBX::Instance> Childern;
	u64 start = comms->read<u64>(address + Offsets::Childern);
	u64 end = comms->read<u64>(start + 0x8);

	for (u64 child = comms->read<u64>(start); child != end; child += 0x10) {
		children.push_back(RBX::Instance(comms->read<u64>(child)));

	}
	return Childern;
}
```
- 이런식으로 해당 주소에 Chidern 오프셋 + 16바이트(0x10) 만큼 떨어지있는 위치의 메모리 주소(자식 인스턴스)를 가져온다.
- 여기서 약간 의문이 생길수도 있다.아니 0x8이 끝주소 이면 0x10씩증가하면 한번에 끝주소를 넘기는거아니야?
- 이걸 설명하자면 0x8은 배열의 끝주소(end 포인터가 저장된 주소)가 저장된 주소이고 0x10은 배열(리스트)가 저장된 주소 이기때문에 16바이트씩 껒지는것이다.
- 예를 들자면 end + 0x8주소에는 0x1020이라는 배열(리스트)끝 주소가 담겨져있다. child가 + 0x10씩 증가하면서 0x1000->0x1010->0x1020이 되면 end 값이랑 같아져서 브레이크가 걸리는 것이다.
- 이제 이렇게 dataModel을 이렇게 childern으로 찾으면 여러 데이터 들이 cildren 벡터에 담겨있을 것이다.
- 이중 우리가 찾는 데이터는 Players라는 인스턴스 이기때문에 이름을 필터링 해야한다. 따라서 아까 추출한 dataModel의 모든데이터의 이름을 가져와 Players라는 이름만 필터링 하면된다.
- 따라서 각 instance의 주소에 Name이라는 Offset 주소 만큼 떨어진 값을 string 형태로 읽어와서 Players라는 이름과 같으면 그 값을 Return해서 알수있게되는 것이다.
> 이름을 얻는 함수
```cpp
str RBX::Instance::getName() {
	u64 name = comms->read<u64>(address + Offsets::Name);
	
	if (name) {
		return comms->readstr(name);
	}

	return "Unknown";
}
```
> 위에서 설명했듯 받은 instance의 자식들을 전부 가져오고 그 자식들의 이름중 받은 파라미터의 값을 탐색 하는 함수
```cpp
RBX::Instance RBX::Instance::findFirstChild(const str& name) {
	std::vector<RBX::Instance> children = getChildern();

	for (RBX::Instance& child : children) {
		if (child.getName() == name) {
			return child;
		}
	}
	return RBX::Instance();
}
```
- 이제 이렇게 해서 만든 함수를 통해 플레이어 리스트와 로컬 플레이어를 업데이트 하는 스레드를 먼저 작동시킨다.
```cpp
void updatePlayers() {
	RBX::Instance playerService = dataModel->findFirstChild("Players");

	
	while (true) {
		g_LocalPlayer = comms->read<u64>(playerService.getAddress() + Offsets::LocalPlayer);

		for (RBX::Instance& player : playerService.getChildren()) {
			if (!player.getAddress()) {
				continue;
			}
			g_PlayerList.push_back(RBX::Player(player.getAddress()));

		}
		sleep_ms(100);
		g_PlayerList.clear();
	}
}
```
- 이제 플레이어들의 정보를 수집했으니 이 정보를 토대로 플레이어의 파트들의 중심점 위치를 가져온다.
- 가져오는 방법은 간단하다. 파트의 이름을 기반으로 findFirstChild함수로 주소를 가져오고 가져온 주소를 primitive offset만큼 더해서 물리 정보를 저장하는 메모리 값을 얻는다.
- 이렇게 가져온 3d 메모리 값을 PartPosition offset만큼 떨어진 곳으로 이동해 파트의 중심 좌표를 얻을수있다.
- 이러한 중심 좌표를 기반으로 몸 전체 파트의 중심 좌표를 구하고 구한 좌표들을 선으로 이으면 스켈레톤 esp가 완성된다.
- 하지만 3d 게임엔진 오브젝트를 2차원 화면에 그릴려면 2d로 변환을 해야하기 때문에 앞에서 말한 visualEngine으로 카메라 변환 행렬을 읽어오고
- 이렇게 가져온 행렬과 world 파라미터(3d 좌표)를 곱해서 4D로 변환한 다음 원근 분활로 NDC(nomarlizeDivceCoordinates)을 실제 픽셀 좌표로 변환 하면
- 2d 좌표를 얻을수 있다.
> 이때 -1 를 곱하는이유는 y축은 위아래가 뒤집혀있기 때문이다.
```cpp
Vector2 RBX::VisualEngine::worldToScreen(const Vector3& world) {
	Vector4 quaternion;

	Vector2 dimensions = getDimensions();
	Matrix4 viewMatrix = getViewMatrix();

	
	quaternion.x = (world.x * viewMatrix.data[0]) + (world.y * viewMatrix.data[1]) + (world.z * viewMatrix.data[2])+ viewMatrix.data[3];
	quaternion.y = (world.x * viewMatrix.data[4]) + (world.y * viewMatrix.data[5]) + (world.z * viewMatrix.data[6]) + viewMatrix.data[7];
	quaternion.z = (world.x * viewMatrix.data[8]) + (world.y * viewMatrix.data[9]) + (world.z * viewMatrix.data[10]) + viewMatrix.data[11];
	quaternion.w = (world.x * viewMatrix.data[12]) + (world.y * viewMatrix.data[13]) + (world.z * viewMatrix.data[14]) + viewMatrix.data[15];

	Vector2 screen;

	if (quaternion.w < 0.1f) {
		return Vector2();
	}
	Vector3 normalizedDeviceCoordinates;
	normalizedDeviceCoordinates.x = quaternion.x / quaternion.w;
	normalizedDeviceCoordinates.y = quaternion.y / quaternion.w;
	normalizedDeviceCoordinates.z = quaternion.z / quaternion.w;

	screen.x = (dimensions.x / 2.0f * normalizedDeviceCoordinates.x) + (dimensions.x / 2.0f);
	screen.y = -(dimensions.y / 2.0f * normalizedDeviceCoordinates.y) + (dimensions.y / 2.0f);

	return screen;
}
```
- 이 방식을 통해 모든 파트의 2d 좌표를 얻고 얻은 좌표를 토대로 선을 이어주면 된다.
> 몇가지 중요하지 않은 부분은 생략했다 예) 플레이어 자기 자신을 무시하는 과정, 거리에 따른 색상 변화 원리, 최적화 ,Box Esp
```cpp
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


// 각도 기반 박스 ESP를 그리는 함수
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
```
- 이런식으로 하면 스캘레톤 Esp를 구현할수있다.

## 4. Aimbot 구현

- Aimbot을 구현하는건 생각보다 간단하다. 왜나햐면 이미 앞에서 Head같은 파트를 쉽게 찾을수있는 방법을 알았기 때문에 쉽게 구현할수있다.
- 먼저 Aimbot에 우선순위를 정해야 한다. 그렇지 않으면 aimbot은 정말 마구 잡이로 쏘아버리기 때문에 마우스를 내 마음대로 사용할수 없게된다. 또한 원하는 적을 처리하지 못할수도 있다.
- 우선 내가 새운 우선순위는 첫번째로 FOV를 안에 있는 적만 잡는것이다.
- 두번째로는 마우스 포인터로 부터의 2d Head 파트의 거리가 가까운 적을 잡는것.
> 이렇게 해야 내가 원하는 상대를 노릴수있다.
- 세번째로는 3d 거리로 나의 머리와 상대의 머리의 위치를 비교하여 인게임에서 더 가까운적을 잡는 것이다.
- 쉽게 말하면 마우스 포인터가 적 두명 사이 가운데에 있는데 멀리있는 적부터 쏘면 어색하기 때문에 마우스 포인터 거리가 두 객체의 머리와 비슷한 거리에 있다면 3d기준으로 더 가까운 쪽으로 에임이 움직이는 것이다.
- 이제 우선순위를 정했으니 마우스를 움직이게 하면되는데 바로 순간이동 하는 식으로 이동하면 너무 핵처럼 보이기 때문에 Smooth라는 값을 추가해서 비교적 천천히 값에 증가하도록 한다
> 이때 거리에 비례해서 빨라지게 했다.
```cpp
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
```
- 이제 모든 기능을 다 구현하게 되었다.

