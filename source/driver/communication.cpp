#include <driver/communication.h>
#include <Psapi.h>

communication::communication() {
    // NtDll 함수 포인터 초기화
    HMODULE ntdll = GetModuleHandleA("ntdll.dll");
    if (ntdll) {
        NtReadVirtualMemory = (_NtReadVirtualMemory)GetProcAddress(ntdll, "NtReadVirtualMemory");
        NtWriteVirtualMemory = (_NtWriteVirtualMemory)GetProcAddress(ntdll, "NtWriteVirtualMemory");

        if (NtReadVirtualMemory == nullptr || NtWriteVirtualMemory == nullptr) {
            printf("Failed. NtDll function pointers: NtReadVirtualMemory=%p, NtWriteVirtualMemory=%p\n",
                NtReadVirtualMemory, NtWriteVirtualMemory);
        }
    }
    else {
        printf("Failed. Handle to ntdll.dll. error: %lu\n", GetLastError());
    }

    process_handle = INVALID_HANDLE_VALUE;
}

communication::~communication() {
    if (process_handle != INVALID_HANDLE_VALUE) {
        CloseHandle(process_handle);
    }
}

bool communication::is_connected() {
    return (NtReadVirtualMemory != nullptr && NtWriteVirtualMemory != nullptr);
}

bool communication::v_attach(i32 process_id) {
    if (process_handle != INVALID_HANDLE_VALUE) {
        CloseHandle(process_handle);
        process_handle = INVALID_HANDLE_VALUE;
    }

    this->process_id = process_id;


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

    DWORD accessRights[] = {
        PROCESS_ALL_ACCESS,
        PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION | PROCESS_QUERY_INFORMATION,
        PROCESS_VM_READ | PROCESS_QUERY_LIMITED_INFORMATION,
        PROCESS_VM_READ
    };

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
        process_handle = INVALID_HANDLE_VALUE;
        return false;
    }

    printf("Successfully attached to process %d, handle: 0x%p\n", process_id, process_handle);
    return true;
}

std::string communication::readstr(u64 address) {
    i32 StrLength = read<i32>(address + 0x18);

    if (StrLength >= 16) {
        address = read<u64>(address);
    }

    std::vector<i8> Buffer(256);
    SIZE_T bytes_read = 0;

    if (process_handle != INVALID_HANDLE_VALUE && NtReadVirtualMemory) {
		NtReadVirtualMemory(process_handle, reinterpret_cast<PVOID>(address), Buffer.data(), Buffer.size(), &bytes_read); // max 256
    }

    return std::string(Buffer.data());
}

i32 communication::find_process(const i8* process_name) {
    PROCESSENTRY32 process_entry = {};
    process_entry.dwSize = sizeof(PROCESSENTRY32);

	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); 
    if (snapshot == INVALID_HANDLE_VALUE) {
        return 0;
    }

    if (Process32First(snapshot, &process_entry)) {
        do {
            if (_stricmp(process_entry.szExeFile, process_name) == 0) {
                process_id = process_entry.th32ProcessID;
                break;
            }
		} while (Process32Next(snapshot, &process_entry)); // 프로세스 목록 순회
    }

    CloseHandle(snapshot);
    return process_id;
}

u64 communication::find_image() {
    u64 image_address = 0;
    HANDLE hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, process_id);
    if (hModuleSnap != INVALID_HANDLE_VALUE) {
        MODULEENTRY32 moduleEntry;
        moduleEntry.dwSize = sizeof(MODULEENTRY32);

        if (Module32First(hModuleSnap, &moduleEntry)) {
            image_address = (u64)moduleEntry.modBaseAddr;
        }
        CloseHandle(hModuleSnap);
    }


    communication::image_address = image_address;
    return image_address;
}