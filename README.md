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
| ESP32 Firmware       | ✅ Completed     |
| IMU Integration      | ✅ Completed       |
| Trick Recognition ML | ✅ In Progress        |
| Cloud Sync & Storage | 🔄 Planned         |
| Hardware Prototype   | ✅ Assembled       |
| Full Integration     | ❌ Not Completed   |

---

## 🔧 Getting Started

### 📱 Mobile App (Swift)


### ⚡ Firmware (ESP32)

- Board: `ESP32-WROVER-E`
- Upload via USB using PlatformIO or Arduino IDE
- Ensure I²C is connected to the IMU (BNO086) with proper pull-ups

## 🧑‍💻 Team 06 — WIT COMP4960

- Emerson Herrera  
- Kenneth Egan
- Aakash Mukherjee 
- David Kennet  
- Victor Lomba  
Instructor: Dr. Joshua Gyllinsky

---

## 📘 Documentation

- [📄 SRS – Software Requirements Spec](Documents/SRS(Version 1.5).pdf)
- [📄 SDD – Software Design Document](Documents/SDD(Version 1.4).pdf)

---

## 🤝 Contributions

Pull requests are welcome! For major changes, please open an issue first to discuss what you'd like to change.  
Please follow our [Code of Conduct](docs/CODE_OF_CONDUCT.md).

---

## 📜 License

This project is intended for academic and research use only.
All rights reserved. Unauthorized use, reproduction, or distribution is prohibited.

---

## 🏔️ Let's Ride.
