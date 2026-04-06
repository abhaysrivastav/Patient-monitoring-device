// WaveformView.qml
// ─────────────────────────────────────────────────────────────────────────────
// Scrolling ECG / plethysmography waveform using QML Canvas.
//
// WIDGETS vs QML comparison:
//   Widgets: Required a custom QWidget subclass (waveformwidget.h/cpp),
//            QPainter calls inside paintEvent(), and a QTimer.
//            ~130 lines of C++.
//
//   QML:     Canvas element + JavaScript + Timer.
//            The drawing logic is identical, but no C++ subclassing is needed.
//            Animations, sizing, and color can be changed via property bindings.
// ─────────────────────────────────────────────────────────────────────────────

import QtQuick 2.15

Item {
    id: root

    property color  waveColor: "#00e676"
    property string label:     "ECG · Lead II"
    property real   amplitude: 1.0    // scale the R-peak height (0.0 – 1.0)

    implicitWidth:  400
    implicitHeight: 100

    // ── Ring buffer (JavaScript array) ────────────────────────────────────────
    // Holds normalised sample values [0.0, 1.0]. Same concept as QVector<double>
    // in the Widgets version, but lives entirely in JS.
    property var  _buffer:   []
    property int  _writePos: 0
    property real _phase:    0.0
    property int  _bufSize:  600

    Component.onCompleted: {
        _buffer = new Array(_bufSize).fill(0.5)
    }

    // ── ECG sample generator ──────────────────────────────────────────────────
    // Mirrors generateECGSample() from waveformwidget.cpp.
    function generateSample() {
        var cycle = _phase % 1.0
        var val   = 0.5   // baseline

        if (cycle < 0.10) {
            val = 0.5 + 0.08 * Math.sin(cycle / 0.10 * Math.PI)
        } else if (cycle < 0.12) {
            val = 0.5 - 0.05 * ((cycle - 0.10) / 0.02)
        } else if (cycle < 0.15) {
            val = 0.45 + 0.50 * root.amplitude * ((cycle - 0.12) / 0.03)
        } else if (cycle < 0.18) {
            var peak = 0.45 + 0.50 * root.amplitude
            val = peak * (1.0 - (cycle - 0.15) / 0.03) + 0.30 * ((cycle - 0.15) / 0.03)
        } else if (cycle < 0.24) {
            val = 0.30 + 0.20 * ((cycle - 0.18) / 0.06)
        } else if (cycle >= 0.30 && cycle < 0.52) {
            val = 0.5 + 0.14 * Math.sin((cycle - 0.30) / 0.22 * Math.PI)
        }

        _phase += 0.008
        if (_phase >= 1.0) _phase -= 1.0

        return val
    }

    // ── Timer ticks the waveform forward (same role as QTimer in Widgets) ─────
    Timer {
        interval: 20     // 50 fps
        running:  true
        repeat:   true
        onTriggered: {
            root._buffer[root._writePos] = root.generateSample()
            root._writePos = (root._writePos + 1) % root._bufSize
            waveCanvas.requestPaint()
        }
    }

    // ── Canvas element ────────────────────────────────────────────────────────
    // The Canvas API is nearly identical to HTML5 Canvas.
    // This replaces the entire paintEvent() method from the Widgets version.
    Canvas {
        id: waveCanvas
        anchors.fill: parent

        onPaint: {
            var ctx = getContext("2d")
            var W   = width
            var H   = height
            var margin = 8

            // Background
            ctx.fillStyle = "#0a120a"
            ctx.fillRect(0, 0, W, H)

            // Grid lines
            ctx.strokeStyle = "#1a3a1a"
            ctx.lineWidth = 1
            ctx.setLineDash([3, 5])
            ctx.beginPath()
            for (var r = 1; r < 4; r++) {
                var gy = H * r / 4
                ctx.moveTo(0, gy)
                ctx.lineTo(W, gy)
            }
            for (var gx = 0; gx < W; gx += 50) {
                ctx.moveTo(gx, 0)
                ctx.lineTo(gx, H)
            }
            ctx.stroke()
            ctx.setLineDash([])

            // Waveform path
            ctx.strokeStyle = root.waveColor
            ctx.lineWidth   = 2
            ctx.lineJoin    = "round"
            ctx.lineCap     = "round"
            ctx.beginPath()

            var bufLen = root._bufSize
            for (var px = 0; px < W; px++) {
                var idx = (root._writePos + Math.floor(px * bufLen / W)) % bufLen
                var sample = root._buffer[idx]
                var x = px
                var y = margin + (1.0 - sample) * (H - 2 * margin)
                if (px === 0) ctx.moveTo(x, y)
                else          ctx.lineTo(x, y)
            }
            ctx.stroke()

            // Label
            ctx.fillStyle = "white"
            ctx.font      = "bold 11px 'Courier New', monospace"
            ctx.fillText(root.label, 6, margin + 13)
        }
    }
}
