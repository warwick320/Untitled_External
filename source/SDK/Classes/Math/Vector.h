
#pragma once
#include <cmath>
struct Vector2 final
{
	f32 x{ 0 }, y{ 0 };
};
struct Vector3 final
{
    float x{ 0 }, y{ 0 }, z{ 0 };

    Vector3 operator+(const Vector3& other) const {
        return { x + other.x, y + other.y, z + other.z };
    }

    Vector3 operator-(const Vector3& other) const {
        return { x - other.x, y - other.y, z - other.z };
    }

    Vector3 operator*(float scalar) const {
        return { x * scalar, y * scalar, z * scalar };
    }

    Vector3 operator/(float scalar) const {
        return { x / scalar, y / scalar, z / scalar };
    }
};
struct Vector4 final
{
	f32 x{ 0 }, y{ 0 }, z{ 0 }, w{0};
};
