#include <QGuiApplication>      // Lighter than QApplication — no widget module needed
#include <QQmlApplicationEngine>
#include <QUrl>

int main(int argc, char *argv[])
{
    // KEY DIFFERENCE vs Widgets:
    //   Widgets → QApplication  (loads full widget framework)
    //   QML     → QGuiApplication (GPU-accelerated scene graph, no widget overhead)
    QGuiApplication app(argc, argv);
    app.setApplicationName("Patient Monitor QML");
    app.setOrganizationName("MedTech Demo");

    QQmlApplicationEngine engine;

    // Load the root QML file. The engine parses it and builds the scene graph.
    // Everything from here is declarative — no C++ layout code needed.
    // Qt6: use u"..."_qs literal. Qt5: use QStringLiteral or plain string.
    const QUrl url(QStringLiteral("qrc:/PatientMonitor/qml/main.qml"));
    QObject::connect(
        &engine, &QQmlApplicationEngine::objectCreated,
        &app, [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection
    );
    engine.load(url);

    return app.exec();
}
