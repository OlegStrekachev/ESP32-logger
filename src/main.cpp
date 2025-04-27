#include "wifi.h"
#include "dht.h"
#include "rtc.h"
#include "timezone.h"
#include "eeprom_logger.h"  //  Correct file name now
#include <Arduino.h>
#include "web_server.h"  // ✅ NEW





// function declarations
// int myFunction(int, int);


bool rtcOk = false;    //  Global tracker


void printTimestamp() {
  if (!rtcOk) {
    Serial.print("[No RTC] ");
    return;
  }

  DateTime now = getTimestamp();  // Get UTC from RTC
  printLocalTimestamp(now);       //  Convert & print in Mountain Time
}


void setup() {
   delay(500);               // ⏳ Give Serial time to settle after reset
  Serial.begin(115200);
  Serial.println("====================");
  Serial.println("🌎 ESP32 Weather Logger Booting...");
  Serial.println("====================");



  Wire.begin(21, 22);  // ESP32 I2C pins

  Serial.println("I2C Scanner");

  for (byte addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);
    if (Wire.endTransmission() == 0) {
  
      Serial.print("✅ Found I2C device at 0x");
      Serial.println(addr, HEX);
    }
  }


  Serial.println("Done.");




  delay(100);            // Allow Serial to stabilize
  Serial.flush();        // Flush any garbage from the buffer

  rtcOk = rtc.begin();

  if (!rtcOk) {    // ✅ NEW: initialize RTC
    Serial.println("❌ Couldn't find RTC! Check wiring.");
    while (true);         // Stop forever if no RTC
  }

  initWiFi();      // Connects to WiFi (used by RTC sync)
  startWebServer();
  initDHT();       // Initializes DHT sensor
  initEEPROMLogger();  // ✅ No need to check return value, we stop inside if EEPROM fails

if (rtc.lostPower()) {
  Serial.println("⚠️ RTC lost power, syncing with NTP...");
  syncRTCWithNTP();   // ✅ FIRST sync
  printTimestamp();   // ✅ AFTER sync, print
} else {
  printTimestamp();   // ✅ Safe because RTC time is good
  Serial.println("✅ RTC time valid, no need for NTP sync.");
}


}

void loop() {
  handleClient();  // ✅ NEW
  printDHTValues();  // Uses getTimestamp() internally
  trySaveSensorReading();  // ✅ Save to EEPROM every minute
  delay(2000);             // Wait 2 seconds
}


