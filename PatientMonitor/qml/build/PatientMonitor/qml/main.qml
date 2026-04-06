// main.qml — Patient Monitor (QML Version)
// ─────────────────────────────────────────────────────────────────────────────
// Root window and application state.
//
// KEY DIFFERENCE from Widgets:
//   Widgets: MainWindow is a C++ class. Screen navigation uses QStackedWidget.
//   QML:     Everything is declarative. StackView handles navigation natively.
//            All state (vitals, alarms) lives in QML properties — no C++ model
//            classes required for this prototype.
// ─────────────────────────────────────────────────────────────────────────────

import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15

ApplicationWindow {
    id: root

    title:   "Patient Monitor — QML Version"
    width:   1024
    height:  680
    visible: true
    color:   "#0d111e"

    // ── Application state (replaces MainWindow's member variables) ────────────
    property int    heartRate: 72
    property int    spo2:      98
    property int    bpSys:     120
    property int    bpDia:     80
    property real   temp:      36.8
    property bool   alarmActive: false
    property string currentTime: ""

    // ── Vital simulation timer (same role as m_vitalsTimer in Widgets) ────────
    // QML Timer replaces QTimer — no header, no connect(), no slot declaration.
    Timer {
        id: vitalsTimer
        interval: 2000
        running:  true
        repeat:   true
        onTriggered: {
            // Simple random jitter around baseline values
            root.heartRate = 72 + Math.round((Math.random() - 0.5) * 8)
            root.spo2      = Math.min(100, 98 + Math.round((Math.random() - 0.5) * 2))
            root.bpSys     = 120 + Math.round((Math.random() - 0.5) * 10)
            root.bpDia     = 80  + Math.round((Math.random() - 0.5) * 6)
            root.temp      = parseFloat((36.8 + (Math.random() - 0.5) * 0.2).toFixed(1))

            root.alarmActive = (root.heartRate < 50 || root.heartRate > 120 || root.spo2 < 90)
        }
    }

    // ── Clock timer ───────────────────────────────────────────────────────────
    Timer {
        interval: 1000
        running:  true
        repeat:   true
        onTriggered: {
            root.currentTime = new Date().toLocaleString(Qt.locale(), "dd MMM yyyy   hh:mm:ss")
        }
        Component.onCompleted: triggered()   // fire immediately so clock shows on startup
    }

    // ── StackView for screen navigation ──────────────────────────────────────
    // Widgets used QStackedWidget::setCurrentIndex(). StackView.push/pop is
    // more flexible and supports animated transitions out of the box.
    StackView {
        id: stackView
        anchors.fill: parent
        initialItem: dashboardComponent
    }

    // ──────────────────────────────────────────────────────────────────────────
    //  Screen 1: Vitals Dashboard
    // ──────────────────────────────────────────────────────────────────────────
    Component {
        id: dashboardComponent

        Rectangle {
            color: "#0d111e"

            ColumnLayout {
                anchors { fill: parent; margins: 12 }
                spacing: 8

                // ── Header bar ───────────────────────────────────────────────
                RowLayout {
                    Layout.fillWidth: true

                    Text {
                        text:  "PATIENT MONITOR"
                        color: "#4fc3f7"
                        font { pixelSize: 20; bold: true }
                    }
                    Text {
                        text:  "ID: P-00142  ·  John Doe, 58M"
                        color: "#90caf9"
                        font.pixelSize: 12
                    }
                    Item { Layout.fillWidth: true }
                    Text {
                        text:  root.currentTime
                        color: "#b0bec5"
                        font { pixelSize: 13; family: "Courier New" }
                    }
                    Item { width: 12 }

                    // Alarm button — pulses red when alarmActive is true
                    Button {
                        id: alarmBtn
                        text: "⚠  ALARMS"
                        onClicked: stackView.push(alarmComponent)

                        contentItem: Text {
                            text: parent.text
                            color: root.alarmActive ? "#ff1744" : "#ffcc02"
                            font { bold: true; pixelSize: 13 }
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment:   Text.AlignVCenter
                        }
                        background: Rectangle {
                            color:        parent.hovered ? "#2a3a60" : "#1e2a4a"
                            border.color: root.alarmActive ? "#ff1744" : "#ffcc02"
                            border.width: 1
                            radius: 4

                            // Blink when alarm is active — impossible to do this simply in Widgets
                            SequentialAnimation on opacity {
                                running: root.alarmActive
                                loops:   Animation.Infinite
                                NumberAnimation { to: 0.4; duration: 600 }
                                NumberAnimation { to: 1.0; duration: 600 }
                            }
                        }
                    }
                }

                // Divider
                Rectangle { height: 1; Layout.fillWidth: true; color: "#1e2a4a" }

                // ── Vital cards row ──────────────────────────────────────────
                // QML: Four VitalCard instances with property bindings.
                // Any change to root.heartRate automatically updates the display.
                // Widgets: Required manual label.setText() calls in a timer slot.
                RowLayout {
                    Layout.fillWidth: true
                    spacing: 10

                    VitalCard {
                        title:       "HEART RATE"
                        value:       root.heartRate.toString()
                        unit:        "bpm"
                        accentColor: "#00e676"
                        alarmState:  root.heartRate < 50 || root.heartRate > 120
                    }
                    VitalCard {
                        title:       "SpO₂"
                        value:       root.spo2.toString()
                        unit:        "%"
                        accentColor: "#00bcd4"
                        alarmState:  root.spo2 < 90
                    }
                    VitalCard {
                        title:       "BLOOD PRESSURE"
                        value:       root.bpSys + "/" + root.bpDia
                        unit:        "mmHg"
                        accentColor: "#ffc107"
                        alarmState:  root.bpSys > 160 || root.bpSys < 90
                    }
                    VitalCard {
                        title:       "TEMPERATURE"
                        value:       root.temp.toFixed(1)
                        unit:        "°C"
                        accentColor: "#ff8a65"
                        alarmState:  root.temp > 38.5 || root.temp < 35.5
                    }
                }

                // ── Waveforms ────────────────────────────────────────────────
                // QML: WaveformView component instances — clean, reusable, no C++.
                WaveformView {
                    Layout.fillWidth: true
                    implicitHeight:   110
                    waveColor: "#00e676"
                    label:     "ECG  ·  Lead II"
                    amplitude: 1.0
                }

                WaveformView {
                    Layout.fillWidth: true
                    implicitHeight:   85
                    waveColor: "#00bcd4"
                    label:     "PLETH  ·  SpO₂"
                    amplitude: 0.6
                }

                // ── Alarm banner ─────────────────────────────────────────────
                // Visible binding — in Widgets this needed setVisible() calls.
                // Here it's just a property expression.
                Rectangle {
                    Layout.fillWidth: true
                    height:  36
                    radius:  4
                    color:   "#b71c1c"
                    visible: root.alarmActive

                    Text {
                        anchors.centerIn: parent
                        text:  "⚠  ALARM: Heart Rate out of range — click ALARMS to view"
                        color: "white"
                        font { pixelSize: 13; bold: true }
                    }

                    // Fade-in animation when alarm appears
                    Behavior on visible {
                        NumberAnimation { property: "opacity"; from: 0; to: 1; duration: 300 }
                    }
                }

                // ── Status bar ───────────────────────────────────────────────
                RowLayout {
                    Layout.fillWidth: true

                    Text { text: "🔋 AC Power";           color: "#66bb6a"; font.pixelSize: 11 }
                    Item { Layout.fillWidth: true }
                    Text { text: "● Network: Connected";  color: "#42a5f5"; font.pixelSize: 11 }
                }
            }
        }
    }

    // ──────────────────────────────────────────────────────────────────────────
    //  Screen 2: Alarm Panel
    // ──────────────────────────────────────────────────────────────────────────
    Component {
        id: alarmComponent

        AlarmPanel {
            onBackRequested: stackView.pop()
        }
    }
}
