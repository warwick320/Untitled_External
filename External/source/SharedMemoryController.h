#pragma once
#include <QObject>
#include <QTimer>
#include "SharedData.h"
#include <iostream>

class SharedMemoryController : public QObject
{
    Q_OBJECT

        // QML 속성들
        Q_PROPERTY(bool aimbotEnabled READ getAimbotEnabled WRITE setAimbotEnabled NOTIFY aimbotEnabledChanged)
        Q_PROPERTY(bool espEnabled READ getEspEnabled WRITE setEspEnabled NOTIFY espEnabledChanged)
        Q_PROPERTY(bool espShowNames READ getEspShowNames WRITE setEspShowNames NOTIFY espShowNamesChanged)
        Q_PROPERTY(bool espShowBox READ getEspShowBox WRITE setEspShowBox NOTIFY espShowBoxChanged)
        Q_PROPERTY(bool espShowBones READ getEspShowBones WRITE setEspShowBones NOTIFY espShowBonesChanged)
        Q_PROPERTY(bool espShowDistance READ getEspShowDistance WRITE setEspShowDistance NOTIFY espShowDistanceChanged)
        Q_PROPERTY(bool espShowTracer READ getEspShowTracer WRITE setEspShowTracer NOTIFY espShowTracerChanged)
        Q_PROPERTY(float fovSize READ getFovSize WRITE setFovSize NOTIFY fovSizeChanged)
        Q_PROPERTY(bool isConnected READ getIsConnected NOTIFY isConnectedChanged)

public:
    explicit SharedMemoryController(QObject* parent = nullptr);
    ~SharedMemoryController();

    // Getters
    bool getAimbotEnabled() const { return m_aimbotEnabled; }
    bool getEspEnabled() const { return m_espEnabled; }
    bool getEspShowNames() const { return m_espShowNames; }
    bool getEspShowBox() const { return m_espShowBox; }
    bool getEspShowBones() const { return m_espShowBones; }
    bool getEspShowDistance() const { return m_espShowDistance; }
    bool getEspShowTracer() const { return m_espShowTracer; }
    float getFovSize() const { return m_fovSize; }
    bool getIsConnected() const { return m_isConnected; }

    // Setters
    void setAimbotEnabled(bool value);
    void setEspEnabled(bool value);
    void setEspShowNames(bool value);
    void setEspShowBox(bool value);
    void setEspShowBones(bool value);
    void setEspShowDistance(bool value);
    void setEspShowTracer(bool value);
    void setFovSize(float value);

signals:
    void aimbotEnabledChanged(); // Qt의 MOC가 자동으로 처리
    void espEnabledChanged();
    void espShowNamesChanged();
    void espShowBoxChanged();
    void espShowBonesChanged();
    void espShowDistanceChanged();
    void espShowTracerChanged();
    void fovSizeChanged();
    void isConnectedChanged();

private slots:
    void syncWithSharedMemory();
    void checkConnection();

private:
    bool connectToSharedMemory();
    void writeToSharedMemory();
    void readFromSharedMemory();

    // Windows 공유 메모리
    HANDLE hMapFile;
    SharedData* pSharedData;

    // 내부 상태
    bool m_aimbotEnabled;
    bool m_espEnabled;
    bool m_espShowNames;
    bool m_espShowBox;
    bool m_espShowBones;
    bool m_espShowDistance;
    bool m_espShowTracer;
    float m_fovSize;
    float m_smoothMultiplier;
    bool m_isConnected;

    // 타이머들
    QTimer* m_syncTimer;
    QTimer* m_connectionTimer;
    ULONGLONG m_lastUpdateTime;
};