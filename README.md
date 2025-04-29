# ESP32 Weather Logger 🌡️

This project uses an **ESP32 microcontroller** to read environmental data from a **DHT11 temperature and humidity sensor** and log it into a **DAOKAI AT24C256 EEPROM chip** over the **I2C protocol**.

The ESP32 also serves a **local web dashboard** (Bootstrap 5 styled) for real-time data visualization.

If you log **every minute**, you get approximately **45 hours** of storage before EEPROM wraps around.

---

## 📦 Parts List

- **ESP32 Dev Board**
- **DHT11 Temperature & Humidity Sensor**
- **AT24C256 EEPROM Module**
- **DS3231 RTC Module** (Real-Time Clock)
- **Breadboard and Jumper Wires**
- **USB Cable and Computer** (for flashing and monitoring)

---

## 🛠️ Project Structure

```plaintext
your-project-folder/
├── include/
│   ├── dht.h             ← DHT sensor initialization and reading
│   ├── rtc.h             ← RTC/NTP synchronization and timestamp functions
│   ├── wifi.h            ← Wi-Fi connection setup
│   ├── eeprom_logger.h   ← EEPROM reading and writing
│   └── web_server.h      ← Webserver endpoints
│
├── src/
│   ├── main.cpp          ← Project entry point: initializes everything
│   ├── dht.cpp           ← DHT11 sensor logic
│   ├── rtc.cpp           ← RTC + NTP logic
│   ├── wifi.cpp          ← Wi-Fi logic
│   ├── eeprom_logger.cpp ← EEPROM memory logger
│   ├── web_server.cpp    ← Web server logic
│
├── platformio.ini        ← PlatformIO configuration file
│
└── README.md             ← This file
```

## Logic Flow

```plaintext
ESP32 Boot
  ↓
Connect to Wi-Fi
  ↓
Initialize RTC → If RTC lost power → Sync time using NTP
  ↓
Initialize EEPROM
  ↓
Start Web Server (Local Browser Access)
  ↓
Every 1 Minute:
    ↓
  Read DHT11 Sensor
    ↓
  Generate Log Entry (Temperature, Humidity, Timestamp)
    ↓
  Write Log Entry to EEPROM
  ↓
Web Dashboard (HTML + JavaScript)
  ↓
Client Requests [GET /data]
  ↓
Update Table on Web Page

```

## 🌐 How the Web Dashboard Works

### On Page Load:

- ESP32 serves a simple Bootstrap page.
- Browser immediately sends a request to `/data`.
- The last **5 logged sensor readings** are retrieved from **EEPROM** and displayed.

### Auto Refresh:

- Every **5 seconds**, the browser fetches the **latest log entry**.
- New entries appear at the **top**; older entries **shift downward**.
- A maximum of **10 entries** are shown at once (oldest entries are deleted dynamically).

---

## 💾 EEPROM Usage and Data Recovery

- The **EEPROM (AT24C256)** stores `LogEntry` structs:
  - **timestamp** (Unix time),
  - **temperature** (Celsius),
  - **humidity** (% Humidity).
- If the EEPROM becomes full, it **automatically wraps** to the beginning, overwriting the oldest data.
- Logs are **persistent** (permanent) until manually erased.

---

### 📋 Data Extraction (work in progress)

You can recover **all stored data** (not just the last 5 entries) by:

- Scanning the EEPROM from address **`0x00`** to the end (**`0x0FFF`**).
- Reconstructing **LogEntry** structs manually (timestamp, temperature, humidity).
- Parsing timestamps into **human-readable format**.

> ✅ **Timestamps are stored as Unix Time**, making conversion easy across all programming languages.

## Visual Diagram

          +------------------------+
          |        ESP32            |
          |                        3.3V ---+---- VCC (DHT11)
          |                                +---- VCC (EEPROM)
          |                                +---- VCC (RTC DS3231)
          |                        GND ----+---- GND (DHT11, EEPROM, RTC)
          |                        GPIO 13 ---- DATA (DHT11 Sensor)
          |                        GPIO 21 ---- SDA  (EEPROM + RTC)
          |                        GPIO 22 ---- SCL  (EEPROM + RTC)
          +------------------------+
