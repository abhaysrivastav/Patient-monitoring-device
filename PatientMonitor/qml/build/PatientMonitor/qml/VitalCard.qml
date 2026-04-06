// VitalCard.qml
// ─────────────────────────────────────────────────────────────────────────────
// Reusable vital-sign display card.
//
// WIDGETS vs QML comparison:
//   In Widgets, createVitalCard() is a C++ function that manually creates
//   QFrame, QVBoxLayout, and three QLabels — ~20 lines of boilerplate per card.
//
//   Here, VitalCard is a self-contained declarative component.
//   The caller simply writes:
//       VitalCard { title: "HEART RATE"; value: "72"; unit: "bpm"; accentColor: "#00e676" }
//   and QML handles layout, sizing, and rendering.
// ─────────────────────────────────────────────────────────────────────────────

import QtQuick 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: root

    // ── Public properties (API of this component) ────────────────────────────
    property string title:       "VITAL"
    property string value:       "--"
    property string unit:        ""
    property color  accentColor: "#00e676"
    property bool   alarmState:  false   // true = blink the border red

    // ── Layout ────────────────────────────────────────────────────────────────
    Layout.fillWidth: true
    implicitHeight:   130
    radius:           8

    // Dark card background + coloured border
    color:  "#10192e"
    border.color: alarmState ? "#f44336" : accentColor
    border.width: 2

    // ── Alarm blink animation ─────────────────────────────────────────────────
    // NOTE: In Qt Widgets, you'd need a QTimer + manual border-color toggle in C++.
    //       In QML, SequentialAnimation makes this a 4-line declaration.
    SequentialAnimation on border.color {
        running: alarmState
        loops:   Animation.Infinite
        ColorAnimation { to: "#f44336"; duration: 500 }
        ColorAnimation { to: "#7f1111"; duration: 500 }
    }

    // ── Content ───────────────────────────────────────────────────────────────
    ColumnLayout {
        anchors.centerIn: parent
        spacing: 4

        Text {
            Layout.alignment: Qt.AlignHCenter
            text:  root.title
            color: root.accentColor
            font { pixelSize: 11; bold: true; family: "Arial" }
        }

        Text {
            id: valueText
            Layout.alignment: Qt.AlignHCenter
            text:  root.value
            color: root.alarmState ? "#f44336" : root.accentColor
            font { pixelSize: 40; bold: true; family: "Courier New" }

            // Smooth value change animation — trivial in QML, hard in Widgets
            Behavior on text {
                SequentialAnimation {
                    NumberAnimation { target: valueText; property: "opacity"; to: 0.3; duration: 100 }
                    NumberAnimation { target: valueText; property: "opacity"; to: 1.0; duration: 100 }
                }
            }
        }

        Text {
            Layout.alignment: Qt.AlignHCenter
            text:  root.unit
            color: "#78909c"
            font { pixelSize: 11 }
        }
    }
}
