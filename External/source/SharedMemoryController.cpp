#include "SharedMemoryController.h"
#include <QDebug>
#include <QCoreApplication>
#include <QDir>
#include <algorithm>

SharedMemoryController::SharedMemoryController(QObject* parent)
    : QObject(parent),
      hMapFile(nullptr),
      pSharedData(nullptr),
      m_aimbotEnabled(false),
      m_aimbotType(0),
      m_espEnabled(false),
      m_espShowNames(false),
      m_espShowBox(false),
      m_espShowBones(false),
      m_espShowDistance(false),
      m_espShowTracer(false),
      m_espCharms(false),
      m_fovSize(150.0f),
      m_smooth(1.0f),           
      m_triggerbot(false),
      m_targetSpeed(16.0f),
      m_jumpPower(50.0f),
      m_aimbotKeybind("LMB"),
      m_isConnected(false),
      m_lastUpdateTime(0)
{

    QString settingsPath = QCoreApplication::applicationDirPath() + "/config.ini";
    m_settings = new QSettings(settingsPath, QSettings::IniFormat, this);
    
    qDebug() << "[CONFIG] Settings file location:" << settingsPath;
    initializeDefaults();
    loadSettings();
    connectToSharedMemory();

    m_syncTimer = new QTimer(this);
    connect(m_syncTimer, &QTimer::timeout, this, &SharedMemoryController::syncWithSharedMemory);
    m_syncTimer->start(16);

    m_connectionTimer = new QTimer(this);
    connect(m_connectionTimer, &QTimer::timeout, this, &SharedMemoryController::checkConnection);
    m_connectionTimer->start(1000);
}

SharedMemoryController::~SharedMemoryController()
{
    if (hasValueChangedFromDefault()) {
        saveSettings();
        qDebug() << "[CONFIG] Settings saved on exit";
    }
    
    if (pSharedData) {
        UnmapViewOfFile(pSharedData);
    }
    if (hMapFile) {
        CloseHandle(hMapFile);
    }
}

void SharedMemoryController::initializeDefaults()
{
    qDebug() << "[CONFIG] Default values initialized";
}

void SharedMemoryController::loadSettings()
{
    m_settings->beginGroup("Settings");

    m_aimbotEnabled = m_settings->value("aimbotEnabled", m_defaults.aimbotEnabled).toBool();
    m_aimbotType = m_settings->value("aimbotType", m_defaults.aimbotType).toInt();
    m_espEnabled = m_settings->value("espEnabled", m_defaults.espEnabled).toBool();
    m_espShowNames = m_settings->value("espShowNames", m_defaults.espShowNames).toBool();
    m_espShowBox = m_settings->value("espShowBox", m_defaults.espShowBox).toBool();
    m_espShowBones = m_settings->value("espShowBones", m_defaults.espShowBones).toBool();
    m_espShowDistance = m_settings->value("espShowDistance", m_defaults.espShowDistance).toBool();
    m_espShowTracer = m_settings->value("espShowTracer", m_defaults.espShowTracer).toBool();
    m_espCharms = m_settings->value("espCharms", m_defaults.espCharms).toBool();
    m_fovSize = m_settings->value("fovSize", m_defaults.fovSize).toFloat();
    m_smooth = m_settings->value("smooth", m_defaults.smooth).toFloat();
    m_triggerbot = m_settings->value("triggerbot", m_defaults.triggerbot).toBool();
    m_targetSpeed = m_settings->value("targetSpeed", m_defaults.targetSpeed).toFloat();
    m_jumpPower = m_settings->value("jumpPower", m_defaults.jumpPower).toFloat();
    m_aimbotKeybind = m_settings->value("aimbotKeybind", m_defaults.aimbotKeybind).toString();
    
    m_settings->endGroup();
    
    qDebug() << "[CONFIG] Settings loaded from file";
    qDebug() << "  ¡æ Aimbot Enabled:" << m_aimbotEnabled;
    qDebug() << "  ¡æ FOV Size:" << m_fovSize;
    qDebug() << "  ¡æ Smooth:" << m_smooth;
    qDebug() << "  ¡æ Target Speed:" << m_targetSpeed;
    qDebug() << "  ¡æ Jump Power:" << m_jumpPower;
    qDebug() << "  ¡æ Aimbot Keybind:" << m_aimbotKeybind;
}

void SharedMemoryController::saveSettings()
{
    m_settings->beginGroup("Settings");
    
    if (m_aimbotEnabled != m_defaults.aimbotEnabled) {
        m_settings->setValue("aimbotEnabled", m_aimbotEnabled);
    } else {
        m_settings->remove("aimbotEnabled");
    }
    
    if (m_aimbotType != m_defaults.aimbotType) {
        m_settings->setValue("aimbotType", m_aimbotType);
    } else {
        m_settings->remove("aimbotType");
    }
    
    if (m_espEnabled != m_defaults.espEnabled) {
        m_settings->setValue("espEnabled", m_espEnabled);
    } else {
        m_settings->remove("espEnabled");
    }
    
    if (m_espShowNames != m_defaults.espShowNames) {
        m_settings->setValue("espShowNames", m_espShowNames);
    } else {
        m_settings->remove("espShowNames");
    }
    
    if (m_espShowBox != m_defaults.espShowBox) {
        m_settings->setValue("espShowBox", m_espShowBox);
    } else {
        m_settings->remove("espShowBox");
    }
    
    if (m_espShowBones != m_defaults.espShowBones) {
        m_settings->setValue("espShowBones", m_espShowBones);
    } else {
        m_settings->remove("espShowBones");
    }
    
    if (m_espShowDistance != m_defaults.espShowDistance) {
        m_settings->setValue("espShowDistance", m_espShowDistance);
    } else {
        m_settings->remove("espShowDistance");
    }
    
    if (m_espShowTracer != m_defaults.espShowTracer) {
        m_settings->setValue("espShowTracer", m_espShowTracer);
    } else {
        m_settings->remove("espShowTracer");
    }
    
    if (m_espCharms != m_defaults.espCharms) {
        m_settings->setValue("espCharms", m_espCharms);
    } else {
        m_settings->remove("espCharms");
    }
    
    if (qAbs(m_fovSize - m_defaults.fovSize) > 0.001f) {
        m_settings->setValue("fovSize", m_fovSize);
    } else {
        m_settings->remove("fovSize");
    }
    
    if (qAbs(m_smooth - m_defaults.smooth) > 0.001f) {
        m_settings->setValue("smooth", m_smooth);
    } else {
        m_settings->remove("smooth");
    }
    
    if (m_triggerbot != m_defaults.triggerbot) {
        m_settings->setValue("triggerbot", m_triggerbot);
    } else {
        m_settings->remove("triggerbot");
    }
    
    if (qAbs(m_targetSpeed - m_defaults.targetSpeed) > 0.001f) {
        m_settings->setValue("targetSpeed", m_targetSpeed);
    } else {
        m_settings->remove("targetSpeed");
    }
    
    if (qAbs(m_jumpPower - m_defaults.jumpPower) > 0.001f) {
        m_settings->setValue("jumpPower", m_jumpPower);
    } else {
        m_settings->remove("jumpPower");
    }
    
    if (m_aimbotKeybind != m_defaults.aimbotKeybind) {
        m_settings->setValue("aimbotKeybind", m_aimbotKeybind);
    } else {
        m_settings->remove("aimbotKeybind");
    }
    
    m_settings->endGroup();
    m_settings->sync();
    
    qDebug() << "[CONFIG] Settings saved to file";
}

bool SharedMemoryController::hasValueChangedFromDefault() const
{
    return (m_aimbotEnabled != m_defaults.aimbotEnabled ||
            m_aimbotType != m_defaults.aimbotType ||
            m_espEnabled != m_defaults.espEnabled ||
            m_espShowNames != m_defaults.espShowNames ||
            m_espShowBox != m_defaults.espShowBox ||
            m_espShowBones != m_defaults.espShowBones ||
            m_espShowDistance != m_defaults.espShowDistance ||
            m_espShowTracer != m_defaults.espShowTracer ||
            m_espCharms != m_defaults.espCharms ||
            qAbs(m_fovSize - m_defaults.fovSize) > 0.001f ||
            qAbs(m_smooth - m_defaults.smooth) > 0.001f ||
            m_triggerbot != m_defaults.triggerbot ||
            qAbs(m_targetSpeed - m_defaults.targetSpeed) > 0.001f ||
            qAbs(m_jumpPower - m_defaults.jumpPower) > 0.001f ||
            m_aimbotKeybind != m_defaults.aimbotKeybind);
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

    writeToSharedMemory();
    qDebug() << "[INIT] Applied loaded settings to shared memory";

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
    auto toBool = [](BYTE v) { return v != 0; };

    if (m_aimbotEnabled != toBool(pSharedData->aimbot_enabled)) {
        m_aimbotEnabled = toBool(pSharedData->aimbot_enabled);
        emit aimbotEnabledChanged();
        changed = true;
    }
    if (m_aimbotType != (int)pSharedData->aimbot_type) {
        m_aimbotType = (int)pSharedData->aimbot_type;
        emit aimbotTypeChanged();
        changed = true;
    }
    if (m_espEnabled != toBool(pSharedData->esp_enabled)) {
        m_espEnabled = toBool(pSharedData->esp_enabled);
        emit espEnabledChanged();
        changed = true;
    }
    if (m_espShowNames != toBool(pSharedData->esp_show_names)) {
        m_espShowNames = toBool(pSharedData->esp_show_names);
        emit espShowNamesChanged();
        changed = true;
    }
    if (m_espShowBox != toBool(pSharedData->esp_show_box)) {
        m_espShowBox = toBool(pSharedData->esp_show_box);
        emit espShowBoxChanged();
        changed = true;
    }
    if (m_espShowBones != toBool(pSharedData->esp_show_bones)) {
        m_espShowBones = toBool(pSharedData->esp_show_bones);
        emit espShowBonesChanged();
        changed = true;
    }
    if (m_espShowDistance != toBool(pSharedData->esp_show_distance)) {
        m_espShowDistance = toBool(pSharedData->esp_show_distance);
        emit espShowDistanceChanged();
        changed = true;
    }
    if (m_espShowTracer != toBool(pSharedData->esp_show_tracer)) {
        m_espShowTracer = toBool(pSharedData->esp_show_tracer);
        emit espShowTracerChanged();
        changed = true;
    }
    if (m_espCharms != toBool(pSharedData->esp_chams)) {
        m_espCharms = toBool(pSharedData->esp_chams);
        emit espCharmsChanged();
        changed = true;
    }
    if (m_triggerbot != toBool(pSharedData->triggerbot)) {
        m_triggerbot = toBool(pSharedData->triggerbot);
        emit triggerbotEnabledChanged();
        changed = true;
    }

    if (qAbs(m_fovSize - pSharedData->fov_size) > 0.001f) {
        m_fovSize = pSharedData->fov_size;
        emit fovSizeChanged();
        changed = true;
    }
    if (qAbs(m_smooth - pSharedData->smooth) > 0.001f) {
        m_smooth = pSharedData->smooth;
        emit smoothChanged();
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

    QString keybindFromMemory = QString::fromUtf8(pSharedData->aimbot_keybind);
    if (m_aimbotKeybind != keybindFromMemory) {
        m_aimbotKeybind = keybindFromMemory;
        emit aimbotKeybindChanged();
        changed = true;
    }

    if (changed && hasValueChangedFromDefault()) {
        saveSettings();
    }
}

void SharedMemoryController::writeToSharedMemory()
{
    if (!pSharedData) return;

    pSharedData->aimbot_enabled = m_aimbotEnabled ? 1 : 0;
    pSharedData->aimbot_type    = (BYTE)std::clamp(m_aimbotType, 0, 2);  
    pSharedData->esp_enabled = m_espEnabled ? 1 : 0;
    pSharedData->esp_show_names = m_espShowNames ? 1 : 0;
    pSharedData->esp_show_box = m_espShowBox ? 1 : 0;
    pSharedData->esp_show_bones = m_espShowBones ? 1 : 0;
    pSharedData->esp_show_distance = m_espShowDistance ? 1 : 0;
    pSharedData->esp_show_tracer = m_espShowTracer ? 1 : 0;
    pSharedData->triggerbot = m_triggerbot ? 1 : 0;
    pSharedData->esp_chams = m_espCharms ? 1 : 0;

    pSharedData->fov_size = m_fovSize;
    pSharedData->smooth = m_smooth;           
    pSharedData->targetSpeed = m_targetSpeed;
    pSharedData->jumpPower = m_jumpPower;

    QByteArray keybindBytes = m_aimbotKeybind.toUtf8();
    memset(pSharedData->aimbot_keybind, 0, sizeof(pSharedData->aimbot_keybind));
    
    if (!keybindBytes.isEmpty()) {
        strncpy_s(pSharedData->aimbot_keybind, 
                  sizeof(pSharedData->aimbot_keybind),
                  keybindBytes.constData(), 
                  _TRUNCATE);
    }

    qDebug() << "[WRITE] Values written to shared memory";
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
            qDebug() << "[WARNING] Main process seems not responding";
        }
    } else {
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
        
        if (hasValueChangedFromDefault()) {
            saveSettings();
        }
    }
}

void SharedMemoryController::setAimbotType(int value)
{
    if (m_aimbotType != value) {
        m_aimbotType = value;
        writeToSharedMemory();
        emit aimbotTypeChanged();
        qDebug() << "[UI] Aimbot Type:" << value;
        
        if (hasValueChangedFromDefault()) {
            saveSettings();
        }
    }
}

void SharedMemoryController::setEspEnabled(bool value)
{
    if (m_espEnabled != value) {
        m_espEnabled = value;
        writeToSharedMemory();
        emit espEnabledChanged();
        qDebug() << "[UI] ESP:" << (value ? "ON" : "OFF");
        
        if (hasValueChangedFromDefault()) {
            saveSettings();
        }
    }
}

void SharedMemoryController::setEspShowNames(bool value)
{
    if (m_espShowNames != value) {
        m_espShowNames = value;
        writeToSharedMemory();
        emit espShowNamesChanged();
        qDebug() << "[UI] ESP Names:" << (value ? "ON" : "OFF");
        
        if (hasValueChangedFromDefault()) {
            saveSettings();
        }
    }
}

void SharedMemoryController::setEspShowBox(bool value)
{
    if (m_espShowBox != value) {
        m_espShowBox = value;
        writeToSharedMemory();
        emit espShowBoxChanged();
        qDebug() << "[UI] ESP Box:" << (value ? "ON" : "OFF");
        
        if (hasValueChangedFromDefault()) {
            saveSettings();
        }
    }
}

void SharedMemoryController::setFovSize(float value)
{
    if (qAbs(m_fovSize - value) > 0.001f) {
        m_fovSize = value;
        writeToSharedMemory();
        emit fovSizeChanged();
        qDebug() << "[UI] FOV Size:" << value;
        
        if (hasValueChangedFromDefault()) {
            saveSettings();
        }
    }
}

void SharedMemoryController::setSmooth(float value)
{
    if (qAbs(m_smooth - value) > 0.001f) {
        m_smooth = value;
        writeToSharedMemory();
        emit smoothChanged();
        qDebug() << "[UI] Smooth:" << value;
        
        if (hasValueChangedFromDefault()) {
            saveSettings();
        }
    }
}

void SharedMemoryController::setEspShowBones(bool value)
{
    if (m_espShowBones != value) {
        m_espShowBones = value;
        writeToSharedMemory();
        emit espShowBonesChanged();
        qDebug() << "[UI] ESP Bones:" << (value ? "ON" : "OFF");
        
        if (hasValueChangedFromDefault()) {
            saveSettings();
        }
    }
}

void SharedMemoryController::setEspShowDistance(bool value)
{
    if (m_espShowDistance != value) {
        m_espShowDistance = value;
        writeToSharedMemory();
        emit espShowDistanceChanged();
        qDebug() << "[UI] ESP Distance:" << (value ? "ON" : "OFF");
        
        if (hasValueChangedFromDefault()) {
            saveSettings();
        }
    }
}

void SharedMemoryController::setEspShowTracer(bool value)
{
    if (m_espShowTracer != value) {
        m_espShowTracer = value;
        writeToSharedMemory();
        emit espShowTracerChanged();
        qDebug() << "[UI] ESP Tracer:" << (value ? "ON" : "OFF");
        
        if (hasValueChangedFromDefault()) {
            saveSettings();
        }
    }
}

void SharedMemoryController::setEspCharms(bool value)
{
    if (m_espCharms != value) {
        m_espCharms = value;
        writeToSharedMemory();
        emit espCharmsChanged();
        qDebug() << "[UI] ESP Charms:" << (value ? "ON" : "OFF");
        
        if (hasValueChangedFromDefault()) {
            saveSettings();
        }
    }
}

void SharedMemoryController::setTriggerbotEnabled(bool value)
{
    if (m_triggerbot != value) {
        m_triggerbot = value;
        writeToSharedMemory();
        emit triggerbotEnabledChanged();
        qDebug() << "[UI] Triggerbot:" << (value ? "ON" : "OFF");
        
        if (hasValueChangedFromDefault()) {
            saveSettings();
        }
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
        
        if (hasValueChangedFromDefault()) {
            saveSettings();
        }
    }
}

void SharedMemoryController::setJumpPower(float value)
{
    if (qAbs(m_jumpPower - value) > 0.001f) {
        m_jumpPower = value;
        writeToSharedMemory();
        emit jumpPowerChanged();
        qDebug() << "[UI] Jump Power:" << value;
        
        if (hasValueChangedFromDefault()) {
            saveSettings();
        }
    }
}

void SharedMemoryController::setAimbotKeybind(const QString& value)
{
    if (m_aimbotKeybind != value) {
        m_aimbotKeybind = value;
        writeToSharedMemory();
        emit aimbotKeybindChanged();
        qDebug() << "[UI] Aimbot Keybind:" << value;
        
        if (hasValueChangedFromDefault()) {
            saveSettings();
        }
    }
}
