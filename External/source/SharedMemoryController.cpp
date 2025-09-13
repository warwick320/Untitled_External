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
    m_fovSize(150.0f),
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
    // 기존 공유 메모리 열기
    hMapFile = OpenFileMapping(
        FILE_MAP_ALL_ACCESS,
        FALSE,
        "RobloxExternalSharedMemory"
    );

    if (hMapFile == NULL) {
        qDebug() << "[ERROR] Could not open shared memory. Main process running?";
        return false;
    }

    // 메모리 맵핑
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

    // 초기값 읽기
    readFromSharedMemory();

    return true;
}

void SharedMemoryController::syncWithSharedMemory()
{
    if (!pSharedData) return;

    // 메인 프로세스에서 업데이트된 값들 읽기
    readFromSharedMemory();
}


void SharedMemoryController::readFromSharedMemory()
{
    if (!pSharedData) return;

    bool changed = false;

    // 기존 boolean 값들
    if (m_aimbotEnabled != pSharedData->aimbot_enabled) {
        m_aimbotEnabled = pSharedData->aimbot_enabled;
        emit aimbotEnabledChanged();
        changed = true;
    }

    if (m_espEnabled != pSharedData->esp_enabled) {
        m_espEnabled = pSharedData->esp_enabled;
        emit espEnabledChanged();
        changed = true;
    }

    if (m_espShowNames != pSharedData->esp_show_names) {
        m_espShowNames = pSharedData->esp_show_names;
        emit espShowNamesChanged();
        changed = true;
    }

    if (m_espShowBox != pSharedData->esp_show_box) {
        m_espShowBox = pSharedData->esp_show_box;
        emit espShowBoxChanged();
        changed = true;
    }

    if (m_espShowBones != pSharedData->esp_show_bones) {
        m_espShowBones = pSharedData->esp_show_bones;
        emit espShowBonesChanged();
        changed = true;
    }

    if (m_espShowDistance != pSharedData->esp_show_distance) {
        m_espShowDistance = pSharedData->esp_show_distance;
        emit espShowDistanceChanged();
        changed = true;
    }

    if (m_espShowTracer != pSharedData->esp_show_tracer) {
        m_espShowTracer = pSharedData->esp_show_tracer;
        emit espShowTracerChanged();
        changed = true;
    }

    // 새로운 float 값들
    if (qAbs(m_fovSize - pSharedData->fov_size) > 0.001f) {
        m_fovSize = pSharedData->fov_size;
        emit fovSizeChanged();
        changed = true;
    }


    if (changed) {
        qDebug() << "[SYNC] Values updated from main process";
    }
}
void SharedMemoryController::writeToSharedMemory()
{
    if (!pSharedData) return;

    pSharedData->aimbot_enabled = m_aimbotEnabled;
    pSharedData->esp_enabled = m_espEnabled;
    pSharedData->esp_show_names = m_espShowNames;
    pSharedData->esp_show_box = m_espShowBox;
    pSharedData->esp_show_bones = m_espShowBones;
    pSharedData->esp_show_distance = m_espShowDistance;
    pSharedData->esp_show_tracer = m_espShowTracer;
    pSharedData->fov_size = m_fovSize;
}
void SharedMemoryController::checkConnection()
{
    if (!pSharedData) {
        if (m_isConnected) {
            m_isConnected = false;
            emit isConnectedChanged();
            qDebug() << "[WARNING] Lost connection to main process";
        }

        // 재연결 시도
        connectToSharedMemory();
        return;
    }

    // 메인 프로세스가 살아있는지 확인
    ULONGLONG currentTime = GetTickCount64();
    if (currentTime - pSharedData->last_update > 5000) { // 5초 이상 업데이트 없음
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

// Setters - QML에서 호출됨
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
