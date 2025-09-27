#include <SDK/Cache.h>
#include <Cheats/Players.h>
#include <Cheats/Visuals.h>
#include <ImGui/imgui_impl_win32.h>
#include <SDK/Classes/AddressMananger.h>
#include "SharedMemoryManager.h"
#include "Cheats/Misc.h"
#include <Cheats/Object.h>
RBX::Instance g_Team;

std::unique_ptr<SharedMemoryManager> sharedMem;


void inject() {
	debug_print("Starting", 0);
	if (!comms->is_connected()) {
		debug_print("Failed connect to Ntdll", 1);
		sleep_ms(5000);
		exit(0);
	}
	debug_print("Connected to Ntdll successfully", 0);
	debug_print("Waiting for roblox", 0);

	while (comms->find_process("RobloxPlayerBeta.exe") == 0) sleep_ms(4000);
	printf("[+] PID: %d\n", comms->process_id);

	debug_print("Attaching to process wait a sec", 0);
    sleep_ms(2000);
	if (!comms->v_attach(comms->process_id)) {
		debug_print("Failed to attach to process", 1);
		sleep_ms(5000);
		exit(0);
	}
	debug_print("Successfully attached to process", 0);

	if (comms->find_image() == 0) {
		debug_print("Failed to get the base address", 1);
		sleep_ms(5000);
		exit(0);
	}

	printf("Found Roblox with base address: %llx\n", comms->image_address);

	fakeDataModel = comms->read<u64>(comms->image_address + Offsets::FakeDataModel);
	dataModel = std::make_unique<RBX::DataModel>(comms->read<u64>(fakeDataModel + Offsets::RealDataModel));

	printf("First DataModel -> 0x%llx\n", dataModel->getAddress());

	visualEngine = std::make_unique<RBX::VisualEngine>(comms->read<u64>(comms->image_address + Offsets::VisualEngine));
	printf("First VisEngine -> 0x%llx\n", visualEngine->getAddress());
}
bool MemShared = false;
void SharedMemoryUpdateThread() {
    while (MemShared){
        sleep_ms(1);

        int frameCounter = 0;

        if (sharedMem) {

            sharedMem->ReadFromShared();
            sharedMem->UpdateToShared();
        }
        else {
            std::cout << "[ERROR] SharedMem is null!" << std::endl;
        }
    }
}
void startQML() {
    debug_print("Starting QML UI", 0);

    char currentDir[MAX_PATH];
    GetCurrentDirectoryA(MAX_PATH, currentDir);

    char fullPath[MAX_PATH];
    snprintf(fullPath, sizeof(fullPath), "%s\\ui\\EX.exe", currentDir);

    // 파일 존재 확인
    if (GetFileAttributesA(fullPath) == INVALID_FILE_ATTRIBUTES) {
        debug_print("EX.exe file does not exist", 1);
        return;
    }

    STARTUPINFOA si = { sizeof(si) };
    PROCESS_INFORMATION pi;

    if (CreateProcessA(
        fullPath,       // application name
        NULL,           // command line
        NULL,           // process attributes
        NULL,           // thread attributes
        FALSE,          // inherit handles
        0,              // creation flags
        NULL,           // environment
        NULL,           // current directory
        &si,            // startup info
        &pi             // process info
    )) {
        debug_print("QML UI started successfully with CreateProcess", 0);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
    else {
        DWORD error = GetLastError();
        char errorMsg[256];
        snprintf(errorMsg, sizeof(errorMsg), "CreateProcess failed - Error code: %lu", error);
        debug_print(errorMsg, 1);
    }
}

auto main(int argc, char** argv) -> int {
    inject();

    sharedMem = std::make_unique<SharedMemoryManager>();
    if (sharedMem->Initialize()) {
        debug_print("Ready for UI communication", 0);
        sharedMem->PrintDebugInfo();
    }
    else {
        debug_print("Failed to initialize shared memory!", 1);
        return -1;
    }

    render = std::make_unique<Render>();
    render->setupOverlay("Untitled_External");

    std::thread playerThread(updatePlayers);
    std::thread(CheckAddressesLoop).detach();
	std::thread(miscLoop).detach();
	std::thread(WorkspaceObjPos).detach();
    std::thread sharedMemThread(SharedMemoryUpdateThread);
    bool firstTime = false;

   
    while (render->isRunning) {
        if (!firstTime) {
			firstTime = true;
			MemShared = true;
            sleep_ms(100);
			std::thread(startQML).detach();
        }

        if (!runningThread) {
            playerThread.join();
            runningThread = true;
            playerThread = std::thread(updatePlayers);
        }

        render->startRender();
		//raycast_test();

        if (Esp_Enabled) {
            espLoop();
        }
        if (Aimbot_Enabled) {
            //triggerBot();
            aimbotLoop();
        }
        if (triggerbot) {
            triggerBot();
        }
        if (render->isVisible) {
            render->renderMenu();
        }

        render->endRender();

    }
    runningThread = false;
    playerThread.join();
    sharedMemThread.join();
    return 0;
}