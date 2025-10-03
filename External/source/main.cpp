#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QDebug>
#include <QDir>
#include <QQmlContext>
#include <QScreen>
#include <QWindow>
#include <QQuickWindow>
#include "SharedMemoryController.h"  
#include "SharedData.h"

//#define _DEBUG
//windeployqt External.exe --release --qmldir C:\Users\warwick\source\repos\External\External\source\QML --compiler-runtime
//FUCK QT
int main(int argc, char* argv[])
{

#ifdef _DEBUG
#ifdef _WIN32
    if (AllocConsole()) {
        freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
        freopen_s((FILE**)stderr, "CONOUT$", "w", stderr);
        freopen_s((FILE**)stdin, "CONIN$", "r", stdin);
        std::cout.clear();
        std::cerr.clear();
        std::cin.clear();
    }
#endif
#endif

    std::cout << "[DEBUG] SharedData structure analysis:" << std::endl;
    std::cout << "  Total size: " << sizeof(SharedData) << " bytes" << std::endl;
    std::cout << "  Offsets:" << std::endl;
    std::cout << "    aimbot_enabled: " << offsetof(SharedData, aimbot_enabled) << std::endl;
    std::cout << "    esp_enabled: " << offsetof(SharedData, esp_enabled) << std::endl;
    std::cout << "    aimbot_type: " << offsetof(SharedData, aimbot_type) << std::endl;
    std::cout << "    padding: " << offsetof(SharedData, padding) << std::endl;
    std::cout << "    fov_size: " << offsetof(SharedData, fov_size) << std::endl;
    std::cout << "    smooth: " << offsetof(SharedData, smooth) << std::endl;
    std::cout << "    targetSpeed: " << offsetof(SharedData, targetSpeed) << std::endl;
    std::cout << "    jumpPower: " << offsetof(SharedData, jumpPower) << std::endl;
    std::cout << "    aimbot_keybind: " << offsetof(SharedData, aimbot_keybind) << std::endl;
    std::cout << "    useCornerBox: " << offsetof(SharedData, useCornerBox) << std::endl;
    std::cout << "    g_Rainbow: " << offsetof(SharedData, g_Rainbow) << std::endl;
    std::cout << "    last_update: " << offsetof(SharedData, last_update) << std::endl;
    std::cout << "    process_running: " << offsetof(SharedData, process_running) << std::endl;
    std::cout << "    reserved: " << offsetof(SharedData, reserved) << std::endl;

    QGuiApplication app(argc, argv);
    QDir::setCurrent(QCoreApplication::applicationDirPath());

    qDebug() << "[INFO] Available screens:";
    for (QScreen* screen : QGuiApplication::screens()) {
        qDebug() << "  - Screen:" << screen->name()
            << "DPI:" << screen->devicePixelRatio()
            << "Geometry:" << screen->geometry();
    }

    SharedMemoryController sharedController;

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("appController", &sharedController);
    qDebug() << "SharedMemoryController set in QML context.";

    std::cout << "[INIT] Roblox External UI Started" << std::endl;
    std::cout << "[INFO] Attempting to connect to main process..." << std::endl;

    const QUrl qmlUrl(QStringLiteral("qrc:/qt/qml/external/main.qml"));
    qDebug() << "Attempting to load QML:" << qmlUrl.toString();
    engine.load(qmlUrl);

    if (engine.rootObjects().isEmpty()) {
        std::cout << "[ERROR] Failed to load QML!" << std::endl;
        return -1;
    }


    QObject* rootObject = engine.rootObjects().first();
    if (rootObject) {
        QQuickWindow* window = qobject_cast<QQuickWindow*>(rootObject);
        if (window) {
            QScreen* primaryScreen = QGuiApplication::primaryScreen();
            if (primaryScreen) {
                window->setScreen(primaryScreen);

                QRect screenGeometry = primaryScreen->availableGeometry();
                int x = (screenGeometry.width() - 1000) / 2 + screenGeometry.x();
                int y = (screenGeometry.height() - 500) / 2 + screenGeometry.y();

                window->setPosition(x, y);
                window->setWidth(1000);
                window->setHeight(500);
                window->update();

                qDebug() << "[INFO] Window positioned at primary screen:" << x << y;
                qDebug() << "[INFO] Primary screen:" << primaryScreen->name();
                qDebug() << "[INFO] Primary screen DPI:" << primaryScreen->devicePixelRatio();
                qDebug() << "[INFO] Window geometry:" << window->geometry();
            }
        }
    }

    std::cout << "[SUCCESS] QML UI loaded successfully!" << std::endl;
    std::cout << "[INFO] Ready for shared memory communication" << std::endl;

    return app.exec();
}