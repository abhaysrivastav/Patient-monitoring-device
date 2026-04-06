// AlarmPanel.qml  —  Screen 2: Alarm Management
// ─────────────────────────────────────────────────────────────────────────────
// Demonstrates QML's ListView + model/delegate pattern.
//
// WIDGETS vs QML comparison:
//   Widgets used QGridLayout + manual QLabel creation in a loop (C++).
//   QML uses ListModel + ListView + a delegate component — far more scalable.
//   Adding a new alarm type = one new ListElement in the model, zero C++ changes.
// ─────────────────────────────────────────────────────────────────────────────

import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15

Rectangle {
    id: root
    color: "#0d111e"

    signal backRequested    // emitted when Back button is pressed

    // ── Alarm data model ──────────────────────────────────────────────────────
    // In Widgets, this data was hardcoded into QListWidget items.
    // In QML, it's a proper data model — bindings keep the UI in sync automatically.
    ListModel {
        id: alarmModel

        ListElement { vital: "Heart Rate";     current: "72 bpm";   lowLimit: "50";   highLimit: "120";  unit: "bpm";  status: "NORMAL" }
        ListElement { vital: "SpO₂";           current: "98%";      lowLimit: "90";   highLimit: "—";    unit: "%";    status: "NORMAL" }
        ListElement { vital: "Systolic BP";    current: "120 mmHg"; lowLimit: "90";   highLimit: "160";  unit: "mmHg"; status: "NORMAL" }
        ListElement { vital: "Temperature";    current: "36.8°C";   lowLimit: "35.5"; highLimit: "38.5"; unit: "°C";   status: "NORMAL" }
    }

    ColumnLayout {
        anchors { fill: parent; margins: 16 }
        spacing: 12

        // ── Header ──────────────────────────────────────────────────────────
        RowLayout {
            Layout.fillWidth: true

            Text {
                text:  "⚠  ALARM MANAGEMENT"
                color: "#ffcc02"
                font { pixelSize: 20; bold: true }
            }

            Item { Layout.fillWidth: true }

            Button {
                text: "◀  Back to Monitor"
                onClicked: root.backRequested()
                contentItem: Text {
                    text: parent.text
                    color: "#90caf9"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment:   Text.AlignVCenter
                }
                background: Rectangle {
                    color:        parent.hovered ? "#2a3a60" : "#1e2a4a"
                    border.color: "#42a5f5"
                    border.width: 1
                    radius: 4
                }
            }
        }

        Rectangle { height: 1; Layout.fillWidth: true; color: "#1e2a4a" }

        // ── Column headers ───────────────────────────────────────────────────
        RowLayout {
            Layout.fillWidth: true
            spacing: 0

            Repeater {
                model: ["VITAL", "CURRENT", "LOW LIMIT", "HIGH LIMIT", "STATUS"]
                delegate: Text {
                    Layout.fillWidth: true
                    text:  modelData
                    color: "#78909c"
                    font { pixelSize: 11; bold: true }
                }
            }
        }

        // ── Alarm rows (ListView + delegate) ─────────────────────────────────
        // This replaces the manual QGridLayout + QLabel loop in Widgets.
        // Adding more rows = adding ListElements to alarmModel — no C++ needed.
        ListView {
            id: alarmListView
            Layout.fillWidth:  true
            Layout.fillHeight: true
            model: alarmModel
            spacing: 1
            clip:    true

            delegate: Rectangle {
                width:  alarmListView.width
                height: 48
                color:  index % 2 === 0 ? "#10192e" : "#12202e"
                radius: 4

                // Pulsing red border when alarm is active
                border.color: status === "ALARM" ? "#f44336" : "transparent"
                border.width: status === "ALARM" ? 1 : 0

                RowLayout {
                    anchors { fill: parent; leftMargin: 8; rightMargin: 8 }
                    spacing: 0

                    Text { Layout.fillWidth: true; text: vital;     color: "white";   font.pixelSize: 13 }
                    Text { Layout.fillWidth: true; text: current;   color: "#b0bec5"; font.pixelSize: 13 }
                    Text { Layout.fillWidth: true; text: lowLimit;  color: "#ffb74d"; font.pixelSize: 13 }
                    Text { Layout.fillWidth: true; text: highLimit; color: "#ef9a9a"; font.pixelSize: 13 }

                    // Status badge — in Widgets this required per-item stylesheet tweaks
                    Rectangle {
                        Layout.fillWidth:  true
                        height: 24
                        radius: 12
                        color:  status === "NORMAL" ? "#1b5e20" : "#b71c1c"

                        Text {
                            anchors.centerIn: parent
                            text:  status === "NORMAL" ? "✓  Normal" : "⚠  ALARM"
                            color: status === "NORMAL" ? "#66bb6a" : "#ff8a80"
                            font { pixelSize: 11; bold: true }
                        }
                    }
                }
            }
        }

        // ── Acknowledge button ───────────────────────────────────────────────
        Button {
            Layout.fillWidth: true
            text: "✓  Acknowledge All Alarms"
            implicitHeight: 46

            onClicked: {
                for (var i = 0; i < alarmModel.count; i++) {
                    alarmModel.setProperty(i, "status", "NORMAL")
                }
            }

            contentItem: Text {
                text: parent.text
                color: "white"
                font { pixelSize: 14; bold: true }
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment:   Text.AlignVCenter
            }
            background: Rectangle {
                color:        parent.hovered ? "#2e7d32" : "#1b5e20"
                border.color: "#66bb6a"
                border.width: 1
                radius: 4
            }
        }
    }
}
