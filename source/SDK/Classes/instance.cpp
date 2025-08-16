#include <SDK/Classes/instance.h>
#include <SDK/Offset.h>
#include <SDK/Classes/world/Primtive.h>
str RBX::Instance::getName() {
	u64 name = comms->read<u64>(address + Offsets::Name);
	
	if (name) {
		return comms->readstr(name);
	}

	return "Unknown";
}

u64 RBX::Instance::getTeam() {
    u64 teamPtr = comms->read<uintptr_t>(address + Offsets::Team);
    if (teamPtr) {

		return teamPtr;
    }
    return 0;
}
str RBX::Instance::getTeamName() {
	u64 teamPtr = getTeam();
	if (teamPtr) {
		u64 namePtr = comms->read<u64>(teamPtr + Offsets::Name);
		if (namePtr) {
			return comms->readstr(namePtr);
		}
	}
	return "No Team";
}
str RBX::Instance::getClassName() {
	u64 name = comms->read<u64>(address + Offsets::ClassDescriptor);
	u64 size = comms->read<u64>(name + 0x8);

	if (size) {
		return comms->readstr(size);
	}

	return "Unknown";
}


std::vector<RBX::Instance> RBX::Instance::getChildern() {
	std::vector<RBX::Instance> children;
	u64 start = comms->read<u64>(address + Offsets::Childern);
	u64 end = comms->read<u64>(start + 0x8);

	for (u64 child = comms->read<u64>(start); child != end; child += 0x10) {
		children.push_back(RBX::Instance(comms->read<u64>(child)));

	}
	return children;
}

RBX::Instance RBX::Instance::findFirstChild(const str& name) {
	std::vector<RBX::Instance> children = getChildern();

	for (RBX::Instance& child : children) {
		if (child.getName() == name) {
			return child;
		}
	}
	return RBX::Instance();
}

RBX::Instance RBX::Instance::findFristChildByClass(const str& name) {
	std::vector<RBX::Instance> children = getChildern();

	for (RBX::Instance& child : children) {
		if (child.getClassName() == name) {
			return child;
		}
	}
	return RBX::Instance();
}
RBX::Primitive RBX::Instance::getPrimitive() {
	return RBX::Primitive(comms->read<u64>(address + Offsets::Primitive));
}
u64 RBX::Instance::getAddress() {
	return address;
}