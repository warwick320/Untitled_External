#pragma once
#include <SDK/Offset.h>
#include <SDK/Classes/DataModel/Player.h>
inline RBX::Instance g_LocalPlayer;
inline RBX::Player g_LocalPlayerInstance;
inline std::vector<RBX::Player> g_PlayerList;

//void updatePlayers() {
//	RBX::Instance playerService = dataModel->findFirstChild("Players");
//
//	
//	while (true) {
//		g_LocalPlayer = comms->read<u64>(playerService.getAddress() + Offsets::LocalPlayer);
//
//		for (RBX::Instance& player : playerService.getChildren()) {
//			if (!player.getAddress()) {
//				continue;
//			}
//			g_PlayerList.push_back(RBX::Player(player.getAddress()));
//
//		}
//		sleep_ms(100);
//		g_PlayerList.clear();
//	}
//}
void updatePlayers() {
    RBX::Instance playerService = dataModel->findFirstChild("Players");

    while (true) {
		//printf("Updating players...\n");
        if (playerService.getAddress() == 0){
			// 플레이어 서비스가 없으면 다시 찾기
			printf("PlayerService not found, retrying...\n");

			playerService = dataModel->findFirstChild("Players");
        }
        
        g_LocalPlayer = comms->read<u64>(playerService.getAddress() + Offsets::LocalPlayer);
		if (g_LocalPlayer.getAddress() == 0) {
			printf("LocalPlayer not found, retrying...\n");
            playerService = dataModel->findFirstChild("Players");
			sleep_ms(100);
			continue;
		}
        // g_LocalPlayerInstance도 설정
        if (g_LocalPlayer.getAddress()) {
            g_LocalPlayerInstance = RBX::Player(g_LocalPlayer.getAddress());
        }

        for (RBX::Instance& player : playerService.getChildern()) {
            if (!player.getAddress()) {
                continue;
            }
            g_PlayerList.push_back(RBX::Player(player.getAddress()));
        }
        sleep_ms(100);
        g_PlayerList.clear();
    }
}