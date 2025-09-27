#include "Primtive.h"
#include <SDK/Offset.h>
#include <SDK/Classes/Math/CFrame.h>

Vector3 RBX::Primitive::getPartPosition() {
	return comms->read<Vector3>(address + Offsets::PartPosition);
}

Vector3 RBX::Primitive::getPartSize() {
	return comms->read<Vector3>(address + Offsets::PartSize);
}

CFrame RBX::Primitive::getPartCFrame() {
    struct CFrameData {
        Matrix3 rotation; 
        Vector3 position;
    };

    CFrameData data = comms->read<CFrameData>(address + Offsets::CFrame);

    return CFrame(data.position, data.rotation);
}