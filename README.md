# 🏂 Snowboard Wearable – IoT-Based Ride Tracker

Welcome to the Snowboard Wearable project repository! This system is designed to help snowboarders visualize, analyze, and improve their performance using a wearable IoT device and a companion mobile application. It combines real-time sensor data, Bluetooth connectivity, trick recognition, and a gamified mobile UI for a next-generation snowboarding experience.

---

## 📱 Mobile App Features

- 🌤 **Weather & Conditions Dashboard** (Live resort data)
- 🏔 **Popular Resorts** listing with status
- 🔋 **Equipment Status Monitor** (Device, goggles, bindings)
- 🏆 **Leaderboards** (Daily, Weekly, Monthly, All-Time)
- 📊 **Trick Analytics** (Top speed, airtime, spins)
- 🕹 **3D Playback** of runs with XP scoring
- 👤 **User Profile & Stats** (XP, badges, levels)

Built using **Flutter**, supports Android 10+ and iOS 14+.

---

## ⚙️ Hardware Features

- 🎯 **ESP32-WROVER Module** for BLE and onboard control
- 🧭 **BNO086 IMU Sensor** for 9-axis motion tracking
- 🔌 **USB-C Charging** with Li-ion battery management
- 📉 **Low-Dropout Regulator (LDO)** for power efficiency
- 🧩 Designed and simulated in **KiCad 7.0**

---

## 🧪 Current Development Status

| Component            | Status            |
|----------------------|-------------------|
| Mobile UI            | ✅ In Progress     |
| ESP32 Firmware       | ✅ In Progress     |
| IMU Integration      | ✅ Completed       |
| Trick Recognition ML | 🔄 Planned         |
| Cloud Sync & Storage | 🔄 Planned         |
| Hardware Prototype   | ✅ Assembled       |
| Full Integration     | ❌ Not Completed   |

---

## 🔧 Getting Started

### 📱 Mobile App (Flutter)

```bash
cd mobile_app/
flutter pub get
flutter run
```

### ⚡ Firmware (ESP32)

- Board: `ESP32-WROVER-E`
- Upload via USB using PlatformIO or Arduino IDE
- Ensure I²C is connected to the IMU (BNO086) with proper pull-ups

---

## 📂 Project Structure

```
snowboard-wearable/
├── mobile_app/            # Flutter mobile UI
├── firmware/              # ESP32 source code
├── hardware/
│   ├── kicad/             # KiCad schematic + PCB
│   └── 3d_render/         # Board mockups
├── assets/                # Images, icons, diagrams
└── docs/                  # SRS, SDD, and supporting files
```

---

## 🧑‍💻 Team 06 — WIT COMP4960

- Aakash Mukherjee  
- Emerson Herrera  
- Kenneth Egan  
- David Kennet  
- Victor Lomba  
Instructor: Dr. Joshua Gyllinsky

---

## 📘 Documentation

- [📄 SRS – Software Requirements Spec](docs/SRS.pdf)
- [📄 SDD – Software Design Document](docs/SDD.pdf)
- [📐 PCB Schematics & Layout](hardware/kicad/)
- [📸 UI Screenshots](assets/screens/)

---

## 🤝 Contributions

Pull requests are welcome! For major changes, please open an issue first to discuss what you'd like to change.  
Please follow our [Code of Conduct](docs/CODE_OF_CONDUCT.md).

---

## 📜 License

This project is for academic and research use. All rights reserved to the student authors.

---

## 🏔️ Let's Ride.
