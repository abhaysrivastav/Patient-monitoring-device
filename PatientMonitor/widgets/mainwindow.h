#pragma once

#include <QMainWindow>
#include <QLabel>
#include <QTimer>
#include <QStackedWidget>
#include <QFrame>
#include <QListWidget>
#include "waveformwidget.h"

/**
 * MainWindow — Patient Monitor (Qt Widgets version)
 * --------------------------------------------------
 * Two screens managed by a QStackedWidget:
 *   Page 0: Vitals Dashboard  (heart rate, SpO2, BP, temperature + waveforms)
 *   Page 1: Alarm Panel       (alarm list with limits, acknowledge button)
 *
 * All layout is done programmatically in C++ — no .ui file, so you can see
 * exactly what Qt Widgets requires vs. the declarative QML approach.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void updateVitals();      // Simulates live patient data changes
    void updateClock();       // Updates the header clock
    void goToAlarmScreen();
    void goToMainScreen();
    void acknowledgeAlarm();

private:
    // ── UI Setup ────────────────────────────────────────────────────────────
    void setupUI();
    QWidget *buildMainScreen();
    QWidget *buildAlarmScreen();

    // Helper: creates one coloured vital-sign card with a title and value label
    QFrame *createVitalCard(const QString &title,
                             const QString &unit,
                             const QColor  &borderColor,
                             QLabel       *&outValueLabel);

    // ── Widgets ─────────────────────────────────────────────────────────────
    QStackedWidget  *m_stack;
    QWidget         *m_mainScreen;
    QWidget         *m_alarmScreen;

    // Vital-value labels (updated by the simulation timer)
    QLabel  *m_hrLabel;
    QLabel  *m_spo2Label;
    QLabel  *m_bpLabel;
    QLabel  *m_tempLabel;
    QLabel  *m_timeLabel;
    QLabel  *m_alarmBanner;

    // Scrolling waveforms
    WaveformWidget  *m_ecgWaveform;
    WaveformWidget  *m_plethWaveform;

    // Alarm list widget (Page 1)
    QListWidget     *m_alarmList;

    // ── Timers ──────────────────────────────────────────────────────────────
    QTimer  *m_vitalsTimer;
    QTimer  *m_clockTimer;

    // ── Simulated patient state ──────────────────────────────────────────────
    int    m_heartRate;
    int    m_spo2;
    int    m_bpSys;
    int    m_bpDia;
    double m_temp;
    bool   m_alarmActive;
};
