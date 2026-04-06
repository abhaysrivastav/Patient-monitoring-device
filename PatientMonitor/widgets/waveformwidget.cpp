#include "waveformwidget.h"

#include <QPainter>
#include <QPainterPath>
#include <QPen>
#include <cmath>

// ─────────────────────────────────────────────
//  Constructor
// ─────────────────────────────────────────────
WaveformWidget::WaveformWidget(QWidget *parent)
    : QWidget(parent)
    , m_writePos(0)
    , m_color(Qt::green)
    , m_label("ECG")
    , m_phase(0.0)
    , m_amplitude(1.0)
{
    setMinimumSize(300, 90);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    // Pre-fill buffer with baseline value (0.5 = middle of the widget height)
    m_buffer.fill(0.5, 600);

    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &WaveformWidget::tick);
    m_timer->start(20);  // ~50 FPS update rate
}

void WaveformWidget::setWaveColor(const QColor &color) { m_color = color; }
void WaveformWidget::setLabel(const QString &label)    { m_label = label; }
void WaveformWidget::setAmplitude(double amplitude)    { m_amplitude = amplitude; }

// ─────────────────────────────────────────────
//  ECG Sample Generator
//  Produces a simplified but realistic ECG waveform.
//  All values are in the range [0.0, 1.0] (normalised height).
//  The cardiac cycle is divided by phase (0 → 1):
//    0.00–0.10  P wave  (atrial depolarisation)
//    0.10–0.22  QRS complex (ventricular depolarisation — the big spike)
//    0.30–0.50  T wave  (ventricular repolarisation)
//    rest       Isoelectric baseline
// ─────────────────────────────────────────────
double WaveformWidget::generateECGSample()
{
    double cycle = fmod(m_phase, 1.0);
    double val   = 0.5;  // baseline

    if (cycle < 0.10) {
        // P wave — small smooth hump
        val = 0.5 + 0.08 * std::sin(cycle / 0.10 * M_PI);

    } else if (cycle < 0.12) {
        // Q dip — small negative deflection before the spike
        double t = (cycle - 0.10) / 0.02;
        val = 0.5 - 0.05 * t;

    } else if (cycle < 0.15) {
        // R peak — rapid rise to the tallest point
        double t = (cycle - 0.12) / 0.03;
        val = 0.45 + 0.50 * m_amplitude * t;

    } else if (cycle < 0.18) {
        // S wave — rapid fall below baseline
        double t = (cycle - 0.15) / 0.03;
        val = (0.45 + 0.50 * m_amplitude) * (1.0 - t) + 0.30 * t;

    } else if (cycle < 0.24) {
        // Return to baseline after S
        double t = (cycle - 0.18) / 0.06;
        val = 0.30 + 0.20 * t;

    } else if (cycle >= 0.30 && cycle < 0.52) {
        // T wave — broad smooth hump
        val = 0.5 + 0.14 * std::sin((cycle - 0.30) / 0.22 * M_PI);
    }

    // Advance phase; one full cycle ≈ 125 ticks at 20ms = ~72 bpm
    m_phase += 0.008;
    if (m_phase >= 1.0) m_phase -= 1.0;

    return val;
}

// ─────────────────────────────────────────────
//  Timer slot — write one new sample, then repaint
// ─────────────────────────────────────────────
void WaveformWidget::tick()
{
    m_buffer[m_writePos] = generateECGSample();
    m_writePos = (m_writePos + 1) % m_buffer.size();
    update();  // triggers paintEvent
}

// ─────────────────────────────────────────────
//  Paint event — draw background, grid, waveform, label
//  NOTE: All of this imperative drawing code is what QML's
//        Canvas element replaces with a much cleaner API.
// ─────────────────────────────────────────────
void WaveformWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    const int W = width();
    const int H = height();
    const double margin = 8.0;

    // ── Background ──────────────────────────────
    painter.fillRect(rect(), QColor(10, 18, 10));

    // ── Grid lines ──────────────────────────────
    QPen gridPen(QColor(25, 55, 25), 1, Qt::DotLine);
    painter.setPen(gridPen);

    int rows = 4;
    for (int r = 1; r < rows; ++r) {
        int y = H * r / rows;
        painter.drawLine(0, y, W, y);
    }
    for (int x = 0; x < W; x += 50) {
        painter.drawLine(x, 0, x, H);
    }

    // ── Waveform ────────────────────────────────
    // We walk across the pixel width, mapping each pixel to the
    // correct ring-buffer index so the waveform scrolls smoothly.
    const int bufLen = m_buffer.size();
    QPainterPath path;
    bool first = true;

    for (int px = 0; px < W; ++px) {
        int bufIdx = (m_writePos + static_cast<int>(px * (double)bufLen / W)) % bufLen;
        double sample = m_buffer[bufIdx];
        double x = px;
        double y = margin + (1.0 - sample) * (H - 2.0 * margin);

        if (first) {
            path.moveTo(x, y);
            first = false;
        } else {
            path.lineTo(x, y);
        }
    }

    painter.setPen(QPen(m_color, 2.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.drawPath(path);

    // ── Label ───────────────────────────────────
    painter.setPen(Qt::white);
    QFont font("Courier New", 9, QFont::Bold);
    painter.setFont(font);
    painter.drawText(6, static_cast<int>(margin) + 12, m_label);
}
