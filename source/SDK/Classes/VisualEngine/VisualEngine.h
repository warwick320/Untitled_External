#pragma once
#include <SDK/Classes/Math/Vector.h>
#include <SDK/Classes/Math/matrix.h> 

namespace RBX {
	class VisualEngine : public Instance {
	private:
		Vector2 getDimensions();
		
	public:
		Matrix4 getViewMatrix();
		bool isOnScreen(const Vector3& world);
		Vector2 worldToScreen(const Vector3& world);
	};
}