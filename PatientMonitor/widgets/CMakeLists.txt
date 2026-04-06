cmake_minimum_required(VERSION 3.16)
project(PatientMonitor_Widgets VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

# Find Qt6 (fallback to Qt5 if needed)
find_package(Qt6 QUIET COMPONENTS Core Widgets)
if (NOT Qt6_FOUND)
    find_package(Qt5 REQUIRED COMPONENTS Core Widgets)
    set(QT_VERSION_MAJOR 5)
else()
    set(QT_VERSION_MAJOR 6)
endif()

set(SOURCES
    main.cpp
    mainwindow.cpp
    waveformwidget.cpp
)

set(HEADERS
    mainwindow.h
    waveformwidget.h
)

if (QT_VERSION_MAJOR EQUAL 6)
    qt_add_executable(PatientMonitor_Widgets ${SOURCES} ${HEADERS})
    target_link_libraries(PatientMonitor_Widgets PRIVATE Qt6::Core Qt6::Widgets)
else()
    add_executable(PatientMonitor_Widgets ${SOURCES} ${HEADERS})
    target_link_libraries(PatientMonitor_Widgets PRIVATE Qt5::Core Qt5::Widgets)
endif()

# Windows: ensure executable finds Qt DLLs
if (WIN32)
    set_target_properties(PatientMonitor_Widgets PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()
