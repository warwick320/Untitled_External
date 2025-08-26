#pragma once

namespace Offsets {

	inline constexpr u64 Childern = 0x68;
	inline constexpr u64 Name = 0x88;
	inline constexpr u64 ClassDescriptor = 0x18;

	inline constexpr u64 Dimensions = 0x720;
	inline constexpr u64 ViewMatrix = 0x4B0;

	inline constexpr u64 FakeDataModel = 0x7022FA8;
	inline constexpr u64 RealDataModel = 0x1C0; // FDMTDM
	inline constexpr u64 VisualEngine = 0x6D7CDA0;

	inline constexpr u64 LocalPlayer = 0x128;
	inline constexpr u64 ModelInstance = 0x328;

	inline constexpr u64 Primitive = 0x178;

	inline constexpr u64 PartSize = 0x254;
	inline constexpr u64 PartPosition = 0x14C;
	inline constexpr u64 Team = 0x248;
	inline constexpr u64 CanCollide = 0x303;
}
