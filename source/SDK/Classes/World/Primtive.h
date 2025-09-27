#pragma once
#include <SDK/Classes/instance.h>
#include <SDK/Classes/Math/Vector.h>
#include <SDK/Classes/Math/CFrame.h>
namespace RBX {
	class Primitive : public Instance {
	public:
		Primitive(u64 address) : Instance(address) {};

		Vector3 getPartPosition();
		Vector3 getPartSize();
		CFrame getPartCFrame();
	};
}