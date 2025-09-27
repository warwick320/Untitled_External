#pragma once
#include <array>
#include <iostream>
#include <cmath>

struct Matrix3 final
{
	f32 data[9];
};

struct Matrix4 final
{
	f32 data[16];
};


struct CMatrix final
{
    std::array<float, 9> data;

    CMatrix() : data{1,0,0, 0,1,0, 0,0,1} {}

    CMatrix(float m0, float m1, float m2,
            float m3, float m4, float m5,
            float m6, float m7, float m8)
        : data{m0, m1, m2, m3, m4, m5, m6, m7, m8} {}

    CMatrix(const Matrix3& m) {
        for (int i = 0; i < 9; ++i) {
            data[i] = m.data[i];
        }
    }
    

    Matrix3 toMatrix3() const {
        Matrix3 result;
        for (int i = 0; i < 9; ++i) {
            result.data[i] = data[i];
        }
        return result;
    }
    
    // ¿ä¼Ò Á¢±Ù
    float& operator[](int index) { return data[index]; }
    const float& operator[](int index) const { return data[index]; }
    
    // Çà·Ä °ö¼À
    CMatrix operator*(const CMatrix& other) const {
        CMatrix result;
        for (int row = 0; row < 3; ++row) {
            for (int col = 0; col < 3; ++col) {
                float sum = 0;
                for (int k = 0; k < 3; ++k) {
                    sum += data[row * 3 + k] * other.data[k * 3 + col];
                }
                result.data[row * 3 + col] = sum;
            }
        }
        return result;
    }
    
    // º¤ÅÍ¿Í Çà·ÄÀÇ °ö¼À (º¤ÅÍ¸¦ ¿­º¤ÅÍ·Î Ãë±Þ)
    CVector operator*(const CVector& vec) const {
        return CVector(
            data[0] * vec.x + data[1] * vec.y + data[2] * vec.z,
            data[3] * vec.x + data[4] * vec.y + data[5] * vec.z,
            data[6] * vec.x + data[7] * vec.y + data[8] * vec.z
        );
    }
    
    // ÀüÄ¡ Çà·Ä
    CMatrix transpose() const {
        return CMatrix(
            data[0], data[3], data[6],
            data[1], data[4], data[7],
            data[2], data[5], data[8]
        );
    }
};

// ½ºÆ®¸² ¿¬»êÀÚ
inline std::ostream& operator<<(std::ostream& os, const CMatrix& m) {
    os << "[";
    for (int i = 0; i < 9; ++i) {
        os << m.data[i];
        if (i != 8) os << ", ";
    }
    os << "]";
    return os;
}