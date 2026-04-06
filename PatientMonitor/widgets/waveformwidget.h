#pragma once

#include <QWidget>
#include <QTimer>
#include <QVector>
#include <QColor>
#include <QString>

/**
 * WaveformWidget
 * --------------
 * A custom QWidget that draws a scrolling, ECG-style waveform using QPainter.
 * This is the "old" Qt Widgets approach — all drawing is manual C++ code.
 *
 * In the QML version, this is replaced by a Canvas element with JavaScript,
 * which is much more declarative and easier to animate/style.
 */
class WaveformWidget : public QWidget
{
    Q_OBJECT

public:
    explicit WaveformWidget(QWidget *parent = nullptr);

    void setWaveColor(const QColor &color);
    void setLabel(const QString &label);
    void setAmplitude(double amplitude);   // 0.0 – 1.0

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void tick();   // Called every 20ms by internal timer

private:
    double generateECGSample();   // Produces a physiologically plausible ECG value

    QTimer         *m_timer;
    QVector<double> m_buffer;     // Circular ring buffer of waveform samples
    int             m_writePos;   // Current write position in the ring buffer
    QColor          m_color;
    QString         m_label;
    double          m_phase;      // Tracks position in the cardiac cycle [0, 1)
    double          m_amplitude;
};
