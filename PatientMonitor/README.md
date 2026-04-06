# Patient Monitor — Qt Widgets → QML Migration Demo

A medical device UI prototype demonstrating how a **Qt Widgets** application is
migrated to **QML**. Two screens:

- **Screen 1 — Vitals Dashboard:** Heart Rate, SpO₂, Blood Pressure, Temperature
  with live scrolling ECG and pleth waveforms.
- **Screen 2 — Alarm Panel:** Alarm limits table, active alarm list, acknowledge
  button.

---

## 1. Install Qt (no Qt installed yet)

### Download Qt Online Installer
1. Go to **https://www.qt.io/download-open-source**
2. Click **"Download the Qt Online Installer"**
3. Run the installer and sign in / create a free Qt account
4. In the component selection screen, choose:
   - **Qt 6.7** (or latest Qt 6.x) → tick:
     - `Desktop Qt 6.7.x MSVC 2022 64-bit`  *(Windows)*
       or `Desktop Qt 6.7.x GCC 64-bit`  *(Linux)*
       or `Desktop Qt 6.7.x macOS`  *(macOS)*
   - Under **Qt 6.7.x → Additional Libraries** → tick `Qt Quick` and `Qt Quick Controls`
   - Under **Developer and Designer Tools** → tick `CMake` and `Ninja`
5. Finish installation. Default path is:
   - Windows: `C:\Qt\6.7.x\`
   - Linux/macOS: `~/Qt/6.7.x/`

> **Qt 5 fallback:** Both CMakeLists.txt files support Qt5 as a fallback.
> If you use Qt 5, install Qt 5.15.x with the same component names.

---

## 2. Set Up VSCode for Qt/QML

### 2a. Install VSCode Extensions

Open VSCode → Extensions (Ctrl+Shift+X) → search and install:

| Extension | Publisher | Purpose |
|-----------|-----------|---------|
| **C/C++** | Microsoft | IntelliSense, debugging |
| **CMake Tools** | Microsoft | Build system integration |
| **Qt All Extensions Pack** | Qt Group | QML syntax, Qt docs |
| **QML** | bbenoist | QML syntax highlighting |

### 2b. Tell CMake Tools where Qt is installed

Open VSCode → Command Palette (Ctrl+Shift+P) → `CMake: Edit User-Local CMake Kits`

This opens `cmake-tools-kits.json`. Add your Qt path to the compiler entry:

```json
{
  "name": "Qt 6 MSVC 2022",
  "compilers": {
    "CXX": "cl.exe"
  },
  "cmakeSettings": {
    "CMAKE_PREFIX_PATH": "C:/Qt/6.7.4/msvc2022_64"
  }
}
```

**On Linux/macOS** replace the path with e.g. `~/Qt/6.7.4/gcc_64`.

Alternatively, create a `.vscode/settings.json` inside each project folder:

```json
{
  "cmake.configureArgs": [
    "-DCMAKE_PREFIX_PATH=C:/Qt/6.7.4/msvc2022_64"
  ]
}
```

### 2c. Optional — Windows PATH

Add Qt's `bin` folder to your `PATH` so Qt DLLs are found at runtime:

```
C:\Qt\6.7.4\msvc2022_64\bin
```

*(System Properties → Environment Variables → Path → Edit → New)*

---

## 3. Build and Run

### Qt Widgets version

```bash
cd PatientMonitor/widgets
mkdir build && cd build
cmake .. -DCMAKE_PREFIX_PATH="C:/Qt/6.7.4/msvc2022_64"
cmake --build . --config Release
./PatientMonitor_Widgets          # Linux/macOS
Release\PatientMonitor_Widgets.exe  # Windows
```

### QML version

```bash
cd PatientMonitor/qml
mkdir build && cd build
cmake .. -DCMAKE_PREFIX_PATH="C:/Qt/6.7.4/msvc2022_64"
cmake --build . --config Release
./PatientMonitor_QML
```

### Using VSCode (recommended workflow)

1. Open the `PatientMonitor/widgets` or `PatientMonitor/qml` folder in VSCode
   (`File → Open Folder`)
2. VSCode detects CMakeLists.txt automatically (CMake Tools extension)
3. Select your kit when prompted (choose the Qt 6 kit you configured)
4. Press **F7** to build, **F5** to run/debug

---

## 4. Project Structure

```
PatientMonitor/
├── README.md                     ← This file
│
├── widgets/                      ← Qt Widgets version (C++)
│   ├── CMakeLists.txt
│   ├── main.cpp                  Entry point — QApplication
│   ├── mainwindow.h/.cpp         Two-screen window (QStackedWidget)
│   ├── waveformwidget.h/.cpp     Custom QPainter waveform widget
│
└── qml/                          ← QML version
    ├── CMakeLists.txt
    ├── main.cpp                  Entry point — QGuiApplication + QQmlEngine
    └── qml/
        ├── main.qml              Root window + application state + Screen 1
        ├── VitalCard.qml         Reusable vital-sign card component
        ├── WaveformView.qml      Canvas waveform (replaces WaveformWidget)
        └── AlarmPanel.qml        Screen 2 — alarm management
```

---

## 5. Key Migration Concepts (Widgets → QML)

This table summarises every significant change between the two versions.
Use this as talking points when presenting the migration to a client.

| Concern | Qt Widgets (C++) | QML |
|---------|-----------------|-----|
| **Entry point** | `QApplication` | `QGuiApplication` + `QQmlApplicationEngine` |
| **Layout** | `QVBoxLayout`, `QHBoxLayout`, `QGridLayout` — all in C++ | Anchors, `ColumnLayout`, `RowLayout` — declarative |
| **Custom drawing** | Subclass `QWidget`, override `paintEvent()`, use `QPainter` | `Canvas` element + JavaScript `onPaint` handler |
| **Timers** | `QTimer` + `connect()` + slot method | `Timer { onTriggered: ... }` inline |
| **Screen navigation** | `QStackedWidget::setCurrentIndex(int)` | `StackView.push()` / `pop()` with animated transitions |
| **Reusable components** | C++ function returning `QFrame*` (no true encapsulation) | `.qml` file = self-contained component with typed properties |
| **Animations** | `QPropertyAnimation` or manual timer + repaint | `Behavior on`, `NumberAnimation`, `SequentialAnimation` — declarative |
| **Data bindings** | `label->setText(QString::number(heartRate))` in a slot | `text: root.heartRate.toString()` — automatic, reactive |
| **Styling** | `setStyleSheet("...")` per widget — hard to maintain | Inline or component-level `color:`, `font:` properties |
| **Alarm blinking** | `QTimer` + toggle border colour in a slot | `SequentialAnimation on border.color { ... }` — 4 lines |
| **List/table views** | `QListWidget` or `QTableWidget` — fixed widget | `ListView` + `ListModel` + delegate — dynamic and scalable |

### Why QML for medical devices?

- **GPU-accelerated rendering** via Qt Quick scene graph — no X11/GDI overhead.
- **Smooth 60fps animations** out of the box — important for waveform displays.
- **Separation of concerns** — UI in QML, business logic in C++ via `Q_PROPERTY`
  and signals/slots. Clinicians can tweak the UI without touching safety-critical C++.
- **Hot reload** during development — change a `.qml` file, see result immediately.
- **Scalable to embedded** — Qt Quick runs on bare-metal Linux (no full desktop stack).

---

## 6. Next Steps (expanding the prototype)

- Expose a C++ `PatientDataModel` via `setContextProperty()` to feed real sensor data into QML.
- Add `Q_PROPERTY` macros for heart rate, SpO₂ etc. so QML bindings work across the C++/QML boundary.
- Implement IEC 62304 compliant alarm logic in C++ (not JS) for production.
- Use `Qt Quick Controls 2` styles (`Material`, `Imagine`) for an off-the-shelf medical look.
- Replace `Canvas` waveform with `QQuickItem` (C++ scene-graph node) for maximum performance on embedded.

---

*Generated as a Qt Widgets → QML migration demonstration prototype.*
