#pragma once
namespace Offsets {

    inline u64 Childern = 0x68;
    inline u64 Name = 0x88;
    inline u64 ClassDescriptor = 0x18;

    inline u64 Dimensions = 0x720;
    inline u64 ViewMatrix = 0x4B0;

    inline u64 FakeDataModel = 0x7168648;
    inline u64 RealDataModel = 0x1C0; // FDMTDM
    inline u64 VisualEngine = 0x6EC49B0;

    inline u64 LocalPlayer = 0x128;
    inline u64 ModelInstance = 0x348;

    inline u64 Primitive = 0x178;

    inline u64 PartSize = 0x23C;
    inline u64 PartPosition = 0x154;
    inline u64 Team = 0x258;
    inline u64 CanCollide = 0x2E5;
    inline u64 CFrame = 0x130;
    inline u64 speedAddress = 0x1DC;
    inline u64 WalkSpeedCheck = 0x3B8;
    inline u64 JumpPowerAddress = 0x1B8;
}
