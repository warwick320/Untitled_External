#pragma once
#include <QObject>
#include <QTimer>
#include <QSettings>
#include "SharedData.h"
#include <iostream>

class SharedMemoryController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool aimbotEnabled READ getAimbotEnabled WRITE setAimbotEnabled NOTIFY aimbotEnabledChanged)
    Q_PROPERTY(int  aimbotType    READ getAimbotType    WRITE setAimbotType    NOTIFY aimbotTypeChanged)
    Q_PROPERTY(bool espEnabled READ getEspEnabled WRITE setEspEnabled NOTIFY espEnabledChanged)
    Q_PROPERTY(bool espShowNames READ getEspShowNames WRITE setEspShowNames NOTIFY espShowNamesChanged)
    Q_PROPERTY(bool espShowBox READ getEspShowBox WRITE setEspShowBox NOTIFY espShowBoxChanged)
    Q_PROPERTY(bool espShowBones READ getEspShowBones WRITE setEspShowBones NOTIFY espShowBonesChanged)
    Q_PROPERTY(bool espShowDistance READ getEspShowDistance WRITE setEspShowDistance NOTIFY espShowDistanceChanged)
    Q_PROPERTY(bool espShowTracer READ getEspShowTracer WRITE setEspShowTracer NOTIFY espShowTracerChanged)
    Q_PROPERTY(bool espCharms READ getEspCharms WRITE setEspCharms NOTIFY espCharmsChanged)
    Q_PROPERTY(bool triggerbotEnabled READ getTriggerbotEnabled WRITE setTriggerbotEnabled NOTIFY triggerbotEnabledChanged)
    Q_PROPERTY(float targetSpeed READ getTargetSpeed WRITE setTargetSpeed NOTIFY targetSpeedChanged)
    Q_PROPERTY(float jumpPower READ getJumpPower WRITE setJumpPower NOTIFY jumpPowerChanged)
    Q_PROPERTY(float fovSize READ getFovSize WRITE setFovSize NOTIFY fovSizeChanged)
    Q_PROPERTY(float smooth READ getSmooth WRITE setSmooth NOTIFY smoothChanged)  
    Q_PROPERTY(QString aimbotKeybind READ getAimbotKeybind WRITE setAimbotKeybind NOTIFY aimbotKeybindChanged)
    Q_PROPERTY(bool isConnected READ getIsConnected NOTIFY isConnectedChanged)

public:
    explicit SharedMemoryController(QObject* parent = nullptr);
    ~SharedMemoryController();

    // Getters
    bool  getTriggerbotEnabled() const { return m_triggerbot; }
    float getTargetSpeed() const { return m_targetSpeed; }
    float getJumpPower() const { return m_jumpPower; }
    bool  getAimbotEnabled() const { return m_aimbotEnabled; }
    int   getAimbotType() const { return m_aimbotType; }
    bool  getEspEnabled() const { return m_espEnabled; }
    bool  getEspShowNames() const { return m_espShowNames; }
    bool  getEspShowBox() const { return m_espShowBox; }
    bool  getEspShowBones() const { return m_espShowBones; }
    bool  getEspShowDistance() const { return m_espShowDistance; }
    bool  getEspShowTracer() const { return m_espShowTracer; }
    bool  getEspCharms() const { return m_espCharms; }
    float getFovSize() const { return m_fovSize; }
    float getSmooth() const { return m_smooth; }  
    QString getAimbotKeybind() const { return m_aimbotKeybind; }
    bool  getIsConnected() const { return m_isConnected; }

public slots:
    // Setters
    void setTriggerbotEnabled(bool value);
    void setTargetSpeed(float value);
    void setJumpPower(float value);
    void setAimbotEnabled(bool value);
    void setAimbotType(int value);
    void setEspEnabled(bool value);
    void setEspShowNames(bool value);
    void setEspShowBox(bool value);
    void setEspShowBones(bool value);
    void setEspShowDistance(bool value);
    void setEspShowTracer(bool value);
    void setEspCharms(bool value);
    void setFovSize(float value);
    void setSmooth(float value);
    void setAimbotKeybind(const QString& value);

signals:
    void triggerbotEnabledChanged();
    void targetSpeedChanged();
    void jumpPowerChanged();
    void aimbotEnabledChanged();
    void aimbotTypeChanged();
    void espEnabledChanged();
    void espShowNamesChanged();
    void espShowBoxChanged();
    void espShowBonesChanged();
    void espShowDistanceChanged();
    void espShowTracerChanged();
    void espCharmsChanged();
    void fovSizeChanged();
    void smoothChanged();
    void aimbotKeybindChanged();
    void isConnectedChanged();

private slots:
    void syncWithSharedMemory();
    void checkConnection();

private:
    bool connectToSharedMemory();
    void writeToSharedMemory();
    void readFromSharedMemory();

    void loadSettings();
    void saveSettings();
    bool hasValueChangedFromDefault() const;
    void initializeDefaults();

    HANDLE hMapFile;
    SharedData* pSharedData;

    bool  m_aimbotEnabled;
    int   m_aimbotType;
    bool  m_espEnabled;
    bool  m_espShowNames;
    bool  m_espShowBox;
    bool  m_espShowBones;
    bool  m_espShowDistance;
    bool  m_espShowTracer;
    bool  m_espCharms;
    float m_fovSize;
    float m_smooth;
    float m_smoothMultiplier;
    bool  m_isConnected;
    bool  m_triggerbot;
    float m_targetSpeed;
    float m_jumpPower;
    QString m_aimbotKeybind;

    // 기본값 저장
    struct DefaultValues {
        bool  aimbotEnabled = false;
        int   aimbotType = 0;
        bool  espEnabled = false;
        bool  espShowNames = false;
        bool  espShowBox = false;
        bool  espShowBones = false;
        bool  espShowDistance = false;
        bool  espShowTracer = false;
        bool  espCharms = false;
        float fovSize = 150.0f;
        float smooth = 1.0f;
        bool  triggerbot = false;
        float targetSpeed = 16.0f;
        float jumpPower = 50.0f;
        QString aimbotKeybind = "LMB";
    } m_defaults;

    QTimer* m_syncTimer;
    QTimer* m_connectionTimer;
    ULONGLONG m_lastUpdateTime;
    QSettings* m_settings;
};