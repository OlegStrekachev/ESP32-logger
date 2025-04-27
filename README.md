# Overview

This project uses an **ESP32 microcontroller** to read environmental data from a **DHT11 temperature and humidity sensor** and log that data into a **DAOKAI AT24C256 EEPROM chip** using the **I2C protocol**.

If you log **every minute**, you’ll get **~45 hours** of storage before it fills up.

## **Parts List:**

- **[[ESP 32]] Dev Board**
- **DHT11 Temperature & Humidity Sensor**
- **AT24C256 EEPROM Module**
- **Breadboard and jumper wires**
- **DS3231 RTC Module**
  - Responsible for maintaining consistent timestamp data logging when wi-fi connection is not available or system has no power.

## **Project Structure**

```
your-project-folder/
├── include/              ← Header files (.h) — declarations
│   ├── dht.h             ← initDHT(), printDHTValues()
│   ├── rtc.h             ← initRTC(), syncRTCWithNTP(), getTimestamp()
│   ├── wifi.h            ← initWiFi()
│   └── eeprom.h          ← initEEPROM(), writeData(), readData() (if used)
│
├── lib/                  ← Optional: for 3rd-party libraries (not used here)
│
├── src/                  ← Your actual source code
│   ├── main.cpp          ← The main entry point: calls initXXX(), loop logic
│   ├── dht.cpp           ← Reads from DHT11 sensor and prints/logs
│   ├── rtc.cpp           ← Handles DS3231 + NTP sync
│   ├── wifi.cpp          ← Connects to WiFi
│   ├── eeprom.cpp        ← EEPROM storage functions (optional/coming soon)
│
├── platformio.ini        ← PlatformIO config file

```

| src/main.cpp   | Entry point. Calls `initDHT()`, `initRTC()`, `printDHTValues()` etc.   |
| -------------- | ---------------------------------------------------------------------- |
| dht.cpp/h      | Encapsulates DHT logic → keeps `main.cpp` clean and modular            |
| rtc.cpp/h      | Handles DS3231 startup, NTP sync, timestamp retrieval                  |
| wifi.cpp/h     | Manages Wi-Fi connection, retries, and status                          |
| eeprom.cpp/h   | For storing and retrieving sensor data or timestamps                   |
| platformio.ini | Tells PlatformIO what board, framework, libraries, and settings to use |

**secrets.h**

- A private header file where you store **sensitive credentials**.
- It uses **C preprocessor macros** via `#define`
- Text replacement at **compile time**

## Logic

```
ESP32 Boot → WiFi Connect → Sensor Init → EEPROM Init → WebServer Start
              ↓
           Every 1 min → Read DHT11 → Save to EEPROM
              ↓
    Browser → [GET /] → Web Page (HTML + Bootstrap)
              ↓
    Browser → [GET /data] → Last 5 LogEntries → JSON → Update Table

```

```
ESP32
  ├── Wi-Fi connected to NTP server
  ├── I2C bus communicating with RTC DS3231 at address 0x68
  ├── (Optionally) EEPROM memory chip at 0x57 available
  └── RTC clock synchronized with internet time

```

```
[ESP32]
 └── EEPROM with sensor logs
      ↓
[Wi-Fi Network]
      ↓
[Node.js Server with API endpoints]
 └── Stores data in
[SQL Database (e.g. MySQL, PostgreSQL)]
	 ↓
[Frontend Bootstrap web Interface]


```

```CPP
#include <WiFi.h>
#include <HTTPClient.h>

HTTPClient http;
http.begin("http://<your-server-ip>:3000/upload");
http.addHeader("Content-Type", "application/json");

String json = "{\"temp\":23.5,\"hum\":50.1,\"timestamp\":1712773456}";
int httpResponseCode = http.POST(json);
http.end();

```

Edge cases

- EEPROM fails
- EEPROM is full

[Go to introduction] (#Overview)
