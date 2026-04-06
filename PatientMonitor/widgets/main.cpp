#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    // QApplication manages the event loop and GUI resources.
    // In QML we use QGuiApplication (lighter) + QQmlApplicationEngine.
    QApplication app(argc, argv);

    app.setApplicationName("Patient Monitor");
    app.setOrganizationName("MedTech Demo");
    app.setApplicationVersion("1.0");

    MainWindow window;
    window.show();

    return app.exec();
}
