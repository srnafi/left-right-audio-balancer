# Left Right Audio Balancer

[![build](https://github.com/srnafi/left-right-audio-balancer/actions/workflows/ci.yml/badge.svg)](https://github.com/srnafi/left-right-audio-balancer/actions/workflows/ci.yml)

Balancing left and right audio on Windows is buried deep inside legacy Control Panel dialogs behind tabs most people never open. When one ear is louder than the other, fixing it shouldn't mean digging through menus every time.

**Left Right Audio Balancer** puts both channels on a clean, modern floating card right on your desktop.

<p align="center">
  <img src="docs/screenshot.png" width="340" alt="Left Right Audio Balancer Card">
</p>

---

## ✨ Features

- **Per-Ear Sliders:** Adjust Left and Right volume independently or keep them linked with your custom balance offset (e.g. `Linked R -19`).
- **Live Level Meters:** Bright real-time audio peak bars inside each slider show what is actually playing out of each ear.
- **Smart Master Volume:** Rescales total volume while preserving your set balance ratio.
- **One-Click Centre:** Instantly centers audio balance to the average of both channels.
- **Bluetooth Fix Helper:** Fixes headsets that only show one combined channel by toggling AVRCP absolute volume with one click.
- **Position Lock:** Lock the card with the padlock button to prevent accidental dragging.
- **Settings Drawer:** Quick toggle for **Start with Windows**, Bluetooth audio fix, and Quit.
- **Smart Desktop Placement:** Stays at the back of the desktop, keeping out of the way of your active windows, and remains visible with Win+D (Show Desktop).

<p align="center">
  <img src="docs/settings.png" width="340" alt="Settings Drawer">
</p>

---

## ⚡ Performance (v1.0 Rewrite)

Version 1.0 is a complete overhaul built in **Native C++ with Direct2D**, drastically cutting down system footprint:

| Metric | Previous (WPF .NET) | New v1.0 (Native C++) | Improvement |
| :--- | :--- | :--- | :--- |
| **RAM Footprint** | 100 MB – 250+ MB | **~0.8 MB working set** | **>99% less RAM** |
| **Download Size** | ~75 MB | **~390 KB** | **99.5% smaller** |
| **Dependencies** | .NET Desktop Runtime | **Zero (Self-contained native Win32)** | Runs out of the box |
| **Idle CPU** | 0.5% – 2.0% | **0.0%** | Reactive event timers |
| **Graphics** | WPF Software/Direct3D | **Direct2D Hardware-Accelerated** | Smooth anti-aliased corners |

---

## 🚀 Download & Install

Download `LeftRightAudioBalancer.exe` from the [**Latest Release**](https://github.com/srnafi/left-right-audio-balancer/releases/latest).

- **No installation needed:** Just place it anywhere and double-click to run.
- **Start on Boot:** Click the settings icon (sliders) in the header and toggle **Start with Windows**.
- The card remembers its desktop position between restarts. Right-click anywhere on the card or use the drawer to quit.

---

## 🎧 Bluetooth Headsets

Many wireless headphones (Sony WH/WF series, AirPods, Galaxy Buds) report only one shared volume slider because Windows delegates volume control to the headphone's built-in amplifier via Bluetooth AVRCP Absolute Volume.

If your wireless headset shows only one channel:
1. Open the settings drawer by clicking the sliders icon in the header.
2. Click **Apply Bluetooth fix** (Windows will prompt for administrator permission).
3. Reconnect your headset. You now have full independent Left and Right control.

---

## 🛠️ Building from Source

Requires Visual Studio 2022 (MSVC) or C++ build tools.

### Via Command Line (Batch Script)
```cmd
build.bat
```
The compiled binary will be placed at `bin\LeftRightAudioBalancer.exe`.

### Via CMake
```cmd
cmake -B build
cmake --build build --config Release
```

### Via Visual Studio
Open `LeftRightAudioBalancer.sln` in Visual Studio and build `Release | x64`.

---

## 📄 License

[MIT](LICENSE) © srnafi
