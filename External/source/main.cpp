#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QDebug>
#include <QDir>
#include <QQmlContext>
#include "SharedMemoryController.h"  

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

    QGuiApplication app(argc, argv);
    QDir::setCurrent(QCoreApplication::applicationDirPath());

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

    std::cout << "[SUCCESS] QML UI loaded successfully!" << std::endl;
    std::cout << "[INFO] Ready for shared memory communication" << std::endl;

    return app.exec();
}