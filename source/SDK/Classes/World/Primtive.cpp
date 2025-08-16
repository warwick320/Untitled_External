#include <SDK/Classes/World/Primtive.h>

Vector3 RBX::Primitive::getPartSize() {
	return comms->read<Vector3>(address + Offsets::PartSize);
}
Vector3 RBX::Primitive::getPartPosition() {
	return comms->read<Vector3>(address + Offsets::PartPosition);
}