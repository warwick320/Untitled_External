#pragma once
#include <Windows.h>
#include <tlhelp32.h>
#include <cstdint>
#include <string>
#include <winternl.h>

// NtDll 함수 타입 정의
typedef NTSTATUS(NTAPI* _NtReadVirtualMemory)(
	HANDLE ProcessHandle,
	PVOID BaseAddress,
	PVOID Buffer,
	SIZE_T BufferSize,
	PSIZE_T NumberOfBytesRead
	);

typedef NTSTATUS(NTAPI* _NtWriteVirtualMemory)(
	HANDLE ProcessHandle,
	PVOID BaseAddress,
	PVOID Buffer,
	SIZE_T BufferSize,
	PSIZE_T NumberOfBytesWritten
	);

// 기존 코드는 유지
#define PRW_CODE CTL_CODE(FILE_DEVICE_UNKNOWN, 0x2ec33, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define VRW_ATTACH_CODE CTL_CODE(FILE_DEVICE_UNKNOWN, 0x2ec34, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define VRW_CODE CTL_CODE(FILE_DEVICE_UNKNOWN, 0x2ec35, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define BA_CODE CTL_CODE(FILE_DEVICE_UNKNOWN, 0x2ec36, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)  
#define SECURITY_CODE 0x94c9e4bc3

// 기존 구조체 정의 유지
struct _PRW {
	u64 security_code;

	i32 process_id;
	void* address;
	void* buffer;

	u64 size;
	u64 return_size;

	bool Type;
};

struct _VRW {
	u64 security_code;

	HANDLE process_handle;
	void* address;
	void* buffer;

	u64 size;
	u64 return_size;

	bool Type;
};

struct _BA {
	u64 security_code;

	i32 process_id;
	u64* address;
};

class communication {
public:
	communication();
	~communication();

	bool is_connected();

	bool v_attach(i32 process_id);

	i32 find_process(const i8* ProcessName);
	u64 find_image();

	template <typename T>
	T v_read(u64 address);
	template <typename T>
	void v_write(u64 address, T& Value);

	template <typename T>
	T read(u64 address);
	template <typename T>
	void write(u64 address, T& Value);

	std::string readstr(u64 address);

	u64 image_address = 0;
	i32 process_id = 0;
private:
	HANDLE process_handle = INVALID_HANDLE_VALUE;
	_NtReadVirtualMemory NtReadVirtualMemory = nullptr;
	_NtWriteVirtualMemory NtWriteVirtualMemory = nullptr;
};

template <typename T>
T communication::read(u64 address) {
	T temp = {};
	SIZE_T bytes_read = 0;

	if (process_handle != INVALID_HANDLE_VALUE && NtReadVirtualMemory) {
		NTSTATUS status = NtReadVirtualMemory(process_handle, reinterpret_cast<PVOID>(address), &temp, sizeof(T), &bytes_read);
		if (status != 0 || bytes_read != sizeof(T)) {
			//printf("Memory read failed at address 0x%llx (Status: 0x%lx, Read: %zu bytes)\n", address, status, bytes_read);
			// 실패한 경우 0으로 초기화
			memset(&temp, 0, sizeof(T));
		}
	}
	/*else {
		printf("Cannot read memory: process handle invalid or NtReadVirtualMemory not available\n");
	}*/

	return temp;
}

template <typename T>
void communication::v_write(u64 address, T& value) {
	SIZE_T bytes_written = 0;

	if (process_handle != INVALID_HANDLE_VALUE && NtWriteVirtualMemory) {
		NTSTATUS status = NtWriteVirtualMemory(process_handle, reinterpret_cast<PVOID>(address), &value, sizeof(T), &bytes_written);
		if(status != 0) {
			printf("failed to write memory");
		}
	}
	
}

template <typename T>
T communication::v_read(u64 address) {
	return read<T>(address);
}

template <typename T>
void communication::write(u64 address, T& value) {
	v_write(address, value);
}