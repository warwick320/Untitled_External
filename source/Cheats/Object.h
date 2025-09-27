#pragma once
#include <Cheats/Players.h>
#include <SDK/Classes/World/Primtive.h>
#include <SDK/Classes/Math/CFrame.h> 
#include <Cheats/RayCast.h>  
#include <chrono>
#include <map>
#include <set>
#include <mutex>

std::vector<Roblox::PartInfo> partInfoList;
std::mutex partInfoMutex;

auto zorkspace() {
	RBX::Instance workspace = dataModel->findFristChildByClass("Workspace");
	return workspace;
}


bool isPlayerModel(const RBX::Instance& instance) {
	for (RBX::Player& player : g_PlayerList) {
		RBX::ModelInstance playerModel = player.getModelInstance();
		if (playerModel.getAddress() == instance.getAddress()) {
			return true;
		}
	}
	return false;
}

void WorkspaceObjPos() {
	while (true)
	{
		std::vector<Roblox::PartInfo> tempPartList;
		std::function<void(RBX::Instance&)> addPartsRecursively;
		addPartsRecursively = [&](RBX::Instance& instance) {
			if (!instance.getAddress()) return;

			try {
				std::string className = instance.getClassName();
				if (className.empty()) return;


				if (className == "Model" && isPlayerModel(instance)) {
					return; 
				}

				if (className == "Script" || className == "LocalScript" ||
					className == "RemoteEvent" || className == "RemoteFunction" ||
					className == "BindableEvent" || className == "BindableFunction" ||
					className == "StringValue" || className == "IntValue" ||
					className == "NumberValue" || className == "BoolValue" ||
					className == "ObjectValue" || className == "Vector3Value" ||
					className == "CFrameValue" || className == "Color3Value" ||
					className == "BrickColorValue" || className == "RayValue" ||
					className == "Camera" || className == "Lighting" ||
					className == "Players" || className == "ReplicatedStorage" ||
					className == "StarterGui" || className == "StarterPack" ||
					className == "StarterPlayer" || className == "SoundService" ||
					className == "TweenService" || className == "UserInputService" ||
					className == "Sound" || className == "SurfaceGui" ||
					className == "BillboardGui" || className == "ScreenGui" ||
					className == "TextLabel" || className == "TextButton" ||
					className == "ImageLabel" || className == "ImageButton" ||
					className == "Frame" || className == "ScrollingFrame") {
				}
				else {
					try {
						RBX::Primitive primitive = instance.getPrimitive();
						if (primitive.getAddress()) {
							Vector3 position, size;  
							CFrame cframe;

							try {
								position = primitive.getPartPosition();
								size = primitive.getPartSize();
								cframe = primitive.getPartCFrame();

								if (size.x > 0 && size.y > 0 && size.z > 0 &&
									size.x < 10000 && size.y < 10000 && size.z < 10000) {

									bool canCollide = true;
									try {
										canCollide = instance.getCanCollide();
									}
									catch (...) {
									}

									Roblox::PartInfo partInfo;
									partInfo.position = CVector(position);  
									partInfo.size = CVector(size);          
									partInfo.cframe = cframe;
									partInfo.className = className;
									tempPartList.push_back(partInfo);
								}
							}
							catch (...) {
							}
						}
					}
					catch (...) {
					}
				}

				try {
					std::vector<RBX::Instance> children = instance.getChildren();
					for (RBX::Instance& child : children) {
						if (child.getAddress()) {
							addPartsRecursively(child);
						}
					}
				}
				catch (...) {
				}
			}
			catch (...) {
				return;
			}
			};

		try {
			RBX::Instance workspace = zorkspace();
			if (workspace.getAddress()) {
				std::vector<RBX::Instance> workspaceChildren = workspace.getChildren();
				for (RBX::Instance& instan : workspaceChildren) {
					if (instan.getAddress()) {
						addPartsRecursively(instan);
					}
				}
			}
		}
		catch (...) {
			return;
		}

		{
			std::lock_guard<std::mutex> lock(partInfoMutex);
			partInfoList = std::move(tempPartList);
		}

		sleep_ms(1);
	}
}


class ObstacleCache {
private:
	static std::vector<Roblox::PartInfo> cachedObstacles;
	static std::chrono::steady_clock::time_point lastUpdate;
	static constexpr std::chrono::milliseconds CACHE_DURATION{ 500 };

public:
	static const std::vector<Roblox::PartInfo>& getObstacles() {
		auto now = std::chrono::steady_clock::now();

		if (cachedObstacles.empty() ||
			(now - lastUpdate) > CACHE_DURATION) {

				{
					std::lock_guard<std::mutex> lock(partInfoMutex);
					cachedObstacles = partInfoList;
				}
				lastUpdate = now;
		}

		return cachedObstacles;
	}

	static void forceUpdate() {
		cachedObstacles.clear();
	}
};

std::vector<Roblox::PartInfo> ObstacleCache::cachedObstacles;
std::chrono::steady_clock::time_point ObstacleCache::lastUpdate;

constexpr float EPSILON = 0.001f;

bool isPointInsideAABB(const CVector& point, const CVector& boxCenter, const CVector& boxSize, float margin = 0.1f) {
	CVector boxMin = boxCenter - (boxSize * 0.5f) - CVector(margin, margin, margin);
	CVector boxMax = boxCenter + (boxSize * 0.5f) + CVector(margin, margin, margin);

	return (point.x >= boxMin.x - EPSILON && point.x <= boxMax.x + EPSILON &&
		point.y >= boxMin.y - EPSILON && point.y <= boxMax.y + EPSILON &&
		point.z >= boxMin.z - EPSILON && point.z <= boxMax.z + EPSILON);
}

std::vector<Roblox::PartInfo> getValidObstaclesForRaycast(const CVector& origin, const CVector& target) {
	std::vector<Roblox::PartInfo> validParts;
	const auto& obstacles = ObstacleCache::getObstacles();

	CVector rayDirection = (target - origin).normalized();
	float rayDistance = (target - origin).magnitude();

	for (const Roblox::PartInfo& partInfo : obstacles) {
		// OBB 기반 점 검사 (회전 고려)
		CVector localOrigin = partInfo.cframe.pointToObjectSpace(origin);
		CVector halfSize = partInfo.size * 0.5f;
		if (abs(localOrigin.x) <= halfSize.x + 0.5f &&
			abs(localOrigin.y) <= halfSize.y + 0.5f &&
			abs(localOrigin.z) <= halfSize.z + 0.5f) {
			continue;
		}

		CVector localTarget = partInfo.cframe.pointToObjectSpace(target);
		if (abs(localTarget.x) <= halfSize.x + 0.5f &&
			abs(localTarget.y) <= halfSize.y + 0.5f &&
			abs(localTarget.z) <= halfSize.z + 0.5f) {
			continue;
		}

		if (partInfo.size.x < 0.1f && partInfo.size.y < 0.1f && partInfo.size.z < 0.1f) {
			continue;
		}

		CVector toPart = partInfo.position - origin;
		float projectionLength = toPart.dot(rayDirection);

		if (projectionLength < -200.0f || projectionLength > rayDistance + 200.0f) {
			continue;
		}

		CVector closestPointOnRay;
		if (projectionLength < 0) {
			closestPointOnRay = origin;
		}
		else if (projectionLength > rayDistance) {
			closestPointOnRay = target;
		}
		else {
			closestPointOnRay = origin + rayDirection * projectionLength;
		}

		float distanceToBox = (closestPointOnRay - partInfo.position).magnitude();
		float maxBoxDimension = (partInfo.size.x > partInfo.size.y) ?
			((partInfo.size.x > partInfo.size.z) ? partInfo.size.x : partInfo.size.z) :
			((partInfo.size.y > partInfo.size.z) ? partInfo.size.y : partInfo.size.z);

		float filterRange = maxBoxDimension + 300.0f;

		if (distanceToBox < filterRange) {
			validParts.push_back(partInfo);
		}
	}

	return validParts;
}

std::vector<Roblox::PartInfo> getRelevantObstaclesWithSize(const CVector& origin, const CVector& target) {
	return getValidObstaclesForRaycast(origin, target);
}

std::vector<Roblox::PartInfo> getRelevantObstaclesWithSize(const Vector3& origin, const Vector3& target) {
	return getValidObstaclesForRaycast(CVector(origin), CVector(target));
}