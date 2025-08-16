#include <SDK/Classes/VisualEngine/VisualEngine.h>
#include <SDK/Offset.h>
Vector2 RBX::VisualEngine::getDimensions() {
	return comms->read<Vector2>(address + Offsets::Dimensions);

}

Matrix4 RBX::VisualEngine::getViewMatrix() {
	return comms->read<Matrix4>(address + Offsets::ViewMatrix);
}
bool RBX::VisualEngine::isOnScreen(const Vector3& world) {
	Vector2 dimensions = getDimensions();
	Matrix4 viewMatrix = getViewMatrix();

	Vector4 q;
	q.x = (world.x * viewMatrix.data[0]) + (world.y * viewMatrix.data[1]) + (world.z * viewMatrix.data[2]) + viewMatrix.data[3];
	q.y = (world.x * viewMatrix.data[4]) + (world.y * viewMatrix.data[5]) + (world.z * viewMatrix.data[6]) + viewMatrix.data[7];
	q.z = (world.x * viewMatrix.data[8]) + (world.y * viewMatrix.data[9]) + (world.z * viewMatrix.data[10]) + viewMatrix.data[11];
	q.w = (world.x * viewMatrix.data[12]) + (world.y * viewMatrix.data[13]) + (world.z * viewMatrix.data[14]) + viewMatrix.data[15];

	if (q.w < 0.1f) return false; // 뒤쪽에 있는 객체

	Vector3 ndc;
	ndc.x = q.x / q.w;
	ndc.y = q.y / q.w;
	ndc.z = q.z / q.w;

	// NDC 기준으로 화면 내 체크 (-1~1 범위)
	if (ndc.x < -1.0f || ndc.x > 1.0f) return false;
	if (ndc.y < -1.0f || ndc.y > 1.0f) return false;
	if (ndc.z < 0.0f || ndc.z > 1.0f) return false;

	return true;
}

Vector2 RBX::VisualEngine::worldToScreen(const Vector3& world) {
	Vector4 quaternion;

	Vector2 dimensions = getDimensions();
	Matrix4 viewMatrix = getViewMatrix();

	
	quaternion.x = (world.x * viewMatrix.data[0]) + (world.y * viewMatrix.data[1]) + (world.z * viewMatrix.data[2])+ viewMatrix.data[3];
	quaternion.y = (world.x * viewMatrix.data[4]) + (world.y * viewMatrix.data[5]) + (world.z * viewMatrix.data[6]) + viewMatrix.data[7];
	quaternion.z = (world.x * viewMatrix.data[8]) + (world.y * viewMatrix.data[9]) + (world.z * viewMatrix.data[10]) + viewMatrix.data[11];
	quaternion.w = (world.x * viewMatrix.data[12]) + (world.y * viewMatrix.data[13]) + (world.z * viewMatrix.data[14]) + viewMatrix.data[15];

	Vector2 screen;

	if (quaternion.w < 0.1f) {
		return Vector2();
	}
	Vector3 normalizedDeviceCoordinates;
	normalizedDeviceCoordinates.x = quaternion.x / quaternion.w;
	normalizedDeviceCoordinates.y = quaternion.y / quaternion.w;
	normalizedDeviceCoordinates.z = quaternion.z / quaternion.w;

	screen.x = (dimensions.x / 2.0f * normalizedDeviceCoordinates.x) + (dimensions.x / 2.0f);
	screen.y = -(dimensions.y / 2.0f * normalizedDeviceCoordinates.y) + (dimensions.y / 2.0f);

	return screen;
}