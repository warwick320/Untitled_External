#pragma once

namespace RBX {
	class Primitive;

	class Instance {
	protected:
		u64 address;
	public:

		Instance() = default;
		Instance(u64 address) : address(address){}

		str getName();
		str getClassName();

		u64 getTeam();
		str getTeamName();
		std::vector<RBX::Instance> getChildren();
		RBX::Instance findFirstChild(const str& name);
		RBX::Instance findFristChildByClass(const str& className);
		str getVaue();
		bool isA(const std::string& className);
		bool getCanCollide();
		RBX::Primitive getPrimitive();
		u64 getAddress() const;
	};
}
