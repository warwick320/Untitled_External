#pragma once
#include <SDK/Offset.h>
#include <SDK/Classes/DataModel/Player.h>
inline RBX::Instance g_LocalPlayer;
inline RBX::Player g_LocalPlayerInstance;
inline std::vector<RBX::Player> g_PlayerList;
inline std::vector<RBX::Player> pf_PlayerPartList;
inline std::vector<RBX::Player> pf_PlayerPartList2;
void pfPlayers() {

	std::vector<RBX::Instance> all_robox_instance = dataModel->getChildren();
	printf("Start Found Pf Players\n");

	RBX::Instance workspace;
	for (RBX::Instance& instance : all_robox_instance) {
		if (instance.getClassName() == "Workspace") {
			workspace = instance;
			break;
		}
	}

	if (workspace.getAddress() == 0) {
		printf("Workspace not found\n");
		return;
	}


	RBX::Instance players = workspace.findFirstChild("Players");
	if (players.getAddress() == 0) {
		printf("Players not found in Workspace\n");
		return;
	}
	RBX::Instance Ava;
	i32 index = 0;
	while (true) {
		std::vector<RBX::Instance> playersChildren = players.getChildren();
		for (RBX::Instance& child1 : playersChildren) {
			std::vector<RBX::Instance> folderChild = child1.getChildren();
			for (RBX::Instance& child : folderChild) {
				//printf("Checking child: %s, Class: %s\n", child.getName().c_str(), child.getClassName().c_str());
				if (child.getClassName() == "Model") {
					std::vector<RBX::Instance> modelChildren = child.getChildren();
					Ava = child;
					for (RBX::Instance& part : modelChildren) {
						if (part.getClassName() == "Part") {
							RBX::Instance nameTagGui = part.findFirstChild("NameTagGui");
							if (nameTagGui.getAddress() != 0) {
								pf_PlayerPartList.push_back(RBX::Player(part.getAddress()));
								printf("Found player part at : %s, Address: 0x%llx\n", part.getName().c_str(), part.getAddress());
							}
						}
					}
				}

			}

		}
		sleep_ms(50);
		pf_PlayerPartList.clear();

	}
}
void updatePlayers() {
	RBX::Instance playerService = dataModel->findFristChildByClass("Players");


	while (true) {
		if (toggePF) {
			printf("PlayerService not found\n");
			pfPlayers();
		}
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


void getAllInstancesRecursive(RBX::Instance& instance, std::vector<RBX::Instance>& result) {
	result.push_back(instance);
	std::vector<RBX::Instance> children = instance.getChildren();
	for (RBX::Instance& child : children) {
		getAllInstancesRecursive(child, result);
	}
}

void getAll() {
	std::vector<RBX::Instance> all_robox_instance = dataModel->getChildren();
	std::vector<RBX::Instance> all_instances;
	for (RBX::Instance& instance : all_robox_instance) {
		getAllInstancesRecursive(instance, all_instances);
	}
}
#include <fstream>
#include <functional>
void saveInstanceTreeToFile(std::vector<RBX::Instance>& instances, std::string& filename) {
	std::ofstream file(filename);
	if (!file.is_open()) return;

	std::function<void(RBX::Instance&, int, std::string)> writeTree;
	writeTree = [&file, &writeTree](RBX::Instance& instance, int depth, std::string path) {

		for (int i = 0; i < depth; ++i) file << "  ";


		std::string currentPath = path.empty() ? instance.getClassName() : path + " -> " + instance.getClassName();
		file << instance.getName() << " [" << instance.getClassName() << "] (Path: " << currentPath << ")\n";

		std::vector<RBX::Instance> children = instance.getChildren();
		for (size_t i = 0; i < children.size(); ++i) {
			writeTree(children[i], depth + 1, currentPath);
		}
		};

	for (size_t i = 0; i < instances.size(); ++i) {
		writeTree(instances[i], 0, "");
	}
	file.close();
}

void getAllAndSaveTree() {
	std::vector<RBX::Instance> all_robox_instance = dataModel->getChildren();
	std::vector<RBX::Instance> all_instances;
	for (RBX::Instance& instance : all_robox_instance) {
		getAllInstancesRecursive(instance, all_instances);
	}
	std::string filename = "InstanceTree.txt";
	saveInstanceTreeToFile(all_robox_instance, filename);
}
