# Untitled External - Project Guide

이 가이드의 의의는 코드의 전체 흐름을 대략적으로 이해하도록 돕는 데 있습니다.
따라서 독자가 코드를 읽을 때 전체적인 동작 방식을 파악할 수 있는 것을 목표로 합니다.
본 가이드는 독자가 기본적인 C++ 문법을 알고 있다는 전제로 작성되었습니다.

## 목차
> QML Ui는 직관적이고 어렵지 않기 때문에 다루지 않음. 코드는 ui branch 에 있음


1. 핵심 개념
2. 프로젝트 구조

## 핵심 개념
> 크게 이렇게 나눌수있다고 생각했다.
### 1. External vs Internal
> 약간 딴 소리일수 있음.

> 여러 사람이 External과 Internal의 차이를 잘 모르는거 같아 써봄.

> 틀릴수 있음.

| 구분 | 익스터널 (External) | 인터널 (Internal) |
|------|-----------------------------|-----------------------------|
| 실행 위치 | 게임 외부 (별도 프로세스) | 게임 내부 (프로세스 안) |
| Lua VM 접근 | 메모리 직접 읽기/쓰기 -> `lua_State*`, `lua_pcall` 등 오프셋 사용 | Lua API를 직접 호출 -> 스택과 함수 그대로 사용 |
| 스크립트 주입 방식 | 외부에서 메모리 영역 할당 후 스택 시뮬레이션 | 내부에서 Lua 스택에 바로 스크립트 올리고 호출 |
| 성능 | 상대적으로 느림, 메모리 복사 필요 | 빠름, 직접 API 호출 |


이걸 보고 이런생각이 들수있다.
왜 같은 인터널(Lua API를 호출) 방식인데 점수차이가 나는걸까?
> 여기서 말하는 점수는 sUnc 및 Unc

우선 UNC 함수들은 로블로스 API가 아닌 커스텀 함수이다.

높은 점수를 가진 익스큐터는 모든 함수를 완벽하게 후킹한다.

반면 낮은 점수를 가진 익스큐터는 일부 함수만 구현(불완전,무결성 등의 이유로) 일부함수만 구현하거나 Lua-side 에뮬레이션으로 대체 한다.
> 아마도

그럼 왜 직접 Lua API를 호출할수있는 인터널 환경이 아닌 익스터널 환경에서 만드는 것일까?

우선 우리는 Lua VM에 크게 접근할 이유가 없다.

또한 로블록스의 안티치트는 주로 프로세스 내부 주입( DLL injection 같은 행위)를 감지한다.

그래서 Read / Write 만 하는 익스터널은 탐지에 비교적 강하다.

따라서 탐지 위험이 있고 힘들게 Lua VM에 접근하지 않고도 Aimbot이나 Esp핵을 구현할수있기 때문에 익스터널로 만드는 것이다.

### 2. 메모리 접근 (기초)
> 익스터널의 핵심! 이라고 생각한다

메모리 읽기 함수는 이렇게 이루어져있다.
1. 읽기
```cpp
template <typename T>
T communication::read(u64 address) {
	T temp = {};
	SIZE_T bytes_read = 0;

	if (process_handle != INVALID_HANDLE_VALUE && NtReadVirtualMemory) {
		NTSTATUS status = NtReadVirtualMemory(process_handle, reinterpret_cast<PVOID>(address), &temp, sizeof(T), &bytes_read);
		if (status != 0 || bytes_read != sizeof(T)) {
			printf("Memory read failed at address 0x%llx (Status: 0x%lx, Read: %zu bytes)\n", address, status, bytes_read);
			// 실패한 경우 0으로 초기화
			memset(&temp, 0, sizeof(T));
		}
	}
	else {
		printf("Cannot read memory: process handle invalid or NtReadVirtualMemory not available\n");
	}

	return temp;
}

```
2. 문자열 읽기
```cpp
std::string communication::readstr(u64 address) {
    i32 StrLength = read<i32>(address + 0x18);

    if (StrLength >= 16) {
        address = read<u64>(address); // pointer
	}// SSo string

    std::vector<i8> Buffer(256);
    SIZE_T bytes_read = 0;

    if (process_handle != INVALID_HANDLE_VALUE && NtReadVirtualMemory) {
		NtReadVirtualMemory(process_handle, reinterpret_cast<PVOID>(address), Buffer.data(), Buffer.size(), &bytes_read); // max 256
    }

    return std::string(Buffer.data());
}
```
> 여기서 StrLength 가 16이상이면 address를 read하는 이유는 SSO(short string optimization) 이라는 짧은 문자열에 대한 최적화 방식이 있기 떄문이다.
> 따라서 16이상이면 포인터로 간주하여 한번더 읽는것이다.
> 즉 저 조건문은 SSO 문자열과 힙 문자열을 구분해서 올바른 메모리 위치를 읽기 위해 존재한다.

  
메모리 접근은 이렇게 이루어진다.
>응용 (기초)
1. 타겟 프로세스 찾기
> example
<details>
  <summary>함수 구조</summary>

```cpp
i32 communication::find_process(const i8* process_name) {
    // PROCESSENTRY32 구조체를 0으로 초기화하고 크기를 설정
    // 이 구조체에 프로세스 정보 ex) 프로세스 이름 pid 등 이 채워짐
    PROCESSENTRY32 process_entry = {};
    process_entry.dwSize = sizeof(PROCESSENTRY32);

    // 현재 시스템의 프로세스 스냅샷 생성 
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); 
    if (snapshot == INVALID_HANDLE_VALUE) {
        return 0;
    }

    // 스냅샷에서 첫 번째 프로세스를 가져오고 순회
    if (Process32First(snapshot, &process_entry)) {
        do {
            // 이름 비교
            if (_stricmp(process_entry.szExeFile, process_name) == 0) {
                process_id = process_entry.th32ProcessID;
                break;
            }
        } while (Process32Next(snapshot, &process_entry)); // 다음 프로세스로 이동
    }

    // 스냅샷 핸들 정리
    CloseHandle(snapshot);
    return process_id;
}
```
> usage

</details>

``` cpp
int processId = comms->find_process("RobloxPlayerBeta.exe");
```

2. 찾은 프로세스 연결

> example

<details>
  <summary>함수 구조</summary>
  
```cpp
bool communication::v_attach(i32 process_id) {
    // 이전에  열려있는 프로세스 핸들이 존재하면 닫고 초기화 ( 중복 attach 방지)
    if (process_handle != INVALID_HANDLE_VALUE) {
        CloseHandle(process_handle);
        process_handle = INVALID_HANDLE_VALUE;
    }
    // 현재 객체에 대상 프로세스 id 저장
    this->process_id = process_id;

    // 권한 확인
    BOOL isElevated = FALSE;
    HANDLE hToken = NULL;
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
        TOKEN_ELEVATION elevation;
        DWORD cbSize = sizeof(TOKEN_ELEVATION);
        if (GetTokenInformation(hToken, TokenElevation, &elevation, sizeof(elevation), &cbSize)) {
            isElevated = elevation.TokenIsElevated;
        }
        CloseHandle(hToken);
    }

    printf(" % s with admin privileges\n", isElevated ? "running" : "NOT running");
    // 시도할 접근 권한 목록( 권한이 낮으면 낮은 권한 부터 시도)
    // 사실 우린 Read 권한만 있어도 크게 신경쓸게 없다.
    DWORD accessRights[] = {
        PROCESS_ALL_ACCESS,
        PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION | PROCESS_QUERY_INFORMATION,
        PROCESS_VM_READ | PROCESS_QUERY_LIMITED_INFORMATION,
        PROCESS_VM_READ
    };
    // 순차적으로 시도하여 프로세스 핸들 얻기
    for (DWORD access : accessRights) {
        process_handle = OpenProcess(access, FALSE, process_id);
        if (process_handle != INVALID_HANDLE_VALUE && process_handle != NULL) {
            printf("Successfully opened process with access rights: 0x%08X\n", access);
            break;
        }
        
        DWORD error = GetLastError();
        printf("failed to open process with access rights 0x%08X, error: %lu\n", access, error);
    }

    // 핸들 유효성 확인
    if (process_handle == INVALID_HANDLE_VALUE || process_handle == NULL) {
        printf("failed to open process with any access rights combination\n");
        process_handle = INVALID_HANDLE_VALUE; // 아무 권한이 없으면 실패 
        return false;
    }

    printf("Successfully attached to process %d, handle: 0x%p\n", process_id, process_handle);
    return true;
}
```
> usage

</details>

``` cpp
bool success = comms->v_attach(processId);
if (!success){
  printf("Failed\n");
  exit(0);
}
```

3. 프로세스의 베이스 주소 얻기
> example
<details>
  <summary>함수 구조</summary>

```cpp
u64 communication::find_image() {
    u64 image_address = 0;
    HANDLE hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, process_id); // 대상 프로세스 모듈 스냅샷
    if (hModuleSnap != INVALID_HANDLE_VALUE) {
        MODULEENTRY32 moduleEntry;
        moduleEntry.dwSize = sizeof(MODULEENTRY32);
        // 스냅샷에서 첫번쨰 모듈을 읽어옴 -> 일반적으로 첫 번째로 반환되는 모듈은 프로세스의 메인 이미지(PE exe)임
        if (Module32First(hModuleSnap, &moduleEntry)) { 
            image_address = (u64)moduleEntry.modBaseAddr; // modBaseAddr은 모듈의 메모리 베이스(실제 로드 주소)를 가르키는 포인터 ( unsinged long long으로 캐스트해서 저장 -> 64비트)
        }
        CloseHandle(hModuleSnap);
    }

    communication::image_address = image_address;
    return image_address;
}
```

> usage

</details>

> unsigned long long 을 쓰는이유는 타겟 프로세스가 64bit이기 때문이다. 
```cpp
unsigned long long base_addr = comms->find_image();
if(base_addr == 0) printf("Failed \n");
```

4. 메모리 주소에서 데이터 읽기
> example
```cpp
unsigned long long fakeDataModel = comms->read<unsinged long long>(comms->image_address + Offsets::FakeDataModel);
```

### 3. 좌표계 변환

우리가 이제 메모리 주소를 읽어서 플레이어의 위치를 알아냈다고 생각해보자.

이제 알아낸 위치를 모니터에 표시해야 Esp를 구현할수있다.

하지만 여기서 문제가 있다.

우리가 구한 플레이어의 위치는 게임엔진에서의 위치지 우리가 보는 모니터에서의 위치가 아니다.

이게 뭐가 다르냐고 할수있는데 게임엔진은 3차원(x,y,z)으로 데이터를 다루고 우리가 보는 모니터는 2차원(x,y) 데이터를 다룬다.

따라서 우린 얻은 좌표를 2차원 데이터로 변환(world to screen)해야 정상적으로 모니터에 표시할수있게 된다.

그럼 이제 코드를 보자.

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

먼저 이 코드를 이해하기 위해서는 행렬 개념을 잘 알아두면 좋다.

이제 단계 별로 설명하면 먼저 Vector2 deimensions 라는 변수로 화면 너비/높이를 가져온다.

그다음 Matrix4 viewMatrix로 viewProjection을 가져온다.

이제 월드 좌표를 클립 공간으로 변환해야 한다.

**클립 공간이란 카메라(view) 기준 좌표  + 투영(projection) 적용좌표 이다.**

클립 공간으로 바꾸는 이유는 4차원 형태로 존재하고 ( x, y, z, w) 화면 밖에 있는 좌표를 쉽게 걸러낼수 있다.
> 이 과정을 clipping이라고 부른다.

> 우리는 3차원 데이터를 2차원 데이터로 변환하는게 목표인데 왜 4차원으로 바꾸냐 라는 생각이 들수있다.

> 이 코드에서의 4차원의 역할은 원근 투영을 구현하기 위해서이다. 3D 좌표 ( x,y,z) 만 있으면 투영 비율을 계산할수없다. 원근효과를 내려면 z(카메라 까지의 거리)에 따라 화면에 작게 또는 크게 찍어야한다.

> 이때 4차원 w가 그 거리 비율 역할을 해준다.

> 따라서 단순히 3D 에서 2D로 가면 가까운 적 오브젝트 등이 화면에서 동일한 크기로 찍힌다

> z 값을 기반으로 원근감(스케일)을 반영해야 2D 화면상에서 외곡없이 표현할수있다.

> 따라서 4D로 확장하면 x/w , y/w 나눔만으로 간단하게 원근을 적용할수있다.

쉽게 생각해서 시야 안/밖 체크 및 원근 변환 준비 단계라고 생각하면된다.

이 다음으로 NDC(normalizedDeviceCoordinates)로 다시 변환한다.
> 화면에 그릴수 있는 2D 좌표를 얻기 위해 다시 3D로 압축

이제 최종적으로 우리의 목표인 2D 데이터로 변환하면 된다.

먼저 dimensions.x = 가로 해상도 / 2.0f (중심을 기준으로 좌우로 나눔) * NDC.x = NDC(-1~1)를 화면 절반 크기로 스케일
> NDC 좌표는 -1 ~ 1의 범위를 갖는 정규화된 좌표계이다.

> 예를 들면 x = -1 - > 화면 왼쪽 끝

> x = 0 - > 화면 가운데

> x = +1 - > 화면 오른쪽 끝

> 즉 NDC는 좌우 중앙을 0을 기준으로 두고 상대적인 위치를 나타내느 값이다.

> 따라서 NDC를 화면 절반 크기로 스케일링 한다는것은 -1 ~ 1 범위를 화면 중심 기준 좌우 절반 픽셀로 늘린다 라는 뜻.

이후 dimensions.x / 2.0f 한 값을 더해주어 좌측 기준으로 욺겨서 실제 픽셀 좌표를 만든다.

> 쉽게 요약 해서 설명하면

> NDC는 **중앙 기준** 좌표

> 화면 픽셀 좌표는 **좌측 기준** 좌표

> 곱하고 더하면 중앙기준 -> **화면 기준**으로 변환됨.

Y축도 똑같이 생각할수 있다.
> -1을 곱하는 이유는 NDC 좌표계는 y가 위로 갈수록 +1 아래로 가면 -1이고 화면 픽셀 좌표계는 y가 위쪽 0, 아래쪽이 최대

> 즉 NDC y와 화면 y가 반대 방향이다. 따라서 -1을 곱하여 방향을 다시 뒤집어준다. 이후 다시 + dimensions.y / 2.0f 를 해주면 된다.

## 프로젝트 구조

이제 대략적인 핵심 개념을 알았으니깐 구조를 알아보자.


### 1. 메인 구조
   
| 이름 | 역할 |
|------|-----------------------------|
| main.cpp | 프로그램 진입점 및 메인 루프,초기 DataModel 수집 및 VisualEngine 수집 |
| global.h | 전연 변수 및 설정값등 정의 |
| SharedData.h | 공유 메모리 데이터 구조체 |
| SharedMemoryManager.h | Ui와 External 간 통신 관리 |
| Players.h | 플레이어 업데이트 및 관리 |
| AddressManager.h | 메모리 관리 |

### 2. SDK 구조

| 이름 | 역할 |
|-----|-------|
| Offset.h | 메모리 오프셋 정의 |
| DataModel(폴더) | 게임 데이터 모델 관련 컨트롤 |
| VisualEngine(폴더) | 3D -> 2D 좌표 변환 및 렌더링 |
| Math(폴더) | Vector2 , Vector3 , Vector4 , Matrix4 수학 관련 구조 선언 |
| World(폴더) | 게임 내 객체(Primitive) 관리 |
| Instance.h / cpp | Roblox 인스턴스 기본 클래스 정의 및 관리 |

### 3. 메모리 관리 구조

| 이름 | 역할 |
|-----|-------|
| communication.cpp / h | 메모리 접근 및 Attach 등 관리 |

### 4. Cheat 구조

| 이름 | 역할 |
|-----|-------|
| Visuals.h | ESP및 Aimbot 로직 |

### 5. 렌더링 시스템 구조

| 이름 | 역할 |
|-----|-------|
| Render.h / cpp | DirectX 11 기반 오버레이 렌더링 |

### 6. 외부 라이브러리 구조

| 이름 | 역할 |
|-----|-------|
| ImGui | 인터페이스 라이브러리 |


