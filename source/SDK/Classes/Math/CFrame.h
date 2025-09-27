#pragma once
#include "Vector.h"
#include "matrix.h"
#include <iostream>

// ---------------- CFrame ----------------
struct CFrame final
{
    CVector Position;
    CMatrix Rotation;

    CFrame() : Position(), Rotation() {}
 
    CFrame(const CVector& pos) : Position(pos), Rotation() {}
    
    CFrame(const CVector& pos, const CMatrix& rot) : Position(pos), Rotation(rot) {}
    CFrame(const Vector3& pos) : Position(CVector(pos)), Rotation() {}
    CFrame(const Vector3& pos, const Matrix3& rot) : Position(CVector(pos)), Rotation(CMatrix(rot)) {}
    

    CFrame operator*(const CFrame& other) const {
        return CFrame(
            Position + Rotation * other.Position,
            Rotation * other.Rotation
        );
    }
    

    CFrame inverse() const {
        CMatrix rotT = Rotation.transpose();
        return CFrame(rotT * (CVector(0, 0, 0) - Position), rotT);
    }

    CVector pointToWorldSpace(const CVector& localPoint) const {
        return Position + Rotation * localPoint;
    }
    

    CVector pointToObjectSpace(const CVector& worldPoint) const {
        return Rotation.transpose() * (worldPoint - Position);
    }
    

    CVector vectorToWorldSpace(const CVector& localVector) const {
        return Rotation * localVector;
    }

    CVector vectorToObjectSpace(const CVector& worldVector) const {
        return Rotation.transpose() * worldVector;
    }

    Vector3 getPosition() const { return Position.toVector3(); }
    Matrix3 getRotation() const { return Rotation.toMatrix3(); }
    
    void setPosition(const Vector3& pos) { Position = CVector(pos); }
    void setRotation(const Matrix3& rot) { Rotation = CMatrix(rot); }
};

// 스트림 연산자
inline std::ostream& operator<<(std::ostream& os, const CFrame& cf) {
    os << "CFrame(Position: " << cf.Position << ", Rotation: " << cf.Rotation << ")";
    return os;
}