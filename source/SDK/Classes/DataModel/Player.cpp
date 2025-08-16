#include <SDK/Classes/DataModel/Player.h>

RBX::ModelInstance RBX::Player::getModelInstance() {
	return RBX::ModelInstance(comms->read<u64>(address + Offsets::ModelInstance));

}