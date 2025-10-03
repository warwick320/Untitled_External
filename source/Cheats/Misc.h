#pragma once

auto Workspace() {
	RBX::Instance workspace = dataModel->findFristChildByClass("Workspace");
	return workspace;
}
void Fly() {
	u64 workspaceaddress = Workspace().getAddress();
	f32 fly = 0.0f;
	RBX::Instance Character = Workspace().findFirstChild(g_LocalPlayer.getName());
	RBX::Instance HumanoidRootPart = Character.findFirstChild("HumanoidRootPart");
	u64 HumanoidRootPartaddr = HumanoidRootPart.getAddress();
	RBX::Primitive Primitive = HumanoidRootPart.getPrimitive();
	while (true) {
		Vector3 Position = Primitive.getPartPosition();
		Vector3 Zero = { 0.0f, 0.0f, 0.0f };
		printf("Current Position: X: %f, Y: %f, Z: %f\n", Position.x, Position.y, Position.z);
		comms->write<Vector3>(HumanoidRootPartaddr + Offsets::PartPosition, Zero);
	}
}
void speedExcute() {

	RBX::Instance Character = Workspace().findFirstChild(g_LocalPlayer.getName());
	RBX::Instance Humanoid = Character.findFirstChild("Humanoid");

	comms->write<f32>(Humanoid.getAddress() + Offsets::speedAddress, targetSpeed);
	comms->write<f32>(Humanoid.getAddress() + Offsets::WalkSpeedCheck, targetSpeed);
	f32 speed = comms->read<f32>(Humanoid.getAddress() + Offsets::speedAddress);
	f32 speedCheck = comms->read<f32>(Humanoid.getAddress() + Offsets::WalkSpeedCheck);
}
void jumpExcute() {

	RBX::Instance Character = Workspace().findFirstChild(g_LocalPlayer.getName());
	RBX::Instance Humanoid = Character.findFirstChild("Humanoid");
	comms->write<f32>(Humanoid.getAddress() + Offsets::JumpPowerAddress, jumpPower);
	f32 jumpPower = comms->read<f32>(Humanoid.getAddress() + Offsets::JumpPowerAddress);
}

void miscLoop() {
	static f32 prevTargetSpeed = 16.0f;
	static f32 prevJumpPower = 50.0f;
	static bool speedChanged = false;
	static bool jumpChanged = false;

	while (true) {

		if (targetSpeed != prevTargetSpeed) {
			printf("TargetSpeed changed: %f -> %f\n", prevTargetSpeed, targetSpeed);
			prevTargetSpeed = targetSpeed;
			speedChanged = true;
		}

		if (jumpPower != prevJumpPower) {
			printf("JumpPower changed: %f -> %f\n", prevJumpPower, jumpPower);
			prevJumpPower = jumpPower;
			jumpChanged = true;
		}

		if (speedChanged) {
			speedExcute();
		}

		if (jumpChanged) {
			jumpExcute();
		}
		sleep_ms(100);
	}
}
