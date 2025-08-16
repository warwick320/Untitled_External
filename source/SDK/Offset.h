#pragma once

namespace Offsets {
	//inline constexpr u64 JobsPointer = 0x6F55900; // Same
	//inline constexpr u64 JobName = 0x18; // Job_Name
	//
	//inline constexpr u64 RenderViewPtr = 0x218; //  RenderJobToRenderView
	//inline constexpr u64 VisualEnginePtr = 0x10; // may be orginal name VisualEngine

	//inline constexpr u64 DataModelPtr = 0x208; // I dont know what is this orgin Name
	//inline constexpr u64 DataModelOffset = 0x1B0; // RenderJobToDataModel

	inline constexpr u64 Childern = 0x68; // Same
	inline constexpr u64 Name = 0x88; // same
	inline constexpr u64 ClassDescriptor = 0x18; // same

	inline constexpr u64 Dimensions = 0x720;
	inline constexpr u64 ViewMatrix = 0x4B0;

	inline constexpr u64 FakeDataModel = 0x6E72138;
	inline constexpr u64 RealDataModel = 0x1C0; // FDMTDM
	inline constexpr u64 VisualEngine = 0x6BFD5C0;

	inline constexpr u64 LocalPlayer = 0x128;
	inline constexpr u64 ModelInstance = 0x328;

	inline constexpr u64 Primitive = 0x178;

	inline constexpr u64 PartSize = 0x254;
	inline constexpr u64 PartPosition = 0x14C;
	inline constexpr u64 Team = 0x248;
	inline constexpr u64 CanCollide = 0x303;
}