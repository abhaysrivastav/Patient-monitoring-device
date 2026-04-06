#include "mainwindow.h"

#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QFrame>
#include <QFont>
#include <QDateTime>
#include <QRandomGenerator>
#include <QListWidgetItem>
#include <QSizePolicy>

// ─────────────────────────────────────────────────────────────────────────────
//  Constructor
// ─────────────────────────────────────────────────────────────────────────────
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_heartRate(72)
    , m_spo2(98)
    , m_bpSys(120)
    , m_bpDia(80)
    , m_temp(36.8)
    , m_alarmActive(false)
{
    setWindowTitle("Patient Monitor — Qt Widgets Version");
    setMinimumSize(900, 600);
    resize(1024, 680);

    // Dark palette for the whole application
    QPalette pal = palette();
    pal.setColor(QPalette::Window,     QColor(13, 17, 30));
    pal.setColor(QPalette::WindowText, Qt::white);
    pal.setColor(QPalette::Base,       QColor(20, 26, 46));
    pal.setColor(QPalette::Text,       Qt::white);
    setPalette(pal);
    setStyleSheet("QWidget { background-color: #0d111e; color: white; }");

    setupUI();

    // ── Simulation timer: update vitals every 2 seconds ─────────────────────
    m_vitalsTimer = new QTimer(this);
    connect(m_vitalsTimer, &QTimer::timeout, this, &MainWindow::updateVitals);
    m_vitalsTimer->start(2000);

    // ── Clock timer: update timestamp every second ───────────────────────────
    m_clockTimer = new QTimer(this);
    connect(m_clockTimer, &QTimer::timeout, this, &MainWindow::updateClock);
    m_clockTimer->start(1000);
    updateClock();  // call immediately so we don't wait 1s on startup
}

// ─────────────────────────────────────────────────────────────────────────────
//  setupUI
// ─────────────────────────────────────────────────────────────────────────────
void MainWindow::setupUI()
{
    m_stack = new QStackedWidget(this);
    m_mainScreen  = buildMainScreen();
    m_alarmScreen = buildAlarmScreen();

    m_stack->addWidget(m_mainScreen);    // index 0
    m_stack->addWidget(m_alarmScreen);   // index 1
    m_stack->setCurrentIndex(0);

    setCentralWidget(m_stack);
}

// ─────────────────────────────────────────────────────────────────────────────
//  buildMainScreen   (Page 0 — Vitals Dashboard)
// ─────────────────────────────────────────────────────────────────────────────
QWidget *MainWindow::buildMainScreen()
{
    QWidget *screen = new QWidget;
    QVBoxLayout *root = new QVBoxLayout(screen);
    root->setContentsMargins(12, 8, 12, 8);
    root->setSpacing(8);

    // ── Header bar ──────────────────────────────────────────────────────────
    QHBoxLayout *header = new QHBoxLayout;

    QLabel *titleLabel = new QLabel("PATIENT MONITOR");
    QFont titleFont("Arial", 16, QFont::Bold);
    titleLabel->setFont(titleFont);
    titleLabel->setStyleSheet("color: #4fc3f7;");

    QLabel *patientLabel = new QLabel("ID: P-00142  |  John Doe, 58M");
    patientLabel->setStyleSheet("color: #90caf9; font-size: 12px;");

    m_timeLabel = new QLabel;
    m_timeLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_timeLabel->setStyleSheet("color: #b0bec5; font-family: 'Courier New'; font-size: 13px;");

    QPushButton *alarmBtn = new QPushButton("⚠  ALARMS");
    alarmBtn->setStyleSheet(
        "QPushButton { background: #1e2a4a; color: #ffcc02; border: 1px solid #ffcc02;"
        "  border-radius: 4px; padding: 6px 14px; font-weight: bold; }"
        "QPushButton:hover { background: #2a3a60; }"
    );
    connect(alarmBtn, &QPushButton::clicked, this, &MainWindow::goToAlarmScreen);

    header->addWidget(titleLabel);
    header->addWidget(patientLabel);
    header->addStretch();
    header->addWidget(m_timeLabel);
    header->addSpacing(12);
    header->addWidget(alarmBtn);
    root->addLayout(header);

    // ── Separator ───────────────────────────────────────────────────────────
    QFrame *sep = new QFrame;
    sep->setFrameShape(QFrame::HLine);
    sep->setStyleSheet("color: #1e2a4a;");
    root->addWidget(sep);

    // ── Vital cards row ─────────────────────────────────────────────────────
    // Qt Widgets: we must manually specify each card's layout, size, style.
    // Compare with QML: each VitalCard is a simple reusable component instance.
    QHBoxLayout *cardsRow = new QHBoxLayout;
    cardsRow->setSpacing(10);

    QFrame *hrCard   = createVitalCard("HEART RATE",    "bpm", QColor(0, 230, 118),  m_hrLabel);
    QFrame *spo2Card = createVitalCard("SpO₂",          "%",   QColor(0, 188, 212),  m_spo2Label);
    QFrame *bpCard   = createVitalCard("BLOOD PRESSURE","mmHg",QColor(255, 193, 7),  m_bpLabel);
    QFrame *tempCard = createVitalCard("TEMPERATURE",   "°C",  QColor(255, 138, 101),m_tempLabel);

    cardsRow->addWidget(hrCard);
    cardsRow->addWidget(spo2Card);
    cardsRow->addWidget(bpCard);
    cardsRow->addWidget(tempCard);
    root->addLayout(cardsRow);

    // ── Waveforms ────────────────────────────────────────────────────────────
    // Qt Widgets: custom QWidget subclass required for canvas drawing.
    // QML: Canvas element with JS — no subclassing needed.
    m_ecgWaveform = new WaveformWidget;
    m_ecgWaveform->setWaveColor(QColor(0, 230, 118));
    m_ecgWaveform->setLabel("ECG  ·  Lead II");
    m_ecgWaveform->setMinimumHeight(100);

    m_plethWaveform = new WaveformWidget;
    m_plethWaveform->setWaveColor(QColor(0, 188, 212));
    m_plethWaveform->setLabel("PLETH  ·  SpO₂");
    m_plethWaveform->setAmplitude(0.6);
    m_plethWaveform->setMinimumHeight(80);

    root->addWidget(m_ecgWaveform);
    root->addWidget(m_plethWaveform);

    // ── Alarm banner (hidden unless alarm fires) ──────────────────────────
    m_alarmBanner = new QLabel("  ⚠  ALARM: Heart Rate HIGH  |  Click ALARMS to view");
    m_alarmBanner->setAlignment(Qt::AlignCenter);
    m_alarmBanner->setStyleSheet(
        "background: #b71c1c; color: white; font-weight: bold;"
        "font-size: 13px; padding: 6px; border-radius: 4px;"
    );
    m_alarmBanner->setVisible(false);
    root->addWidget(m_alarmBanner);

    // ── Status bar ──────────────────────────────────────────────────────────
    QHBoxLayout *statusBar = new QHBoxLayout;
    QLabel *battLabel = new QLabel("🔋 AC Power");
    battLabel->setStyleSheet("color: #66bb6a; font-size: 11px;");
    QLabel *netLabel  = new QLabel("● Network: Connected");
    netLabel->setStyleSheet("color: #42a5f5; font-size: 11px;");
    statusBar->addWidget(battLabel);
    statusBar->addStretch();
    statusBar->addWidget(netLabel);
    root->addLayout(statusBar);

    return screen;
}

// ─────────────────────────────────────────────────────────────────────────────
//  buildAlarmScreen   (Page 1 — Alarm Panel)
// ─────────────────────────────────────────────────────────────────────────────
QWidget *MainWindow::buildAlarmScreen()
{
    QWidget *screen = new QWidget;
    QVBoxLayout *root = new QVBoxLayout(screen);
    root->setContentsMargins(16, 12, 16, 12);
    root->setSpacing(10);

    // ── Header ──────────────────────────────────────────────────────────────
    QHBoxLayout *header = new QHBoxLayout;
    QLabel *title = new QLabel("⚠  ALARM MANAGEMENT");
    title->setFont(QFont("Arial", 15, QFont::Bold));
    title->setStyleSheet("color: #ffcc02;");

    QPushButton *backBtn = new QPushButton("◀  Back to Monitor");
    backBtn->setStyleSheet(
        "QPushButton { background: #1e2a4a; color: #90caf9; border: 1px solid #42a5f5;"
        "  border-radius: 4px; padding: 6px 14px; }"
        "QPushButton:hover { background: #2a3a60; }"
    );
    connect(backBtn, &QPushButton::clicked, this, &MainWindow::goToMainScreen);

    header->addWidget(title);
    header->addStretch();
    header->addWidget(backBtn);
    root->addLayout(header);

    QFrame *sep = new QFrame;
    sep->setFrameShape(QFrame::HLine);
    sep->setStyleSheet("color: #1e2a4a;");
    root->addWidget(sep);

    // ── Alarm limit table ────────────────────────────────────────────────────
    // Note: In Qt Widgets, building tables/lists requires QTableWidget or
    // manual layouts. QML's ListView + delegate model is far more flexible.
    QLabel *limitsTitle = new QLabel("Alarm Limits");
    limitsTitle->setFont(QFont("Arial", 11, QFont::Bold));
    limitsTitle->setStyleSheet("color: #90caf9;");
    root->addWidget(limitsTitle);

    struct AlarmLimit {
        QString vital;
        QString low;
        QString high;
        QString unit;
    };
    QList<AlarmLimit> limits = {
        {"Heart Rate",    "50",  "120",  "bpm"},
        {"SpO₂",          "90",  "—",    "%"},
        {"Systolic BP",   "90",  "160",  "mmHg"},
        {"Temperature",   "35.5","38.5", "°C"},
    };

    QGridLayout *grid = new QGridLayout;
    grid->setHorizontalSpacing(20);
    grid->setVerticalSpacing(6);

    // Header row
    auto *hVital = new QLabel("Vital");      hVital->setStyleSheet("color:#78909c; font-weight:bold;");
    auto *hLow   = new QLabel("Low Limit");  hLow->setStyleSheet("color:#78909c; font-weight:bold;");
    auto *hHigh  = new QLabel("High Limit"); hHigh->setStyleSheet("color:#78909c; font-weight:bold;");
    auto *hUnit  = new QLabel("Unit");       hUnit->setStyleSheet("color:#78909c; font-weight:bold;");
    grid->addWidget(hVital, 0, 0);
    grid->addWidget(hLow,   0, 1);
    grid->addWidget(hHigh,  0, 2);
    grid->addWidget(hUnit,  0, 3);

    for (int i = 0; i < limits.size(); ++i) {
        grid->addWidget(new QLabel(limits[i].vital), i+1, 0);
        grid->addWidget(new QLabel(limits[i].low),   i+1, 1);
        grid->addWidget(new QLabel(limits[i].high),  i+1, 2);
        grid->addWidget(new QLabel(limits[i].unit),  i+1, 3);
    }
    root->addLayout(grid);

    // ── Active alarms list ────────────────────────────────────────────────────
    QLabel *activeTitle = new QLabel("Active Alarms");
    activeTitle->setFont(QFont("Arial", 11, QFont::Bold));
    activeTitle->setStyleSheet("color: #ef9a9a;");
    root->addWidget(activeTitle);

    m_alarmList = new QListWidget;
    m_alarmList->setStyleSheet(
        "QListWidget { background: #12192d; border: 1px solid #1e2a4a;"
        "  border-radius: 4px; color: white; font-size: 12px; }"
        "QListWidget::item { padding: 8px; border-bottom: 1px solid #1e2a4a; }"
        "QListWidget::item:selected { background: #1e2a4a; }"
    );

    // Pre-populate with some sample alarms
    auto addAlarm = [&](const QString &text, const QColor &color) {
        QListWidgetItem *item = new QListWidgetItem(text, m_alarmList);
        item->setForeground(color);
    };
    addAlarm("✓  SpO₂  —  NORMAL  (Current: 98%)",   QColor(0, 230, 118));
    addAlarm("✓  Heart Rate  —  NORMAL  (Current: 72 bpm)", QColor(0, 230, 118));
    addAlarm("✓  Blood Pressure  —  NORMAL  (120/80)", QColor(0, 230, 118));
    addAlarm("✓  Temperature  —  NORMAL  (36.8°C)",  QColor(0, 230, 118));

    root->addWidget(m_alarmList, 1);  // stretch factor 1 so it takes remaining space

    // ── Acknowledge button ────────────────────────────────────────────────────
    QPushButton *ackBtn = new QPushButton("✓  Acknowledge All Alarms");
    ackBtn->setStyleSheet(
        "QPushButton { background: #1b5e20; color: white; font-weight: bold;"
        "  border: 1px solid #66bb6a; border-radius: 4px; padding: 10px; font-size: 13px; }"
        "QPushButton:hover { background: #2e7d32; }"
        "QPushButton:pressed { background: #1b5e20; }"
    );
    connect(ackBtn, &QPushButton::clicked, this, &MainWindow::acknowledgeAlarm);
    root->addWidget(ackBtn);

    return screen;
}

// ─────────────────────────────────────────────────────────────────────────────
//  createVitalCard
//  Returns a styled QFrame containing the vital name, value, and unit.
//  In Qt Widgets: each card is built entirely in C++.
//  In QML: this entire block becomes < VitalCard title="HR" value="72" color="green" />
// ─────────────────────────────────────────────────────────────────────────────
QFrame *MainWindow::createVitalCard(const QString &title,
                                     const QString &unit,
                                     const QColor  &borderColor,
                                     QLabel       *&outValueLabel)
{
    QFrame *card = new QFrame;
    card->setFrameShape(QFrame::StyledPanel);
    card->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    card->setMinimumHeight(130);

    QString color = borderColor.name();
    card->setStyleSheet(QString(
        "QFrame { border: 2px solid %1; border-radius: 8px; background: #10192e; }"
    ).arg(color));

    QVBoxLayout *lay = new QVBoxLayout(card);
    lay->setAlignment(Qt::AlignCenter);
    lay->setSpacing(4);

    QLabel *titleLbl = new QLabel(title);
    titleLbl->setAlignment(Qt::AlignCenter);
    titleLbl->setStyleSheet(QString("color: %1; font-size: 11px; font-weight: bold;"
                                    "border: none;").arg(color));

    outValueLabel = new QLabel("--");
    outValueLabel->setAlignment(Qt::AlignCenter);
    outValueLabel->setStyleSheet(QString("color: %1; font-size: 38px; font-weight: bold;"
                                          "font-family: 'Courier New'; border: none;").arg(color));

    QLabel *unitLbl = new QLabel(unit);
    unitLbl->setAlignment(Qt::AlignCenter);
    unitLbl->setStyleSheet("color: #78909c; font-size: 11px; border: none;");

    lay->addWidget(titleLbl);
    lay->addWidget(outValueLabel);
    lay->addWidget(unitLbl);

    return card;
}

// ─────────────────────────────────────────────────────────────────────────────
//  Slots
// ─────────────────────────────────────────────────────────────────────────────
void MainWindow::updateVitals()
{
    auto jitter = [&](int base, int range) {
        return base + QRandomGenerator::global()->bounded(range * 2) - range;
    };
    auto jitterD = [&](double base, double range) {
        return base + (QRandomGenerator::global()->bounded(200) / 100.0 - 1.0) * range;
    };

    m_heartRate = jitter(72, 4);
    m_spo2      = qMin(100, jitter(98, 1));
    m_bpSys     = jitter(120, 5);
    m_bpDia     = jitter(80, 3);
    m_temp      = jitterD(36.8, 0.1);

    m_hrLabel->setText(QString::number(m_heartRate));
    m_spo2Label->setText(QString::number(m_spo2));
    m_bpLabel->setText(QString("%1/%2").arg(m_bpSys).arg(m_bpDia));
    m_tempLabel->setText(QString::number(m_temp, 'f', 1));

    // Trigger alarm if HR out of range
    bool newAlarm = (m_heartRate < 50 || m_heartRate > 120 || m_spo2 < 90);
    if (newAlarm != m_alarmActive) {
        m_alarmActive = newAlarm;
        m_alarmBanner->setVisible(m_alarmActive);
    }
}

void MainWindow::updateClock()
{
    m_timeLabel->setText(QDateTime::currentDateTime().toString("dd MMM yyyy   hh:mm:ss"));
}

void MainWindow::goToAlarmScreen()
{
    m_stack->setCurrentIndex(1);
}

void MainWindow::goToMainScreen()
{
    m_stack->setCurrentIndex(0);
}

void MainWindow::acknowledgeAlarm()
{
    m_alarmActive = false;
    m_alarmBanner->setVisible(false);

    // Update list to show all acknowledged
    for (int i = 0; i < m_alarmList->count(); ++i) {
        QListWidgetItem *item = m_alarmList->item(i);
        if (item->text().startsWith("⚠")) {
            item->setText("✓  " + item->text().mid(2) + " — Acknowledged");
            item->setForeground(QColor(0, 230, 118));
        }
    }
}
