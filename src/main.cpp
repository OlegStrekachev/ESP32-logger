#include "wifi.h"             // WiFi connection setup
#include "dht.h"              // DHT sensor handling (temperature/humidity)
#include "rtc.h"              // RTC (Real Time Clock) handling
#include "timezone.h"         // Timezone adjustment functions
#include "eeprom_logger.h"    // EEPROM logging utilities
#include <Arduino.h>          // Core Arduino functions
#include "web_server.h"       // Web server routes and setup

#include <LittleFS.h>         // Filesystem to serve local Bootstrap files

// --- Function Declarations (Optional) ---
// int myFunction(int, int);    // Example if needed in the future

// --- Global Variables ---
bool rtcOk = false;    // Tracks if RTC is working properly

// --- Helper Functions ---

// Prints the current timestamp (Mountain Time if RTC OK, otherwise fallback message)
void printTimestamp() {
  if (!rtcOk) {
    Serial.print("[No RTC] "); // Warn if no valid RTC time
    return;
  }

  DateTime now = getTimestamp();      // Get UTC time from RTC
  printLocalTimestamp(now);           // Convert and print local (Mountain) time
}

// --- Main Setup Code ---
void setup() {
  delay(500);                       // Allow Serial connection to settle
  Serial.begin(115200);              // Start Serial console
  Serial.println("====================");
  Serial.println("🌎 ESP32 Weather Logger Booting...");
  Serial.println("====================");

  // Mount LittleFS to serve Bootstrap CSS/JS locally
  if (!LittleFS.begin()) {
    Serial.println("An error has occurred while mounting LittleFS");
    return;                         // Stop setup if filesystem fails
  }

  Wire.begin(21, 22);                // Initialize I2C communication on ESP32 (pins 21=SDA, 22=SCL)

  Serial.println("I2C Scanner");
  // Scan for connected I2C devices
  for (byte addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);
    if (Wire.endTransmission() == 0) {
      Serial.print("✅ Found I2C device at 0x");
      Serial.println(addr, HEX);
    }
  }
  Serial.println("Done.");

  delay(100);                        // Allow Serial output to complete
  Serial.flush();                    // Flush Serial buffer

  rtcOk = rtc.begin();                // Initialize the RTC module

  // If RTC is not found, halt execution
  if (!rtcOk) {
    Serial.println("❌ Couldn't find RTC! Check wiring.");
    while (true);                     // Infinite loop to stop
  }

  initWiFi();                         // Connect to WiFi (needed for NTP sync)
  startWebServer();                   // Start the web server
  initDHT();                          // Initialize DHT sensor (temperature/humidity)
  initEEPROMLogger();                 // Initialize EEPROM storage for logging

  // Handle RTC power loss
  if (rtc.lostPower()) {
    Serial.println("⚠️ RTC lost power, syncing with NTP...");
    syncRTCWithNTP();                 // Sync RTC time with NTP server
    printTimestamp();                 // Print timestamp after NTP sync
  } else {
    printTimestamp();                 // Print RTC timestamp (already good)
    Serial.println("✅ RTC time valid, no need for NTP sync.");
  }
}

// --- Main Loop Code ---
void loop() {
  handleClient();              // Handle incoming web server requests
  printDHTValues();             // Read and print DHT sensor values
  trySaveSensorReading();       // Attempt to save new sensor reading into EEPROM (once per minute)
  delay(2000);                  // Wait 2 seconds before next cycle
}
