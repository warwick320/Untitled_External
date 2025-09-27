#include "SharedMemoryController.h"
#include <QDebug>

SharedMemoryController::SharedMemoryController(QObject* parent)
    : QObject(parent),
    hMapFile(nullptr),
    pSharedData(nullptr),
    m_aimbotEnabled(false),
    m_espEnabled(false),
    m_espShowNames(false),
    m_espShowBox(false),
    m_espShowBones(false),
    m_espShowDistance(false),
    m_espShowTracer(false),
    m_espCharms(false),
    m_fovSize(150.0f),
	m_triggerbot(false),
	m_targetSpeed(16.0f),
	m_jumpPower(50.0f),
    m_isConnected(false),
    m_lastUpdateTime(0)

{
    // 공유 메모리 연결
    connectToSharedMemory();

    // 동기화 타이머 (60fps)
    m_syncTimer = new QTimer(this);
    connect(m_syncTimer, &QTimer::timeout, this, &SharedMemoryController::syncWithSharedMemory);
    m_syncTimer->start(16);

    // 연결 상태 확인 타이머 (1초마다)
    m_connectionTimer = new QTimer(this);
    connect(m_connectionTimer, &QTimer::timeout, this, &SharedMemoryController::checkConnection);
    m_connectionTimer->start(1000);
}

SharedMemoryController::~SharedMemoryController()
{
    if (pSharedData) {
        UnmapViewOfFile(pSharedData);
    }
    if (hMapFile) {
        CloseHandle(hMapFile);
    }
}

bool SharedMemoryController::connectToSharedMemory()
{
    hMapFile = OpenFileMapping(
        FILE_MAP_ALL_ACCESS,
        FALSE,
        "RobloxExternalSharedMemory"
    );

    if (hMapFile == NULL) {
        qDebug() << "[ERROR] Could not open shared memory. Main process running?";
        return false;
    }

    pSharedData = (SharedData*)MapViewOfFile(
        hMapFile,
        FILE_MAP_ALL_ACCESS,
        0, 0,
        sizeof(SharedData)
    );

    if (pSharedData == NULL) {
        qDebug() << "[ERROR] Could not map view of shared memory";
        CloseHandle(hMapFile);
        hMapFile = nullptr;
        return false;
    }

    qDebug() << "[SUCCESS] Connected to shared memory";
    m_isConnected = true;
    emit isConnectedChanged();

    readFromSharedMemory();

    return true;
}

void SharedMemoryController::syncWithSharedMemory()
{
    if (!pSharedData) return;

    readFromSharedMemory();
}

void SharedMemoryController::readFromSharedMemory()
{
    if (!pSharedData) return;

    bool changed = false;

    // BYTE를 bool로 변환
    if (m_aimbotEnabled != (pSharedData->aimbot_enabled != 0)) {
        m_aimbotEnabled = (pSharedData->aimbot_enabled != 0);
        emit aimbotEnabledChanged();
        changed = true;
    }

    if (m_espEnabled != (pSharedData->esp_enabled != 0)) {
        m_espEnabled = (pSharedData->esp_enabled != 0);
        emit espEnabledChanged();
        changed = true;
    }

    if (m_espShowNames != (pSharedData->esp_show_names != 0)) {
        m_espShowNames = (pSharedData->esp_show_names != 0);
        emit espShowNamesChanged();
        changed = true;
    }

    if (m_espShowBox != (pSharedData->esp_show_box != 0)) {
        m_espShowBox = (pSharedData->esp_show_box != 0);
        emit espShowBoxChanged();
        changed = true;
    }

    if (m_espShowBones != (pSharedData->esp_show_bones != 0)) {
        m_espShowBones = (pSharedData->esp_show_bones != 0);
        emit espShowBonesChanged();
        changed = true;
    }

    if (m_espShowDistance != (pSharedData->esp_show_distance != 0)) {
        m_espShowDistance = (pSharedData->esp_show_distance != 0);
        emit espShowDistanceChanged();
        changed = true;
    }

    if (m_espShowTracer != (pSharedData->esp_show_tracer != 0)) {
        m_espShowTracer = (pSharedData->esp_show_tracer != 0);
        emit espShowTracerChanged();
        changed = true;
    }

    if (m_espCharms != (pSharedData->esp_chams != 0)) {
        m_espCharms = (pSharedData->esp_chams != 0);
        emit espCharmsChanged();
        changed = true;
    }

	if (m_triggerbot != (pSharedData->triggerbot != 0)) {
        m_triggerbot = (pSharedData->triggerbot != 0);
        emit triggerbotEnabledChanged();
		changed = true;
	}

    if (qAbs(m_fovSize - pSharedData->fov_size) > 0.001f) {
        m_fovSize = pSharedData->fov_size;
        emit fovSizeChanged();
        changed = true;
    }
    if (qAbs(m_targetSpeed - pSharedData->targetSpeed) > 0.001f) {
        m_targetSpeed = pSharedData->targetSpeed;
        emit targetSpeedChanged();
        changed = true;
	}
    if (qAbs(m_jumpPower - pSharedData->jumpPower) > 0.001f) {
        m_jumpPower = pSharedData->jumpPower;
        emit jumpPowerChanged();
        changed = true;
    }

    if (changed) {
        qDebug() << "[SYNC] Values updated from main process";
    }
}

void SharedMemoryController::writeToSharedMemory()
{
    if (!pSharedData) return;

    // bool을 BYTE로 변환
    pSharedData->aimbot_enabled = m_aimbotEnabled ? 1 : 0;
    pSharedData->esp_enabled = m_espEnabled ? 1 : 0;
    pSharedData->esp_show_names = m_espShowNames ? 1 : 0;
    pSharedData->esp_show_box = m_espShowBox ? 1 : 0;
    pSharedData->esp_show_bones = m_espShowBones ? 1 : 0;
    pSharedData->esp_show_distance = m_espShowDistance ? 1 : 0;
    pSharedData->esp_show_tracer = m_espShowTracer ? 1 : 0;
    pSharedData->triggerbot = m_triggerbot ? 1 : 0;
    pSharedData->esp_chams = m_espCharms ? 1 : 0;  // charms → chams 변경!
    
    pSharedData->fov_size = m_fovSize;
    pSharedData->targetSpeed = m_targetSpeed;
    pSharedData->jumpPower = m_jumpPower;
    
    // 디버깅용
    qDebug() << "[WRITE] Memory state - targetSpeed:" << pSharedData->targetSpeed 
             << "jumpPower:" << pSharedData->jumpPower;
}

void SharedMemoryController::checkConnection()
{
    if (!pSharedData) {
        if (m_isConnected) {
            m_isConnected = false;
            emit isConnectedChanged();
            qDebug() << "[WARNING] Lost connection to main process";
        }

        connectToSharedMemory();
        return;
    }

    ULONGLONG currentTime = GetTickCount64();
    if (currentTime - pSharedData->last_update > 5000) {
        if (m_isConnected) {
            m_isConnected = false;
            emit isConnectedChanged();
            qDebug() << "[WARNING] Main process seems to be not responding";
        }
    }
    else {
        if (!m_isConnected) {
            m_isConnected = true;
            emit isConnectedChanged();
            qDebug() << "[SUCCESS] Connection restored";
        }
    }
}

void SharedMemoryController::setAimbotEnabled(bool value)
{
    if (m_aimbotEnabled != value) {
        m_aimbotEnabled = value;
        writeToSharedMemory();
        emit aimbotEnabledChanged();
        qDebug() << "[UI] Aimbot:" << (value ? "ON" : "OFF");
    }
}

void SharedMemoryController::setEspEnabled(bool value)
{
    if (m_espEnabled != value) {
        m_espEnabled = value;
        writeToSharedMemory();
        emit espEnabledChanged();
        qDebug() << "[UI] ESP:" << (value ? "ON" : "OFF");
    }
}

void SharedMemoryController::setEspShowNames(bool value)
{
    if (m_espShowNames != value) {
        m_espShowNames = value;
        writeToSharedMemory();
        emit espShowNamesChanged();
        qDebug() << "[UI] ESP Names:" << (value ? "ON" : "OFF");
    }
}

void SharedMemoryController::setEspShowBox(bool value)
{
    if (m_espShowBox != value) {
        m_espShowBox = value;
        writeToSharedMemory();
        emit espShowBoxChanged();
        qDebug() << "[UI] ESP Box:" << (value ? "ON" : "OFF");
    }
}

void SharedMemoryController::setFovSize(float value)
{
    if (qAbs(m_fovSize - value) > 0.001f) {
        m_fovSize = value;
        writeToSharedMemory();
        emit fovSizeChanged();
        qDebug() << "[UI] FOV Size:" << value;
    }
}

void SharedMemoryController::setEspShowBones(bool value)
{
    if (m_espShowBones != value) {
        m_espShowBones = value;
        writeToSharedMemory();
        emit espShowBonesChanged();
        qDebug() << "[UI] ESP Bones:" << (value ? "ON" : "OFF");
    }
}

void SharedMemoryController::setEspShowDistance(bool value)
{
    if (m_espShowDistance != value) {
        m_espShowDistance = value;
        writeToSharedMemory();
        emit espShowDistanceChanged();
        qDebug() << "[UI] ESP Distance:" << (value ? "ON" : "OFF");
    }
}

void SharedMemoryController::setEspShowTracer(bool value)
{
    if (m_espShowTracer != value) {
        m_espShowTracer = value;
        writeToSharedMemory();
        emit espShowTracerChanged();
        qDebug() << "[UI] ESP Tracer:" << (value ? "ON" : "OFF");
    }
}

void SharedMemoryController::setEspCharms(bool value)
{
    if (m_espCharms != value) {
        m_espCharms = value;
        writeToSharedMemory();
        emit espCharmsChanged();
        qDebug() << "[UI] ESP Charms:" << (value ? "ON" : "OFF");
    }
}

void SharedMemoryController::setTriggerbotEnabled(bool value)
{
    if (m_triggerbot != value) {
        m_triggerbot = value;
        writeToSharedMemory();
        emit triggerbotEnabledChanged();
        qDebug() << "[UI] Triggerbot:" << (value ? "ON" : "OFF");
    }
}

void SharedMemoryController::setTargetSpeed(float value)
{
    if (qAbs(m_targetSpeed - value) > 0.001f) {
        float oldValue = m_targetSpeed;
        m_targetSpeed = value;
        writeToSharedMemory();
        emit targetSpeedChanged();
        qDebug() << "[UI] Target Speed changed from" << oldValue << "to" << value;
        
        // 디버깅을 위해 쓰기 후 바로 읽기
        qDebug() << "  → Shared memory now has:" << pSharedData->targetSpeed;
        qDebug() << "  → Jump power in shared memory:" << pSharedData->jumpPower;
    }
}

void SharedMemoryController::setJumpPower(float value)
{
    if (qAbs(m_jumpPower - value) > 0.001f) {
        m_jumpPower = value;
        writeToSharedMemory();
        emit jumpPowerChanged();
        qDebug() << "[UI] Jump Power:" << value;
    }
}
