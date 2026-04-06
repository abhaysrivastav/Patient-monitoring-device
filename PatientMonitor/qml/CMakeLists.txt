cmake_minimum_required(VERSION 3.16)
project(PatientMonitor_QML VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 QUIET COMPONENTS Core Quick)
if (NOT Qt6_FOUND)
    find_package(Qt5 REQUIRED COMPONENTS Core Quick)
    set(QT_VERSION_MAJOR 5)
else()
    set(QT_VERSION_MAJOR 6)
endif()

# ── QML source files ──────────────────────────────────────────────────────────
set(QML_FILES
    qml/main.qml
    qml/VitalCard.qml
    qml/WaveformView.qml
    qml/AlarmPanel.qml
)

if (QT_VERSION_MAJOR EQUAL 6)
    qt_add_executable(PatientMonitor_QML main.cpp)
    qt_add_qml_module(PatientMonitor_QML
        URI PatientMonitor
        VERSION 1.0
        QML_FILES ${QML_FILES}
    )
    target_link_libraries(PatientMonitor_QML PRIVATE Qt6::Core Qt6::Quick)
else()
    add_executable(PatientMonitor_QML main.cpp)
    # For Qt5, embed QML files as resources
    qt5_add_resources(QML_RESOURCES resources.qrc)
    target_sources(PatientMonitor_QML PRIVATE ${QML_RESOURCES})
    target_link_libraries(PatientMonitor_QML PRIVATE Qt5::Core Qt5::Quick)
endif()

if (WIN32)
    set_target_properties(PatientMonitor_QML PROPERTIES WIN32_EXECUTABLE TRUE)
endif()
