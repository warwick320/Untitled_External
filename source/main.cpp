
#include <SDK/Cache.h>
#include <Cheats/Players.h>
#include <Cheats/Visuals.h>
inline RBX::Instance g_Team;
void debug_print(str text, i32 lev) {
	str log = (lev == 0) ? "[+]" : (lev == 1) ? "[-]" : "[?]";
	std::cout << std::format("{} {}\n", log, text) << std::endl;
}
void inject() {
	debug_print("Starting", 0);
	if (!comms->is_connected()) {
		debug_print("Failed to connect to the driver", 1);
		sleep_ms(5000);
		exit(0);
	}
	debug_print("Connected to driver successfully", 0);
	debug_print("Looking for roblox", 0);
	while (comms->find_process("RobloxPlayerBeta.exe") == 0) sleep_ms(100);
	printf("[+] Found roblox with process id: %d\n", comms->process_id);
	if (comms->find_image() == 0) debug_print("Failed to get the base address", 1);

	printf("Found Roblox with base address: %llx\n", comms->image_address);


	u64 fakeDataModel = comms->read<u64>(comms->image_address + Offsets::FakeDataModel);
	dataModel = std::make_unique<RBX::DataModel>(comms->read<u64>(fakeDataModel + Offsets::RealDataModel));

	printf("DataModel -> 0x%llx\n", dataModel->getAddress());

	visualEngine = std::make_unique<RBX::VisualEngine>(comms->read<u64>(comms->image_address + Offsets::VisualEngine));
	printf("VisEngine -> 0x%llx\n", visualEngine->getAddress());

	//printf("Team -> %llx\n", g_LocalPlayer.getTeam());
}
auto main(i32 argc, i8** argv) -> i32 {
	inject();

	render = std::make_unique<Render>();
	render->setupOverlay("Untitled_External");

	std::thread(updatePlayers).detach();
	
	while (render->isRunning) {

		render->startRender();
		if (Esp_Enabled) espLoop();
		if (Aimbot_Enabled) aimbotLoop();
		if (render->isVisible) render->renderMenu();
		render->endRender();
	}
	return 0;
}
