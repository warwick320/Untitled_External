#include <SDK/Cache.h>
#include <Cheats/Players.h>
#include <Cheats/Visuals.h>
#include <ImGui/imgui_impl_win32.h>
#include <SDK/Classes/AddressMananger.h>
#include "SharedMemoryManager.h" // 추가

inline RBX::Instance g_Team;

// 공유 메모리 매니저 전역 선언
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

            sharedMem->ReadFromShared();    // UI -> External

            sharedMem->UpdateToShared();    // External -> UI

            //frameCounter++;
            //if (frameCounter % 600 == 0) { 
            //    if (sharedMem->IsUIConnected()) {
            //        std::cout << "[INFO] UI is connected and active" << std::endl;
            //    }
            //    else {
            //        std::cout << "[WARNING] UI seems disconnected or inactive" << std::endl;
            //    }
            //    frameCounter = 0;
            //}
        }
        else {
            std::cout << "[ERROR] SharedMem is null!" << std::endl;
        }
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
    std::thread sharedMemThread(SharedMemoryUpdateThread);
    bool firstTime = false;

    while (render->isRunning) {
        if (!firstTime) {
			firstTime = true;
			MemShared = true;
        }
        if (!runningThread) {
            playerThread.join();
            runningThread = true;
            playerThread = std::thread(updatePlayers);
        }

        render->startRender();

        if (Esp_Enabled) {
            espLoop();
        }
        if (Aimbot_Enabled) {
            aimbotLoop();
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