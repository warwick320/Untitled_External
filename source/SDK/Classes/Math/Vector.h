#pragma once
#include <cmath>
#include <iostream>
#include <array>

struct Vector2 final
{
	f32 x{ 0 }, y{ 0 };
};

struct Vector3 final
{
    float x{ 0 }, y{ 0 }, z{ 0 };

    Vector3 operator+(const Vector3& other) const { return { x + other.x, y + other.y, z + other.z };}
    Vector3 operator-(const Vector3& other) const { return { x - other.x, y - other.y, z - other.z };}
    Vector3 operator*(f32 scalar) const { return { x * scalar, y * scalar, z * scalar }; }
    Vector3 operator/(f32 scalar) const { return { x / scalar, y / scalar, z / scalar };}
	f32 mgnitude() const { return std::sqrt(x * x + y * y + z * z); }
    Vector3 normalized() const { f32 m = mgnitude(); return { x / m , y / m , z / m }; }
	f32 dot(const Vector3& o) const { return x * o.x + y * o.y + z * o.z; }
};

struct Vector4 final
{
	f32 x{ 0 }, y{ 0 }, z{ 0 }, w{0};
};


struct CVector final
{
    float x, y, z;


    CVector() : x(0), y(0), z(0) {}
    

    CVector(float x, float y, float z) : x(x), y(y), z(z) {}
    

    CVector(const Vector3& v) : x(v.x), y(v.y), z(v.z) {}

    Vector3 toVector3() const { return {x, y, z}; }
    

    CVector operator+(const CVector& other) const { return {x + other.x, y + other.y, z + other.z}; }
    CVector operator-(const CVector& other) const { return {x - other.x, y - other.y, z - other.z}; }
    CVector operator*(float scalar) const { return {x * scalar, y * scalar, z * scalar}; }
    CVector operator/(float scalar) const { return {x / scalar, y / scalar, z / scalar}; }
    

    float magnitude() const { return std::sqrt(x * x + y * y + z * z); }
    CVector normalized() const { 
        float m = magnitude(); 
        return (m > 0) ? CVector(x / m, y / m, z / m) : CVector(); 
    }
    float dot(const CVector& o) const { return x * o.x + y * o.y + z * o.z; }
    CVector cross(const CVector& o) const {
        return CVector(
            y * o.z - z * o.y,
            z * o.x - x * o.z,
            x * o.y - y * o.x
        );
    }
};

// 스트림 연산자
inline std::ostream& operator<<(std::ostream& os, const CVector& v) {
    os << "[" << v.x << ", " << v.y << ", " << v.z << "]";
    return os;
}
