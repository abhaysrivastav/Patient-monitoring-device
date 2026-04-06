# Manual QML Build Guide - Step by Step

## Current Status
✅ **Widgets Version:** Running successfully  
✅ **QML Version:** Built and running (PatientMonitor_QML.exe)

---

## IMPORTANT: Deviation from Original Instructions

The original guide assumed MSVC (Visual Studio) as the compiler and Qt 6.7.0 msvc2019_64 as the Qt build.
**Actual working setup uses:**
- **Compiler:** MinGW-w64 (GCC 15.2.0) via MSYS2 at `C:\msys64\mingw64\bin\`
- **Qt:** 6.11.0 from MSYS2 at `C:\msys64\mingw64\` (requires `mingw-w64-x86_64-qt6-declarative`)
- **Build system:** Ninja (via MSYS2)
- **CMake:** MSYS2 CMake at `C:\msys64\mingw64\bin\cmake.exe`

MSVC / Qt 6.7.0 msvc2019_64 cannot be used because no Visual Studio compiler is installed.

---

## QUICK START (Actual Working Commands)

Open **Git Bash** or **MSYS2 terminal** and paste these commands ONE AT A TIME:

### Step 0: (One-time) Install Qt6 Quick via MSYS2

Only needed once. If already done, skip to Step 1.

```bash
/c/msys64/usr/bin/pacman -S --noconfirm mingw-w64-x86_64-qt6-declarative
```

### Step 1: Navigate to build directory
```bash
cd "/c/Users/Admin/Documents/Claude/Projects/Freelauncing-1/PatientMonitor/qml/build"
```

### Step 2: Clean previous attempts
```bash
rm -f CMakeCache.txt cmake_install.cmake
rm -rf CMakeFiles
ls
```
**Expected:** Only `cmake-config.log` remaining.

### Step 3: Configure CMake
```bash
PATH=/c/msys64/mingw64/bin:$PATH /c/msys64/mingw64/bin/cmake.exe .. -G "Ninja" -DCMAKE_PREFIX_PATH="/c/msys64/mingw64" -DCMAKE_BUILD_TYPE=Release
```
**Expected:** Ends with "Configuring done" and "Generating done"

### Step 4: Build
```bash
PATH=/c/msys64/mingw64/bin:$PATH /c/msys64/mingw64/bin/cmake.exe --build . --config Release -j 4
```
**Expected:** `[1/26]` through `[26/26]` steps, ending with "Linking CXX executable PatientMonitor_QML.exe"

### Step 5: Launch
```bash
PATH=/c/msys64/mingw64/bin:$PATH ./PatientMonitor_QML.exe
```
**Expected:** Application window opens with Patient Monitor UI

---

## DETAILED STEP-BY-STEP GUIDE

### Prerequisites Check

**1. Check MSYS2 MinGW g++:**
```bash
/c/msys64/mingw64/bin/g++.exe --version
```
Should output: `g++ (Rev1, Built by MSYS2 project) 15.x.x`

**2. Check CMake (MSYS2):**
```bash
/c/msys64/mingw64/bin/cmake.exe --version
```
Should output: `cmake version 4.x.x`

**3. Check Qt6 Quick installed:**
```bash
ls /c/msys64/mingw64/bin/Qt6Quick.dll
```
If not found, install it:
```bash
/c/msys64/usr/bin/pacman -S --noconfirm mingw-w64-x86_64-qt6-declarative
```

**4. Check Ninja:**
```bash
/c/msys64/mingw64/bin/ninja.exe --version
```

---

## STEP 1: Navigate to Build Directory

```bash
cd "/c/Users/Admin/Documents/Claude/Projects/Freelauncing-1/PatientMonitor/qml/build"
```

**Verify:** `pwd` should show the full path.

---

## STEP 2: Clean Previous Build Artifacts

```bash
rm -f CMakeCache.txt cmake_install.cmake
rm -rf CMakeFiles
ls
```

**Expected result:** Only `cmake-config.log` should be present

---

## STEP 3: Run CMake Configuration

```bash
PATH=/c/msys64/mingw64/bin:$PATH /c/msys64/mingw64/bin/cmake.exe .. \
  -G "Ninja" \
  -DCMAKE_PREFIX_PATH="/c/msys64/mingw64" \
  -DCMAKE_BUILD_TYPE=Release
```

**Expected output (last lines):**
```
-- Configuring done (26.3s)
-- Generating done (0.2s)
-- Build files have been written to: ...
```

**CMake warnings about QTP0001 and QTP0004 are harmless — ignore them.**

After success:
- `build.ninja` file will be created
- `CMakeFiles/` directory will appear
- `CMakeCache.txt` will appear

---

## STEP 4: Build the Application

```bash
PATH=/c/msys64/mingw64/bin:$PATH /c/msys64/mingw64/bin/cmake.exe --build . --config Release -j 4
```

**Expected output:**
```
[1/26] Copying PatientMonitor_QML qml sources...
[2/26] ...
...
[26/26] Linking CXX executable PatientMonitor_QML.exe
```

**Build time:** ~1-3 minutes

**Verify:** `ls *.exe` should show `PatientMonitor_QML.exe`

---

## STEP 5: Run the Application

```bash
PATH=/c/msys64/mingw64/bin:$PATH ./PatientMonitor_QML.exe
```

**Expected:** A window opens showing the Patient Monitor UI with:
- 4 vital sign cards
- 2 waveforms
- Alarm system
- Status indicators

---

## TROUBLESHOOTING

### Problem 1: Qt6Quick.dll not found / CMake can't find Qt6::Quick
**Error:** `Could not find Qt6 QUIET COMPONENTS Core Quick`
**Solution:** Install the package:
```bash
/c/msys64/usr/bin/pacman -S --noconfirm mingw-w64-x86_64-qt6-declarative
```

### Problem 2: cmake or ninja not found
**Solution:** Use full paths:
```bash
/c/msys64/mingw64/bin/cmake.exe --version
/c/msys64/mingw64/bin/ninja.exe --version
```
Or add to PATH first: `export PATH=/c/msys64/mingw64/bin:$PATH`

### Problem 3: CMake configuration errors
**Error:** `Could not find a package configuration file provided by Qt6`
**Solution:** Ensure PREFIX_PATH is correct:
```bash
ls /c/msys64/mingw64/lib/cmake/Qt6/Qt6Config.cmake
```
If missing, reinstall: `pacman -S mingw-w64-x86_64-qt6-base`

### Problem 4: Build errors / stale cache
**Solution:** Full clean rebuild:
```bash
cd /c/Users/Admin/Documents/Claude/Projects/Freelauncing-1/PatientMonitor/qml
rm -rf build && mkdir build && cd build
PATH=/c/msys64/mingw64/bin:$PATH /c/msys64/mingw64/bin/cmake.exe .. -G "Ninja" -DCMAKE_PREFIX_PATH="/c/msys64/mingw64" -DCMAKE_BUILD_TYPE=Release
PATH=/c/msys64/mingw64/bin:$PATH /c/msys64/mingw64/bin/cmake.exe --build . -j 4
```

### Problem 5: Application crashes on launch (DLL not found)
**Solution:** Ensure Qt DLLs are in PATH:
```bash
PATH=/c/msys64/mingw64/bin:$PATH ./PatientMonitor_QML.exe
```

---

## VERIFICATION CHECKLIST

After launching QML app:

- [x] Window opens with title "Patient Monitor — QML Version"
- [ ] Four vital card boxes visible (HR, SpO₂, BP, Temperature)
- [ ] Vitals display numbers (not empty or "—")
- [ ] Vitals change every 2 seconds
- [ ] Two waveforms visible with scrolling animation
- [ ] Alarm system works (button visible)
- [ ] No console errors

---

## SIDE-BY-SIDE COMPARISON

**Terminal 1 (Widgets - if built):**
```bash
cd /c/Users/Admin/Documents/Claude/Projects/Freelauncing-1/PatientMonitor/widgets/build
./PatientMonitor_Widgets.exe
```

**Terminal 2 (QML):**
```bash
cd "/c/Users/Admin/Documents/Claude/Projects/Freelauncing-1/PatientMonitor/qml/build"
PATH=/c/msys64/mingw64/bin:$PATH ./PatientMonitor_QML.exe
```

---

## File Locations

```
C:\Users\Admin\Documents\Claude\Projects\Freelauncing-1\PatientMonitor\
└── qml\
    ├── build\
    │   └── PatientMonitor_QML.exe   ✅ BUILT
    ├── qml\
    │   ├── main.qml
    │   ├── VitalCard.qml
    │   ├── WaveformView.qml
    │   └── AlarmPanel.qml
    ├── main.cpp
    ├── CMakeLists.txt
    └── resources.qrc
```

---

## QUICK COMMAND SUMMARY (Copy & Paste)

```bash
# Add MinGW to PATH
export PATH=/c/msys64/mingw64/bin:$PATH

# Navigate
cd "/c/Users/Admin/Documents/Claude/Projects/Freelauncing-1/PatientMonitor/qml/build"

# Clean
rm -f CMakeCache.txt cmake_install.cmake && rm -rf CMakeFiles

# Configure
cmake .. -G "Ninja" -DCMAKE_PREFIX_PATH="/c/msys64/mingw64" -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build . --config Release -j 4

# Run
./PatientMonitor_QML.exe
```

---

## Status

- ✅ Qt6 Quick installed via MSYS2 pacman
- ✅ CMake configured with Ninja + MinGW + Qt 6.11.0
- ✅ Build completed: 26/26 steps, PatientMonitor_QML.exe created
- ✅ Application launched successfully
