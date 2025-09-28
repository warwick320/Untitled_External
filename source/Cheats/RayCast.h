#pragma once
#define NOMINMAX // Windows.h�� min/max ��ũ�� ����
#include <vector>
#include <limits>
#include <SDK/Classes/Math/Vector.h>
#include <SDK/Classes/Math/CFrame.h>
#include <algorithm> // for std::swap

namespace Roblox {
    struct PartInfo {
        CVector position;
        CVector size;
        CFrame cframe;
        std::string className;
        float distanceFromOrigin;

        PartInfo() = default;
        PartInfo(const Vector3& pos, const Vector3& sz, const CFrame& cf, const std::string& name)
            : position(CVector(pos)), size(CVector(sz)), cframe(cf), className(name), distanceFromOrigin(0.0f) {
        }
    };

    struct RaycastResult {
        bool hit = false;
        float distance = std::numeric_limits<float>::max();
        CVector hitPosition;
        PartInfo hitPart;
        std::vector<PartInfo> partsUpToHit; // ��Ʈ ���������� ��Ʈ�鸸 ����

        Vector3 getHitPositionAsVector3() const {
            return hitPosition.toVector3();
        }
    };

    namespace Ray {
        inline RaycastResult cast_ray_with_rotation_optimized(const CVector& origin, const CVector& direction, float max_dist, std::vector<PartInfo> parts) {
            RaycastResult result;
            CVector dir_normalized = direction.normalized();

            // ��Ʈ���� �������κ����� �Ÿ��� ����
            for (auto& part : parts) {
                part.distanceFromOrigin = (part.position - origin).magnitude();
            }

            std::sort(parts.begin(), parts.end(), [](const PartInfo& a, const PartInfo& b) {
                return a.distanceFromOrigin < b.distanceFromOrigin;
                });

            for (const auto& part : parts) {
                // �̹� ��Ʈ�� �Ÿ����� �ָ� �ִ� ��Ʈ���� �ǳʶٱ�
                if (result.hit && part.distanceFromOrigin > result.distance + 1.0f) {
                    break; // ���� �ߴ� -> �� �̻� �˻��� �ʿ� ����
                }

                const CVector& part_pos = part.position;
                const CVector& part_size = part.size;
                const CFrame& part_cframe = part.cframe;

                // ���� ��ǥ��� ��ȯ
                CVector local_origin = part_cframe.pointToObjectSpace(origin);
                CVector local_dir = part_cframe.vectorToObjectSpace(dir_normalized);

                // OBB ��� (�߽��� �������� �ϴ� ���� ��ǥ��)
                CVector min_bounds = part_size * -0.5f;
                CVector max_bounds = part_size * 0.5f;

                // ���� �׽�Ʈ (Slab Test) for OBB
                float tmin = -std::numeric_limits<float>::infinity();
                float tmax = std::numeric_limits<float>::infinity();
                bool intersects = true;

                // X�� ����
                if (std::abs(local_dir.x) < 1e-8f) {
                    if (local_origin.x < min_bounds.x || local_origin.x > max_bounds.x) {
                        intersects = false;
                    }
                }
                else {
                    float inv_dir_x = 1.0f / local_dir.x;
                    float t1 = (min_bounds.x - local_origin.x) * inv_dir_x;
                    float t2 = (max_bounds.x - local_origin.x) * inv_dir_x;

                    if (t1 > t2) std::swap(t1, t2);

                    tmin = std::max(tmin, t1);
                    tmax = std::min(tmax, t2);

                    if (tmin > tmax) intersects = false;
                }

                if (!intersects) continue;

                // Y�� ����
                if (std::abs(local_dir.y) < 1e-8f) {
                    if (local_origin.y < min_bounds.y || local_origin.y > max_bounds.y) {
                        intersects = false;
                    }
                }
                else {
                    float inv_dir_y = 1.0f / local_dir.y;
                    float t1 = (min_bounds.y - local_origin.y) * inv_dir_y;
                    float t2 = (max_bounds.y - local_origin.y) * inv_dir_y;

                    if (t1 > t2) std::swap(t1, t2);

                    tmin = std::max(tmin, t1);
                    tmax = std::min(tmax, t2);

                    if (tmin > tmax) intersects = false;
                }

                if (!intersects) continue;

                // Z�� ����
                if (std::abs(local_dir.z) < 1e-8f) {
                    if (local_origin.z < min_bounds.z || local_origin.z > max_bounds.z) {
                        intersects = false;
                    }
                }
                else {
                    float inv_dir_z = 1.0f / local_dir.z;
                    float t1 = (min_bounds.z - local_origin.z) * inv_dir_z;
                    float t2 = (max_bounds.z - local_origin.z) * inv_dir_z;

                    if (t1 > t2) std::swap(t1, t2);

                    tmin = std::max(tmin, t1);
                    tmax = std::min(tmax, t2);

                    if (tmin > tmax) intersects = false;
                }

                if (!intersects) continue;

                // ��ȿ�� ������ Ȯ�� �� ���� ��Ʈ ó��
                if (tmin >= 0 && tmin <= max_dist) {
                    // ù ��° ��Ʈ�̰ų� �� ����� ��Ʈ�� ���
                    if (!result.hit || tmin < result.distance) {
                        result.hit = true;
                        result.distance = tmin;
                        result.hitPosition = origin + dir_normalized * tmin;
                        result.hitPart = part;

                        // ��Ʈ ���������� ��Ʈ�鸸 ����
                        result.partsUpToHit.clear();
                        for (const auto& p : parts) {
                            if (p.distanceFromOrigin <= result.distance + 1.0f) { // �ణ�� ������
                                result.partsUpToHit.push_back(p);
                            }
                            else {
                                break; // ���ĵǾ� �����Ƿ� ���⼭ �ߴ�
                            }
                        }

                        if (tmin < 1.0f) {
                            return result;
                        }
                    }
                }
            }
            return result;
        }

        inline RaycastResult cast_ray_with_rotation(const CVector& origin, const CVector& direction, float max_dist, const std::vector<PartInfo>& parts) {
            std::vector<PartInfo> mutable_parts = parts;
            return cast_ray_with_rotation_optimized(origin, direction, max_dist, std::move(mutable_parts));
        }

        inline RaycastResult cast_ray_with_rotation(const Vector3& origin, const Vector3& direction, float max_dist, const std::vector<PartInfo>& parts) {
            return cast_ray_with_rotation(CVector(origin), CVector(direction), max_dist, parts);
        }
    }
}