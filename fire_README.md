<div align="center">

# 🔥 Intelligent Fire Detection System

### *IoT-Based Automated Fire Hazard Monitoring with Real-Time Alerts*

[![ESP8266](https://img.shields.io/badge/MCU-ESP8266_NodeMCU-blue?style=for-the-badge&logo=espressif)](https://www.espressif.com/)
[![Arduino](https://img.shields.io/badge/IDE-Arduino-teal?style=for-the-badge&logo=arduino)](https://www.arduino.cc/)
[![Blynk](https://img.shields.io/badge/Platform-Blynk_IoT-00C9FF?style=for-the-badge)](https://blynk.io/)
[![IoT](https://img.shields.io/badge/Type-IoT_Safety-red?style=for-the-badge)](https://github.com/)

A smart IoT fire detection system that continuously monitors for fire hazards and instantly triggers a local buzzer alarm while simultaneously sending real-time push notifications to your smartphone via the Blynk IoT platform.

</div>

---

## 📋 Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Hardware Requirements](#hardware-requirements)
- [Circuit Diagram](#circuit-diagram)
- [System Workflow](#system-workflow)
- [Software & Libraries](#software--libraries)
- [Pin Configuration](#pin-configuration)
- [Setup & Installation](#setup--installation)
- [Blynk Dashboard Setup](#blynk-dashboard-setup)
- [How It Works](#how-it-works)
- [Project Structure](#project-structure)
- [Future Enhancements](#future-enhancements)

---

## 📌 Overview

This project is an **Intelligent Fire Detection System** built using an **ESP8266 NodeMCU** microcontroller integrated with a flame sensor, local buzzer alarm, and Blynk IoT cloud connectivity. The system detects fire hazards by monitoring infrared radiation from flames, smoke, and sudden heat changes.

When a fire is detected the system responds in two ways simultaneously — a local buzzer alarm alerts anyone nearby, and a push notification is delivered to the user's smartphone through Blynk, even when they are away from the location.

> **Use Cases:** Homes, offices, factories, warehouses, server rooms, laboratories, hospitals

---

## ✨ Features

| Feature | Description |
|---|---|
| 🔥 Flame Detection | IR-based flame sensor detects fire, smoke, and heat changes |
| 🔔 Instant Buzzer Alarm | Local audio alert activates within milliseconds of detection |
| 📱 Blynk Push Notification | Remote smartphone alert sent immediately on fire detection |
| 📊 Real-Time Dashboard | Live sensor readings and detection status on Blynk mobile app |
| 🕐 Cloud Event Logging | All detection events stored with timestamps for analysis |
| ✅ Auto Recovery | Buzzer stops and dashboard updates automatically when fire clears |
| 📶 WiFi Reconnection | Auto-reconnects if WiFi drops, no manual restart needed |
| 🔁 Continuous Monitoring | 24-hour automated operation, no manual observation required |

---

## 🔧 Hardware Requirements

| Component | Quantity | Purpose |
|---|---|---|
| ESP8266 NodeMCU | 1 | Main microcontroller with WiFi |
| Flame Sensor Module | 1 | Fire and heat detection |
| Active Buzzer (5V) | 1 | Local audio alarm |
| Micro USB Cable | 1 | Power and programming |
| USB Power Adapter / Power Bank | 1 | 5V power supply |
| Connecting Wires | — | Circuit connections |
| Breadboard | 1 | Prototyping |

**Estimated Cost:** ₹400 – ₹700 (approx.)

---

## 🔌 Circuit Diagram

```
                    ┌──────────────────────────┐
                    │     ESP8266 NodeMCU       │
                    │                          │
  Flame Sensor D0 ──┤ D1 / GPIO5 (Digital IN)  │
  Flame Sensor A0 ──┤ A0         (Analog  IN)  │
  Buzzer (+)      ──┤ D2 / GPIO4 (Digital OUT) │
                    │                          │
  3.3V ────────────►│ 3.3V → Flame Sensor VCC  │
  GND  ────────────►│ GND  → All Component GND │
  USB  ────────────►│ VIN  → 5V Power Input    │
                    └──────────────────────────┘
```

> 📷 See `circuit_diagram.png` in this repository for the full visual schematic.

### Wiring Summary

| Component Pin | ESP8266 Pin | Notes |
|---|---|---|
| Flame Sensor VCC | 3.3V | Power supply |
| Flame Sensor GND | GND | Common ground |
| Flame Sensor D0 | D1 (GPIO5) | Digital detection output |
| Flame Sensor A0 | A0 | Analog intensity output |
| Buzzer (+) | D2 (GPIO4) | Alarm output |
| Buzzer (-) | GND | Ground |

---

## 🔄 System Workflow

```
Power ON → ESP8266 Boot
         ↓
    WiFi Connect + Blynk Init
         ↓
    ┌────────────────────────┐
    │   Continuous Loop       │
    │  Read Flame Sensor      │
    │  Every 500ms            │
    └──────────┬─────────────┘
               │
       ┌───────▼────────┐
       │  Fire Detected? │
       └───────┬─────────┘
               │
        ┌──────┴──────┐
       YES            NO
        │              │
        ▼              ▼
  ┌──────────┐   ┌──────────────┐
  │  Buzzer  │   │ Send normal  │
  │  Sounds  │   │ reading to   │
  └──────────┘   │ Blynk cloud  │
        │         └──────────────┘
        ▼
  ┌──────────────────────┐
  │ Blynk Push Notification│
  │ + Dashboard Update    │
  │ + Event Log Saved     │
  └──────────────────────┘
        │
        ▼
  Fire Cleared? → YES → Buzzer OFF + Safe Status
```

---

## 📦 Software & Libraries

Install via Arduino IDE → **Sketch → Include Library → Manage Libraries:**

| Library | Purpose |
|---|---|
| `Blynk` by Volodymyr Shymanskyy | IoT platform, push notifications, dashboard |
| `ESP8266WiFi` | Built-in WiFi connectivity |
| `BlynkSimpleEsp8266` | Blynk ESP8266 integration |

---

## 🛠️ Setup & Installation

### Step 1 — Arduino IDE Setup
1. Install [Arduino IDE 2.x](https://www.arduino.cc/en/software)
2. Add ESP8266 board URL in **File → Preferences:**
   ```
   http://arduino.esp8266.com/stable/package_esp8266com_index.json
   ```
3. Install **esp8266 by ESP8266 Community** via Board Manager

### Step 2 — Install Blynk Library
Sketch → Include Library → Manage Libraries → Search **Blynk** → Install

### Step 3 — Create Blynk Project
1. Download **Blynk IoT app** on your phone
2. Create new Template → name it `Fire Detection System`
3. Add Datastreams: V0 (integer, gauge), V1 (integer, LED), V2 (string, terminal)
4. Enable **Fire Alert** event in Events tab
5. Copy your **Auth Token**

### Step 4 — Configure the Code
Open `main.ino` and update:
```cpp
#define BLYNK_TEMPLATE_ID   "YOUR_TEMPLATE_ID"
#define BLYNK_AUTH_TOKEN    "YOUR_BLYNK_AUTH_TOKEN"
#define WIFI_SSID           "YOUR_WIFI_SSID"
#define WIFI_PASSWORD       "YOUR_WIFI_PASSWORD"
```

### Step 5 — Upload
1. Select **Tools → Board → NodeMCU 1.0 (ESP-12E Module)**
2. Select correct **Port**
3. Click **Upload** ▶️

### Step 6 — Test
1. Open Serial Monitor (115200 baud)
2. Check WiFi and Blynk connection
3. Hold a lighter or candle near the flame sensor
4. Watch buzzer activate and Blynk push notification arrive!

---

## 📱 Blynk Dashboard Setup

| Widget | Type | Virtual Pin | Purpose |
|---|---|---|---|
| Sensor Value | Gauge | V0 | Shows analog flame intensity (0-1023) |
| Status | LED | V1 | Green = Safe, Red = Fire Detected |
| Event Log | Terminal | V2 | Shows detection history |

Enable **Push Notifications** in the Blynk app for fire alert events.

---

## ⚙️ How It Works

1. **Power ON** → ESP8266 initializes, connects to WiFi and Blynk
2. **Continuous Loop** → Flame sensor digital pin read every 500ms
3. **No Fire** → Analog value sent to Blynk dashboard every 2 seconds
4. **Fire Detected** → Digital pin goes LOW
5. **Immediate Response:**
   - Buzzer activates instantly (GPIO HIGH)
   - Blynk push notification sent to smartphone
   - Dashboard LED turns red
   - Event logged with timestamp
6. **Fire Clears** → Digital pin returns HIGH → Buzzer off → Dashboard green
7. **WiFi Drop** → Auto-reconnect without system restart

---

## 📁 Project Structure

```
Intelligent-Fire-Detection/
│
├── main.ino                          # Arduino sketch (complete code)
├── fire_detection_micropython.py     # MicroPython version
├── circuit_diagram.png               # Full circuit schematic
├── workflow_diagram.png              # System workflow diagram
└── README.md                         # This file
```

---

## 🚀 Future Enhancements

- [ ] Add **MQ2 smoke sensor** for dedicated smoke concentration reading
- [ ] Add **MQ135 gas sensor** for LPG/CO gas detection
- [ ] Add **DHT11** for ambient temperature monitoring
- [ ] Integrate **GSM module** for SMS alerts when WiFi is unavailable
- [ ] Add **OLED display** for on-site sensor value display
- [ ] Add **relay-controlled sprinkler** for automatic fire suppression
- [ ] Add **battery backup** for operation during power outages
- [ ] Implement **email alerts** via SMTP for additional notification channel
- [ ] **Multi-zone deployment** with unified Blynk dashboard for all rooms

---

## 📄 License

This project is open-source under the [MIT License](LICENSE).

---

<div align="center">

**Built with ❤️ using ESP8266 + Blynk IoT**

*If this project helped you, please ⭐ star the repo!*

</div>
