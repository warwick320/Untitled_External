#include "SharedMemoryController.h"
#include <QDebug>
#include <QCoreApplication>
#include <QDir>
#include <QDataStream>
#include <algorithm>

SharedMemoryController::SharedMemoryController(QObject* parent)
    : QObject(parent),
    m_socket(nullptr),
    m_pipeName("RobloxExternalPipe"),
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
    m_useCornerBox(false),
    m_rainbow(false),
    m_isConnected(false)
{
    QString settingsPath = QCoreApplication::applicationDirPath() + "/config.ini";
    m_settings = new QSettings(settingsPath, QSettings::IniFormat, this);

    qDebug() << "[CONFIG] Settings file location:" << settingsPath;
    initializeDefaults();
    loadSettings();

    // 소켓 초기화
    m_socket = new QLocalSocket(this);
    connect(m_socket, &QLocalSocket::connected, this, &SharedMemoryController::onConnected);
    connect(m_socket, &QLocalSocket::disconnected, this, &SharedMemoryController::onDisconnected);
    connect(m_socket, &QLocalSocket::readyRead, this, &SharedMemoryController::onReadyRead);
    connect(m_socket, &QLocalSocket::errorOccurred, this, &SharedMemoryController::onErrorOccurred);


    m_connectionTimer = new QTimer(this);
    connect(m_connectionTimer, &QTimer::timeout, this, &SharedMemoryController::checkConnection);
    m_connectionTimer->start(1000);

    connectToPipe();
}

SharedMemoryController::~SharedMemoryController()
{
    if (hasValueChangedFromDefault()) {
        saveSettings();
        qDebug() << "[CONFIG] Settings saved on exit";
    }

    if (m_socket) {
        m_socket->disconnectFromServer();
        if (m_socket->state() != QLocalSocket::UnconnectedState) {
            m_socket->waitForDisconnected(1000);
        }
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
    m_useCornerBox = m_settings->value("useCornerBox", m_defaults.useCornerBox).toBool();
    m_rainbow = m_settings->value("rainbow", m_defaults.rainbow).toBool();

    m_settings->endGroup();

    qDebug() << "[CONFIG] Settings loaded from file";
    qDebug() << "  ● Target Speed:" << m_targetSpeed;
    qDebug() << "  ● Jump Power:" << m_jumpPower;
}

void SharedMemoryController::saveSettings()
{
    m_settings->beginGroup("Settings");

    if (m_aimbotEnabled != m_defaults.aimbotEnabled) {
        m_settings->setValue("aimbotEnabled", m_aimbotEnabled);
    }
    else {
        m_settings->remove("aimbotEnabled");
    }

    if (m_aimbotType != m_defaults.aimbotType) {
        m_settings->setValue("aimbotType", m_aimbotType);
    }
    else {
        m_settings->remove("aimbotType");
    }

    if (m_espEnabled != m_defaults.espEnabled) {
        m_settings->setValue("espEnabled", m_espEnabled);
    }
    else {
        m_settings->remove("espEnabled");
    }

    if (m_espShowNames != m_defaults.espShowNames) {
        m_settings->setValue("espShowNames", m_espShowNames);
    }
    else {
        m_settings->remove("espShowNames");
    }

    if (m_espShowBox != m_defaults.espShowBox) {
        m_settings->setValue("espShowBox", m_espShowBox);
    }
    else {
        m_settings->remove("espShowBox");
    }

    if (m_espShowBones != m_defaults.espShowBones) {
        m_settings->setValue("espShowBones", m_espShowBones);
    }
    else {
        m_settings->remove("espShowBones");
    }

    if (m_espShowDistance != m_defaults.espShowDistance) {
        m_settings->setValue("espShowDistance", m_espShowDistance);
    }
    else {
        m_settings->remove("espShowDistance");
    }

    if (m_espShowTracer != m_defaults.espShowTracer) {
        m_settings->setValue("espShowTracer", m_espShowTracer);
    }
    else {
        m_settings->remove("espShowTracer");
    }

    if (m_espCharms != m_defaults.espCharms) {
        m_settings->setValue("espCharms", m_espCharms);
    }
    else {
        m_settings->remove("espCharms");
    }

    if (qAbs(m_fovSize - m_defaults.fovSize) > 0.001f) {
        m_settings->setValue("fovSize", m_fovSize);
    }
    else {
        m_settings->remove("fovSize");
    }

    if (qAbs(m_smooth - m_defaults.smooth) > 0.001f) {
        m_settings->setValue("smooth", m_smooth);
    }
    else {
        m_settings->remove("smooth");
    }

    if (m_triggerbot != m_defaults.triggerbot) {
        m_settings->setValue("triggerbot", m_triggerbot);
    }
    else {
        m_settings->remove("triggerbot");
    }

    if (qAbs(m_targetSpeed - m_defaults.targetSpeed) > 0.001f) {
        m_settings->setValue("targetSpeed", m_targetSpeed);
    }
    else {
        m_settings->remove("targetSpeed");
    }

    if (qAbs(m_jumpPower - m_defaults.jumpPower) > 0.001f) {
        m_settings->setValue("jumpPower", m_jumpPower);
    }
    else {
        m_settings->remove("jumpPower");
    }

    if (m_aimbotKeybind != m_defaults.aimbotKeybind) {
        m_settings->setValue("aimbotKeybind", m_aimbotKeybind);
    }
    else {
        m_settings->remove("aimbotKeybind");
    }

    if (m_useCornerBox != m_defaults.useCornerBox) {
        m_settings->setValue("useCornerBox", m_useCornerBox);
    }
    else {
        m_settings->remove("useCornerBox");
    }

    if (m_rainbow != m_defaults.rainbow) {
        m_settings->setValue("rainbow", m_rainbow);
    }
    else {
        m_settings->remove("rainbow");
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
        m_aimbotKeybind != m_defaults.aimbotKeybind ||
        m_useCornerBox != m_defaults.useCornerBox ||
        m_rainbow != m_defaults.rainbow);
}

bool SharedMemoryController::connectToPipe()
{
    if (m_socket->state() == QLocalSocket::ConnectedState ||
        m_socket->state() == QLocalSocket::ConnectingState) {
        return true;
    }

    qDebug() << "[PIPE] Attempting to connect to:" << m_pipeName;
    m_socket->connectToServer(m_pipeName);

    if (!m_socket->waitForConnected(1000)) {
        qDebug() << "[ERROR] Could not connect to pipe:" << m_socket->errorString();
        return false;
    }

    return true;
}

void SharedMemoryController::onConnected()
{
    qDebug() << "[SUCCESS] Connected to named pipe";
    m_isConnected = true;
    emit isConnectedChanged();
    sendData();
}

void SharedMemoryController::onDisconnected()
{
    qDebug() << "[WARNING] Disconnected from named pipe";
    m_isConnected = false;
    emit isConnectedChanged();
}

void SharedMemoryController::onReadyRead()
{
    QByteArray data = m_socket->readAll();
    if (!data.isEmpty()) {
        processReceivedData(data);
    }
}

void SharedMemoryController::onErrorOccurred(QLocalSocket::LocalSocketError error)
{
    if (error != QLocalSocket::ServerNotFoundError &&
        error != QLocalSocket::ConnectionRefusedError) {
        qDebug() << "[ERROR] Pipe error:" << m_socket->errorString();
    }
}

void SharedMemoryController::checkConnection()
{
    if (m_socket->state() != QLocalSocket::ConnectedState) {
        attemptReconnect();
    }
}

void SharedMemoryController::attemptReconnect()
{
    if (m_socket->state() == QLocalSocket::UnconnectedState) {
        connectToPipe();
    }
}

void SharedMemoryController::sendData()
{
    if (m_socket->state() != QLocalSocket::ConnectedState) {
        return;
    }

    SharedData data;
    data.aimbot_enabled = m_aimbotEnabled ? 1 : 0;
    data.aimbot_type = (BYTE)std::clamp(m_aimbotType, 0, 2);
    data.esp_enabled = m_espEnabled ? 1 : 0;
    data.esp_show_names = m_espShowNames ? 1 : 0;
    data.esp_show_box = m_espShowBox ? 1 : 0;
    data.esp_show_bones = m_espShowBones ? 1 : 0;
    data.esp_show_distance = m_espShowDistance ? 1 : 0;
    data.esp_show_tracer = m_espShowTracer ? 1 : 0;
    data.triggerbot = m_triggerbot ? 1 : 0;
    data.esp_chams = m_espCharms ? 1 : 0;
    data.useCornerBox = m_useCornerBox ? 1 : 0;
    data.g_Rainbow = m_rainbow ? 1 : 0;
    data.fov_size = m_fovSize;
    data.smooth = m_smooth;
    data.targetSpeed = m_targetSpeed;
    data.jumpPower = m_jumpPower;

    QByteArray keybindBytes = m_aimbotKeybind.toUtf8();
    memset(data.aimbot_keybind, 0, sizeof(data.aimbot_keybind));
    if (!keybindBytes.isEmpty()) {
        strncpy_s(data.aimbot_keybind, sizeof(data.aimbot_keybind),
            keybindBytes.constData(), _TRUNCATE);
    }

    // 바이너리 데이터 전송
    QByteArray buffer(reinterpret_cast<const char*>(&data), sizeof(SharedData));
    qint64 written = m_socket->write(buffer);
    m_socket->flush();

    if (written == buffer.size()) {
        qDebug() << "[PIPE] Data sent successfully";
    }
    else {
        qDebug() << "[ERROR] Failed to send complete data";
    }
}

void SharedMemoryController::processReceivedData(const QByteArray& data)
{
    if (data.size() != sizeof(SharedData)) {
        qDebug() << "[ERROR] Received data size mismatch:" << data.size() << "expected:" << sizeof(SharedData);
        return;
    }

    const SharedData* receivedData = reinterpret_cast<const SharedData*>(data.constData());
    bool changed = false;
    auto toBool = [](BYTE v) { return v != 0; };

    if (m_aimbotEnabled != toBool(receivedData->aimbot_enabled)) {
        m_aimbotEnabled = toBool(receivedData->aimbot_enabled);
        emit aimbotEnabledChanged();
        changed = true;
    }

    if (m_aimbotType != (int)receivedData->aimbot_type) {
        m_aimbotType = (int)receivedData->aimbot_type;
        emit aimbotTypeChanged();
        changed = true;
    }

    if (m_espEnabled != toBool(receivedData->esp_enabled)) {
        m_espEnabled = toBool(receivedData->esp_enabled);
        emit espEnabledChanged();
        changed = true;
    }

    if (m_espShowNames != toBool(receivedData->esp_show_names)) {
        m_espShowNames = toBool(receivedData->esp_show_names);
        emit espShowNamesChanged();
        changed = true;
    }

    if (m_espShowBox != toBool(receivedData->esp_show_box)) {
        m_espShowBox = toBool(receivedData->esp_show_box);
        emit espShowBoxChanged();
        changed = true;
    }

    if (m_espShowBones != toBool(receivedData->esp_show_bones)) {
        m_espShowBones = toBool(receivedData->esp_show_bones);
        emit espShowBonesChanged();
        changed = true;
    }

    if (m_espShowDistance != toBool(receivedData->esp_show_distance)) {
        m_espShowDistance = toBool(receivedData->esp_show_distance);
        emit espShowDistanceChanged();
        changed = true;
    }

    if (m_espShowTracer != toBool(receivedData->esp_show_tracer)) {
        m_espShowTracer = toBool(receivedData->esp_show_tracer);
        emit espShowTracerChanged();
        changed = true;
    }

    if (m_espCharms != toBool(receivedData->esp_chams)) {
        m_espCharms = toBool(receivedData->esp_chams);
        emit espCharmsChanged();
        changed = true;
    }

    if (m_triggerbot != toBool(receivedData->triggerbot)) {
        m_triggerbot = toBool(receivedData->triggerbot);
        emit triggerbotEnabledChanged();
        changed = true;
    }

    if (m_useCornerBox != toBool(receivedData->useCornerBox)) {
        m_useCornerBox = toBool(receivedData->useCornerBox);
        emit useCornerBoxChanged();
        changed = true;
    }

    if (m_rainbow != toBool(receivedData->g_Rainbow)) {
        m_rainbow = toBool(receivedData->g_Rainbow);
        emit rainbowChanged();
        changed = true;
    }

    if (qAbs(m_fovSize - receivedData->fov_size) > 0.001f) {
        m_fovSize = receivedData->fov_size;
        emit fovSizeChanged();
        changed = true;
    }

    if (qAbs(m_smooth - receivedData->smooth) > 0.001f) {
        m_smooth = receivedData->smooth;
        emit smoothChanged();
        changed = true;
    }

    if (qAbs(m_targetSpeed - receivedData->targetSpeed) > 0.001f) {
        m_targetSpeed = receivedData->targetSpeed;
        emit targetSpeedChanged();
        changed = true;
    }

    if (qAbs(m_jumpPower - receivedData->jumpPower) > 0.001f) {
        m_jumpPower = receivedData->jumpPower;
        emit jumpPowerChanged();
        changed = true;
    }

    QString keybindFromPipe = QString::fromUtf8(receivedData->aimbot_keybind);
    if (m_aimbotKeybind != keybindFromPipe) {
        m_aimbotKeybind = keybindFromPipe;
        emit aimbotKeybindChanged();
        changed = true;
    }

    if (changed && hasValueChangedFromDefault()) {
        saveSettings();
    }
}

// Setter implementations
void SharedMemoryController::setAimbotEnabled(bool value)
{
    if (m_aimbotEnabled != value) {
        m_aimbotEnabled = value;
        sendData();
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
        sendData();
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
        sendData();
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
        sendData();
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
        sendData();
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
        sendData();
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
        sendData();
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
        sendData();
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
        sendData();
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
        sendData();
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
        sendData();
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
        sendData();
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
        sendData();
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
        sendData();
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
        sendData();
        emit aimbotKeybindChanged();
        qDebug() << "[UI] Aimbot Keybind:" << value;

        if (hasValueChangedFromDefault()) {
            saveSettings();
        }
    }
}

void SharedMemoryController::setUseCornerBox(bool value)
{
    if (m_useCornerBox != value) {
        m_useCornerBox = value;
        sendData();
        emit useCornerBoxChanged();
        qDebug() << "[UI] Use Corner Box:" << (value ? "ON" : "OFF");

        if (hasValueChangedFromDefault()) {
            saveSettings();
        }
    }
}

void SharedMemoryController::setRainbow(bool value)
{
    if (m_rainbow != value) {
        m_rainbow = value;
        sendData();
        emit rainbowChanged();
        qDebug() << "[UI] Rainbow:" << (value ? "ON" : "OFF");

        if (hasValueChangedFromDefault()) {
            saveSettings();
        }
    }
}